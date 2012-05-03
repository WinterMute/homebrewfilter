#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <gccore.h>
#include <unistd.h>
#include <wiiuse/wpad.h>
#include <dirent.h>
#include <malloc.h>
#include "common.h"
#include "gecko.h"


extern struct SSettingsNeek2o n2oSettings;


bool FileExist (char *fn)
{
	FILE * f;
	f = fopen(fn, "rb");
	if (!f) return false;
	fclose(f);
	return true;
}
	
bool DirExist (char *path)
{
	DIR *dir;
	
	dir=opendir(path);
	if (dir)
	{
		closedir(dir);
		return true;
	}
	
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
u8 *ReadFile2Buffer (char *path, size_t *filesize, int *err)
	{
	u8 *buff = NULL;
	int size;
	int bytes;
	int block = 65536;
	FILE* f = NULL;
	
	if (filesize) *filesize = 0;
	if (err) *err = 0;
	
	f = fopen(path, "rb");
	if (!f)
		{
		if (err != NULL) *err = -1;
		return NULL;
		}

	//Get file size
	fseek( f, 0, SEEK_END);
	size = ftell(f);
	if (filesize) *filesize = size;
	
	if (size <= 0)
		{
		if (err != NULL) *err = -2;
		fclose (f);
		return NULL;
		}
		
	// Return to beginning....
	fseek( f, 0, SEEK_SET);
	
	//buff = malloc (size);
 	buff = memalign(32,size);	
	
	if (buff == NULL) 
		{
		if (err != NULL) *err = -3;
		fclose (f);
		return NULL;
		}
	
	bytes = 0;
	do
		{
		bytes += fread(&buff[bytes], 1, block, f );
		}
	while (bytes < size);

	fclose (f);
	
	return buff;
	}

/*
this function will check if a folder "may" contain a valid nand
*/	
bool IsNandFolder (char *path)
	{
	char npath[300];
	
	sprintf (npath, "%s/TITLE", path);
	if (!DirExist(npath)) return false;

	sprintf (npath, "%s/TICKET", path);
	if (!DirExist(npath)) return false;

	sprintf (npath, "%s/SHARED1", path);
	if (!DirExist(npath)) return false;

	sprintf (npath, "%s/SYS", path);
	if (!DirExist(npath)) return false;

	return true;
	}




s32 adjust_nandpath (void)
{
	char nandpath[128];
	FILE *fp = NULL;
	s32 check_npbin = false;

	// let's keep things simple:
	// 1. check if we point to a valid nandpath
	// 2. if this is the case, create a nandpath.bin with it's contents.
	// 3. delete the nandcfg.bin.
	// 4. if not 2, check for a valid nandpath in nandpath.bin
	// 5. if there is one, everything ok, delete nandcfg.bin
	// 6. if this all fails, maybe there is a nandcfg.bin or a nand in root.
	// 7. I don't feel like analysing that for the moment.
	strcpy(nandpath,n2oSettings.nanddisc);
	strcat(nandpath,n2oSettings.nandfolder);
	if (IsNandFolder(nandpath))
	{
		strcpy(nandpath,n2oSettings.nanddisc);
		strcat(nandpath,"/sneek/nandcfg.bin");
		if (FileExist(nandpath))
		{
			remove(nandpath);
		}
		//don't erase nandpath.bin if xml contents incorrect
		//nandpath.bin should have priority above root folder nand
		//isn't backwards compatibility fun?
		if(n2oSettings.neeknandpath[0] != 0)
		{		
			strcpy(nandpath,n2oSettings.nanddisc);
			strcat(nandpath,"/sneek/nandpath.bin");
			if (FileExist(nandpath))
			{
				remove(nandpath);
			}
			fp = fopen(nandpath, "wb");
			if(fp)
			{
				int write = strlen(n2oSettings.nandfolder)+1;
				fwrite(n2oSettings.nandfolder, 1, write, fp);
				fclose (fp);
				return 1;
			}
			else
			{
				//unable to create nandpath.bin, how is that possible
				return -2;
			}
		}
		else
		{
			check_npbin = true;
		}
	}
	else
	{
		check_npbin = true;
	}
	// path in bootneek.xml incorrect or non existent
	// so we check path in nandpath.bin
	if(check_npbin == true)
	{
		strcpy(nandpath,n2oSettings.nanddisc);
		if (get_nand_folder(nandpath))
		{
			//so, if we have a valid path in nandpath.bin, 
			//use that for channel launch adjustment
			strcpy(n2oSettings.nandfolder,nandpath);
			//we have a nandpath.bin
			//so delete nandcfg.bin
			strcpy(nandpath,n2oSettings.nanddisc);
			strcat(nandpath,"/sneek/nandcfg.bin");
			if (FileExist(nandpath))
			{
				remove(nandpath);
			}
			return -3;
		}
		//we could check for the path in nandcfg.bin.
		//or maybe check for a root nand
		return -2;
	}
	//will keep compiler happy
	//should never arrive here
	return -4;
}

void create_dipath(void)
{
		
	char dipath[64];		
	FILE *fp = NULL;
		
	strcpy(dipath,n2oSettings.nanddisc);
	strcat(dipath,"/sneek/dipath.bin");	
	if (FileExist(dipath))
	{
		remove(dipath);
	}
	fp = fopen(dipath, "wb");
	if(fp)
	{
		int write = strlen(n2oSettings.neekdipath)+1;
		fwrite(n2oSettings.neekdipath, 1, write, fp);
		fclose (fp);
	}
}

s32 set_neek_channel(void)
{
	char launchfile[128];
	char stitlexsb[32];
	char titlefolder[128];
	u64 title;
	int titlemsb, titlelsb;
	FILE *fp = NULL;

	//strcpy(launchfile,n2oSettings.neeknandpath);
	strcpy(launchfile,n2oSettings.nanddisc);
	strcat(launchfile,n2oSettings.nandfolder);
	strcat(launchfile,"/sys/launch.sys");
	if(n2oSettings.neekbootchannel[0] == 0)
	{
		//seems like we don't need to boot a specific channel...
		if (FileExist(launchfile))
		{
			remove(launchfile);
		}
		return 1;
	}
	else
	{
		//I think we should check if the channel exists
		//If it doesn't, remove a potential launchfile
		//And give a warning message
		//strcpy(titlefolder,n2oSettings.neeknandpath);
		strcpy(titlefolder,n2oSettings.nanddisc);
		strcat(titlefolder,n2oSettings.nandfolder);
	    strcat(titlefolder,"/title/");
		strncpy(stitlexsb,n2oSettings.neekbootchannel,8);
	    stitlexsb[8] = '/';
		strncpy(stitlexsb+9,n2oSettings.neekbootchannel+8,8);
		stitlexsb[17]= 0;
		strcat(titlefolder,stitlexsb);
		gprintf("Titlefolder = %s\n",titlefolder);
		if (DirExist(titlefolder))
		{
	    	//calculate the channel for storage in launch.sys
			strncpy(stitlexsb,n2oSettings.neekbootchannel,8);
			stitlexsb[8]=0;
			sscanf(stitlexsb,"%x",&titlemsb);
			strncpy(stitlexsb,n2oSettings.neekbootchannel+8,8);
			stitlexsb[8]=0;
			sscanf(stitlexsb,"%x",&titlelsb);
			title = (u64)(titlemsb) << 32;
			title = title + (u64)(titlelsb);
			gprintf( "titleid = %08x %08x\r\n", (u32)((title)>>32), (u32)(title) );
			fp = fopen(launchfile, "wb");
			if(fp)
			{
				fwrite((void*)(&title), 1, 8, fp);
				fclose (fp);
				return 1;
			}
			else
			{
				//unable to create /sys/launch.sys, how is that possible
				return -2;
			}
		}
		else
		{
			//the boot channel title folder doesn't exist
			return -3;
		}
	}
}

void xml_set_default(void)
{

	n2oSettings.neeknandpath[0] = 0;
	strcpy(n2oSettings.nanddisc,"sd1:/");
	strcpy(n2oSettings.nandfolder,"/");
	n2oSettings.neekbootchannel[0]= 0;
	n2oSettings.neekdipath[0]= 0;
	
	
//	strcpy(n2oSettings.neeknandpath,"usb://nands/nand1");
//	strcpy(n2oSettings.nanddisc,"usb:/");
//	strcpy(n2oSettings.nandfolder,"/nands/nand1");

	//strcpy(n2oSettings.neekdipath,"/sneek");
	strcpy(n2oSettings.nandbootdelay,"0");
	strcpy(n2oSettings.neekbootdelay,"0");
	strcpy(n2oSettings.nandbootapplication,"usb://apps/postloader/boot.dol");
	strcpy(n2oSettings.neekbootapplication,"usb://apps/postloader/boot.dol");
	strcpy(n2oSettings.nandbootdescription,"postloader");
	strcpy(n2oSettings.neekbootdescription,"postloader");
	strcpy(n2oSettings.nandbootargument,"priibooter");
	strcpy(n2oSettings.neekbootargument,"priibooter");
	//strcpy(n2oSettings.neekbootchannel,"00000000000000010000000000000002");
	strcpy(n2oSettings.neekselection,"0");
}


s32 get_nand_folder(char* nandpath)
{
	char nandpathfile[128];
	char buffer[128];
	FILE *fp = NULL;
	u32 counter;

	strcpy(nandpathfile, nandpath);
	strcat(nandpathfile,"/sneek/nandpath.bin");
	nandpath[0] = 0;
	fp = fopen(nandpathfile, "rb");				
	if(fp)
	{
		fseek (fp, 0, SEEK_END);
		int len = ftell(fp);
		fseek (fp, 0, SEEK_SET);
		if (len > 80)
			len = 80;
		if (len > 0)
		{
			fread(buffer, 1, len, fp);
			fclose (fp);
			for (counter=0;counter<len;counter++)
			{
				if ((buffer[counter] == 13)||(buffer[counter] == 10)||(buffer[counter] == 32))	
				{
					buffer[counter] = 0;
				}
			}
			if (buffer[0] != '/')
			{
				strcat(nandpath,"/");
			}
			strcat(nandpath,buffer);
			return(true);
		}
		else
		{
			fclose (fp);
			return (false);
		}
	}
	else
	{
		return (false);
	}

}
