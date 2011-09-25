/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * menu.h
 * Menu flow routines - handles all menu logic
 ***************************************************************************/

#ifndef _MENU_H_
#define _MENU_H_

#include <ogcsys.h>

#include "Prompts/prompts.h"

void InitGUIThreads();
void MainMenu (int menuitem);

enum
{
    JAPANESE,
    ENGLISH,
    GERMAN,
    FRENCH,
    SPANISH,
    ITALIAN,
    DUTCH,
    S_CHINESE,
    T_CHINESE,
    KOREAN,
};

enum
{
	MENU_EXIT = -1,
	MENU_NONE,
	MENU_MAIN,
	MENU_SETTINGS,
	MENU_SETTINGS_FILE,
	MENU_SETTINGS_THEME,
	MENU_SETTINGS_LANGUAGE,
	MENU_SETTINGS_FONT,
	MENU_SETTINGS_CHILDLOCK,
	MENU_SETTINGS_DISPLAY,
	MENU_SETTINGS_NETWORK,
};

#endif
