/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * main.h
 ***************************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_

#include <string>
#include <vector>

#include "libwiigui/gui.h"
#include "FreeTypeGX.h"
#include "Tools/category.h"
#include "Tools/theme.h"
#include "Tools/translate.h"
#include "Tools/fileop.h"
#include "../svnrev/svnrev.h"

#define HW_REG_BASE   0xcd800000
#define HW_ARMIRQMASK (HW_REG_BASE + 0x03c)
#define HW_ARMIRQFLAG (HW_REG_BASE + 0x038)

using namespace std;

class homebrew_list
{
public:
	// default constructor
	homebrew_list(string name, string info, string foldername, u8 *icon, string pathboot, string arg)
	{
		this->name = name;
		this->info = info;
		this->foldername = foldername;
		this->icon = icon;
		this->pathboot = pathboot;
		this->arg = arg;
	}
	string name;
	string info;
	string foldername;
	u8 *icon;
	string pathboot;
	string arg;
};
extern std::vector<homebrew_list> vechomebrew_list_category[99];
extern std::vector<homebrew_list> vechomebrew_list_unassigned;
extern std::vector<homebrew_list> vechomebrew_list_choice;

class app_ios
{
public:
	// default constructor
	app_ios(string foldername, int ios) {
		this->foldername = foldername;
		this->ios = ios;
	}
	string foldername;
	int ios;
};
extern std::vector<app_ios> appios;
void addAppIos(string foldername, int ios);

struct SSettings
{
	string	device;
	int		sd_insert;
	int		usb_insert;
	string	category_name_all;
	string	category_name;
	char	new_category_name[256];
	int		current_category;
	int		current_page;
	int		last_app_pos;
	int		total_page;
	int		checkrev;
	char	code[5];
	
	string	device_dat;
	string	dir_dat;
	string	settings_dat;
	string	ios_dat;
	string	forwarder_path;
	string	startingAppName;
	string	forwarder_arg;
	
	int		Apps_from;
	int		Apps_to;
	
	bool	grid;
	bool	unassigned;
	int		system;					// 0 = GC  /  1 = Wii  /  2 = Wii/GC
	
	int		temp_width;
	int		temp_height;
	int		temp_horizontal;
	int		temp_vertical;

	int		left, right, bottom, top;
};
extern struct SSettings Settings;

struct SOptions
{
	int			temp_last_setting;
	string		temp_theme;
	string		temp_language;
	string		temp_font;
	char		temp_code[5];
	int			temp_network;
	int			temp_newrevtext;
	
    int			last_category;			// letzte kategorie merken
    int			slide_effect;
	char		theme[256];
	char		language[256];
	char		font[256];
	int			apps;
	bool		quick_start;
	int			device_icon;
	bool		navigation;
	bool		network;
	bool		newrevtext;
};
extern struct SOptions Options;

void ExitApp();
extern int ExitRequested;

extern s8 PowerOff;

#endif
