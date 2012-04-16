
#include <sstream>

#include "main.h"
#include "theme.h"

string get_setting(string source, string search)
{
	if((signed)source.find(search + " = ") != -1)
	{
		source.erase(0, source.find(search + " = ") + search.length() +4);
		source.erase(source.find("\""));
	}
	else 
		return "";
		
	return source;
}

void load()
{
	s32 fd;
	u32 file_size;
	static fstats filestats_settings	ATTRIBUTE_ALIGN(32);
	static fstats filestats_appios		ATTRIBUTE_ALIGN(32);
	static u8 filearray_settings[1024]	ATTRIBUTE_ALIGN(32);
	static u8 filearray_appios[1024]	ATTRIBUTE_ALIGN(32);
				
	Settings.ios_dat		= "/title/00010001/54484246/data/appios.dat";
	Settings.dir_dat		= "/title/00010001/54484246/data/list.dat";
	Settings.settings_dat	= "/title/00010001/54484246/data/settings.dat";

	AvailableCategoryLoad(Settings.dir_dat);
	
	// get settings
	fd = ISFS_Open(Settings.settings_dat.c_str(), ISFS_OPEN_READ);
	if (fd <= 0)
		ISFS_Close(fd);
	
	ISFS_GetFileStats(fd, &filestats_settings);
	
	file_size = ISFS_Read(fd, filearray_settings, filestats_settings.file_length);
	
	ISFS_Close(fd);
	
	if(file_size >= 0)
	{
		string source = (char*)filearray_settings;
		
		if(get_setting(source, "device_dat") != "")
			Settings.device_dat = get_setting(source, "device_dat");
			
		if (get_setting(source, "theme") != "")
		{
			sprintf (Options.theme, get_setting(source, "theme").c_str());
			theme(check_path(Settings.device_dat + ":/config/HBF/themes/") + Options.theme + "/");
		}
		
		if(get_setting(source, "language") != "")
		{
			sprintf (Options.language, get_setting(source, "language").c_str());
			ini_Open(check_path(Settings.device_dat + ":/config/HBF/languages/") + Options.language + ".lang");
			AvailableCategory.categories[0] = tr(Settings.category_name_all);
		}
		
		if(get_setting(source, "font") != "")
			sprintf (Options.font, get_setting(source, "font").c_str());
			
		if(get_setting(source, "slide_effect") != "")
			Options.slide_effect = atoi(get_setting(source, "slide_effect").c_str());
		
		if(get_setting(source, "last_category_name") != "")
		{
			if(KategorieNr(get_setting(source, "last_category_name")) != -1)
				Settings.current_category = KategorieNr(get_setting(source, "last_category_name").c_str());
		}
		
		if(get_setting(source, "last_category") != "")
		{
			Options.last_category = atoi(get_setting(source, "last_category").c_str());
			if(Options.last_category == 1)
				Options.last_category = Settings.current_category +1;
		}
		
		if(get_setting(source, "last_app_name") != "")
			Settings.startingAppName = get_setting(source, "last_app_name");
		
		if(get_setting(source, "apps_nr") != "")
		{
			Options.apps = atoi(get_setting(source, "apps_nr").c_str());
			if(Options.apps > 5)
				Options.apps = 5;
			else if(Options.apps < 4)
				Options.apps = 4;
		}
		
		if(get_setting(source, "quick_start") != "")
			Options.quick_start = atoi(get_setting(source, "quick_start").c_str());

		if(get_setting(source, "show_all") != "")
			Options.show_all = atoi(get_setting(source, "show_all").c_str());

		if(get_setting(source, "navigation") != "")
			Options.navigation = atoi(get_setting(source, "navigation").c_str());
		
		if(get_setting(source, "network") != "")
		{
			Options.network = atoi(get_setting(source, "network").c_str());
			Options.temp_network = Options.network;
		}
		
		if(get_setting(source, "newrevtext") != "")
		{
			Options.newrevtext = atoi(get_setting(source, "newrevtext").c_str());
			Options.temp_newrevtext = Options.newrevtext;
		}
		
		if(get_setting(source, "code") != "")
		{
			sprintf (Settings.code, get_setting(source, "code").c_str());
			strcpy (Options.temp_code, Settings.code);
		}
		
		if(get_setting(source, "grid") != "")
			Settings.grid = atoi(get_setting(source, "grid").c_str());
			
		if(get_setting(source, "device") != "")
			Settings.device = get_setting(source, "device");
			
		if(get_setting(source, "device_icon") != "")
			Options.device_icon = atoi(get_setting(source, "device_icon").c_str());
			
		if(get_setting(source, "system") != "")
			Settings.system = atoi(get_setting(source, "system").c_str());
			
		if(get_setting(source, "top") != "")
			Settings.top = atoi(get_setting(source, "top").c_str());
		
		if(get_setting(source, "bottom") != "")
			Settings.bottom = atoi(get_setting(source, "bottom").c_str());
		
		if(get_setting(source, "left") != "")
			Settings.left = atoi(get_setting(source, "left").c_str());
		
		if(get_setting(source, "right") != "")
			Settings.right = atoi(get_setting(source, "right").c_str());
	}
	
	// 	get appios
	fd = ISFS_Open(Settings.ios_dat.c_str(), ISFS_OPEN_READ);
	if (fd <= 0)
		ISFS_Close(fd);

	ISFS_GetFileStats(fd, &filestats_appios);
	
	file_size = ISFS_Read(fd, filearray_appios,  filestats_appios.file_length);
	
	ISFS_Close(fd);

	if(file_size >= 0)
	{
		string line;
		istringstream in((char*)filearray_appios);
		while(getline(in, line))
			appios.push_back(app_ios(line.substr(0, line.find(" = ")), atoi(line.substr(line.find(" = ") +3).c_str()))); 
	}
}
