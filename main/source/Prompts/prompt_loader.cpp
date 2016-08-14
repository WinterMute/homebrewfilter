
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"
#include "menu.h"
#include "Tools/SelectIos.h"
#include <dirent.h>
// #include "Neek/bootneek.h"
#include "Neek/uneek_fs.h"
#include "BootHomebrew/BootHomebrew.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern bool goneek2o;
extern bool gorealnand;
extern bool gosegui;
extern const char* segui_loc;

int priicheck = 0;
int seguicheck = 0;

bool check_segui()
{

	if (opendir(check_path("sd1:/apps/SettingsEditorGUI/").c_str()) != NULL)
	{
		segui_loc = "sd1:/apps/SettingsEditorGUI/boot.dol";
		return true;
	}
	else if(opendir(check_path("usb1:/apps/SettingsEditorGUI/").c_str()) != NULL)
	{
		segui_loc = "usb1:/apps/SettingsEditorGUI/boot.dol";
		return true;
	}
#ifndef VWII
	else if (opendir(check_path("dvd:/apps/SettingsEditorGUI/").c_str()) != NULL)
	{
		segui_loc = "dvd1:/apps/SettingsEditorGUI/boot.dol";
		return true;
	}
	else if(opendir(check_path("gca:/apps/SettingsEditorGUI/").c_str()) != NULL)
	{
		segui_loc = "gca:/apps/SettingsEditorGUI/boot.dol";
		return true;
	}
	else if(opendir(check_path("gcb:/apps/SettingsEditorGUI/").c_str()) != NULL)
	{
		segui_loc = "gcb:/apps/SettingsEditorGUI/boot.dol";
		return true;
	}
#endif
	else
	{
		segui_loc = "";
		return false;
	}

}

int set_height(int num)
{
	if(num == 1)
		return 75;
	if(num == 2)
		return 140;
	if(num == 3)
		return 205;
	if(num == 4)
		return 270;

	return 0;

}

int
loaderPrompt()
{
	bool stop = false;
	int menu = 0;

	listIOS();
#ifndef VWII
	if( ! priicheck )
	{
		priicheck = 1;
		check_priiloader();
	}
#endif
	bool have_segui;
	have_segui = false;
	if( ! seguicheck )
	{
		seguicheck = 1;
		have_segui = check_segui();
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

	GuiText titleTxt(tr("External Applications"), 26, (GXColor){Theme.title_1, Theme.title_2, Theme.title_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,40);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

 	// Buttons data
	GuiImageData btn(Theme.button);
#ifndef VWII
	GuiImage nandemuImg(&btn);
	GuiImage neek2oImg(&btn);
	GuiImage realnandImg(&btn);
	GuiImage priiloaderImg(&btn);
#endif
	GuiImage seguiImg(&btn);

	// Buttons over data
	GuiImageData btn_over(Theme.button_focus);
#ifndef VWII
	GuiImage nandemuImgOver(&btn_over);
	GuiImage neek2oImgOver(&btn_over);
	GuiImage realnandImgOver(&btn_over);
	GuiImage priiloaderImgOver(&btn_over);
#endif
	GuiImage seguiImgOver(&btn_over);

#ifndef VWII
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
	neek2o.SetLabel(&neek2oTxt);
	neek2o.SetImage(&neek2oImg);
	neek2o.SetImageOver(&neek2oImgOver);
	neek2o.SetTrigger(&trigA);

	GuiText realnandTxt(tr("Enter real NAND"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton realnand(btn.GetWidth(), btn.GetHeight());
	realnand.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	realnand.SetPosition(0, 90);
	realnand.SetLabel(&realnandTxt);
	realnand.SetImage(&realnandImg);
	realnand.SetImageOver(&realnandImgOver);
	realnand.SetTrigger(&trigA);

	GuiText priiloaderTxt(tr("Launch Priiloader"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton priiloader(btn.GetWidth(), btn.GetHeight());
	priiloader.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	priiloader.SetPosition(0, 90);
	priiloader.SetLabel(&priiloaderTxt);
	priiloader.SetImage(&priiloaderImg);
	priiloader.SetImageOver(&priiloaderImgOver);
	priiloader.SetTrigger(&trigA);
#endif

	GuiText seguiTxt(tr("Launch Settings Editor GUI"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton segui(btn.GetWidth(), btn.GetHeight());
	segui.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	segui.SetPosition(0, 90);
	segui.SetLabel(&seguiTxt);
	segui.SetImage(&seguiImg);
	segui.SetImageOver(&seguiImgOver);
	segui.SetTrigger(&trigA);

	GuiButton back(0, 0);
	back.SetTrigger(&trigB);
	back.SetTrigger(&trigHOME);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);

	int num_btns = 1;
#ifndef VWII
	if(!check_uneek_fs())
	{
		if(get_nandemu())
		{
			nandemu.SetPosition(0, set_height(num_btns));
			num_btns++;
			promptWindow.Append(&nandemu);
		}

		if(check_neek2o())
		{
			neek2o.SetPosition(0, set_height(num_btns));
			num_btns++;
			promptWindow.Append(&neek2o);
		}

		if(get_priiloader() > 0)
		{
			priiloader.SetPosition(0, set_height(num_btns));
			num_btns++;
			promptWindow.Append(&priiloader);
		}
#endif
		if(have_segui)
		{
			segui.SetPosition(0, set_height(num_btns));
			num_btns++;
			promptWindow.Append(&segui);
		}
#ifndef VWII
	}
	else
	{
		realnand.SetPosition(0, set_height(num_btns));
		num_btns++;
		promptWindow.Append(&realnand);

		if(get_priiloader() > 0)
		{
			priiloader.SetPosition(0, set_height(num_btns));
			num_btns++;
			promptWindow.Append(&priiloader);
		}

		if(have_segui)
		{
			segui.SetPosition(0, set_height(num_btns));
			num_btns++;
			promptWindow.Append(&segui);
		}

	}
#endif

	promptWindow.Append(&back);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(!stop)
	{
		usleep(100);
#ifndef VWII
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

		if(realnand.GetState() == STATE_CLICKED)
		{
			gorealnand = true;
			menu = MENU_EXIT;
			stop = true;
		}
#endif
		if(segui.GetState() == STATE_CLICKED)
		{
			gosegui = true;
			menu = MENU_EXIT;
			stop = true;
		}
#ifndef VWII
		if(priiloader.GetState() == STATE_CLICKED)
		{
			set_priiloader(2);
			menu = MENU_EXIT;
			stop = true;
		}
#endif
		if(back.GetState() == STATE_CLICKED)
			stop = true;
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	return menu;
}

