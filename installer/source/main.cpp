
#include <malloc.h>
#include <wiiuse/wpad.h>
#include <runtimeiospatch.h>

#include "menu.h"
#include "video.h"
#include "wad.h"
#include "getios.h"

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	// Initialise the video system
	VIDEO_Init();
	Video_SetMode();

	WPAD_Init();
	PAD_Init();

   	if(AHBPROT_DISABLED)
		IosPatch_RUNTIME(true, false, true, true);
	else
		IOS_ReloadIOS(236);

	menu();

	signed_blob *p_tmd = NULL;
	u32 len;
	s32 ret;

	ret = GetTMD(0x0001000154484246LL, &p_tmd, &len);

	if(ret < 0)
	{
		*(vu32*)0x8132FFFB = 0x50756E65;
		DCFlushRange((void*)0x8132FFFB, 4);
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}
	else
	{
		WII_LaunchTitle(0x0001000154484246);

	}

	return 0;
}
