
#include <unistd.h>
#include <dirent.h>

#include "libwiigui/gui.h"
#include "main.h"

#include "Tools/textline.h"
#include "Tools/save.h"

#include "Network/network.h"
#include "Network/update.h"
#include "Network/http.h"

#/*** Extern variables ***/
extern GuiWindow * mainWindow;

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

void fontDownload(string fontname)
{
	bool stop = true;

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
	
	GuiText titleTxt(tr("Download"), 26, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0, 40);
	GuiText downloadTxt(tr("Downloading file..."), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	downloadTxt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	downloadTxt.SetPosition(0, -20);

	GuiText msgTxt(tr("please wait"), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	msgTxt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	msgTxt.SetPosition(0, 20);

	GuiText btn1Txt(tr("OK"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton btn1(btnOutline.GetWidth(), btnOutline.GetHeight());

	btn1.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	btn1.SetPosition(0, -25);	
	btn1.SetLabel(&btn1Txt);
	btn1.SetImage(&btn1Img);
	btn1.SetImageOver(&btn1ImgOver);
	btn1.SetTrigger(&trigA);
	btn1.SetState(STATE_SELECTED);
	btn1.SetEffectGrow();

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	promptWindow.Append(&downloadTxt);
	promptWindow.Append(&msgTxt);


	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	char buffer[100];
	msgTxt.SetText(fontname.c_str());
	sprintf(buffer, "http://hbf.hamachi-mp.bplaced.net/Fonts/%s", fontname.c_str());	
	struct block file = downloadfile(buffer);
	if (file.data && file.size > 0 && folder_exists())
	{
		FILE * data = fopen((Settings.device_dat + ":/config/Homebrew Filter/Fonts/"+ fontname).c_str(), "wb");
		if(data)
		{
			fwrite(file.data, 1, file.size, data);
			fclose(data);
		}
	}
	if(file.data)
		free(file.data);
		
	msgTxt.SetText("");
	downloadTxt.SetText(tr("finished"));

	promptWindow.Append(&btn1);


	while(stop)
	{
		usleep(100);

		if(btn1.GetState() == STATE_CLICKED)
			stop = false;
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
}

string FontList()
{
	string downloadfont = "error";
	bool stop = false;
	
	char buffer[100];
	sprintf(buffer, "http://hbf.hamachi-mp.bplaced.net/Fonts/");	

	struct block file = downloadfile(buffer);
	if (file.data != NULL)
	{
		string source_fonts = (char*)file.data;
		vector<string> fonts;
		
		while(1)
		{
			if((signed)source_fonts.find("../Fonts/") == -1)
				break;
				
			source_fonts.erase(0, source_fonts.find("../Fonts/"));
			source_fonts.erase(0, source_fonts.find("s/") +2);
			
			fonts.push_back(source_fonts.substr(0, source_fonts.find("\"")));
			
			source_fonts.erase(0, source_fonts.find("<"));
		}
		
		free(file.data);

		GuiText titleTxt(tr("Download"), 26, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
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
		int textScrollPos = 0;
		int selctionPos = y;
		
		GuiText selectionTxt(">>                                                                    <<", 20, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
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
		
		for(i=0; i < number && i < (signed)fonts.size(); i++)
		{
			Entrie[i] = new GuiText(fonts[i].c_str(), 20, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
			Entrie[i]->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
			Entrie[i]->SetPosition(0, y);
			Entrie[i]->SetMaxWidth(300, SCROLL_HORIZONTAL);
			y += place;
		}

		promptWindow.Append(&dialogBoxImg);
		promptWindow.Append(&titleTxt);
		promptWindow.Append(&selectionTxt);
		
		for(int x=0; x < i; x++)
			promptWindow.Append(Entrie[x]);
		
		if((signed)fonts.size() >= number)
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
			
			if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_UP | WPAD_CLASSIC_BUTTON_UP) || PAD_ButtonsDown(0) & PAD_BUTTON_UP)
			{
				selection--;
				if(selection < 0)
				{
					selection = 0;
					textScrollPos--;
					if(textScrollPos < 0)
						textScrollPos = 0;
						
					for(int x=0; x < number && x < (signed)fonts.size(); x++)
						Entrie[x]->SetText(fonts[x + textScrollPos].c_str());
				}
				selectionTxt.SetPosition(0, selection * place + selctionPos);
			
				HaltResumeGui();
			}
			
			if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN) || PAD_ButtonsDown(0) & PAD_BUTTON_DOWN)
			{
				selection++;
				if(selection == (signed)fonts.size())
					selection = fonts.size() -1;
				if(selection > number -1)
				{
					selection = number -1;
					textScrollPos++;
					if(textScrollPos > (signed)fonts.size() - number)
						textScrollPos = fonts.size() - number;
					
					for(int x=0; x < number && x < (signed)fonts.size(); x++)
						Entrie[x]->SetText(fonts[x + textScrollPos].c_str());
				}
				selectionTxt.SetPosition(0, selection * place + selctionPos);
				
				HaltResumeGui();
			}

			if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A) || PAD_ButtonsDown(0) & PAD_BUTTON_A)
			{
				downloadfont = fonts[selection + textScrollPos];
				stop = true;
			}
				
			if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B) || PAD_ButtonsDown(0) & PAD_BUTTON_B)
			{
				downloadfont = "NULL";
				stop = true;
			}
		}

		promptWindow.SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_OUT, 50);
		while(promptWindow.GetEffect() > 0) usleep(100);
		HaltGui();
		mainWindow->Remove(&promptWindow);
		mainWindow->SetState(STATE_DEFAULT);
		ResumeGui();
	}
	
	return downloadfont;
}

string checkFontsPrompt()
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

	GuiText titleTxt(tr("Download"), 26, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
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

	string downloadfont = "NULL";
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
		downloadfont = FontList();
		if(downloadfont == "error")
		{
			downloadfont = "NULL";
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
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	
	return downloadfont;
}