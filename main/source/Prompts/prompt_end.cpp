
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"
#include "menu.h"
#include "Tools/SelectIos.h"
#include "xprintf.h"

extern bool runaway;
extern bool restarthbf;

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

int
endPrompt()
{
	bool stop = false;
	int menu = 0;
	listIOS();

	GuiWindow promptWindow(520,360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiTrigger trigA;
	GuiTrigger trigB;
	GuiTrigger trigHOME;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigB.SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);
	trigHOME.SetButtonOnlyTrigger(-1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, PAD_BUTTON_START);

	GuiText titleTxt(tr("Exit"), 26, (GXColor){Theme.title_1, Theme.title_2, Theme.title_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,40);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

 	// Buttons data
	GuiImageData btn(Theme.button);
	GuiImage systemmenuImg(&btn);
	GuiImage bootmiiImg(&btn);
	GuiImage hbfImg(&btn);
	GuiImage shutdownImg(&btn);

	// Buttons over data
	GuiImageData btn_over(Theme.button_focus);
	GuiImage bootmiiImgOver(&btn_over);
	GuiImage hbfImgOver(&btn_over);
	GuiImage systemmenuImgOver(&btn_over);
	GuiImage shutdownImgOver(&btn_over);
#ifndef VWII
	GuiText bootmiiTxtBoot2(tr("Launch BootMii (Boot2)"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiText bootmiiTxtIOS(tr("Launch BootMii (IOS)"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton bootmii(btn.GetWidth(), btn.GetHeight());
	if(get_bootmii() == 3)
		bootmii.SetLabel(&bootmiiTxtBoot2);
	else
		bootmii.SetLabel(&bootmiiTxtIOS);
	bootmii.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	bootmii.SetPosition(0, 75);
	bootmii.SetImage(&bootmiiImg);
	bootmii.SetImageOver(&bootmiiImgOver);
	bootmii.SetTrigger(&trigA);
#endif
	GuiText hbfTxt(tr("Restart HBF"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton hbf(btn.GetWidth(), btn.GetHeight());
	hbf.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	hbf.SetPosition(0, 90);
	if(get_bootmii() != 0)
		hbf.SetPosition(0, 140);
	hbf.SetLabel(&hbfTxt);
	hbf.SetImage(&hbfImg);
	hbf.SetImageOver(&hbfImgOver);
	hbf.SetTrigger(&trigA);

	GuiText systemmenuTxt(tr("Exit to System Menu"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton systemmenu(btn.GetWidth(), btn.GetHeight());
	systemmenu.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	systemmenu.SetPosition(0, 180);
	if(get_bootmii() != 0)
		systemmenu.SetPosition(0, 205);
	systemmenu.SetLabel(&systemmenuTxt);
	systemmenu.SetImage(&systemmenuImg);
	systemmenu.SetImageOver(&systemmenuImgOver);
	systemmenu.SetTrigger(&trigA);

	GuiText shutdownTxt(tr("Shutdown"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton shutdown(btn.GetWidth(), btn.GetHeight());
	shutdown.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	shutdown.SetPosition(0, 270);
	shutdown.SetLabel(&shutdownTxt);
	shutdown.SetImage(&shutdownImg);
	shutdown.SetImageOver(&shutdownImgOver);
	shutdown.SetTrigger(&trigA);

	GuiButton back(0, 0);
	back.SetTrigger(&trigB);

	GuiButton back2(0, 0);
	back.SetTrigger(&trigHOME);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
#ifndef VWII
	if(get_bootmii() != 0)
		promptWindow.Append(&bootmii);
#endif
	promptWindow.Append(&systemmenu);
	promptWindow.Append(&shutdown);
	promptWindow.Append(&back);
	promptWindow.Append(&back2);
	promptWindow.Append(&hbf);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(!stop)
	{
		usleep(100);
#ifndef VWII
		if(bootmii.GetState() == STATE_CLICKED)
		{
			menu = MENU_EXIT;
			stop = true;
		}
#endif
		if(hbf.GetState() == STATE_CLICKED)
		{
			restarthbf = true;
			menu = MENU_EXIT;
			stop = true;
		}

		if(systemmenu.GetState() == STATE_CLICKED)
		{
			PowerOff = SYS_RETURNTOMENU;
			menu = MENU_EXIT;
			stop = true;
		}

		if(shutdown.GetState() == STATE_CLICKED)
		{
			PowerOff = SYS_POWEROFF_STANDBY;
			menu = MENU_EXIT;
			stop = true;
		}

		if(back.GetState() == STATE_CLICKED || back2.GetState() == STATE_CLICKED)
			stop = true;

		if(runaway == true)
			stop = true;

	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	return menu;
}

