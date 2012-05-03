
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

/****************************************************************************
 * App entfernen
 ***************************************************************************/
bool eraseDir(const char *AppName)
{
	bool stop = true;
	bool auswahl = false;

	GuiWindow promptWindow(520,360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

	GuiText zeile1Txt(AppName, 24, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	zeile1Txt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	zeile1Txt.SetPosition(0,-100);

	GuiText zeile2Txt(tr("Delete Forever?"), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	zeile2Txt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	zeile2Txt.SetPosition(0,-10);

	// image
	GuiImageData btn(Theme.button_small);
	GuiImage btn1Img(&btn);
	GuiImage btn2Img(&btn);

	// image over
	GuiImageData btn_over(Theme.button_small_focus);
	GuiImage btn1ImgOver(&btn_over);
	GuiImage btn2ImgOver(&btn_over);

	// papierkorb
	GuiImageData recycle_bin_data(Theme.recycle_bin);
	GuiImage recycle_bin(&recycle_bin_data);

	// löschen
	GuiButton btn1(btn.GetWidth(), btn.GetHeight());
	GuiText btn1Txt(tr("Delete"), 22, (GXColor){Theme.button_tiny_text_1, Theme.button_tiny_text_2, Theme.button_tiny_text_3, 255});
	btn1Txt.SetPosition(recycle_bin_data.GetWidth()/2, 0);
	recycle_bin.SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
	recycle_bin.SetPosition(10, 0);
	btn1.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	btn1.SetPosition(50, -45);
	btn1.SetIcon(&recycle_bin);
	btn1.SetLabel(&btn1Txt);
	btn1.SetImage(&btn1Img);
	btn1.SetImageOver(&btn1ImgOver);
	btn1.SetTrigger(&trigA);

	// abbrechen
	GuiButton btn2(btn.GetWidth(), btn.GetHeight());
	GuiText btn2Txt(tr("Stop"), 22, (GXColor){Theme.button_tiny_text_1, Theme.button_tiny_text_2, Theme.button_tiny_text_3, 255});
	btn2.SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	btn2.SetPosition(-50, -45);
	btn2.SetLabel(&btn2Txt);
	btn2.SetImage(&btn2Img);
	btn2.SetImageOver(&btn2ImgOver);
	btn2.SetTrigger(&trigA);
	btn2.SetState(STATE_SELECTED);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&zeile1Txt);
	promptWindow.Append(&zeile2Txt);
	promptWindow.Append(&btn1);
	promptWindow.Append(&btn2);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(stop)
	{
		usleep(100);

		if(btn1.GetState() == STATE_CLICKED)
		{
			auswahl = true;
			stop = false;
		}

		if(btn2.GetState() == STATE_CLICKED)
			stop = false;
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();

	return auswahl;
}

