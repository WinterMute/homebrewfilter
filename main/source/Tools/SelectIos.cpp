
#include <ogcsys.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <string>
#include <vector>
#include <algorithm>
#include "main.h"
#include "xprintf.h"

std::vector<int> ioslist;
int selectedIos = IOS_GetVersion();
int ios_pos = 0;
int bootmii = 0;
int nandemu = 0;
int priiloader = 0;

bool check_neek2o() {

	if(fopen("sd1:/sneek/kernel.bin", "rb") || fopen("usb1:/sneek/kernel.bin", "rb"))
		return true;

	return false;

}

s32 NandReadFile(char *filepath, u8 **buffer, u32 *filesize)
{
	s32 Fd;
	int ret;

	if (buffer == NULL)
	{
		printf("NULL Pointer\n");
		return -1;
	}

	Fd = ISFS_Open(filepath, ISFS_OPEN_READ);
	if (Fd < 0)
	{
		printf("ISFS_Open %s failed %d\n", filepath, Fd);
		return Fd;
	}

	fstats *status;
	status = (fstats *)memalign(32, ((sizeof(fstats))+31)&(~31));
	if (status == NULL)
	{
		printf("Out of memory for status\n");
		return -1;
	}

	ret = ISFS_GetFileStats(Fd, status);
	if (ret < 0)
	{
		printf("ISFS_GetFileStats failed %d\n", ret);
		ISFS_Close(Fd);
		free(status);
		return -1;
	}

	*buffer = (u8 *)memalign(32, ((status->file_length)+31)&(~31));
	if (*buffer == NULL)
	{
		printf("Out of memory for buffer\n");
		ISFS_Close(Fd);
		free(status);
		return -1;
	}

	ret = ISFS_Read(Fd, *buffer, status->file_length);
	if (ret < 0)
	{
		printf("ISFS_Read failed %d\n", ret);
		ISFS_Close(Fd);
		free(status);
		free(*buffer);
		return ret;
	}

	ISFS_Close(Fd);

	*filesize = status->file_length;
	free(status);

	if (*filesize > 0)
	{
		DCFlushRange(*buffer, *filesize);
		ICInvalidateRange(*buffer, *filesize);
	}

	return 0;
}

s32 GetTMD(u64 TicketID, signed_blob **Output, u32 *Length)
{
    signed_blob* TMD = NULL;

    u32 TMD_Length;
    s32 ret;

    /* Retrieve TMD length */
    ret = ES_GetStoredTMDSize(TicketID, &TMD_Length);
    if (ret < 0)
        return ret;

    /* Allocate memory */
    TMD = (signed_blob*)memalign(32, (TMD_Length+31)&(~31));
    if (!TMD)
        return IPC_ENOMEM;

    /* Retrieve TMD */
    ret = ES_GetStoredTMD(TicketID, TMD, TMD_Length);
    if (ret < 0)
    {
        free(TMD);
        return ret;
    }

    /* Set values */
    *Output = TMD;
    *Length = TMD_Length;

    return 0;
}

int check_priiloader() {
	char filepath[ISFS_MAXPATH] ATTRIBUTE_ALIGN(0x20);
	static u64 titleId ATTRIBUTE_ALIGN(32) = 0x0000000100000002LL;
	int ret = 0;
	tmd *ptmd = NULL;
	u32 TMD_size = 0;
	signed_blob *stmd = NULL;
	u32 i = 0;
	u32 filesize = 0;
	u8 *buffer = NULL;
	const char* checkStr = "priiloader";
	int retValue = -1;

	ret = GetTMD(titleId, &stmd, &TMD_size);

	if (ret < 0)
		goto end;

	if (!stmd)
	{
		ret = -1;

		goto end;
	}

	ptmd = (tmd*)SIGNATURE_PAYLOAD(stmd);

	for (i = 0; i < ptmd->num_contents; i++)
	{
		if (ptmd->contents[i].index == ptmd->boot_index)
		{
			sprintf(filepath, "/title/%08x/%08x/content/%08x.app" , 0x00000001, 0x00000002, ptmd->contents[i].cid);
			ret = NandReadFile(filepath, &buffer, &filesize);
			if (ret < 0 || filesize < 0) {
				retValue = -2;
				goto end;
			}
			break;
		}
	}

	for (i = 0; i < filesize - strlen(checkStr); i++)
	{
		if (!strncmp((char*)buffer + i, checkStr, strlen(checkStr)))
		{
			retValue = 1;

			break;
		}
	}

end:
	free(buffer);

	free(stmd);
	ptmd = NULL;

	priiloader = retValue;
	return retValue;

}

// Check if this is an IOS stub (according to WiiBrew.org)
bool IsKnownStub(u32 noIOS, s32 noRevision)
{
	if (noIOS ==   3 && noRevision == 65280) return true;
	if (noIOS ==   4 && noRevision == 65280) return true;
	if (noIOS ==   5 && noRevision == 65280) return true;
	if (noIOS ==  10 && noRevision ==   768) return true;
	if (noIOS ==  11 && noRevision ==   256) return true;
	if (noIOS ==  16 && noRevision ==   512) return true;
	if (noIOS ==  20 && noRevision ==   256) return true;
	if (noIOS ==  30 && noRevision ==  2816) return true;
	if (noIOS ==  40 && noRevision ==  3072) return true;
	if (noIOS ==  50 && noRevision ==  5120) return true;
	if (noIOS ==  51 && noRevision ==  4864) return true;
	if (noIOS ==  52 && noRevision ==  5888) return true;
	if (noIOS ==  60 && noRevision ==  6400) return true;
	if (noIOS ==  70 && noRevision ==  6912) return true;
	if (noIOS == 222 && noRevision == 65280) return true;
	if (noIOS == 223 && noRevision == 65280) return true;
	if (noIOS == 249 && noRevision == 65280) return true;
	if (noIOS == 250 && noRevision == 65280) return true;
	if (noIOS == 254 && noRevision ==     2) return true;
	if (noIOS == 254 && noRevision ==     3) return true;
	if (noIOS == 254 && noRevision ==   260) return true;
	if (noIOS == 254 && noRevision == 65280) return true;
	// NAND Emu (to avoid freeze when accessing this IOS)
	if (noIOS == 253 && noRevision == 65535) return true;

	return false;
}

int getIosPos(int getios)
{
	for(int i = 0; i < (signed)ioslist.size(); i++)
	{
		if(ioslist[i] == getios)
		{
			ios_pos = i;
			break;
		}
	}
	return ios_pos;
}

int nextIos()
{
	if(ioslist.size() != 0)
	{
		ios_pos++;
		if(ios_pos > (signed)ioslist.size() -1)
			ios_pos = ioslist.size() -1;

		selectedIos= ioslist[ios_pos];
	}

	xprintf("Previous IOS: %d\n", selectedIos);
	return selectedIos;
}
int previousIos()
{
	if(ioslist.size() != 0)
	{
		ios_pos--;
		if(ios_pos < 0)
			ios_pos = 0;

		selectedIos= ioslist[ios_pos];
	}

	xprintf("Next IOS: %d\n", selectedIos);
	return selectedIos;
}

bool listIOS()
{
	if(ios_pos > 0)
		return true;

	ioslist.clear();
	u32 nbTitles;
	if (ES_GetNumTitles(&nbTitles) < 0)
		return false;

	// Allocate the memory for titles
	u64 *titles = (u64*)memalign(32, nbTitles*sizeof(u64));

	if (titles == NULL)
		return false;

	if (ES_GetTitles(titles, nbTitles) < 0)
		return false;

	int i;
	u32 titleID;

	// For each titles found
	for (i = 0; i < (signed)nbTitles; i++)
	{
		// Skip non-system titles
		if (titles[i] >> 32 != 1) continue;

		// Skip the system menu
		titleID = titles[i] & 0xFFFFFFFF;

		if (titleID == 2) continue;

		// Skip BC, MIOS and possible other non-IOS titles
		if (titleID > 0xFF) continue;

		// Skip the running IOS
		if (titleID == 0) continue;

		// Skip NAND-Emu IOS
		if (titleID == 253)
		{
			nandemu = 1;
			continue;
		}

		// Skip bootmii IOS
		if (titleID == 254)
		{
			continue;
		}

		// Check if this title is an IOS stub
		u32 tmdSize;
		tmd *iosTMD ATTRIBUTE_ALIGN(32);

		// Get the stored TMD size for the title
		if (ES_GetStoredTMDSize(0x0000000100000000ULL | titleID, &tmdSize) < 0)
			return false;

		signed_blob *iosTMDBuffer = (signed_blob *)memalign(32, (tmdSize+32)&(~31));
		memset(iosTMDBuffer, 0, tmdSize);

		// Get the stored TMD for the title
		if (ES_GetStoredTMD(0x0000000100000000ULL | titleID, iosTMDBuffer, tmdSize) < 0)
			return false;

		iosTMD = (tmd *)SIGNATURE_PAYLOAD(iosTMDBuffer);

		free(iosTMDBuffer);

		// Get the title version
		u8 noVersion = iosTMD->title_version;
		bool isStub = false;

		// Check if this is an IOS stub (according to WiiBrew.org)
		if (IsKnownStub(titleID, iosTMD->title_version))
			isStub = true;
		else
		{
			// If the version is 00, it's probably a stub
			//
			// Stubs have these things in common:
			//	- Title version is mostly 65280, or even better, the last 2 hexadecimal digits are 0;
			// 	- Stub have one app of their own (type 0x1) and 2 shared apps (type 0x8001).
			if (noVersion == 0)
				isStub = ((iosTMD->num_contents == 3) && (iosTMD->contents[0].type == 1 && iosTMD->contents[1].type == 0x8001 && iosTMD->contents[2].type == 0x8001));
			else
				isStub = false;
		}

		xprintf("testing IOS: %d\n", titleID);

		if(!isStub)
		{
			if(titleID == 254)
			{	
				bootmii = 1;
			}
			xprintf("added IOS %d to list.\n", titleID);
			ioslist.push_back(titleID);
		}

	}
	std::sort( ioslist.begin(), ioslist.end() ); // sortieren
	return true;

}

int SelectedIOS()
{
	return selectedIos;
}

int SearchAppIOS(std::string foldername)
{
	if(listIOS())
	{
		bool found = false;
		for(int i = 0; i < (signed)appios.size(); i++)
		{
			if(appios[i].foldername == foldername)
			{
				selectedIos = appios[i].ios;
				found = true;
				break;
			}
		}
		if(!found)
			selectedIos = IOS_GetVersion();
		getIosPos(selectedIos);
	}

	return selectedIos;
}

int GetAppIOS(std::string foldername)
{
	if(listIOS())
	{
		for(int i = 0; i < (signed)appios.size(); i++)
		{
			if(appios[i].foldername == foldername)
				return appios[i].ios;
		}
	}

	return selectedIos;
}

int get_priiloader()
{
	return priiloader;
}

void set_priiloader(int choice)
{
	priiloader = choice;
}

int get_bootmii()
{
#ifndef VWII
	if(Options.bootmii_boot2)
		return 3;
	else
#endif
	return bootmii;
}

void set_bootmii(int choice)
{
	bootmii = choice;
}

int get_nandemu()
{
	return nandemu;
}

void set_nandemu(int choice)
{
	nandemu = choice;
}
