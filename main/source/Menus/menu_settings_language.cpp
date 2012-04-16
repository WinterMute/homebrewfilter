
#include <unistd.h>
#include <dirent.h>

#include "libwiigui/gui.h"
#include "menu.h"
#include "main.h"
#include "Prompts/prompts.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern void HaltResumeGui();

bool language_dl = false;

/****************************************************************************
 * MenuSettingsFile
 ***************************************************************************/

int MenuSettingsLanguage()
{
	int menu = MENU_NONE;
	
	int ret = -1;
	int activated = -1;
	int i = 0;
	int focus = 0;
	
	OptionList options;
	
	sprintf(options.name[i], tr("STANDARD"));
	if(stricmp(Options.temp_language.c_str(), tr("STANDARD")) == 0)
	{
		sprintf (options.value[i], tr("activated"));
		activated = i;
	}
	else
		sprintf (options.value[i], " ");

	i++;

	DIR *dirHandle;
	struct dirent * dirEntry;
	
	dirHandle = opendir(check_path(Settings.device_dat + ":/config/HBF/Languages").c_str());
	if (dirHandle)
	{
		while (0 != (dirEntry = readdir(dirHandle)))
		{
			if(stricmp(dirEntry->d_name, ".") != 0 && stricmp(dirEntry->d_name, "..") != 0)
			{
				string temp = dirEntry->d_name;
				if(temp.length() > 4 && stricmp(temp.substr(temp.length() -5, 5).c_str(), ".lang") == 0)
				{
					temp.erase(temp.length() -5, 5);	// endung entfernen
					sprintf(options.name[i], temp.c_str());
					
					if(stricmp(Options.temp_language.c_str(), temp.c_str()) == 0)
					{
						sprintf (options.value[i], tr("activated"));
						activated = i;
					}
					else
						sprintf (options.value[i], " ");
					
					i++;
				}
			}
		}
		closedir(dirHandle);
	}

	options.length = i;

	GuiImageData bgImgData(Theme.background);
	GuiImageData btnOutline(Theme.button_small);
	GuiImageData btnOutlineOver(Theme.button_small_focus);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiImage bgImg(&bgImgData);

	GuiText titleTxt(tr("Languages"), 28, (GXColor){Theme.title_1, Theme.title_2, Theme.title_3, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(50,50);

	GuiText downloadBtnTxt(tr("Download"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiImage downloadBtnImg(&btnOutline);
	GuiImage downloadBtnImgOver(&btnOutlineOver);
	GuiButton downloadBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	downloadBtn.SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	downloadBtn.SetPosition(-100, 38);
	downloadBtn.SetLabel(&downloadBtnTxt);
	downloadBtn.SetImage(&downloadBtnImg);
	downloadBtn.SetImageOver(&downloadBtnImgOver);
	downloadBtn.SetTrigger(&trigA);
	downloadBtn.SetEffectGrow();

	GuiText okBtnTxt(tr("OK"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiImage okBtnImg(&btnOutline);
	GuiImage okBtnImgOver(&btnOutlineOver);
	GuiButton okBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	okBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	okBtn.SetPosition(100, -35);
	okBtn.SetLabel(&okBtnTxt);
	okBtn.SetImage(&okBtnImg);
	okBtn.SetImageOver(&okBtnImgOver);
	okBtn.SetTrigger(&trigA);
	okBtn.SetEffectGrow();

	GuiText backBtnTxt(tr("Stop"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiImage backBtnImg(&btnOutline);
	GuiImage backBtnImgOver(&btnOutlineOver);
	GuiButton backBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	backBtn.SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	backBtn.SetPosition(-100, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetEffectGrow();

	GuiOptionBrowser optionBrowser(552, 248, &options);
	optionBrowser.SetPosition(0, 108);
	optionBrowser.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	optionBrowser.SetCol2Position(340);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&bgImg);
	w.Append(&titleTxt);
	w.Append(&downloadBtn);
	w.Append(&okBtn);
	w.Append(&backBtn);
	mainWindow->Append(&w);
	mainWindow->Append(&optionBrowser);
	
	mainWindow->ChangeFocus(&optionBrowser);
	ResumeGui();

	while(menu == MENU_NONE)
	{
		usleep(100);

		ret = optionBrowser.GetClickedOption();

		if(ret != -1)
		{
			for(i=0; i < options.length; i++)
			{
				if(i == ret)
				{
					sprintf (options.value[i], tr("activated"));
					activated = i;
				}
				else
					sprintf (options.value[i], " ");
			}
			optionBrowser.TriggerUpdate();
		}

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B) || PAD_ButtonsDown(0) & PAD_BUTTON_B)
		{
			if(focus == 0)
			{
				focus = 1;
				mainWindow->ChangeFocus(&w);
			}
			else
			{
				focus = 0;
				mainWindow->ChangeFocus(&optionBrowser);
			}
			HaltResumeGui();
		}
		
		if(downloadBtn.GetState() == STATE_CLICKED)
		{
			downloadBtn.ResetState();
			string languagedownload = checkLanguagesPrompt();
			
			if(languagedownload != "NULL" && language_folder_exists())
			{
				languageDownload(languagedownload);
				menu = MENU_SETTINGS_LANGUAGE;
				language_dl = true;
				break;
			}
		}
		
		if(okBtn.GetState() == STATE_CLICKED)
		{
			Options.temp_last_setting = 1;
			Options.temp_language = options.name[activated];
			menu = MENU_SETTINGS_FILE;
		}
		
		if(backBtn.GetState() == STATE_CLICKED)
		{
			Options.temp_last_setting = 1;
			menu = MENU_SETTINGS_FILE;
		}
	}
	HaltGui();
	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	return menu;
}

bool GetMenuSettingsLanguageDL()
{
	return language_dl;
}

void SetMenuSettingsLanguageDL(bool dl)
{
	language_dl = dl;
}
