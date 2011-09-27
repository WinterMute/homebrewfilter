
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
int devicePrompt()
{
	bool stop = false;
	int device = -1;

	GuiWindow promptWindow(144,64);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	promptWindow.SetPosition(-135, 280);
	GuiTrigger trigA;
	GuiTrigger trigB;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigB.SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);

	// Hintergrund
	GuiImageData dialogBox(Theme.choice);
	GuiImage dialogBoxImg(&dialogBox);

	// button data
	GuiImageData sd_BtnImgData(Theme.sd_inactive);
	GuiImageData sd_BtnImgDataOver(Theme.sd_active);
	GuiImageData usb_BtnImgData(Theme.usb_inactive);
	GuiImageData usb_BtnImgDataOver(Theme.usb_active);
	GuiImageData sd_usb_BtnImgData(Theme.sd_usb_inactive);
	GuiImageData sd_usb_BtnImgDataOver(Theme.sd_usb_active);

	// button
	GuiImage sd_BtnImg(&sd_BtnImgData);
	GuiImage usb_BtnImg(&usb_BtnImgData);
	GuiImage sd_usb_BtnImg(&sd_usb_BtnImgData);

	// button over
	GuiImage sd_BtnImgOver(&sd_BtnImgDataOver);
	GuiImage usb_BtnImgOver(&usb_BtnImgDataOver);
	GuiImage sd_usb_BtnImgOver(&sd_usb_BtnImgDataOver);
	
	GuiButton sd_Btn(sd_BtnImgData.GetWidth(), sd_BtnImgData.GetHeight());
	sd_Btn.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	sd_Btn.SetPosition(8, 0);
	sd_Btn.SetImage(&sd_BtnImg);
	sd_Btn.SetImageOver(&sd_BtnImgOver);
	sd_Btn.SetTrigger(&trigA);
	
	GuiButton usb_Btn(usb_BtnImgData.GetWidth(), usb_BtnImgData.GetHeight());
	usb_Btn.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	usb_Btn.SetPosition(0, 0);
	usb_Btn.SetImage(&usb_BtnImg);
	usb_Btn.SetImageOver(&usb_BtnImgOver);
	usb_Btn.SetTrigger(&trigA);
	
	GuiButton sd_usb_Btn(sd_usb_BtnImgData.GetWidth(), sd_usb_BtnImgData.GetHeight());
	sd_usb_Btn.SetAlignment(ALIGN_RIGHT, ALIGN_MIDDLE);
	sd_usb_Btn.SetPosition(-8, 0);
	sd_usb_Btn.SetImage(&sd_usb_BtnImg);
	sd_usb_Btn.SetImageOver(&sd_usb_BtnImgOver);
	sd_usb_Btn.SetTrigger(&trigA);

	GuiButton back(0, 0);
	back.SetTrigger(&trigB);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&sd_Btn);
	promptWindow.Append(&usb_Btn);
	promptWindow.Append(&sd_usb_Btn);
	promptWindow.Append(&back);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED2);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(!stop)
	{
		usleep(100);

		if(sd_Btn.GetState() == STATE_CLICKED)
		{
			device = 1;
			stop = true;
		}
		
		if(usb_Btn.GetState() == STATE_CLICKED)
		{
			device = 2;
			stop = true;
		}
		
		if(sd_usb_Btn.GetState() == STATE_CLICKED)
		{
			device = 3;
			stop = true;
		}
		
		if(back.GetState() == STATE_CLICKED)
			stop = true;
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	
	return device;
}

