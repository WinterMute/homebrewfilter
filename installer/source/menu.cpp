
#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <sstream>
#include <vector>
#include <wiiuse/wpad.h>

#include "getios.h"
#include "menu.h"
#include "video.h"
#include "wad.h"
#include "../../svnrev/svnrev.h"

using namespace std;
std::vector<string> text1, text2, text3;

int startpos_x = 6;
int startpos_y = 12;
int menu_main_choice;

void Pad_unpressed( void )
{
	while(1)
	{
		PAD_ScanPads();
		u32 PAD_Unpressed  = PAD_ButtonsUp(0);
		if ( PAD_Unpressed )
			break;
	}
	return;
}

void show_menu_head()
{
	Con_FgColor(6, 1);
	printf("\x1b[%i;%iH", startpos_x, startpos_y);
	printf("HBF installer v0.4");

	Con_FgColor(7, 1);
	printf("\t\t\t\t\t(C) 2011");
	Con_FgColor(6, 1);
	printf(" hamachi-mp");

	Con_FgColor(7, 1);
	printf("\n\t\t\t\t\t\t\t\t\t\t\t\t(C) 2012");
	Con_FgColor(6, 1);
	printf(" Nano & Obcd");
}

void fill_menu_main()
{
	text1.clear();
	stringstream buffer;
	buffer << "Install the HomebrewFilter rev" << SvnRev();
	text1.push_back(buffer.str());
	if(CheckAppFound(GetTitleID()))
	{
		text1.push_back("Reinstall the HomebrewFilter");
		text1.push_back("Uninstall the HomebrewFilter");
	}
	else
	{
		text1.push_back("");
	}
	text1.push_back("");
	text1.push_back("Copyright");
	text1.push_back("");
	text1.push_back("Exit");
}

void fill_menu_install_uninstall()
{
	text2.push_back("Continue");
	text2.push_back("Go Back");
}

void fill_menu_copyright()
{
	text3.push_back("Copyright (C) hamachi-mp");
	text3.push_back("Copyright (C) Nano");
	text3.push_back("");
	text3.push_back("Thanks to:");
	text3.push_back("\tTeam Twiizers");
	text3.push_back("\tdevkitPRO");
	text3.push_back("\tWaninkoko (WAD Manager)");
	text3.push_back("\tWiiCrazy/I.R.on (Crap Installer)");
}

void menu()
{
	mopen();
	ISFS_Initialize();
	listIOS();

	fill_menu_main();
	fill_menu_install_uninstall();
	fill_menu_copyright();

	menu_main_choice = text1.size() -1;

	int currentMenu = 0;
	while(currentMenu != MENU_EXIT)
	{
		switch (currentMenu)
		{
			case MENU_MAIN:
				currentMenu = menu_main(menu_main_choice);
				break;

			case MENU_INSTALL:
				currentMenu = menu_install_uninstall(1);
				break;

			case MENU_REINSTALL:
				currentMenu = menu_install_uninstall(2);
				break;

			case MENU_UNINSTALL:
				currentMenu = menu_install_uninstall(0);
				break;

			case MENU_INSTALLING:
				currentMenu = menu_install();
				break;

			case MENU_REINSTALLING:
				currentMenu = menu_reinstall();
				break;

			case MENU_UNINSTALLING:
				currentMenu = menu_uninstall();
				break;

			case MENU_COPYRIGHT:
				currentMenu = menu_copyright();
				break;
		}
	}

	ISFS_Deinitialize();
}

int menu_main(int scrollpos)
{
	Con_Clear();

	show_menu_head();
	fill_menu_main();
	if(text1[scrollpos] == "")
		scrollpos = text1.size() -1;

	Con_FgColor(3, 1);
	printf("\x1b[%i;%iH", startpos_x +2, startpos_y);
	printf("Main Menu");

	Con_FgColor(7, 1);
	for(int i=0; i < (signed)text1.size(); i++)
	{
		printf("\x1b[%i;%iH", startpos_x +4 +i, startpos_y +3);
			printf("%s", text1[i].c_str());
	}

	bool scroll = true;
	while(1)
	{
		WPAD_ScanPads();
		PAD_ScanPads();

		if((WPAD_ButtonsDown(0) & (WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN)
			|| (PAD_ButtonsDown(0) & PAD_BUTTON_DOWN)) && scrollpos < (signed)text1.size() -1)
		{
			scrollpos++;
			while(text1[scrollpos] == "")
				scrollpos++;

			scroll = true;

			if( PAD_ButtonsDown(0) )
				Pad_unpressed();
		}
		else if((WPAD_ButtonsDown(0) & (WPAD_BUTTON_UP | WPAD_CLASSIC_BUTTON_UP)
			|| (PAD_ButtonsDown(0) & PAD_BUTTON_UP)) && scrollpos != 0)
		{
			scrollpos--;
			while(text1[scrollpos] == "")
				scrollpos--;

			scroll = true;

			if( PAD_ButtonsDown(0) )
				Pad_unpressed();
		}

		if(scroll)
		{
			for(int i=0; i < (signed)text1.size(); i++)
			{
				printf("\x1b[%i;%iH", startpos_x +4 +i, startpos_y);
				if(i == scrollpos)
					printf(">>");
				else
					printf("  ");

			}
			scroll = false;
		}

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A) || PAD_ButtonsDown(0) & PAD_BUTTON_A)
		{
			menu_main_choice = scrollpos;
			switch(scrollpos)
			{
				case 0:
					return MENU_INSTALL;

				case 1:
					return MENU_REINSTALL;

				case 2:
					return MENU_UNINSTALL;

				case 4:
					return MENU_COPYRIGHT;

				default:
					return MENU_EXIT;
			}
		}
	}
}

int menu_install_uninstall(int install)
{
	Con_Clear();
	show_menu_head();

	Con_FgColor(7, 1);
	printf("\x1b[%i;%iH", startpos_x +2, startpos_y);
	if(install == 1)
		printf("Install the HomebrewFilter now");
	else if(install == 2)
		printf("Uninstall the HomebrewFilter now");
	else if(install == 2)
		printf("Reinstall the HomebrewFilter now");

	for(int i=0; i < (signed)text2.size(); i++)
	{
		printf("\x1b[%i;%iH", startpos_x +4 +i, startpos_y +3);
		printf("%s", text2[i].c_str());
	}

	int scrollpos = 1;
	bool scroll = true;
	while(1)
	{
		WPAD_ScanPads();
		PAD_ScanPads();

		if((WPAD_ButtonsDown(0) & (WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN)
			|| (PAD_ButtonsDown(0) & PAD_BUTTON_DOWN)) && scrollpos < (signed)text2.size() -1)
		{
			scrollpos++;
			scroll = true;

			if( PAD_ButtonsDown(0) )
				Pad_unpressed();
		}
		else if((WPAD_ButtonsDown(0) & (WPAD_BUTTON_UP | WPAD_CLASSIC_BUTTON_UP)
			|| (PAD_ButtonsDown(0) & PAD_BUTTON_UP)) && scrollpos != 0)
		{
			scrollpos--;
			scroll = true;

			if( PAD_ButtonsDown(0) )
				Pad_unpressed();
		}

		if(scroll)
		{
			for(int i=0; i < (signed)text2.size(); i++)
			{
				printf("\x1b[%d;12H", startpos_x +4 +i);
				if(i == scrollpos)
					printf(">>");
				else
					printf("  ");

			}
			scroll = false;
		}

		if( WPAD_ButtonsDown(0) & (WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A) || PAD_ButtonsDown(0) & PAD_BUTTON_A )
		{
			switch(scrollpos)
			{
				case 0:
					if(install == 1)
						return MENU_INSTALLING;
					else if(install == 0)
						return MENU_UNINSTALLING;
					else if(install == 2)
						return MENU_REINSTALLING;
					break;

				case 1:
					return MENU_MAIN;
			}
		}
	}
}

int menu_install()
{
	Con_Clear();
	show_menu_head();

	Con_FgColor(7, 1);
	printf("\x1b[%i;%iH", startpos_x +2, startpos_y);
	printf("Installing the HomebrewFilter");

	printf("\x1b[%i;%iH", startpos_x +4, startpos_y);
	if(Wad_InstallFromMemory(startpos_x, startpos_y) >= 0)
	{
		if(!getIOS(58))
		{
			s32 fd;
			u32 high = (u32)(GetTitleID() >> 32);
			u32 low  = (u32)(GetTitleID() & 0xFFFFFFFF);

			char filepath[ISFS_MAXPATH];
			sprintf(filepath, "/title/%08x/%08x/content/title.tmd", high, low);

			static fstats filestats ATTRIBUTE_ALIGN(32);
			static u8 filearray[1024] ATTRIBUTE_ALIGN(32);

			fd = ISFS_Open(filepath, ISFS_OPEN_READ);
			if (fd <= 0)
				ISFS_Close(fd);

			ISFS_GetFileStats(fd, &filestats);
			ISFS_Read(fd, filearray, filestats.file_length);
			ISFS_Close(fd);

			if(filestats.file_length >= 0)
			{
				fd = ISFS_Open(filepath, ISFS_OPEN_RW);

				if(getIOS(61))
					filearray[395] = 61;
				else
					filearray[395] = IOS_GetVersion();

				ISFS_Write(fd, filearray, sizeof( filearray ));
				ISFS_Close(fd);
			}
		}
	}

	Con_FgColor(7, 1);
	printf("\x1b[%i;%iH", startpos_x +8, startpos_y);
	printf(">> Continue");

	while(1)
	{
		WPAD_ScanPads();
		PAD_ScanPads();

		if( WPAD_ButtonsDown(0) & (WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A) || PAD_ButtonsDown(0) & PAD_BUTTON_A )
			return MENU_MAIN;
	}
}

int menu_reinstall()
{
	Con_Clear();
	show_menu_head();

	Con_FgColor(7, 1);
	printf("\x1b[%i;%iH", startpos_x +2, startpos_y);
	printf("Uninstalling the HomebrewFilter");

	Wad_UninstallFromMemory(startpos_x, startpos_y);

	Con_FgColor(7, 1);
	printf("\x1b[%i;%iH", startpos_x +8, startpos_y);
	printf("Installing the HomebrewFilter");

	printf("\x1b[%i;%iH", startpos_x +9, startpos_y);
	if(Wad_InstallFromMemory(startpos_x, startpos_y) >= 0)
	{
		if(!getIOS(58))
		{
			s32 fd;
			u32 high = (u32)(GetTitleID() >> 32);
			u32 low  = (u32)(GetTitleID() & 0xFFFFFFFF);

			char filepath[ISFS_MAXPATH];
			sprintf(filepath, "/title/%08x/%08x/content/title.tmd", high, low);

			static fstats filestats ATTRIBUTE_ALIGN(32);
			static u8 filearray[1024] ATTRIBUTE_ALIGN(32);

			fd = ISFS_Open(filepath, ISFS_OPEN_READ);
			if (fd <= 0)
				ISFS_Close(fd);

			ISFS_GetFileStats(fd, &filestats);
			ISFS_Read(fd, filearray, filestats.file_length);
			ISFS_Close(fd);

			if(filestats.file_length >= 0)
			{
				fd = ISFS_Open(filepath, ISFS_OPEN_RW);

				if(getIOS(61))
					filearray[395] = 61;
				else
					filearray[395] = IOS_GetVersion();

				ISFS_Write(fd, filearray, sizeof( filearray ));
				ISFS_Close(fd);
			}
		}
	}

	Con_FgColor(7, 1);
	printf("\x1b[%i;%iH", startpos_x +11, startpos_y);
	printf(">> Continue");

	while(1)
	{
		WPAD_ScanPads();
		PAD_ScanPads();
		if( WPAD_ButtonsDown(0) & (WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A) || PAD_ButtonsDown(0) & PAD_BUTTON_A )
			return MENU_MAIN;
	}

}

int menu_uninstall()
{
	Con_Clear();
	show_menu_head();

	Con_FgColor(7, 1);
	printf("\x1b[%i;%iH", startpos_x +2, startpos_y);
	printf("Uninstalling the HomebrewFilter");

	Wad_UninstallFromMemory(startpos_x, startpos_y);

	Con_FgColor(7, 1);
	printf("\x1b[%i;%iH", startpos_x +8, startpos_y);
	printf(">> Continue");

	while(1)
	{
		WPAD_ScanPads();
		PAD_ScanPads();
		if( WPAD_ButtonsDown(0) & (WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A) || PAD_ButtonsDown(0) & PAD_BUTTON_A )
			return MENU_MAIN;
	}
}

int menu_copyright()
{
	Con_Clear();
	show_menu_head();

	Con_FgColor(7, 1);

	for(int i=0; i < (signed)text3.size(); i++)
	{
		printf("\x1b[%i;%iH", startpos_x +2 +i, startpos_y);
		printf("%s", text3[i].c_str());
	}

	printf("\x1b[%i;%iH", startpos_x + text3.size() +3, startpos_y);
	printf(">> Back");

	while(1)
	{
		WPAD_ScanPads();
		PAD_ScanPads();

		if( WPAD_ButtonsDown(0) & (WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A) || PAD_ButtonsDown(0) & PAD_BUTTON_A )
			return MENU_MAIN;
	}
}
