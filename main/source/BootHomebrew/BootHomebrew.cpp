#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <ogc/machine/processor.h>
#include <wiiuse/wpad.h>
#include <vector>
#include <string>
#include <sys/dir.h>

#include "main.h"
#include "dolloader.h"
#include "elfloader.h"
#include "filelist.h"
#include "DiskOperations/di2.h"
#include "Tools/load_channel.h"
#include "Tools/parser.h"
#include "Tools/SelectIos.h"
#include "Neek/uneek_fs.h"
#include "gecko.h"
#include "ahbfix.h"

#define BLOCKSIZE               70*1024      //70KB
#define MAX_CMDLINE 4096
#define MAX_ARGV    1000

#define GC_DOL_MAGIC    "gchomebrew dol"
#define GC_MAGIC_BUF    (char *) 0x807FFFE0
#define GC_DOL_BUF      (u8 *)0x80800000
#define BC              0x0000000100000100ULL

struct __argv args;
char cmdline[MAX_CMDLINE];
char *a_argv[MAX_ARGV];
char *meta_buf = NULL;
static u8 *homebrewbuffer = (u8 *) 0x92000000;
static u32 homebrewsize = 0;
static int wiiload_args = 0;
static char temp_arg[1024];
std::string filepath;

extern bool in_neek;
extern bool wiiload;

void arg_init()
{
	memset(&args, 0, sizeof(args));
	memset(cmdline, 0, sizeof(cmdline));
	memset(a_argv, 0, sizeof(a_argv));
	args.argvMagic = ARGV_MAGIC;
	args.length = 1; // double \0\0
	args.argc = 0;
	args.commandLine = cmdline;
	args.argv = a_argv;
	args.endARGV = a_argv;
}

char* strcopy(char *dest, const char *src, int size)
{
	strncpy(dest,src,size);
	dest[size-1] = 0;
	return dest;
}

int arg_add(const char *arg)
{
	int len = strlen(arg);
	if (args.argc >= MAX_ARGV) return -1;
	if (args.length + len + 1 > MAX_CMDLINE) return -1;
	strcopy(cmdline + args.length - 1, arg, len+1);
	args.length += len + 1; // 0 term.
	cmdline[args.length - 1] = 0; // double \0\0
	args.argc++;
	args.endARGV = args.argv + args.argc;
	return 0;
}

/****************************************************************************
 * FileSize
 *
 * Get filesize in bytes. u64 for files bigger than 4GB
 ***************************************************************************/
u64 FileSize(const char * filepath)
{
  struct stat filestat;

  if (stat(filepath, &filestat) != 0)
    return 0;

  return filestat.st_size;
}

/****************************************************************************
 * LoadFileToMem
 *
 * Load up the file into a block of memory
 ***************************************************************************/
int LoadFileToMem(const char *filepath, u8 **inbuffer, u64 *size)
{
    int ret = -1;
    u64 filesize = FileSize(filepath);
	char * filename = strrchr(filepath, '/');
	if(filename)
        filename++;

    *inbuffer = NULL;
    *size = 0;

    FILE *file = fopen(filepath, "rb");

    if (file == NULL)
        return -1;

    u8 *buffer = (u8 *) malloc(filesize);
    if (buffer == NULL)
    {
        fclose(file);
        return -2;
    }

    u64 done = 0;
    u32 blocksize = BLOCKSIZE;

    do
    {

        if(blocksize > filesize-done)
            blocksize = filesize-done;

        ret = fread(buffer+done, 1, blocksize, file);
        if(ret < 0)
        {
            free(buffer);
            fclose(file);
            return -3;
        }
        else if(ret == 0)
        {
            //we are done
            break;
        }

        done += ret;

    }
    while(done < filesize);

    fclose(file);

    if (done != filesize)
    {
        free(buffer);
        return -3;
    }

    *inbuffer = buffer;
    *size = filesize;

    return 1;
}

int CopyHomebrewMemory(u8 *temp, u32 pos, u32 len)
{
    homebrewsize += len;
    memcpy((homebrewbuffer)+pos, temp, len);

    return 1;
}

int CopyArgs(u8* temp, u32 len)
{
	memcpy(temp_arg,temp,len);
	wiiload_args = 1;
	return 1;
}

int LoadHomebrew(const char * path)
{
	filepath = path;
	u8 *buffer = NULL;
	u64 filesize = 0;
	int ret = LoadFileToMem(path, &buffer, &filesize);
	if(ret < 0)
		return ret;

	ret = CopyHomebrewMemory(buffer, 0, filesize);
	if(buffer) {
		free(buffer);
		buffer = NULL;
	}

	return ret;
}

int BootHomebrew()
{

    char* abuf;
    size_t asize;

    if(homebrewsize == 0)
        return -1;

    entrypoint entry;
    u32 cpu_isr;

	arg_init();

	if (wiiload_args)
	{
		abuf = temp_arg;
		asize = strlen(abuf);
		while (asize != 0)
		{
			gprintf("argument = %s\n",abuf);
			arg_add(abuf);
			abuf+=asize;
			abuf+=1;
			asize = strlen(abuf);
		}
	}
	else
	{
		arg_add(filepath.c_str()); // argv[0] = filepath
		while(parser(Settings.forwarder_arg, "<arg>", "</arg>") != "")
		{
			arg_add(parser(Settings.forwarder_arg, "<arg>", "</arg>").c_str());
			Settings.forwarder_arg.erase(0, Settings.forwarder_arg.find("</arg>") +1);
		}
	}

	if ( valid_elf_image(homebrewbuffer) == 1 )
		entry = (entrypoint) load_elf_image(homebrewbuffer);
	else
		entry = (entrypoint) load_dol(homebrewbuffer, &args);

    if (!entry)
        return -1;

	//ExitApp();
//we can't use check_uneek_fs
//as we already shut down the uneek_fs system
//so it will always return false

	if (in_neek == false)
	{
		if(wiiload)
		{
			if(Options.wiiload_ahb == 2)
			{
				Patch_ahbprot();
			}

			if(Options.wiiload_ahb != 0)
			{
				IOS_ReloadIOS(Options.wiiload_ios);
			}
		}
		else
		{
			if(Settings.force_reload == "HW_AHBPROT")
			{
				Patch_ahbprot();
			}

			if(Settings.force_reload != "NORELOAD")
			{
				IOS_ReloadIOS(SelectedIOS());
			}
		}
	}

	wiiload_args = 0;

	/*this will also be called when wiiloading an application
	will need to check if it's expected behavour? */

/*
	if(!wiiload_args)
	{

		if(SelectedIOS() != IOS_GetVersion() || Settings.force_reload != "")
		{
			//keep ahbprot rights in new ios
			Patch_ahbprot();
			IOS_ReloadIOS(SelectedIOS());
		}
	}
    wiiload_args = 0;
*/
    SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
    _CPU_ISR_Disable (cpu_isr);
    __exception_closeall();
    entry();
    _CPU_ISR_Restore (cpu_isr);

    return 0;
}

int BootGameCubeHomebrew()
{
    if(homebrewsize == 0)
        return -1;

    static tikview view ATTRIBUTE_ALIGN(32);

    DI2_Init();
    DI2_Reset();
    DI2_ReadDiscID((u64 *) 0x80000000);
    DI2_Mount();

    strcpy(GC_MAGIC_BUF, GC_DOL_MAGIC);
    DCFlushRange(GC_MAGIC_BUF, 32);
    memcpy(GC_DOL_BUF, homebrewbuffer, homebrewsize);
    DCFlushRange(GC_DOL_BUF, homebrewsize);
    *(vu32 *) 0xCC003024 |= 0x07;

    ES_GetTicketViews(BC, &view, 1);
    int ret = ES_LaunchTitle(BC, &view);
    exit_uneek_fs();

    return ret;
}
