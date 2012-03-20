/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * demo.cpp
 * Basic template/demonstration of libwiigui capabilities. For a
 * full-featured app using many more extensions, check out Snes9x GX.
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <ogcsys.h>
#include <unistd.h>
#include <wiiuse/wpad.h>
#include <dirent.h>

//#include "audio.h"
#include "filelist.h"
#include "FreeTypeGX.h"
#include "input.h"
#include "main.h"
#include "menu.h"
#include "libwiigui/gui.h"
#include "Tools/FontSystem.h"
#include "Tools/load.h"
#include "Tools/load_channel.h"
#include "Tools/RuntimeIOSPatch.h"
#include "Tools/save.h"
#include "Tools/SelectIos.h"
#include "Tools/throbber.h"
#include "Network/network.h"
#include "Network/tcp.h"
#include "BootHomebrew/BootHomebrew.h"
#include "BootHomebrew/dolloader.h"
#include "DiskOperations/di2.h"

#define HAVE_AHBPROT ((*(vu32*)0xcd800064 == 0xFFFFFFFF) ? 1 : 0)

/*** Extern variables ***/
extern GuiWindow * mainWindow;
extern GuiImageData * pointer;

/*** Extern functions ***/
extern void ResumeGui();

std::vector<homebrew_list> vechomebrew_list_category[99];
std::vector<homebrew_list> vechomebrew_list_unassigned;
std::vector<homebrew_list> vechomebrew_list_choice;
std::vector<app_ios> appios;

struct SSettings Settings;
struct SOptions Options;

int ExitRequested = 0;
s8 PowerOff = -1;
bool boothomebrew = false;
bool boot_buffer = false;

// kopiere ios für app in einen vector
void addAppIos(string foldername, int ios)
{
	if(appios.size() == 0)
		appios.push_back(app_ios(foldername, ios));
	else
	{
		bool found = false;
		int i;
		for(i = 0; i < (signed)appios.size(); i++)
		{
			if(appios[i].foldername == foldername)
			{
				found = true;
				break;
			}
		}
		if(found)
			appios[i].ios = ios;
		if(!found)
			appios.push_back(app_ios(foldername, ios));
	}
}

void ExitApp()
{
	ShutdownPads();
	StopGX();
	//if(strcasecmp(Settings.code,"NULL") == 0)
		save();
    	UnmountAllDevices();
	ISFS_Deinitialize();
}

static void WiiResetPressed()
{
	PowerOff = SYS_RETURNTOMENU;
}

static void WiiPowerPressed()
{
	PowerOff = SYS_POWEROFF_STANDBY;
}

static void WiimotePowerPressed(s32 chan)
{
	WiiPowerPressed();
}

void
DefaultSettings()
{
	Settings.sd_insert			= SDCard_Inserted();
	Settings.usb_insert			= USBDevice_Inserted();
	Settings.dvd_insert			= DVD_Inserted();
	Settings.category_name_all	= "All";
	sprintf (Settings.new_category_name, "New Category");
	Settings.current_category	= 0;
	Settings.current_page		= 1;
	Settings.last_app_pos		= -1;
	Settings.checkrev			= -1;
	sprintf (Settings.code, "NULL");

	// in kleinbuchstaben umwandeln
/*	transform(Settings.MyDir.begin(),Settings.MyDir.end(),Settings.MyDir.begin(),::tolower);
	if(Settings.MyDir.substr(0, Settings.MyDir.find(":/")) == "sd")
		Settings.MyPath			= "sd1" + Settings.MyDir.substr(Settings.MyDir.find(":/"), Settings.MyDir.rfind("/") - Settings.MyDir.find(":/") +1);
	else if(Settings.MyDir.substr(0, Settings.MyDir.find(":/")) == "usb")
		Settings.MyPath			= "usb1" + Settings.MyDir.substr(Settings.MyDir.find(":/"), Settings.MyDir.rfind("/") - Settings.MyDir.find(":/") +1);
	else
		Settings.MyPath			= Settings.MyDir.substr(0, Settings.MyDir.rfind("/") +1);
*/
	Settings.Apps_from			= EFFECT_SLIDE_TOP;	// Apps kommen von "EFFECT_SLIDE_TOP", "EFFECT_SLIDE_BOTTOM", "EFFECT_SLIDE_RIGHT", "EFFECT_SLIDE_LEFT"
	Settings.Apps_to			= 0;				// Apps geht nach "EFFECT_SLIDE_TOP", "EFFECT_SLIDE_BOTTOM", "EFFECT_SLIDE_RIGHT", "EFFECT_SLIDE_LEFT"
	Settings.grid				= false;
	Settings.system				= 1;
	Settings.device				= "sd1";
	Settings.device_dat			= "sd1";
}

void
DefaultOptions()
{
	sprintf (Options.theme, tr("STANDARD"));
	sprintf (Options.language, tr("STANDARD"));
	sprintf (Options.font, tr("STANDARD"));
	sprintf (Options.temp_code, "NULL");

	Options.slide_effect	= 1;
	Options.last_category	= 1;
	Options.apps		= 4;
	Options.quick_start	= 0;
	Options.show_all	= 1;
	Options.navigation	= 0;
	Options.temp_network	= 0;
	Options.temp_newrevtext	= 1;
}

int
main(int argc, char *argv[])
{

	SYS_SetResetCallback(WiiResetPressed);
	SYS_SetPowerCallback(WiiPowerPressed);
	WPAD_SetPowerButtonCallback(WiimotePowerPressed);

	InitVideo();			// Initialize video
	SetupPads();			// Initialize input
//	InitAudio();			// Initialize audio
	InitGUIThreads();		// Initialize GUI
	//DI2_Init();			// Initialize DVD
	MountAllDevices();
	InitNetworkThread();	// Initialize Network
	InitTcpThread();
	InitThrobberThread();	// Initialize Throbber
	ISFS_Initialize();		// Initialize Nand

	LoadHBF();

	DefaultSettings();
	DefaultOptions();
	DefaultTheme();

	load();
	if(Options.network)
		ResumeNetworkThread();

	SetFont();

	#ifdef HW_RVL
	pointer = new GuiImageData(Theme.player_point);
	#endif

	mainWindow = new GuiWindow(screenwidth, screenheight);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	ResumeGui();
	stretch(Settings.top, Settings.bottom, Settings.left, Settings.right);

	if(HAVE_AHBPROT)
		runtimePatchApply();

	DI2_Init(); // Init DVD

	if(strstr(Options.language, tr("STANDARD")))
		translate();

	AvailableCategory.categories[0] = tr(Settings.category_name_all);
	check_device();

	check_priiloader();

/*	while(1)
	{
		WPAD_ScanPads();
		if(PowerOff != -1)
		{
			char buffer[20];
			sprintf(buffer, "%i", PowerOff);
			PowerOff = -1;
			debug(buffer);
		//	menu = MENU_EXIT;
		}

		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A)
			break;
	}
*/	MainMenu(MENU_NONE);

    if(boothomebrew)
    {
		if(SelectedIOS() != IOS_GetVersion())
			IOS_ReloadIOS(SelectedIOS());

		if(strstr(Settings.forwarder_path.c_str(), ":/apps/") != 0)
			BootHomebrew();
		else if(strstr(Settings.forwarder_path.c_str(), ":/gc_apps/") != 0)
			BootGameCubeHomebrew();
    }
    else if(boot_buffer)
	//	BootHomebrew();

	if(get_bootmii() == 2)
		IOS_ReloadIOS(254);

	if(get_nandemu() == 2)
	{
		if (opendir(check_path("sd1:/apps/NANDEmu-Boot").c_str()) != NULL)
		{
			LoadHomebrew ("sd1:/apps/NANDEmu-Boot/boot.dol");
			BootHomebrew ();
		}
		else if (opendir(check_path("usb1:/apps/NANDEmu-Boot").c_str()) != NULL)
		{
			LoadHomebrew ("usb1:/apps/NANDEmu-Boot/boot.dol");
			BootHomebrew ();
		}
	}

	if(get_priiloader() == 2)
	{
		*(vu32*)0x8132FFFB = 0x4461636f;
		DCFlushRange((void*)0x8132FFFB, 4);
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}

	if(PowerOff == SYS_RETURNTOMENU)
	{
		*(vu32*)0x8132FFFB = 0x50756E65;
		DCFlushRange((void*)0x8132FFFB, 4);
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}
	else if(PowerOff != -1)
		SYS_ResetSystem(PowerOff, 0, 0);

	return 0;
}
