
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"
#include "menu.h"
#include "Tools/SelectIos.h"

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
	GuiImage nandemuImg(&btn);
	GuiImage shutdownImg(&btn);
	GuiImage backImg(&btn);

	// Buttons over data
	GuiImageData btn_over(Theme.button_focus);
	GuiImage bootmiiImgOver(&btn_over);
	GuiImage nandemuImgOver(&btn_over);
	GuiImage systemmenuImgOver(&btn_over);
	GuiImage shutdownImgOver(&btn_over);
	GuiImage backImgOver(&btn_over);

	GuiText bootmiiTxt(tr("Launch BootMii"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton bootmii(btn.GetWidth(), btn.GetHeight());
	bootmii.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	bootmii.SetPosition(0, 75);
	bootmii.SetLabel(&bootmiiTxt);
	bootmii.SetImage(&bootmiiImg);
	bootmii.SetImageOver(&bootmiiImgOver);
	bootmii.SetTrigger(&trigA);

	GuiText nandemuTxt(tr("Launch NandEmu"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton nandemu(btn.GetWidth(), btn.GetHeight());
	nandemu.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	nandemu.SetPosition(0, 75);
	if(get_bootmii())
		nandemu.SetPosition(0, 120);
	nandemu.SetLabel(&nandemuTxt);
	nandemu.SetImage(&nandemuImg);
	nandemu.SetImageOver(&nandemuImgOver);
	nandemu.SetTrigger(&trigA);

	GuiText systemmenuTxt(tr("Exit to System Menu"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton systemmenu(btn.GetWidth(), btn.GetHeight());
	systemmenu.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	systemmenu.SetPosition(0, 90);
	if(get_bootmii() && get_nandemu())
	{
		systemmenu.SetPosition(0, 180);
	}
	else if (get_bootmii() || get_nandemu())
	{
		systemmenu.SetPosition(0, 120);
	}
	systemmenu.SetLabel(&systemmenuTxt);
	systemmenu.SetImage(&systemmenuImg);
	systemmenu.SetImageOver(&systemmenuImgOver);
	systemmenu.SetTrigger(&trigA);

	GuiText shutdownTxt(tr("Shutdown"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton shutdown(btn.GetWidth(), btn.GetHeight());
	shutdown.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	shutdown.SetPosition(0, 180);
	if(get_bootmii() && get_nandemu())
	{
		shutdown.SetPosition(0, 225);
	}
	else if (get_bootmii() || get_nandemu())
	{
		shutdown.SetPosition(0, 205);
	}
	shutdown.SetLabel(&shutdownTxt);
	shutdown.SetImage(&shutdownImg);
	shutdown.SetImageOver(&shutdownImgOver);
	shutdown.SetTrigger(&trigA);

	GuiText backTxt(tr("Back"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton back(btn.GetWidth(), btn.GetHeight());
	back.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	back.SetPosition(0, 270);
	back.SetLabel(&backTxt);
	back.SetImage(&backImg);
	back.SetImageOver(&backImgOver);
	back.SetTrigger(&trigA);
	back.SetTrigger(&trigB);

	GuiButton back2(0, 0);
	back2.SetTrigger(&trigHOME);


	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	if(get_bootmii())
		promptWindow.Append(&bootmii);
	if(get_nandemu())
		promptWindow.Append(&nandemu);
	promptWindow.Append(&systemmenu);
	promptWindow.Append(&shutdown);
	promptWindow.Append(&back);
	promptWindow.Append(&back2);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(!stop)
	{
		usleep(100);

		if(bootmii.GetState() == STATE_CLICKED)
		{
			set_bootmii(2);
			menu = MENU_EXIT;
			stop = true;
		}

		if(nandemu.GetState() == STATE_CLICKED)
		{
			set_nandemu(2);
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
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	return menu;
}

