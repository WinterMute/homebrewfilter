#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"
#include "menu.h"
#include "menus.h"
#include "Tools/FontSystem.h"
#include "Tools/save.h"
#include "Tools/theme.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;
extern GuiImageData * pointer;
extern GuiImage * bgImg;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern void HaltResumeGui();

extern bool runaway;
int temp_last_category;
int temp_slide_effect;
int temp_apps;
bool temp_quick_start;
bool temp_show_all;
int temp_device_icon;
bool temp_navigation;
string temp_device_dat;

/****************************************************************************
 * MenuSettingsFile
 ***************************************************************************/

int MenuSettingsFile()
{
	int menu = MENU_NONE;
	int ret, ret2;
	int i = 0;
	bool firstRun = true;
	int focus = 0;

	int last_category 	= Options.last_category;
	bool quick_start 	= Options.quick_start;
	bool show_all 		= Options.show_all;
	int device_icon 	= Options.device_icon;
	bool childlock;
	if(strcasecmp(Settings.code,"NULL") == 0 )
		childlock = 0;
	else
		childlock = 1;
	bool navigation = Options.navigation;

	string device_dat;
	if(Settings.device_dat == "sd1")
		device_dat = "SD";
	else if(Settings.device_dat == "usb1")
		device_dat = "USB";

	OptionList options;
	sprintf(options.name[i++], tr("Theme"));
	sprintf(options.name[i++], tr("Language"));
	sprintf(options.name[i++], tr("Font"));
	sprintf(options.name[i++], tr("Slide Effect"));
	sprintf(options.name[i++], tr("Category remember"));
	sprintf(options.name[i++], tr("Number of Apps"));
	sprintf(options.name[i++], tr("Quick Start"));
	sprintf(options.name[i++], tr("Show All"));
	sprintf(options.name[i++], tr("Storage Device"));
	sprintf(options.name[i++], tr("Device icon"));
	sprintf(options.name[i++], tr("Childlock"));
	sprintf(options.name[i++], tr("Navigation key exchange"));
	sprintf(options.name[i++], tr("Display"));
	sprintf(options.name[i++], tr("Network Settings"));
	options.length = i;

	GuiText titleTxt(tr("Options"), 28, (GXColor){Theme.title_1, Theme.title_2, Theme.title_3, 255});
	titleTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	titleTxt.SetPosition(50,50);

	GuiImageData btnOutline(Theme.button_small);
	GuiImageData btnOutlineOver(Theme.button_small_focus);

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	GuiTrigger trigHome;
	trigHome.SetButtonOnlyTrigger(-1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, 0);

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
	backBtn.SetTrigger(&trigHome);
	backBtn.SetEffectGrow();

	GuiText saveBtnTxt(tr("Save"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiImage saveBtnImg(&btnOutline);
	GuiImage saveBtnImgOver(&btnOutlineOver);
	GuiButton saveBtn(btnOutline.GetWidth(), btnOutline.GetHeight());
	saveBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	saveBtn.SetPosition(100, -35);
	saveBtn.SetLabel(&saveBtnTxt);
	saveBtn.SetImage(&saveBtnImg);
	saveBtn.SetImageOver(&saveBtnImgOver);
	saveBtn.SetTrigger(&trigA);
	saveBtn.SetEffectGrow();

	GuiOptionBrowser optionBrowser(552, 248, &options);
	optionBrowser.SetPosition(0, 108);
	optionBrowser.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	optionBrowser.SetCol2Position(280);
	optionBrowser.Col2Scroll(280);

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&titleTxt);
	w.Append(&backBtn);
	w.Append(&saveBtn);
	mainWindow->Append(&optionBrowser);
	mainWindow->Append(&w);
	ResumeGui();

	int change = 0;
	while(menu == MENU_NONE)
	{
		usleep(100);

		ret = optionBrowser.GetChangedOption();
		ret2 = optionBrowser.GetClickedOption();

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT) || PAD_ButtonsDown(0) & PAD_BUTTON_RIGHT)
		{
			change = 0;
			switch (ret)
			{
				case SLIDE_EFFECT:
					change = atoi(options.value[ret]);
					change++;
					if(change > 5)
						change = 5;
					sprintf (options.value[ret], "%i", change);
					break;

				case CATEGORY_REMEMBER:
					change = last_category;
					change++;
					if(change > (signed)AvailableCategory.categories.size())
						change = AvailableCategory.categories.size();
					last_category = change;
					break;

				case NUMBER_OF_APPS:
					change = atoi(options.value[ret]);
					change++;
					if(change > 5)
						change = 5;
					sprintf (options.value[ret], "%i", change);
					break;

				case QUICK_START:
					quick_start = 1;
					break;

				case SHOW_ALL:
					show_all = 1;
					break;

				case DEVICE_ICON:
					change = device_icon;
					change++;
					if(change > 3)
						change = 3;
					device_icon = change;
					break;

				case STORAGE_DEVICE:
					device_dat = "USB";
					sprintf (options.value[STORAGE_DEVICE], device_dat.c_str());
					break;

				case NAVIGATION:
					navigation = 1;
					break;
			}
			HaltResumeGui();
			optionBrowser.TriggerUpdate();
		}

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT) || PAD_ButtonsDown(0) & PAD_BUTTON_LEFT)
		{
			change = 0;
			switch (ret)
			{
				case SLIDE_EFFECT:
					change = atoi(options.value[ret]);
					change--;
					if(change < 0)
						change = 0;
					sprintf (options.value[ret], "%i", change);
					break;

				case CATEGORY_REMEMBER:
					change = last_category;
					change--;
					if(change < 0)
						change = 0;
					last_category = change;
					break;

				case NUMBER_OF_APPS:
					change = atoi(options.value[ret]);
					change--;
					if(change < 4)
						change = 4;
					sprintf (options.value[ret], "%i", change);
					break;

				case QUICK_START:
					quick_start = 0;
					break;

				case SHOW_ALL:
					show_all = 0;
					break;

				case DEVICE_ICON:
					change = device_icon;
					change--;
					if(change < 0)
						change = 0;
					device_icon = change;
					break;

				case STORAGE_DEVICE:
					device_dat = "SD";
					sprintf (options.value[STORAGE_DEVICE], device_dat.c_str());
					break;

				case NAVIGATION:
					navigation = 0;
					break;
			}
			HaltResumeGui();
			optionBrowser.TriggerUpdate();
		}

		if(ret2 != -1)
		{
			// einstellungen temporär speichern
			/******************************************************************************/
			Options.temp_theme		= options.value[THEME];
			Options.temp_language		= options.value[LANGUAGE];
			Options.temp_font		= options.value[FONT];
			temp_slide_effect		= atoi(options.value[SLIDE_EFFECT]);
			temp_last_category		= last_category;
			temp_apps			= atoi(options.value[NUMBER_OF_APPS]);
			temp_quick_start		= quick_start;
			temp_show_all			= show_all;
			temp_device_icon		= device_icon;
			temp_device_dat			= device_dat;
			temp_navigation			= navigation;
			/******************************************************************************/

			// in weitere einstellungen gehen
			switch (ret2)
			{
				case THEME:
					menu = MENU_SETTINGS_THEME;
					break;

				case LANGUAGE:
					menu = MENU_SETTINGS_LANGUAGE;
					break;

				case FONT:
					menu = MENU_SETTINGS_FONT;
					break;

				case CHILDLOCK:
					menu = MENU_SETTINGS_CHILDLOCK;
					break;

				case DISPLAY:
					menu = MENU_SETTINGS_DISPLAY;
					break;

				case NETWORK:
					menu = MENU_SETTINGS_NETWORK;
					break;

			}
		}

		if(firstRun)
		{
			firstRun = false;

			if(Options.temp_last_setting == 1)
			{
				sprintf (options.value[THEME], Options.temp_theme.c_str());
				sprintf (options.value[LANGUAGE], Options.temp_language.c_str());
				sprintf (options.value[FONT], Options.temp_font.c_str());
				sprintf (options.value[SLIDE_EFFECT], "%i", temp_slide_effect);
				sprintf (options.value[NUMBER_OF_APPS], "%i", temp_apps);
				sprintf (options.value[STORAGE_DEVICE], temp_device_dat.c_str());
				sprintf (options.value[DISPLAY], " ");
				sprintf (options.value[NETWORK], " ");

				last_category 	= temp_last_category;
				quick_start 	= temp_quick_start;
				show_all	= temp_show_all;
				device_icon 	= temp_device_icon;
				if(strcasecmp(Options.temp_code,"NULL") == 0 )
					childlock = 0;
				else
					childlock = 1;
				navigation = temp_navigation;
			}
			else
			{
				sprintf (options.value[THEME], Options.theme);
				sprintf (options.value[LANGUAGE], Options.language);
				sprintf (options.value[FONT], Options.font);
				sprintf (options.value[SLIDE_EFFECT], "%i", Options.slide_effect);
				sprintf (options.value[NUMBER_OF_APPS], "%i", Options.apps);
				sprintf (options.value[STORAGE_DEVICE], device_dat.c_str());
				sprintf (options.value[DISPLAY], " ");
				sprintf (options.value[NETWORK], " ");
			}
		}

		if(change != -1)
		{
			change = -1;

			if(!last_category)
				sprintf (options.value[CATEGORY_REMEMBER], tr("last"));
			else
				sprintf (options.value[CATEGORY_REMEMBER], AvailableCategory.categories[last_category - 1].c_str());

			if(!quick_start)
				sprintf (options.value[QUICK_START], tr("No"));
			else
				sprintf (options.value[QUICK_START], tr("Yes"));

			if(!show_all)
				sprintf (options.value[SHOW_ALL], tr("No"));
			else
				sprintf (options.value[SHOW_ALL], tr("Yes"));

			if(!childlock)
				sprintf (options.value[CHILDLOCK], tr("No"));
			else
				sprintf (options.value[CHILDLOCK], tr("Yes"));

			if(device_icon == 0)
				sprintf (options.value[DEVICE_ICON], tr("Off"));
			else if(device_icon == 1)
				sprintf (options.value[DEVICE_ICON], tr("Menu"));
			else if(device_icon == 2)
				sprintf (options.value[DEVICE_ICON], tr("Dialog box"));
			else if(device_icon == 3)
				sprintf (options.value[DEVICE_ICON], tr("All"));

			if(!navigation)
				sprintf (options.value[NAVIGATION], tr("No"));
			else
				sprintf (options.value[NAVIGATION], tr("Yes"));

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

		if(backBtn.GetState() == STATE_CLICKED)
		{
			strcpy (Options.temp_code, Settings.code);
			Options.temp_network	= Options.network;
			Options.temp_newrevtext	= Options.newrevtext;
			menu					= MENU_SETTINGS;
		}

		if(saveBtn.GetState() == STATE_CLICKED)
		{
			// Theme ändern
			if(stricmp(Options.theme, options.value[THEME]) != 0 || GetMenuSettingsThemeDL())
			{
				sprintf (Options.theme, options.value[THEME]);
				DefaultTheme();
				if(stricmp(Options.theme, tr("STANDARD")) != 0)
					theme(Settings.device_dat + ":/config/HBF/themes/" + Options.theme + "/");
				// Cursor und Hintergrund ändern
				#ifdef HW_RVL
				pointer = new GuiImageData(Theme.player_point);
				#endif

				mainWindow->Remove(bgImg);
				bgImg = new GuiImage(new GuiImageData(Theme.background));
				mainWindow->Append(bgImg);
			}

			// Schriftart ändern
			if(stricmp(Options.font, options.value[FONT]) != 0 || GetMenuSettingsFontDL())
			{
				sprintf(Options.font, options.value[FONT]);
				// change font
				HaltGui();
				SetFont();
				ResumeGui();
			}

			strcpy(Settings.code, Options.temp_code);
			Options.slide_effect		= atoi(options.value[SLIDE_EFFECT]);
			Options.last_category		= last_category;
			Options.apps			= atoi(options.value[NUMBER_OF_APPS]);
			Options.quick_start		= quick_start;
			Options.show_all		= show_all;
			Options.device_icon		= device_icon;
			device_dat			= options.value[STORAGE_DEVICE];
			Options.navigation		= navigation;
			Options.network			= Options.temp_network;
			Options.newrevtext		= Options.temp_newrevtext;

			if(device_dat == "SD")
				Settings.device_dat	= "sd1";
			else if(device_dat == "USB")
				Settings.device_dat	= "usb1";

			// Sprache ändern zum schluss wegen STANDARD
			if(stricmp(Options.language, options.value[LANGUAGE]) != 0 || GetMenuSettingsLanguageDL())
			{
				sprintf (Options.language, options.value[LANGUAGE]);

				bool theme = 0, language = 0, font = 0;
				if(stricmp(Options.theme, tr("STANDARD")) == 0)
					theme = 1;

				if(stricmp(Options.language, tr("STANDARD")) == 0)
					language = 1;

				if(stricmp(Options.font, tr("STANDARD")) == 0)
					font = 1;

				/*********************************************************************/
				if(stricmp(Options.language, tr("STANDARD")) == 0)
					translate();
				else
					ini_Open(check_path(Settings.device_dat + ":/config/HBF/languages/") + Options.language + ".lang");
				/*********************************************************************/

				AvailableCategory.categories[0] = tr(Settings.category_name_all);

				if(theme)
					sprintf(Options.theme, tr("STANDARD"));

				if(language)
					sprintf(Options.language, tr("STANDARD"));

				if(font)
					sprintf(Options.font, tr("STANDARD"));
			}

			menu = MENU_SETTINGS;

		}

		if(runaway == true)
		{
			strcpy (Options.temp_code, Settings.code);
			Options.temp_network	= Options.network;
			Options.temp_newrevtext	= Options.newrevtext;
			menu = MENU_SETTINGS;
		}

	}
	HaltGui();
	Options.temp_last_setting = 0;

	mainWindow->Remove(&optionBrowser);
	mainWindow->Remove(&w);
	return menu;
}