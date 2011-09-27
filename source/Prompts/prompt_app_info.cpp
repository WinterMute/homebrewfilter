
#include <unistd.h>
#include <fstream>

#include "libwiigui/gui.h"
#include "main.h"
#include "Tools/parser.h"
#include "Tools/textline.h"
#include "Tools/load_channel.h"
#include "Tools/SelectIos.h"
#include "Prompts/prompts.h"
/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern void HaltResumeGui();

/****************************************************************************
 * AppInfo
 *
 * Infos zum App
 ***************************************************************************/
int
AppInfo(const char *title, string dir, u8* icon)
{
	int choice = -1;

	if(dir != "the homebrew channel")
		dir += "meta.xml";

	string line, quelltext = "", version, coder, descriptionTxt;
	
	if(strcasecmp(title, "the homebrew channel") == 0)
	{
		string space = "  ";
		version = tr("Version:") + space + HBC_version;
		coder = tr("Coder:") + space + "HBC Team";
		descriptionTxt = "The Homebrew Channel is a channel for launching Wii homebrew applications. It will list homebrew applications stored and organised on an SD card or USB mass storage device in a nice little GUI, which you can very easily customise with descriptions and shiny little icons all by yourself. You can also launch applications via TCP (with a correctly configured PC) or a USBGecko. Both of those built in options make it extremely convenient for testing out new code, as well as a general purpose homebrew launcher.";
	}
	else
	{
		ifstream in(dir.c_str());
		while(getline(in, line))
			quelltext = quelltext + line + "\n";
		
		string space = "  ";
		version = tr("Version:") + space + parser(quelltext, "<version>", "</version>");
		coder = tr("Coder:") + space + parser(quelltext, "<coder>", "</coder>");
		descriptionTxt = parser(quelltext, "<long_description>", "</long_description>");
	}
	
	GuiWindow promptWindow(520, 360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	GuiTrigger trigB;
	trigB.SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

	// Symbole
    GuiImageData icon_data((const u8*)icon);
    GuiImage viewicon(&icon_data);
	viewicon.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	viewicon.SetPosition(40, 54);
	
	GuiImage * viewdevice = NULL;
	
	if((Options.device_icon == 2 || Options.device_icon == 3) && Settings.device == "sd_usb")
	{
		bool icon = false;
		if(strncmp(dir.c_str(), "sd", 2) == 0)
		{
			viewdevice = new GuiImage(new GuiImageData(Theme.sd_inactive));
			icon = true;
		}
		else if(strncmp(dir.c_str(), "usb", 3) == 0)
		{
			viewdevice = new GuiImage(new GuiImageData(Theme.usb_inactive));
			icon = true;
		}
		
		if(icon)
		{
			viewdevice->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
			viewdevice->SetPosition(10, 10);
			viewdevice->SetScale(0.8);
		}
	}
	
	GuiText titleTxt(title, 26, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0, 20);
	GuiText versionTxt(version.c_str(), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	versionTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	versionTxt.SetPosition(200, 60);
	versionTxt.SetMaxWidth(300, DOTTED);
	GuiText coderTxt(coder.c_str(), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	coderTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	coderTxt.SetPosition(200, 80);
	coderTxt.SetMaxWidth(300, DOTTED);

	TextLine description;
	description.text(descriptionTxt.c_str(), 18, 400);

    int i = 0;
    int y = 125;
	int place = 25;
	
	int number = 5;
	
	GuiText upTxt("c", 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	upTxt.SetFont(symbol_ttf, symbol_ttf_size);
	upTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	upTxt.SetPosition(0, y -20);
	
	GuiText downTxt("d", 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	downTxt.SetFont(symbol_ttf, symbol_ttf_size);
	downTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	downTxt.SetPosition(0, y + (place * (number-1)) + 15);
	
	GuiText * Entrie[number];
	for(i=0; i < number && i < (signed)description.line.size(); i++)
	{
		Entrie[i] = new GuiText(description.line[i].c_str(), 18, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
		Entrie[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
		Entrie[i]->SetPosition(50, y);
		y += place;
	}

	// image
	GuiImageData cancelBtn(Theme.cancel_inactive);
	GuiImage cancelBtnImg(&cancelBtn);
	GuiImageData editBtn(Theme.edit_inactive);
	GuiImage editBtnImg(&editBtn);
	GuiImageData button_square_Btn(Theme.button_square);
	GuiImage button_square_BtnImg(&button_square_Btn);
	GuiImageData btn(Theme.button_tiny);
	GuiImage btn1Img(&btn);
	GuiImage btn2Img(&btn);
	GuiImage eraseImg(&btn);
	
	// image over
	GuiImageData cancelBtn_over(Theme.cancel_active);
	GuiImage cancelBtnImgOver(&cancelBtn_over);
	GuiImageData editBtn_over(Theme.edit_active);
	GuiImage editBtnImgOver(&editBtn_over);
	GuiImageData button_square_Btn_over(Theme.button_square_focus);
	GuiImage button_square_BtnImgOver(&button_square_Btn_over);
	GuiImageData btn_over(Theme.button_tiny_focus);
	GuiImage btn1ImgOver(&btn_over);
	GuiImage btn2ImgOver(&btn_over);
	GuiImage eraseImgOver(&btn_over);
	
	// Edit
	GuiButton edit(editBtn.GetWidth(), editBtn.GetHeight());
	edit.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	edit.SetPosition(10, 10);
	edit.SetImage(&editBtnImg);
	edit.SetImageOver(&editBtnImgOver);
	edit.SetTrigger(&trigA);
	
	// Cancel
	GuiButton cancel(cancelBtn.GetWidth(), cancelBtn.GetHeight());
	cancel.SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	cancel.SetPosition(-10, 10);
	cancel.SetImage(&cancelBtnImg);
	cancel.SetImageOver(&cancelBtnImgOver);
	cancel.SetTrigger(&trigA);
	cancel.SetTrigger(&trigB);
//	cancel.SetState(STATE_SELECTED);
	
	// Laden
	GuiButton btn1(btn.GetWidth(), btn.GetHeight());
	GuiText btn1Txt(tr("Loading"), 22, (GXColor){Theme.button_tiny_text_1, Theme.button_tiny_text_2, Theme.button_tiny_text_3, 255});
	if(AvailableCategory.categories.size() == 1 || strcasecmp(Settings.code,"NULL") != 0)
	{
		btn1.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
		btn1.SetPosition(0, -45);
	}
	else
	{
		btn1.SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
		btn1.SetPosition(-50, -45);
	}
	btn1.SetLabel(&btn1Txt);
	btn1.SetImage(&btn1Img);
	btn1.SetImageOver(&btn1ImgOver);
	btn1.SetTrigger(&trigA);

	// Einfügen / Löschen
	GuiButton btn2(btn.GetWidth(), btn.GetHeight());
	GuiText btn2Txt(tr("Remove"), 22, (GXColor){Theme.button_tiny_text_1, Theme.button_tiny_text_2, Theme.button_tiny_text_3, 255});
	if(Settings.current_category == 0)
		btn2Txt.SetText(tr("Insert"));
	btn2.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	btn2.SetPosition(50, -45);
	btn2.SetLabel(&btn2Txt);
	btn2.SetImage(&btn2Img);
	btn2.SetImageOver(&btn2ImgOver);
	btn2.SetTrigger(&trigA);

	// Löschen
	GuiButton erase(button_square_Btn.GetWidth(), button_square_Btn.GetHeight());
	// papierkorb
	GuiImageData recycle_bin_data(Theme.recycle_bin);
	GuiImage recycle_bin(&recycle_bin_data);
	recycle_bin.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	if(AvailableCategory.categories.size() == 1)
	{
		erase.SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
		erase.SetPosition(-50, -45);
	}
	else
	{
		erase.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
		erase.SetPosition(0, -45);
	}
	erase.SetIcon(&recycle_bin);
	erase.SetImage(&button_square_BtnImg);
	erase.SetImageOver(&button_square_BtnImgOver);
	erase.SetTrigger(&trigA);

	char SelectIos[50];
	string startingAppName = dir;
	if((signed)startingAppName.rfind("/") != -1)
		startingAppName.erase(startingAppName.rfind("/"));
	startingAppName.erase(0, startingAppName.rfind("/") +1);
	sprintf(SelectIos, tr("Start with IOS %i   (-/+)"), SearchAppIOS(startingAppName));
	
	GuiText SelectIosTxt(SelectIos, 18, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	SelectIosTxt.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	SelectIosTxt.SetPosition(0, -15);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	promptWindow.Append(&versionTxt);
	promptWindow.Append(&coderTxt);
	promptWindow.Append(viewdevice);
	
	for(int x=0; x < i; x++)
		promptWindow.Append(Entrie[x]);
	
	if((signed)description.line.size() >= number)
	{
		promptWindow.Append(&upTxt);
		promptWindow.Append(&downTxt);
	}
	
	if(quelltext != "")
		promptWindow.Append(&edit);
	promptWindow.Append(&btn1);
	if(strcasecmp(title, "the homebrew channel") != 0 && strcasecmp(Settings.code,"NULL") == 0)
		promptWindow.Append(&erase);
	if(AvailableCategory.categories.size() != 1 && strcasecmp(Settings.code,"NULL") == 0)
		promptWindow.Append(&btn2);
	promptWindow.Append(&viewicon);
	promptWindow.Append(&SelectIosTxt);
	promptWindow.Append(&cancel);

	promptWindow.SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_IN, 50);
	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	edit.ResetState();
	cancel.SetState(STATE_SELECTED);
	while(choice == -1)
	{
		usleep(100);

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_UP | WPAD_CLASSIC_BUTTON_UP) || PAD_ButtonsDown(0) & PAD_BUTTON_UP)
		{
			int z = description.text_up();
			
			for(int x=0; x < i; x++)
				Entrie[x]->SetText(description.line[x + z].c_str());
		

			HaltResumeGui();
		}
		
		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN) || PAD_ButtonsDown(0) & PAD_BUTTON_DOWN)
		{
			int z = description.text_down(number);

			for(int x=0; x < i; x++)
				Entrie[x]->SetText(description.line[x + z].c_str());


			HaltResumeGui();
		}

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS) || PAD_ButtonsDown(0) & PAD_TRIGGER_R)
		{
			sprintf(SelectIos, tr("Start with IOS %i   (-/+)"), nextIos());
			SelectIosTxt.SetText(SelectIos);
			HaltResumeGui();
		}
		
		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS) || PAD_ButtonsDown(0) & PAD_TRIGGER_L)
		{
			sprintf(SelectIos, tr("Start with IOS %i   (-/+)"), previousIos());
			SelectIosTxt.SetText(SelectIos);
			HaltResumeGui();
		}
		

		if(cancel.GetState() == STATE_CLICKED)			// Zurück
			choice = 0;
		else if(btn1.GetState() == STATE_CLICKED)		// Laden
		{
			Settings.startingAppName = startingAppName;
			choice = 1;
		}
		else if(btn2.GetState() == STATE_CLICKED)		// Einfügen / Entfernen
		{
			if(Settings.current_category == 0)			// Einfügen
				choice = 2;
			else										// Entfernen
				choice = 3;
		}
		else if(erase.GetState() == STATE_CLICKED)	 	// Löschen
			choice = 4;
		else if(edit.GetState() == STATE_CLICKED)		// Bearbeiten
			choice = 5;
	}

	promptWindow.SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_OUT, 50);
	while(promptWindow.GetEffect() > 0) usleep(100);
	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	return choice;
}

