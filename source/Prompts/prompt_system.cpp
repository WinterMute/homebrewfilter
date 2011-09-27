
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

/****************************************************************************
 * WindowPrompt
 *
 * Displays a prompt window to user, with information, an error message, or
 * presenting a user with a choice
 ***************************************************************************/
int systemPrompt()
{
	bool stop = false;
	int system = -1;

	GuiWindow promptWindow(144,64);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	promptWindow.SetPosition(-185, 280);
	GuiTrigger trigA;
	GuiTrigger trigB;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigB.SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);

	// Hintergrund
	GuiImageData dialogBox(Theme.choice);
	GuiImage dialogBoxImg(&dialogBox);

	// button data
	GuiImageData wii_BtnImgData(Theme.wii_inactive);
	GuiImageData wii_BtnImgDataOver(Theme.wii_active);
	GuiImageData gc_BtnImgData(Theme.gc_inactive);
	GuiImageData gc_BtnImgDataOver(Theme.gc_active);
	GuiImageData wii_gc_BtnImgData(Theme.wii_gc_inactive);
	GuiImageData wii_gc_BtnImgDataOver(Theme.wii_gc_active);

	// button
	GuiImage wii_BtnImg(&wii_BtnImgData);
	GuiImage gc_BtnImg(&gc_BtnImgData);
	GuiImage wii_gc_BtnImg(&wii_gc_BtnImgData);

	// button over
	GuiImage wii_BtnImgOver(&wii_BtnImgDataOver);
	GuiImage gc_BtnImgOver(&gc_BtnImgDataOver);
	GuiImage wii_gc_BtnImgOver(&wii_gc_BtnImgDataOver);
	
	GuiButton wii_Btn(wii_BtnImgData.GetWidth(), wii_BtnImgData.GetHeight());
	wii_Btn.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	wii_Btn.SetPosition(8, 0);
	wii_Btn.SetImage(&wii_BtnImg);
	wii_Btn.SetImageOver(&wii_BtnImgOver);
	wii_Btn.SetTrigger(&trigA);
	
	GuiButton gc_Btn(gc_BtnImgData.GetWidth(), gc_BtnImgData.GetHeight());
	gc_Btn.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	gc_Btn.SetPosition(0, 0);
	gc_Btn.SetImage(&gc_BtnImg);
	gc_Btn.SetImageOver(&gc_BtnImgOver);
	gc_Btn.SetTrigger(&trigA);
	
	GuiButton wii_gc_Btn(wii_gc_BtnImgData.GetWidth(), wii_gc_BtnImgData.GetHeight());
	wii_gc_Btn.SetAlignment(ALIGN_RIGHT, ALIGN_MIDDLE);
	wii_gc_Btn.SetPosition(-8, 0);
	wii_gc_Btn.SetImage(&wii_gc_BtnImg);
	wii_gc_Btn.SetImageOver(&wii_gc_BtnImgOver);
	wii_gc_Btn.SetTrigger(&trigA);

	GuiButton back(0, 0);
	back.SetTrigger(&trigB);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&wii_Btn);
	promptWindow.Append(&gc_Btn);
	promptWindow.Append(&wii_gc_Btn);
	promptWindow.Append(&back);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED2);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(!stop)
	{
		usleep(100);

		if(wii_Btn.GetState() == STATE_CLICKED)
		{
			system = 0;
			stop = true;
		}
		
		if(gc_Btn.GetState() == STATE_CLICKED)
		{
			system = 1;
			stop = true;
		}
		
		if(wii_gc_Btn.GetState() == STATE_CLICKED)
		{
			system = 2;
			stop = true;
		}
		
		if(back.GetState() == STATE_CLICKED)
			stop = true;
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	
	return system;
}

