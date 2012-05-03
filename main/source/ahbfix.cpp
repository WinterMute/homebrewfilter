/*--------------------------------------------------------------
Copyright (C) 2011 Tueidj

This software falls under the GPL license

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.

-------------------------------------------------------------*/



#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <ogc/machine/processor.h>
#include "gecko.h"


#define CHECK_AHB       0x0D800064
#define MEM2_PROT       0x0D8B420A
#define ES_MODULE_START (u16*)0x939F0000

static const u16 ticket_check[] = {
	0x685B,          // ldr r3,[r3,#4]  ; get TMD pointer
	0x22EC, 0x0052,  // movls r2, 0x1D8
	0x189B,          // adds r3, r3, r2 ; add offset of access rights field in TMD
	0x681B,          // ldr r3, [r3]    ; load access rights (haxxme!)
	0x4698,          // mov r8, r3      ; store it for the DVD video bitcheck later
	0x07DB           // lsls r3, r3, #31; check AHBPROT bit
};

s32 Patch_ahbprot(void)
{
	// patch HID4 write in HBC stub - this should be done when the extra HID4 bits are turned on
	*(u32*)0x80002174 = 0x60000000;

	if (read32(CHECK_AHB) != 0xffffffff)
	{
		gprintf("AHBPROT doesn't seem to be disabled.\n");
		return false;
	}
	else
	{
		u16 *patchme;
		write16(MEM2_PROT, 2);
		for (patchme=ES_MODULE_START; patchme < ES_MODULE_START+0x4000; patchme++)
		{
			if (!memcmp(patchme, ticket_check, sizeof(ticket_check)))
			{
				gprintf("Found TMD Access rights check at %p\n", patchme);
				// write16/uncached poke doesn't work for this. Go figure.
				patchme[4] = 0x23FF; // li r3, 0xFF
				DCFlushRange(patchme+4, 2);
				return true;
				//break;
			}
		}
	}
	return false;
}
/*
	IOS_ReloadIOS(58); // MEM2 protection re-enabled here
	dprintf("Loaded a new IOS.\n");
	if (read32(CHECK_AHB) != -1)
		dprintf("AHBPROT doesn't seem to be disabled.\n");
	else
		dprintf("AHBPROT is disabled (TADA!).\n");
*/
