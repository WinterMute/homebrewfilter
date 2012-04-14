 /***************************************************************************
 * Copyright (C) 2009
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 * networkops.cpp
 *
 * Network operations
 * for Wii-Xplorer 2009
 ***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <ogcsys.h>
#include <ogc/machine/processor.h>

#include "http.h"
//#include "netreceiver.h"
#include "update.h"
#include "main.h"
#include "tcp.h"
//#include "libftp/FTPServer.h"

//static NetReceiver Receiver;
static bool networkinit = false;
static bool networkerror = false;
static char IP[16];
static u8 * ThreadStack = NULL;
static bool firstRun = false;

static lwp_t networkthread = LWP_THREAD_NULL;
static bool networkHalt = true;
static bool exitRequested = false;
static string revs;

void CheckVersion(void)
{
	if(Settings.checkrev == -1)
	{
		struct block file = downloadfile("http://www.nanolx.org/hbf/DOL/revisions");
		if (file.data != NULL)
		{
			revs = (char*)file.data;
			free(file.data);

			if((signed)revs.find("\n") != -1)
				revs.erase(revs.find("\n"));

			Settings.checkrev = atoi(revs.c_str());
		}
	}
}

/****************************************************************************
 * Initialize_Network
 ***************************************************************************/
void Initialize_Network(void)
{
    if(networkinit)
        return;

	s32 result;

    result = if_config(IP, NULL, NULL, true);

    if(result < 0) {
        networkinit = false;
		networkerror = true;
		return;
	}

    networkinit = true;
	networkerror = false;
    return;
}

/****************************************************************************
 * DeInit_Network
 ***************************************************************************/
void DeInit_Network(void)
{
    net_deinit();
}
/****************************************************************************
 * Check if network was initialised
 ***************************************************************************/
bool IsNetworkInit(void)
{
    return networkinit;
}

/****************************************************************************
 * Check that network has error
 ***************************************************************************/
bool IsNetworkError(void)
{
    return networkerror;
}

/****************************************************************************
 * Check that network thread is halted
 ***************************************************************************/
bool IsNetworkHalted(void)
{
    return networkHalt;
}

/****************************************************************************
 * Get network IP
 ***************************************************************************/
char * GetNetworkIP(void)
{
    return IP;
}

/****************************************************************************
 * HaltNetwork
 ***************************************************************************/
void HaltNetworkThread()
{
	if(!networkHalt)
	{
		networkHalt = true;

		// wait for thread to finish
		while(!LWP_ThreadIsSuspended(networkthread))
			usleep(100);
	}
}

/****************************************************************************
 * ResumeNetworkThread
 ***************************************************************************/
void ResumeNetworkThread()
{
	if(networkHalt)
	{
		networkHalt = false;
		LWP_ResumeThread(networkthread);
	}
}

/*********************************************************************************
 * Networkthread for background network initialize and update check with idle prio
 *********************************************************************************/
static void * networkinitcallback(void *arg)
{
    while(!exitRequested)
    {
        if(networkHalt)
        {
            LWP_SuspendThread(networkthread);
			usleep(100);
			continue;
        }

        if(!networkinit)
            Initialize_Network();

        if(!firstRun)
        {
            LWP_SetThreadPriority(networkthread, 0);
            firstRun = true;
	    //CheckVersion();
        }

        usleep(200000);
    }
	return NULL;
}

/****************************************************************************
 * InitNetworkThread with priority 0 (idle)
 ***************************************************************************/
void InitNetworkThread()
{
    ThreadStack = (u8 *) memalign(32, 16384);
    if(!ThreadStack)
        return;

	LWP_CreateThread (&networkthread, networkinitcallback, NULL, ThreadStack, 16384, 30);
}

/****************************************************************************
 * ShutdownThread
 ***************************************************************************/
void ShutdownNetworkThread()
{
	ShutdownTcpThread();
    exitRequested = true;
	networkinit = false;
	networkerror = false;


    if(networkthread != LWP_THREAD_NULL)
    {
        ResumeNetworkThread();
        LWP_JoinThread (networkthread, NULL);
        networkthread = LWP_THREAD_NULL;
    }

	if(ThreadStack)
        free(ThreadStack);
    ThreadStack = NULL;
}
