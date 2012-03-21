
#include <malloc.h>
#include <wiiuse/wpad.h>

#include "RuntimeIOSPatch.h"
#include "menu.h"
#include "video.h"
#include "wad.h"
#include "Network/network.h"

#define HAVE_AHBPROT ((*(vu32*)0xcd800064 == 0xFFFFFFFF) ? 1 : 0)

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	// Initialise the video system
	InitNetworkThread();
	VIDEO_Init();
	Video_SetMode();

	if(HAVE_AHBPROT)
		runtimePatchApply();

	WPAD_Init();
	PAD_Init();

	menu();

	SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);

	return 0;
}
