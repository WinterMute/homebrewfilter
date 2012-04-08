
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"
#include "menu.h"
#include "Tools/SelectIos.h"
#include "Neek/boot_neek2o.h"
#include "Neek/uneek_fs.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern bool goneek2o;

int priicheck = 0;

int
loaderPrompt()
{
	bool stop = false;
	int menu = 0;
	listIOS();

	if( ! priicheck )
	{
		priicheck = 1;
		check_priiloader();
	}

	GuiWindow promptWindow(520,360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiTrigger trigA;
	GuiTrigger trigB;
	GuiTrigger trigHOME;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigB.SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);
	trigHOME.SetButtonOnlyTrigger(-1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, PAD_BUTTON_START);

	GuiText titleTxt(tr("External Loaders"), 26, (GXColor){Theme.title_1, Theme.title_2, Theme.title_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,40);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

 	// Buttons data
	GuiImageData btn(Theme.button);
	GuiImage nandemuImg(&btn);
	GuiImage neek2oImg(&btn);
	GuiImage priiloaderImg(&btn);
	GuiImage backImg(&btn);

	// Buttons over data
	GuiImageData btn_over(Theme.button_focus);
	GuiImage nandemuImgOver(&btn_over);
	GuiImage neek2oImgOver(&btn_over);
	GuiImage priiloaderImgOver(&btn_over);
	GuiImage backImgOver(&btn_over);

	GuiText nandemuTxt(tr("Launch Uniiloader"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton nandemu(btn.GetWidth(), btn.GetHeight());
	nandemu.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	nandemu.SetPosition(0, 90);
	nandemu.SetLabel(&nandemuTxt);
	nandemu.SetImage(&nandemuImg);
	nandemu.SetImageOver(&nandemuImgOver);
	nandemu.SetTrigger(&trigA);

	GuiText neek2oTxt(tr("Launch Neek2o"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton neek2o(btn.GetWidth(), btn.GetHeight());
	neek2o.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	neek2o.SetPosition(0, 90);
	if(get_nandemu())
		neek2o.SetPosition(0, 140);
	neek2o.SetLabel(&neek2oTxt);
	neek2o.SetImage(&neek2oImg);
	neek2o.SetImageOver(&neek2oImgOver);
	neek2o.SetTrigger(&trigA);

	GuiText priiloaderTxt(tr("Launch Priiloader"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton priiloader(btn.GetWidth(), btn.GetHeight());
	priiloader.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	priiloader.SetPosition(0, 90);
	if(get_nandemu() && check_neek2o())
		priiloader.SetPosition(0, 190);
	else if (get_nandemu() || check_neek2o())
		priiloader.SetPosition(0, 140);
	priiloader.SetLabel(&priiloaderTxt);
	priiloader.SetImage(&priiloaderImg);
	priiloader.SetImageOver(&priiloaderImgOver);
	priiloader.SetTrigger(&trigA);

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
	if(get_nandemu() && ! check_uneek_fs())
		promptWindow.Append(&nandemu);
	if(check_neek2o())
		promptWindow.Append(&neek2o);
	if(get_priiloader() == 1)
		promptWindow.Append(&priiloader);
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

		if(nandemu.GetState() == STATE_CLICKED)
		{
			set_nandemu(2);
			menu = MENU_EXIT;
			stop = true;
		}

		if(neek2o.GetState() == STATE_CLICKED)
		{
			goneek2o = true;
			menu = MENU_EXIT;
			stop = true;
		}

		if(priiloader.GetState() == STATE_CLICKED)
		{
			set_priiloader(2);
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

