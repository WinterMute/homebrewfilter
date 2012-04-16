
#include <unistd.h>
#include <dirent.h>

#include "libwiigui/gui.h"
#include "main.h"

#include "BootHomebrew/BootHomebrew.h"

#include "Tools/save.h"
#include "Tools/textline.h"

#include "Network/network.h"
#include "Network/update.h"
#include "Network/http.h"

#include <fstream>
#include <ios>

/*** Extern variables ***/
extern GuiWindow * mainWindow;
extern bool boot_buffer;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern void HaltResumeGui();

/****************************************************************************
 * WindowPrompt
 *
 * Displays a prompt window to user, with information, an error message, or
 * presenting a user with a choice
 ***************************************************************************/
void
updatePrompt(string rev)
{
//	bool stop = true;

	GuiWindow promptWindow(520,360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

	GuiImageData btnOutline(Theme.button_small);
	GuiImage btn1Img(&btnOutline);
	GuiImage btn2Img(&btnOutline);
	
	GuiImageData btnOutlineOver(Theme.button_small_focus);
	GuiImage btn1ImgOver(&btnOutlineOver);
	GuiImage btn2ImgOver(&btnOutlineOver);
	
	GuiText titleTxt(tr("Update"), 26, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0, 40);
	GuiText downloadTxt(tr("Downloading file..."), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	downloadTxt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	downloadTxt.SetPosition(0, -20);

	GuiText msgTxt(tr("please wait"), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	msgTxt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	msgTxt.SetPosition(0, 20);

	GuiText btn1Txt(tr("Yes"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton btn1(btnOutline.GetWidth(), btnOutline.GetHeight());

	btn1.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	btn1.SetPosition(20, -25);	
	btn1.SetLabel(&btn1Txt);
	btn1.SetImage(&btn1Img);
	btn1.SetImageOver(&btn1ImgOver);
	btn1.SetTrigger(&trigA);
	btn1.SetState(STATE_SELECTED);
	btn1.SetEffectGrow();

	GuiText btn2Txt(tr("No"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton btn2(btnOutline.GetWidth(), btnOutline.GetHeight());

	btn2.SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	btn2.SetPosition(-20, -25);	
	btn2.SetLabel(&btn2Txt);
	btn2.SetImage(&btn2Img);
	btn2.SetImageOver(&btn2ImgOver);
	btn2.SetTrigger(&trigA);
	btn2.SetEffectGrow();

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	promptWindow.Append(&downloadTxt);
	promptWindow.Append(&msgTxt);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	char url[100];
	if(rev == "Beta")
		sprintf(url, "http://www.nanolx.org/hbf/DOL/Beta/boot.dol");
	else
		sprintf(url, "http://www.nanolx.org/hbf/DOL/rev%s/boot.dol", rev.c_str());

	// copy boot.dol to prev.dol
	std::ifstream infile((Settings.device_dat + ":/apps/HomebrewFilter/boot.dol").c_str(), std::ios_base::binary);
	std::ofstream outfile((Settings.device_dat + ":/apps/HomebrewFilter/prev.dol").c_str(), std::ios_base::binary);

	outfile << infile.rdbuf();

	struct block file = downloadfile(url);
	if (file.data && file.size > 0)
	{
		// write file
		FILE * data = fopen((Settings.device_dat + ":/apps/HomebrewFilter/boot.dol").c_str(), "wb");
		if(data)
		{
			fwrite(file.data, 1, file.size, data);
			fclose(data);
		}

		if(file.data)
			free(file.data);

		LoadHomebrew ((Settings.device_dat + ":/apps/HomebrewFilter/boot.dol").c_str());
		BootHomebrew ();
	}
	else
	{
		if(file.data)
			free(file.data);
	}
	
	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
}

string choiceRev(string revs)
{
	bool stop = false;
	string rev = "NULL";
	
	TextLine versions;
	versions.list(revs);

	GuiText titleTxt(tr("Update"), 26, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0, 40);

	GuiWindow promptWindow(520,360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

	int place = 23;
	int y = 150;
	int i = 0;
	int number = 5;
	int selection = 0;
	int selctionPos = y;
	
	GuiText selectionTxt(">>                              <<", 20, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	selectionTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	selectionTxt.SetPosition(0, y);

	GuiText upTxt("c", 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	upTxt.SetFont(symbol_ttf, symbol_ttf_size);
	upTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	upTxt.SetPosition(0, y -20);
	
	GuiText downTxt("d", 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	downTxt.SetFont(symbol_ttf, symbol_ttf_size);
	downTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	downTxt.SetPosition(0, y + (place * (number-1)) + 15);
	
	GuiText * Entrie[number];
	
	for(i=0; i < number && i < (signed)versions.line.size(); i++)
	{
		Entrie[i] = new GuiText(versions.line[i].c_str(), 20, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
		Entrie[i]->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
		Entrie[i]->SetPosition(0, y);
		y += place;
	}

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	promptWindow.Append(&selectionTxt);
	
	for(int x=0; x < i; x++)
		promptWindow.Append(Entrie[x]);
	
	if((signed)versions.line.size() >= number)
	{
		promptWindow.Append(&upTxt);
		promptWindow.Append(&downTxt);
	}

	promptWindow.SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_IN, 50);
	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(!stop)
	{
		usleep(100);
		
		if(WPAD_ButtonsHeld(0) & (WPAD_BUTTON_UP | WPAD_CLASSIC_BUTTON_UP) || PAD_ButtonsDown(0) & PAD_BUTTON_UP)
		{
		
			selection--;
			if(selection < 0)
			{
				selection = 0;
				
				int z = versions.text_up();
				for(int x=0; x < i; x++)
					Entrie[x]->SetText(versions.line[x + z].c_str());
			}
			selectionTxt.SetPosition(0, selection * place + selctionPos);
			
			HaltResumeGui();
			usleep(100000);
		}
		
		if(WPAD_ButtonsHeld(0) & (WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN) || PAD_ButtonsDown(0) & PAD_BUTTON_DOWN)
		{
			
			selection++;
			if(selection > number -1)
			{
				selection = number -1;
				
				int z = versions.text_down(number);
				for(int x=0; x < i; x++)
					Entrie[x]->SetText(versions.line[x + z].c_str());
			}
			selectionTxt.SetPosition(0, selection * place + selctionPos);
			
			HaltResumeGui();
			usleep(100000);
		}

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A) || PAD_ButtonsDown(0) & PAD_BUTTON_A)
		{
			rev = versions.line[selection + versions.textScrollPos];
			stop = true;
		}
			
		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B) || PAD_ButtonsDown(0) & PAD_BUTTON_B)
			stop = true;
	}

	promptWindow.SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_OUT, 50);
	while(promptWindow.GetEffect() > 0) usleep(100);
	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	return rev;
}

string checkUpdatePrompt()
{
	GuiWindow promptWindow(520,360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

	GuiImageData btnOutline(Theme.button_small);
	GuiImage btn1Img(&btnOutline);
	
	GuiImageData btnOutlineOver(Theme.button_small_focus);
	GuiImage btn1ImgOver(&btnOutlineOver);
	
	// ok button
	GuiText backTxt(tr("OK"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiImage backImg(&btnOutline);
	GuiImage backImgOver(&btnOutlineOver);
	GuiButton back(btnOutline.GetWidth(), btnOutline.GetHeight());
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	
	back.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	back.SetPosition(0, -25);
	back.SetLabel(&backTxt);
	back.SetImage(&backImg);
	back.SetImageOver(&backImgOver);
	back.SetTrigger(&trigA);
	back.SetState(STATE_SELECTED);

	GuiText titleTxt(tr("Update"), 26, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0, 40);
	GuiText msgTxt(tr("Initialise network..."), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	msgTxt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	promptWindow.Append(&msgTxt);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	string rev = "NULL";
	// überprüfen, ob netzwerk initialisiert wird
	Initialize_Network();
	if(!IsNetworkInit())
	{
		msgTxt.SetText(tr("No network connection"));
		bool stop = false;
		
		promptWindow.Append(&back);
		while(!stop)
		{
			usleep(100);
				
			if(back.GetState() == STATE_CLICKED)
				stop = true;
		}
		promptWindow.Remove(&back);
	}
	else
	{
		string revs = CheckNewVersions();
		if(revs == "error")
		{
			msgTxt.SetText(tr("Error while reading file"));
			bool stop = false;
			
			promptWindow.Append(&back);
			while(!stop)
			{
				usleep(100);
					
				if(back.GetState() == STATE_CLICKED)
					stop = true;
			}
			promptWindow.Remove(&back);
		}
		else
			rev = choiceRev(revs);
	}
	
	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();

	return rev;
}
