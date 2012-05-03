

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ogc/machine/processor.h>
#include <sys/iosupport.h>
#include <dirent.h>

#include "uneek_fs.h"
#include "../gecko.h"
#include "common.h"

struct SSettingsNeek2o n2oSettings;


#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

bool IsNandFolder (char *path);
void DoMini(u8* kbuf, size_t kernel_size);
s32 get_nand_folder(char* nandpath);

// xml.cpp
void CreateXmlFile(const char* filename, struct SSettingsNeek2o *Settings);
int LoadXmlFile(const char* filename, struct SSettingsNeek2o *Settings);

#ifdef __cplusplus
}
#endif  //__cplusplus



void ExitApp();



int boot_neek2o() 
{
	int allfound = 0;	
	s32 nresult;
	u32 kernel_size;
	int ferr;
	u8* kernel_buff;
	char filepath[1024];
	char kpath[1024] = { 0 };

	xml_set_default();
	strcpy(filepath,"sd1:/sneek/kernel.bin");
	if (FileExist(filepath))
	{
		strcpy(kpath,filepath);
		strcpy(filepath,"sd1:/sneek/bootneek.xml");
		if (FileExist(filepath))
		{
			LoadXmlFile(filepath,&n2oSettings);
			allfound = 1;
		}
	}
	if (allfound == 0)
	{						
		strcpy(filepath,"usb1:/sneek/kernel.bin");
	 	if (FileExist(filepath))
		{
			strcpy(kpath,filepath);
			strcpy(filepath,"usb1:/sneek/bootneek.xml");
			if (FileExist(filepath))
			{
				LoadXmlFile(filepath,&n2oSettings);
				allfound = 1;
			}
	 	}
	}
	if(kpath[0] != 0)
	{
		gprintf ("Booting neek...\n");
		//check if it's sneek or uneek
		if (n2oSettings.neeknandpath[0] == 0)
		{
			//no nandpath defined
			//their still could be a nandpath.bin and a nandcfg.bin
			//assume loading from where we found kernel.bin
			if ((kpath[0] == 's')||(kpath[0] == 'S'))
			{
				strcpy(n2oSettings.nanddisc,"sd1:/");
				strcpy(n2oSettings.nandfolder,"/");			
			}
			else
			{
				strcpy(n2oSettings.nanddisc,"usb1:/");
				strcpy(n2oSettings.nandfolder,"/");
			}			
		}
		nresult = adjust_nandpath();
		if((nresult == -2) || (nresult == -3))
		{
			if (n2oSettings.neeknandpath[0] == 0)
			{
				gprintf("Warning:bootneek.xml not found\n");
			}
			else
			{
				gprintf("Warning:bootneek.xml contains an invalid nandpath\n");
			}
		}
		if (nresult == -2)
		{
			gprintf("Will use nandcfg.bin nand or rootnand instead\n"); 	
		}
		if (nresult == -3)
		{
			//we might still check if nandpath.bin contains a valid nandpath
			gprintf("Will use nandpath.bin instead\n");
		}
		//if a specific dipath is requested, let's generate the file
		if (n2oSettings.neekdipath[0] != 0)
		{
			create_dipath();
		}
		//if there is a request to boot a specific channel
		//let's make this possible as well
		//for future expansion
		nresult = set_neek_channel();
		if (nresult == -2)
		{
			gprintf("Error: could not create /sys/launch.sys\n");
			gprintf("Unable to boot the neek with the requested startup channel\n");
		}
		if (nresult == -3)
		{
			gprintf("Error: could not locate the requested neek startup channel on the nand\n");
		}
		if (nresult == 1)
		{
			kernel_buff = ReadFile2Buffer(kpath,&kernel_size,&ferr);
			ExitApp();
			//if it failed reading....
			//ferr = 1;
			if (ferr == 0)
			{
				//if the kernel should be patched
				//this is the place to do it.
				//I had it, let's go for it
				DoMini(kernel_buff,kernel_size);
			}
			else
			{
				gprintf("ERROR:unable to load kernel.bin\n");
			}
		}
		else
		{
			ExitApp();
		}
	}
	else
	{
		gprintf("couldn't find sneek2o or uneek2o\n");
		ExitApp();
	}
	return false;
}
	