
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"
#include "Tools/fileop.h"

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

	GuiWindow promptWindow(328,64);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	promptWindow.SetPosition(-135, 280);
	GuiTrigger trigA;
	GuiTrigger trigB;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigB.SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);

	// Hintergrund
	GuiImageData dialogBox(Theme.choice_large);
	GuiImage dialogBoxImg(&dialogBox);

	// button data
	GuiImageData sd_BtnImgData(Theme.sd_inactive);
	GuiImageData sd_BtnImgDataOver(Theme.sd_active);
	GuiImageData usb_BtnImgData(Theme.usb_inactive);
	GuiImageData usb_BtnImgDataOver(Theme.usb_active);
	GuiImageData sd_usb_BtnImgData(Theme.sd_usb_inactive);
	GuiImageData sd_usb_BtnImgDataOver(Theme.sd_usb_active);
	GuiImageData dvd_BtnImgData(Theme.dvd_inactive);
	GuiImageData dvd_BtnImgDataOver(Theme.dvd_active);
	GuiImageData gca_BtnImgData(Theme.gca_inactive);
	GuiImageData gca_BtnImgDataOver(Theme.gca_active);
	GuiImageData gcb_BtnImgData(Theme.gcb_inactive);
	GuiImageData gcb_BtnImgDataOver(Theme.gcb_active);
	GuiImageData all_BtnImgData(Theme.all_inactive);
	GuiImageData all_BtnImgDataOver(Theme.all_active);

	// button
	GuiImage sd_BtnImg(&sd_BtnImgData);
	GuiImage usb_BtnImg(&usb_BtnImgData);
	GuiImage sd_usb_BtnImg(&sd_usb_BtnImgData);
	GuiImage dvd_BtnImg(&dvd_BtnImgData);
	GuiImage gca_BtnImg(&gca_BtnImgData);
	GuiImage gcb_BtnImg(&gca_BtnImgData);
	GuiImage all_BtnImg(&all_BtnImgData);

	// button over
	GuiImage sd_BtnImgOver(&sd_BtnImgDataOver);
	GuiImage usb_BtnImgOver(&usb_BtnImgDataOver);
	GuiImage sd_usb_BtnImgOver(&sd_usb_BtnImgDataOver);
	GuiImage dvd_BtnImgOver(&dvd_BtnImgDataOver);
	GuiImage gca_BtnImgOver(&gca_BtnImgDataOver);
	GuiImage gcb_BtnImgOver(&gca_BtnImgDataOver);
	GuiImage all_BtnImgOver(&all_BtnImgDataOver);

	GuiButton sd_Btn(sd_BtnImgData.GetWidth(), sd_BtnImgData.GetHeight());
	sd_Btn.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	sd_Btn.SetPosition(8, 0);
	sd_Btn.SetImage(&sd_BtnImg);
	sd_Btn.SetImageOver(&sd_BtnImgOver);
	sd_Btn.SetTrigger(&trigA);

	GuiButton usb_Btn(usb_BtnImgData.GetWidth(), usb_BtnImgData.GetHeight());
	usb_Btn.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	usb_Btn.SetPosition(52, 0);
	usb_Btn.SetImage(&usb_BtnImg);
	usb_Btn.SetImageOver(&usb_BtnImgOver);
	usb_Btn.SetTrigger(&trigA);

	GuiButton sd_usb_Btn(sd_usb_BtnImgData.GetWidth(), sd_usb_BtnImgData.GetHeight());
	sd_usb_Btn.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	sd_usb_Btn.SetPosition(96, 0);
	sd_usb_Btn.SetImage(&sd_usb_BtnImg);
	sd_usb_Btn.SetImageOver(&sd_usb_BtnImgOver);
	sd_usb_Btn.SetTrigger(&trigA);

	GuiButton dvd_Btn(dvd_BtnImgData.GetWidth(), dvd_BtnImgData.GetHeight());
	dvd_Btn.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	dvd_Btn.SetPosition(0, 0);
	dvd_Btn.SetImage(&dvd_BtnImg);
	dvd_Btn.SetImageOver(&dvd_BtnImgOver);
	dvd_Btn.SetTrigger(&trigA);

	GuiButton gca_Btn(gca_BtnImgData.GetWidth(), gca_BtnImgData.GetHeight());
	gca_Btn.SetAlignment(ALIGN_RIGHT, ALIGN_MIDDLE);
	gca_Btn.SetPosition(-96, 0);
	gca_Btn.SetImage(&gca_BtnImg);
	gca_Btn.SetImageOver(&gca_BtnImgOver);
	gca_Btn.SetTrigger(&trigA);

	GuiButton gcb_Btn(gcb_BtnImgData.GetWidth(), gcb_BtnImgData.GetHeight());
	gcb_Btn.SetAlignment(ALIGN_RIGHT, ALIGN_MIDDLE);
	gcb_Btn.SetPosition(-52, 0);
	gcb_Btn.SetImage(&gcb_BtnImg);
	gcb_Btn.SetImageOver(&gcb_BtnImgOver);
	gcb_Btn.SetTrigger(&trigA);

	GuiButton all_Btn(all_BtnImgData.GetWidth(), all_BtnImgData.GetHeight());
	all_Btn.SetAlignment(ALIGN_RIGHT, ALIGN_MIDDLE);
	all_Btn.SetPosition(-8, 0);
	all_Btn.SetImage(&all_BtnImg);
	all_Btn.SetImageOver(&all_BtnImgOver);
	all_Btn.SetTrigger(&trigA);

	GuiButton back(0, 0);
	back.SetTrigger(&trigB);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&sd_Btn);
	promptWindow.Append(&usb_Btn);
	promptWindow.Append(&sd_usb_Btn);
	promptWindow.Append(&dvd_Btn);
	promptWindow.Append(&gca_Btn);
	promptWindow.Append(&gcb_Btn);
	promptWindow.Append(&all_Btn);
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

		if(dvd_Btn.GetState() == STATE_CLICKED)
		{
			device = 4;
			stop = true;
		}

		if(gca_Btn.GetState() == STATE_CLICKED)
		{
			device = 5;
			GCA_Remount();
			stop = true;
		}

		if(gcb_Btn.GetState() == STATE_CLICKED)
		{
			device = 6;
			GCB_Remount();
			stop = true;
		}

		if(all_Btn.GetState() == STATE_CLICKED)
		{
			device = 7;
			GCA_Remount();
			GCB_Remount();
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

