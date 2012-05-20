
#include <malloc.h>
#include <wiiuse/wpad.h>

#include "RuntimeIOSPatch.h"
#include "menu.h"
#include "video.h"
#include "wad.h"
#include "getios.h"

#define HAVE_AHBPROT ((*(vu32*)0xcd800064 == 0xFFFFFFFF) ? 1 : 0)

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	// Initialise the video system
	VIDEO_Init();
	Video_SetMode();

	if(HAVE_AHBPROT)
		runtimePatchApply();

	WPAD_Init();
	PAD_Init();

	menu();

	if(CheckAppFound(GetTitleID()))
	{
		WII_LaunchTitle(0x0001000154484246);

	}
	else
	{
		*(vu32*)0x8132FFFB = 0x50756E65;
		DCFlushRange((void*)0x8132FFFB, 4);
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}

	return 0;
}
