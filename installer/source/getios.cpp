
#include <ogcsys.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <string>
#include <vector>

std::vector<int> ioslist;

// Check if this is an IOS stub (according to WiiBrew.org)
bool IsKnownStub(u32 noIOS, s32 noRevision)
{
	/*
	if (noIOS ==   3 && noRevision == 65280) return true;
	if (noIOS ==   4 && noRevision == 65280) return true;
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
	if (noIOS == 250 && noRevision == 65280) return true;*/
	if (noIOS == 254 && noRevision ==     2) return true;
	if (noIOS == 254 && noRevision ==     3) return true;
	if (noIOS == 254 && noRevision ==   260) return true;
	if (noIOS == 254 && noRevision == 65280) return true;

	// BootMii As IOS is installed on IOS254 rev 31338
	if (noIOS == 254 && noRevision == 31338) return true;

	// NAND Emu
	if (noIOS == 253 && noRevision == 65535) return true;

	return false;
}

bool listIOS()
{
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

		// Skip nandemu IOS
		if (titleID == 253) continue;

		// Skip bootmii IOS
		if (titleID == 254) continue;

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

		if(!isStub)
			ioslist.push_back(titleID);
	}
	return true;
}

bool getIOS(int ios)
{

	for(int i = 0; i < (signed)ioslist.size(); i++)
	{
		if(ioslist[i] == ios)
			return true;
	}

	return false;
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