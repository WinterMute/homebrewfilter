/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * menu.cpp
 * Menu flow routines - handles all menu logic
 ***************************************************************************/

#include <gccore.h>
#include <ogcsys.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wiiuse/wpad.h>

#include "libwiigui/gui.h"
#include "menu.h"
#include "main.h"
#include "input.h"
#include "filelist.h"

#include "Menus/menus.h"
#include "Menus/menus_command.h"

#include "Prompts/prompts.h"

#include "Tools/app_list.h"
#include "Tools/copy_app_in_category.h"
#include "Tools/SelectIos.h"
#include "Tools/TakeScreenshot.h"
#include "Tools/theme.h"

#include "BootHomebrew/BootHomebrew.h"

#define THREAD_SLEEP 100

GuiWindow * mainWindow = NULL;
GuiImageData * pointer = NULL;
GuiImage * bgImg = NULL;
extern bool boothomebrew;
extern bool goneek2o;

static lwp_t guithread = LWP_THREAD_NULL;
static bool guiHalt = true;

/****************************************************************************
 * ResumeGui
 *
 * Signals the GUI thread to start, and resumes the thread. This is called
 * after finishing the removal/insertion of new elements, and after initial
 * GUI setup.
 ***************************************************************************/
void
ResumeGui()
{
	guiHalt = false;
	LWP_ResumeThread (guithread);
}

/****************************************************************************
 * HaltGui
 *
 * Signals the GUI thread to stop, and waits for GUI thread to stop
 * This is necessary whenever removing/inserting new elements into the GUI.
 * This eliminates the possibility that the GUI is in the middle of accessing
 * an element that is being changed.
 ***************************************************************************/
void
HaltGui()
{
	guiHalt = true;

	// wait for thread to finish
	while(!LWP_ThreadIsSuspended(guithread))
		usleep(THREAD_SLEEP);
}

/****************************************************************************
 * HaltResumeGui
 ***************************************************************************/
void
HaltResumeGui()
{
	HaltGui();
	ResumeGui();
}

/****************************************************************************
 * UpdateGUI
 *
 * Primary thread to allow GUI to respond to state changes, and draws GUI
 ***************************************************************************/

static void *
UpdateGUI (void *arg)
{
	int i;

	while(1)
	{
		if(guiHalt)
		{
			LWP_SuspendThread(guithread);
		}
		else
		{
			UpdatePads();
			mainWindow->Draw();

			#ifdef HW_RVL
		//	for(i=3; i >= 0; i--) // so that player 1's cursor appears on top!
		//	{
				if(userInput[0].wpad->ir.valid)
					Menu_DrawImg(userInput[0].wpad->ir.x-48, userInput[0].wpad->ir.y-48,
						96, 96, pointer->GetImage(), userInput[0].wpad->ir.angle, 1, 1, 255);
		//	}
			#endif

			Menu_Render();

		//	for(i=0; i < 4; i++)
				mainWindow->Update(&userInput[0]);

			if(ExitRequested)
			{
				for(i = 0; i < 255; i += 15)
				{
					mainWindow->Draw();
					GXColor Color = (GXColor) {0, 0, 0, i};
					Menu_DrawRectangle(0,0,screenwidth,screenheight,&Color,false,true);
					Menu_Render();
				}

				if (boothomebrew)
				{
					LoadHomebrew(Settings.forwarder_path.c_str());

					string startingAppName = Settings.forwarder_path;

					if((signed)startingAppName.rfind("/") != -1)
						startingAppName.erase(startingAppName.rfind("/"));

					startingAppName.erase(0, startingAppName.rfind("/") +1);

					if(IOS_GetVersion() != GetAppIOS(startingAppName))
						addAppIos(Settings.startingAppName, SelectedIOS());
				}
				if (!goneek2o)
					ExitApp();
			}

			// sd check
			if(Settings.device == "sd1")
				check_sd();

			// usb check
			else if(Settings.device == "usb1")
				check_usb();

			// sd und usb check
			else if(Settings.device == "sd_usb")
			{
				check_sd();
				check_usb();
			}
#ifndef VWII
			else if(Settings.device == "dvd")
				check_dvd();

			else if(Settings.device == "gca")
				check_gca();

			else if(Settings.device == "gcb")
				check_gcb();
#endif
			else if(Settings.device == "all")
			{
				check_sd();
				check_usb();
#ifndef VWII
				check_dvd();
				check_gca();
				check_gcb();
#endif
			}

			// screenshoot
			if(WPAD_ButtonsDown(0) & WPAD_BUTTON_1 && WPAD_ButtonsDown(0) & WPAD_BUTTON_2)
				Screenshot();
		}
	}
	return NULL;
}

/****************************************************************************
 * InitGUIThread
 *
 * Startup GUI threads
 ***************************************************************************/
void
InitGUIThreads()
{
	LWP_CreateThread (&guithread, UpdateGUI, NULL, NULL, 0, 70);
}

/****************************************************************************
 * MainMenu
 ***************************************************************************/
void MainMenu(int menu)
{
	int currentMenu = menu;

	// position der letzten app
	for(int i = 0; i < (signed)vechomebrew_list_category[Settings.current_category].size(); i++)
	{
		string name = vechomebrew_list_category[Settings.current_category][i].foldername;
		if((signed)name.rfind("/") != -1)
			name.erase(name.rfind("/"));
		name.erase(0, name.rfind("/") +1);

		if(name == Settings.startingAppName)
		{
			Settings.last_app_pos = i;
			break;
		}
	}

	bgImg = new GuiImage(new GuiImageData(Theme.background));
	mainWindow->Append(bgImg);

	while(currentMenu != MENU_EXIT)
	{
		switch (currentMenu)
		{
			case MENU_MAIN:
				currentMenu = MenuMain();
				break;

			case MENU_SETTINGS:
				currentMenu = MenuSettings();
				break;

			case MENU_SETTINGS_FILE:
				currentMenu = MenuSettingsFile();
				break;

			case MENU_SETTINGS_THEME:
				currentMenu = MenuSettingsTheme();
				break;

			case MENU_SETTINGS_LANGUAGE:
				currentMenu = MenuSettingsLanguage();
				break;

			case MENU_SETTINGS_FONT:
				currentMenu = MenuSettingsFont();
				break;

			case MENU_SETTINGS_DISPLAY:
				currentMenu = MenuSettingsDisplay();
				break;

			case MENU_SETTINGS_CHILDLOCK:
				currentMenu = MenuSettingsChildlock();
				break;

			case MENU_SETTINGS_NETWORK:
				currentMenu = MenuSettingsNetwork();
				break;

			default: // unrecognized menu
				currentMenu = MenuMain();
				break;
		}
	}

	ResumeGui();
	ExitRequested = 1;
	HaltGui();

	delete mainWindow;

	delete pointer;

	mainWindow = NULL;
}
