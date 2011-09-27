
#include <unistd.h>
#include <algorithm>

#include "libwiigui/gui.h"
#include "main.h"
#include "menu.h"
#include "Prompts/prompts.h"

#include "Network/update.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();


/****************************************************************************
 * MenuSettings
 ***************************************************************************/
int MenuSettingsChildlock()
{
	int menu = MENU_NONE;

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	GuiTrigger trigHome;
	trigHome.SetButtonOnlyTrigger(-1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, PAD_BUTTON_START);

	GuiText titleTxt(tr("Childlock"), 30, (GXColor){Theme.title_1, Theme.title_2, Theme.title_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,30);

	// normale Buttons
	GuiImageData btn(Theme.button);
	GuiImage btn1Img(&btn);
	GuiImage btn2Img(&btn);
	GuiImage backBtnImg(&btn);
	
	// normale Buttons over
	GuiImageData btn_over(Theme.button_focus);
	GuiImage btn1ImgOver(&btn_over);
	GuiImage btn2ImgOver(&btn_over);
	GuiImage backBtnImgOver(&btn_over);
	
	GuiText btn1Txt(tr("Activate"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton btn1(btn.GetWidth(), btn.GetHeight());
	btn1.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	btn1.SetPosition(0, 160);
	btn1.SetLabel(&btn1Txt);
	btn1.SetImage(&btn1Img);
	btn1.SetImageOver(&btn1ImgOver);
	btn1.SetTrigger(&trigA);
	if(strcasecmp(Options.temp_code,"NULL") != 0 )
	{
		btn1Txt.SetText(tr("Deactivate"));
		btn1.SetPosition(0, 110);
	}

	GuiText btn2Txt(tr("Change Code"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton btn2(btn.GetWidth(), btn.GetHeight());
	btn2.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	btn2.SetPosition(0, 210);
	btn2.SetLabel(&btn2Txt);
	btn2.SetImage(&btn2Img);
	btn2.SetImageOver(&btn2ImgOver);
	btn2.SetTrigger(&trigA);

	GuiText backBtnTxt(tr("Back"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton backBtn(btn.GetWidth(), btn.GetHeight());
	backBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	backBtn.SetPosition(0, 260);
	if(strcasecmp(Options.temp_code,"NULL") != 0 )
		backBtn.SetPosition(0, 310);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetTrigger(&trigHome);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&titleTxt);
	w.Append(&btn1);
	if(strcasecmp(Options.temp_code,"NULL") != 0 )
		w.Append(&btn2);
	w.Append(&backBtn);

	mainWindow->Append(&w);

	ResumeGui();

	while(menu == MENU_NONE)
	{
		usleep(100);

		if(btn1.GetState() == STATE_CLICKED)
		{
			btn1.ResetState();
			char buffer[5];
			if(strcasecmp(Options.temp_code,"NULL") != 0 )
			{
				while(1)
				{
					for(int i = 0; i < (signed)strlen(buffer); i++)
						buffer[i] = '\0';
						
					OnScreenCodeboard(buffer, 4);
					// wenn eingabe richtig
					if(strcasecmp(buffer, Options.temp_code) == 0 )
					{
						sprintf (Options.temp_code, "NULL");
						break;
					}
					// wenn eingabe abgebrochen
					else if(strcasecmp(buffer, "NULL") == 0 )
						break;
					// wenn eingabe falsch
					else
						WindowPrompt(NULL, tr("Error"), tr("Back"));
				}
			}
			else
			{
				// neuen code eingeben
				buffer[0] = '\0';
				OnScreenCodeboard(buffer, 4);
				if(strcasecmp(buffer,"NULL") != 0 )
					sprintf (Options.temp_code, buffer);
			}
			
			menu = MENU_SETTINGS_CHILDLOCK;
		}
		else if(btn2.GetState() == STATE_CLICKED)
		{
			btn2.ResetState();
			char buffer[5];
			while(1)
			{
				for(int i = 0; i < (signed)strlen(buffer); i++)
					buffer[i] = '\0';
					
				// alten code eingeben
				WindowPrompt(tr("Info"), tr("Old Code"), tr("Back"));
				OnScreenCodeboard(buffer, 4);
				// wenn eingabe richtig
				if(strcasecmp(buffer, Options.temp_code) == 0 )
				{
					for(int i = 0; i < (signed)strlen(buffer); i++)
						buffer[i] = '\0';
					// neuen code eingeben
					WindowPrompt(tr("Info"), tr("New Code"), tr("Back"));
					OnScreenCodeboard(buffer, 4);
					if(strcasecmp(buffer,"NULL") != 0 )
					{
						sprintf (Options.temp_code, buffer);
						break;
					}
					// wenn eingabe abgebrochen
					else
						break;;
				}
				// wenn eingabe abgebrochen
				else if(strcasecmp(buffer, "NULL") == 0 )
					break;
				// wenn eingabe falsch
				else
					WindowPrompt(NULL, tr("Error"), tr("Back"));
			}
		}
		else if(backBtn.GetState() == STATE_CLICKED)
		{
			Options.temp_last_setting = 1;
			menu = MENU_SETTINGS_FILE;
		}
	}

	HaltGui();
	mainWindow->Remove(&w);
	return menu;
}

