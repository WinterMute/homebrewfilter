
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <unistd.h>
#include <ogc/machine/processor.h>
#include <ogc/irq.h>
#include "uneek_fs.h"

/* Video variables */
static void *framebuffer = NULL;
static GXRModeObj *vmode = NULL;

extern void __exception_closeall();
typedef void (*entrypoint) (void);

static u8 *buffer = (u8 *) 0x92000000;

extern u8  hbf_dol[];
extern u32 hbf_dol_size;

typedef struct _dolheader {
	u32 text_pos[7];
	u32 data_pos[11];
	u32 text_start[7];
	u32 data_start[11];
	u32 text_size[7];
	u32 data_size[11];
	u32 bss_start;
	u32 bss_size;
	u32 entry_point;
} dolheader;

u32 load_dol_image(void *dolstart)
{
	u32 i;
	dolheader *dolfile;

	if (dolstart)
	{
		dolfile = (dolheader *) dolstart;
		for (i = 0; i < 7; i++)
		{
			if ((!dolfile->text_size[i]) || (dolfile->text_start[i] < 0x100))
				continue;

			memmove((void *) dolfile->text_start[i], dolstart
				+ dolfile->text_pos[i], dolfile->text_size[i]);

			DCFlushRange ((void *) dolfile->text_start[i], dolfile->text_size[i]);
			ICInvalidateRange((void *) dolfile->text_start[i], dolfile->text_size[i]);
		}

		for (i = 0; i < 11; i++)
		{
			if ((!dolfile->data_size[i]) || (dolfile->data_start[i] < 0x100))
				continue;

			memmove((void *) dolfile->data_start[i], dolstart
					+ dolfile->data_pos[i], dolfile->data_size[i]);

			DCFlushRange((void *) dolfile->data_start[i],
					dolfile->data_size[i]);
		}
		return dolfile->entry_point;
	}
	return 0;
}

void Video_Clear(s32 color)
{
	VIDEO_ClearFrameBuffer(vmode, framebuffer, color);
}

int main(int argc, char **argv) {


/*
	raw_irq_handler_t irq_handler;

	__IOS_ShutdownSubsystems();
	__ES_Init();

	__ES_Reset();

	// Mask IPC IRQ while we're busy reloading
	__MaskIrq(IRQ_PI_ACR);
	irq_handler = IRQ_Free(IRQ_PI_ACR);
	IRQ_Request(IRQ_PI_ACR, irq_handler, NULL);
    __UnmaskIrq(IRQ_PI_ACR);
	__IPC_Reinitialize();

	__IOS_InitializeSubsystems();

*/
	VIDEO_Init();
	vmode = VIDEO_GetPreferredMode(NULL);

	/* Allocate memory for the framebuffer */
	framebuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(vmode));

	/* Configure the video subsystem */
	VIDEO_Configure(vmode);

	/* Setup video */
	VIDEO_SetNextFramebuffer(framebuffer);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();

	if (vmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();

	/* Clear the screen */
	Video_Clear(COLOR_BLACK);


	memcpy(buffer, (void *) hbf_dol, hbf_dol_size);

	u32 exeEntryPointAddress = load_dol_image(buffer);
	entrypoint exeEntryPoint = (entrypoint) exeEntryPointAddress;
	u32 level;
	SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	_CPU_ISR_Disable(level);
	__exception_closeall();
	exeEntryPoint();
	_CPU_ISR_Restore(level);

	exit(0);
}

