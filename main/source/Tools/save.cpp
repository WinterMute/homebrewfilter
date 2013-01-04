#include "main.h"
#include <dirent.h>
#include <sstream>
#include <fstream>
#include "xprintf.h"

extern const u8  banner_bin[];
extern const u32 banner_bin_size;

bool folder_exists()
{
	bool create = false;

	if(Settings.device_dat == "sd1" && SDCard_Inserted())
		create = true;
	else if(Settings.device_dat == "usb1" && USBDevice_Inserted())
		create = true;

	if(create)
	{
		// speicher ordner erstellen
		DIR *pDir;

		pDir = opendir(check_path(Settings.device_dat + ":/config").c_str());

		if(pDir != NULL)
		{
			pDir = opendir(check_path(Settings.device_dat + ":/config/HBF").c_str());
			if(pDir != NULL)
			{
				closedir (pDir);
				return true;
			}
			else
			{
				if (mkdir((Settings.device_dat + ":/config/HBF").c_str(), 0777) != -1)
					return true;
			}
		}
		else
		{
			if (mkdir((Settings.device_dat + ":/config").c_str(), 0777) != -1)
			{
				if (mkdir((Settings.device_dat + ":/config/HBF").c_str(), 0777) != -1)
					return true;
			}
		}
	}
	return false;
}


void save()
{
#if defined(STBOOT) || defined(STBOOTVWII)
	ofstream save_settings;
	save_settings.open(Settings.ios_dat.c_str());

	int last_category;
	string last_category_name;

	if(Options.last_category > 0)
	{
		last_category = 1;
		last_category_name = AvailableCategory.categories[Options.last_category - 1];
	}
	else
	{
		last_category = 0;
		last_category_name = Settings.category_name;
	}

	save_settings << "theme = \""			<< Options.theme		<< "\"" << endl;
	save_settings << "language = \""		<< Options.language		<< "\"" << endl;
	save_settings << "font = \""			<< Options.font			<< "\"" << endl;
	save_settings << "slide_effect = \""		<< Options.slide_effect		<< "\"" << endl;
	save_settings << "last_category = \""		<< last_category		<< "\"" << endl;
	save_settings << "last_category_name = \""	<< last_category_name		<< "\"" << endl;
	save_settings << "last_app_name = \""		<< Settings.startingAppName	<< "\"" << endl;
	save_settings << "apps_nr = \""			<< Options.apps			<< "\"" << endl;
	save_settings << "quick_start = \""		<< Options.quick_start		<< "\"" << endl;
	save_settings << "show_all = \""		<< Options.show_all		<< "\"" << endl;
	save_settings << "sdgecko = \""			<< Options.sdgecko		<< "\"" << endl;
#ifndef VWII
	save_settings << "bootmii_boot2 = \""		<< Options.bootmii_boot2	<< "\"" << endl;
#endif
	save_settings << "navigation = \""		<< Options.navigation		<< "\"" << endl;
	save_settings << "network = \""			<< Options.network		<< "\"" << endl;
	save_settings << "wifigecko = \""		<< Options.wifigecko		<< "\"" << endl;
	save_settings << "newrevtext = \""		<< Options.newrevtext		<< "\"" << endl;
	save_settings << "code = \""			<< Settings.code		<< "\"" << endl;
	save_settings << "grid = \""			<< Settings.grid		<< "\"" << endl;
	save_settings << "device = \""			<< Settings.device		<< "\"" << endl;
	save_settings << "device_dat = \""		<< Settings.device_dat		<< "\"" << endl;
	save_settings << "device_icon = \""		<< Options.device_icon		<< "\"" << endl;
	save_settings << "wiiload_ahb = \""		<< Options.wiiload_ahb		<< "\"" << endl;
	save_settings << "wiiload_ios = \""		<< Options.wiiload_ios		<< "\"" << endl;
	save_settings << "system = \""			<< Settings.system		<< "\"" << endl;
	save_settings << "top = \""			<< Settings.top			<< "\"" << endl;
	save_settings << "bottom = \""			<< Settings.bottom		<< "\"" << endl;
	save_settings << "left = \""			<< Settings.left		<< "\"" << endl;
	save_settings << "right = \""			<< Settings.right		<< "\"" << endl;

	save_settings.close();

	ofstream outfile;
	outfile.open(Settings.ios_dat.c_str());
	for(int i = 0; i < (signed)appios.size(); i++)
		outfile << appios[i].foldername << " = " << appios[i].ios << endl;
	outfile.close();

#else
	s32 file;

	// create save banner
	string save_banner_path = "/title/00010001/54484246/data/banner.bin";

	file = ISFS_Open(save_banner_path.c_str(), ISFS_OPEN_READ);
	if (file <= 0)
	{
		ISFS_CreateFile(save_banner_path.c_str(), 0, 3, 3, 3);

		file = ISFS_Open(save_banner_path.c_str(), ISFS_OPEN_RW);
		if (file > 0)
			ISFS_Write(file, banner_bin, banner_bin_size);

		ISFS_Close(file);
	}
	ISFS_Close(file);

	// save settings
	ISFS_Delete(Settings.settings_dat.c_str());

	ISFS_CreateFile(Settings.settings_dat.c_str(), 0, 3, 3, 3);
	file = ISFS_Open(Settings.settings_dat.c_str(), ISFS_OPEN_RW);

	if (file > 0)
	{
		int last_category;
		string last_category_name;

		if(Options.last_category > 0)
		{
			last_category = 1;
			last_category_name = AvailableCategory.categories[Options.last_category - 1];
		}
		else
		{
			last_category = 0;
			last_category_name = Settings.category_name;
		}

		stringstream save_settings;
		save_settings << "theme = \""			<< Options.theme		<< "\"" << endl;
		save_settings << "language = \""		<< Options.language		<< "\"" << endl;
		save_settings << "font = \""			<< Options.font			<< "\"" << endl;
		save_settings << "slide_effect = \""		<< Options.slide_effect		<< "\"" << endl;
		save_settings << "last_category = \""		<< last_category		<< "\"" << endl;
		save_settings << "last_category_name = \""	<< last_category_name		<< "\"" << endl;
		save_settings << "last_app_name = \""		<< Settings.startingAppName	<< "\"" << endl;
		save_settings << "apps_nr = \""			<< Options.apps			<< "\"" << endl;
		save_settings << "quick_start = \""		<< Options.quick_start		<< "\"" << endl;
		save_settings << "show_all = \""		<< Options.show_all		<< "\"" << endl;
		save_settings << "sdgecko = \""			<< Options.sdgecko		<< "\"" << endl;
#ifndef VWII
		save_settings << "bootmii_boot2 = \""		<< Options.bootmii_boot2	<< "\"" << endl;
#endif
		save_settings << "navigation = \""		<< Options.navigation		<< "\"" << endl;
		save_settings << "network = \""			<< Options.network		<< "\"" << endl;
		save_settings << "wifigecko = \""		<< Options.wifigecko		<< "\"" << endl;
		save_settings << "newrevtext = \""		<< Options.newrevtext		<< "\"" << endl;
		save_settings << "code = \""			<< Settings.code		<< "\"" << endl;
		save_settings << "grid = \""			<< Settings.grid		<< "\"" << endl;
		save_settings << "device = \""			<< Settings.device		<< "\"" << endl;
		save_settings << "device_dat = \""		<< Settings.device_dat		<< "\"" << endl;
		save_settings << "device_icon = \""		<< Options.device_icon		<< "\"" << endl;
		save_settings << "wiiload_ahb = \""		<< Options.wiiload_ahb		<< "\"" << endl;
		save_settings << "wiiload_ios = \""		<< Options.wiiload_ios		<< "\"" << endl;
		save_settings << "system = \""			<< Settings.system		<< "\"" << endl;
		save_settings << "top = \""			<< Settings.top			<< "\"" << endl;
		save_settings << "bottom = \""			<< Settings.bottom		<< "\"" << endl;
		save_settings << "left = \""			<< Settings.left		<< "\"" << endl;
		save_settings << "right = \""			<< Settings.right		<< "\"" << endl;

		char *pbuf = NULL;
		unsigned int psize = save_settings.str().size();

		pbuf = (char*)memalign( 32, sizeof(char) *psize );
		memset( pbuf, 0, sizeof(char) *psize );

		char text[psize];
		sprintf(text, "%s", save_settings.str().c_str());
		strcpy(pbuf, text);

		ISFS_Write(file, pbuf, sizeof(char) *psize);
	}
	else
	{
		xprintf("ERROR: ISFS: opening %s failed\n", Settings.settings_dat.c_str());
	}

	ISFS_Close(file);

	// save appios
	ISFS_Delete(Settings.ios_dat.c_str());

	ISFS_CreateFile(Settings.ios_dat.c_str(), 0, 3, 3, 3);
	file = ISFS_Open(Settings.ios_dat.c_str(), ISFS_OPEN_RW);
	if (file > 0)
	{
		stringstream save_appios;
		for(int i = 0; i < (signed)appios.size(); i++)
			save_appios << appios[i].foldername << " = " << appios[i].ios << endl;

		char *pbuf = NULL;
		unsigned int psize = save_appios.str().size();

		pbuf = (char*)memalign( 32, sizeof(char) *psize );
		memset( pbuf, 0, sizeof(char) *psize );

		char text[psize];
		sprintf(text, "%s", save_appios.str().c_str());
		strcpy(pbuf, text);

		ISFS_Write(file, pbuf, sizeof(char) *psize);
	}
	ISFS_Close(file);
#endif
	// save category
	AvailableCategorySave(Settings.dir_dat);

}
