
#include <unistd.h>
#include <algorithm>

#include "libwiigui/gui.h"
#include "main.h"
#include "menu.h"
#include "Prompts/prompts.h"

#include "Network/update.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;
extern bool boot_buffer;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

extern bool runaway;

/****************************************************************************
 * MenuSettings
 ***************************************************************************/
int MenuSettings()
{
	int menu = MENU_NONE;

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	GuiTrigger trigHome;
	trigHome.SetButtonOnlyTrigger(-1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, PAD_BUTTON_START);

	GuiText titleTxt(tr("Settings"), 30, (GXColor){Theme.title_1, Theme.title_2, Theme.title_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,30);

	// normale Buttons
	GuiImageData btn(Theme.button);
	GuiImage categoryCreateBtnImg(&btn);
	GuiImage categoryEraseBtnImg(&btn);
	GuiImage infoBtnImg(&btn);
	GuiImage optionsBtnImg(&btn);
	GuiImage updateBtnImg(&btn);
	GuiImage backBtnImg(&btn);

	// normale Buttons over
	GuiImageData btn_over(Theme.button_focus);
	GuiImage categoryCreateBtnImgOver(&btn_over);
	GuiImage categoryEraseBtnImgOver(&btn_over);
	GuiImage infoBtnImgOver(&btn_over);
	GuiImage optionsBtnImgOver(&btn_over);
	GuiImage updateBtnImgOver(&btn_over);
	GuiImage backBtnImgOver(&btn_over);

	GuiText categoryCreateBtnTxt(tr("Create Category"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton categoryCreateBtn(btn.GetWidth(), btn.GetHeight());
	categoryCreateBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	categoryCreateBtn.SetPosition(0, 80);
	categoryCreateBtn.SetLabel(&categoryCreateBtnTxt);
	categoryCreateBtn.SetImage(&categoryCreateBtnImg);
	categoryCreateBtn.SetImageOver(&categoryCreateBtnImgOver);
	categoryCreateBtn.SetTrigger(&trigA);

	GuiText categoryEraseBtnTxt(tr("Delete Category"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton categoryEraseBtn(btn.GetWidth(), btn.GetHeight());
	categoryEraseBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	categoryEraseBtn.SetPosition(0, 140);
	categoryEraseBtn.SetLabel(&categoryEraseBtnTxt);
	categoryEraseBtn.SetImage(&categoryEraseBtnImg);
	categoryEraseBtn.SetImageOver(&categoryEraseBtnImgOver);
	categoryEraseBtn.SetTrigger(&trigA);

	GuiText optionsBtnTxt(tr("Options"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton optionsBtn(btn.GetWidth(), btn.GetHeight());
	optionsBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	optionsBtn.SetPosition(0, 200);
	optionsBtn.SetLabel(&optionsBtnTxt);
	optionsBtn.SetImage(&optionsBtnImg);
	optionsBtn.SetImageOver(&optionsBtnImgOver);
	optionsBtn.SetTrigger(&trigA);

	GuiText infoBtnTxt(tr("Info"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton infoBtn(btn.GetWidth(), btn.GetHeight());
	infoBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	infoBtn.SetPosition(0, 260);
	infoBtn.SetLabel(&infoBtnTxt);
	infoBtn.SetImage(&infoBtnImg);
	infoBtn.SetImageOver(&infoBtnImgOver);
	infoBtn.SetTrigger(&trigA);

	GuiText updateBtnTxt(tr("Update"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton updateBtn(btn.GetWidth(), btn.GetHeight());
	updateBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	updateBtn.SetPosition(0, 320);
	updateBtn.SetLabel(&updateBtnTxt);
	updateBtn.SetImage(&updateBtnImg);
	updateBtn.SetImageOver(&updateBtnImgOver);
	updateBtn.SetTrigger(&trigA);

	GuiText backBtnTxt(tr("Back"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton backBtn(btn.GetWidth(), btn.GetHeight());
	backBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	backBtn.SetPosition(0, 380);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetTrigger(&trigHome);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&titleTxt);
	w.Append(&categoryCreateBtn);
	w.Append(&categoryEraseBtn);
	w.Append(&optionsBtn);
	w.Append(&infoBtn);
	w.Append(&updateBtn);
	w.Append(&backBtn);

	mainWindow->Append(&w);

	ResumeGui();

	while(menu == MENU_NONE)
	{
		usleep(100);

		// kinderschutz aus
		if(strcasecmp(Settings.code,"NULL") == 0 )
		{
			categoryCreateBtn.SetClickable(true);
			categoryEraseBtn.SetClickable(true);
			updateBtn.SetClickable(true);
		}
		// kinderschutz an
		else
		{
			categoryCreateBtn.SetClickable(false);
			categoryEraseBtn.SetClickable(false);
			updateBtn.SetClickable(false);
		}

		if(categoryCreateBtn.GetState() == STATE_CLICKED)
		{
			categoryCreateBtn.ResetState();

			char new_category_name[256];
			sprintf (new_category_name, tr(Settings.new_category_name));
			OnScreenKeyboard(new_category_name, 256, false);

			if(strcasecmp(new_category_name,"NULL") != 0 )
				KategorieEinfuegen(new_category_name);
		}
		else if(categoryEraseBtn.GetState() == STATE_CLICKED)
		{
			categoryEraseBtn.ResetState();
			if(AvailableCategory.categories.size() > 1)
			{
				string entferne_kategorie = eraseCategory();

				if( entferne_kategorie != "NULL" )
				{
					int choice = WindowPrompt(entferne_kategorie.c_str(), tr("Really remove?"), tr("Yes"), tr("No"));
					if(choice == 1)
					{
						KategorieEntfernen(entferne_kategorie);
						Settings.current_category = 0;
					}
				}
			}
		}
		else if(optionsBtn.GetState() == STATE_CLICKED)
		{
			if(strcasecmp(Settings.code,"NULL") != 0 )
			{
				char buffer[5];
				while(1)
				{
					for(int i = 0; i < (signed)strlen(buffer); i++)
						buffer[i] = '\0';

					OnScreenCodeboard(buffer, 4);
					// wenn eingabe richtig
					if(strcasecmp(buffer, Settings.code) == 0 )
					{
						menu = MENU_SETTINGS_FILE;
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
				menu = MENU_SETTINGS_FILE;
		}
		else if(infoBtn.GetState() == STATE_CLICKED)
		{
			infoBtn.ResetState();
			infoPrompt();
		}
		else if(updateBtn.GetState() == STATE_CLICKED)
		{
			updateBtn.ResetState();

			string revnumber = checkUpdatePrompt();

			if(revnumber != "NULL")
			{
				char title[100];
				if(revnumber == "Beta")
					sprintf(title, "%s ( rev.%i  %s  Beta )", tr("Update"), SvnRev(), "»");
				else
					sprintf(title, "%s ( rev.%i  %s  rev.%s )", tr("Update"), SvnRev(), "»", revnumber.c_str());

				// auflisten
				string version_text = NewVersionsText();
				vector <string> versions_text;

				int strpos = version_text.find("//rev");
				while(strpos != -1)
				{
					version_text.erase(0, strpos +2);
					int strpos2 = version_text.find("//rev");
					if(strpos2 != -1)
					{
						versions_text.push_back(version_text.substr(0, strpos2 -2));
						version_text.erase(0, strpos2 -2);
					}
					else
						versions_text.push_back(version_text.substr(0, version_text.find("end") -2));

					strpos = version_text.find("//rev");
				}

				char revinfo[10];
				if(revnumber == "Beta")
					sprintf(revinfo, "rev_Beta");
				else
					sprintf(revinfo, "rev%i", atoi(revnumber.c_str()));

				string text;

				for(int i=0; i < (signed)versions_text.size(); i++)
				{
					if(strcasecmp(versions_text[i].substr(0, versions_text[i].find(":")).c_str(), revinfo) == 0)
					{
						versions_text[i].erase(0, versions_text[i].find(":") +2);
						text = versions_text[i];
						break;
					}
				}
				revtext(text.c_str());

				// anzeigen
				int choice = WindowPrompt(title, tr("Do you want to update now ?"), tr("Yes"), tr("No"));
				if(choice == 1)
				{
					updatePrompt(revnumber);
					if(boot_buffer)
						menu = MENU_EXIT;
				}
			}
		}
		else if(backBtn.GetState() == STATE_CLICKED)
			menu = MENU_MAIN;

		if(runaway == true)
			menu = MENU_MAIN;
	}

	HaltGui();
	mainWindow->Remove(&w);
	return menu;
}

