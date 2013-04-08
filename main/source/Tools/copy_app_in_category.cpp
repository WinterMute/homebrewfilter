
#include "main.h"
#include "Menus/menus.h"

void copy_app_in_unassigned()
{
	vector<string> apps;
	vechomebrew_list_unassigned.clear();

	for(int a = 1; a < (signed)AvailableCategory.categories.size(); a++)
		for(int b = 0; b < (signed)AvailableCategory.apps[a].size(); b++)
			apps.push_back(AvailableCategory.apps[a][b]);

	for(int i = 0; i < (signed)vechomebrew_list_category[0].size(); i++)
	{
		bool present = true;

		int anzahl_device = 0, anzahl_ordner = 0;

		// Geräte SD, USB
		if(Settings.device == "sd1" || Settings.device == "usb1"
#ifndef VWII
		|| Settings.device == "dvd" || Settings.device == "gca" || Settings.device == "gcb"
#endif
		)
			anzahl_device = 1;
		else if(Settings.device == "sd_usb")
			anzahl_device = 2;
		else if(Settings.device == "all")
#ifndef VWII
			anzahl_device = 5;
#else
			anzahl_device = 2;
#endif

		// Ordner APPS, GC_APPS
		if(Settings.system == 1 || Settings.system == 0)
			anzahl_ordner = 1;
		else if(Settings.system == 2)
			anzahl_ordner = 2;

		for(int x = 0; x < anzahl_device; x++)
		{
			for(int y = 0; y < anzahl_ordner; y++)
			{
				for(int a = 0; a < (signed)apps.size(); a++)
				{
					string foldername, ordner;
#ifndef VWII
					if(Settings.system == 1)
						ordner = ":/apps/";
					else if(Settings.system == 0)
						ordner = ":/gc_apps/";
					else if(Settings.system == 2 && y == 0)
						ordner = ":/apps/";
					else if(Settings.system == 2 && y == 1)
						ordner = ":/gc_apps/";
#else
					ordner = ":/apps/";
#endif

					if(anzahl_device == 1)
						foldername = Settings.device  + ordner + apps[a] + "/";
					else
					{
						if(x == 0)
							foldername = "sd1" + ordner + apps[a] + "/";
						else if(x == 1)
							foldername = "usb1" + ordner + apps[a] + "/";
#ifndef VWII
						else if(x == 2)
							foldername = "dvd" + ordner + apps[a] + "/";
						else if(x == 3)
							foldername = "gca" + ordner + apps[a] + "/";
						else if(x == 4)
							foldername = "gcb" + ordner + apps[a] + "/";
#endif
					}
				}
			}
		}

		if(present)
		{
			vechomebrew_list_unassigned.push_back(homebrew_list(
			vechomebrew_list_category[0][i].name,
			vechomebrew_list_category[0][i].info,
			vechomebrew_list_category[0][i].foldername,
			vechomebrew_list_category[0][i].icon,
			vechomebrew_list_category[0][i].pathboot,
			vechomebrew_list_category[0][i].arg,
			vechomebrew_list_category[0][i].force_reload
			));
		}
	}
}

void copy_app_in_category(int category) //Apps im Kategorie kopieren
{
	copy_app_in_unassigned();
	vechomebrew_list_category[category].clear();

	for(int a = 0; a < (signed)AvailableCategory.apps[category].size(); a++)
	{
		for(int i = 0; i < (signed)vechomebrew_list_category[0].size(); i++)
		{
			int anzahl_device = 0, anzahl_ordner = 0;

			// Geräte SD, USB
			if(Settings.device == "sd1" || Settings.device == "usb1" 
#ifndef VWII
			|| Settings.device == "dvd" || Settings.device == "gca" || Settings.device == "gcb"
#endif
			)
				anzahl_device = 1;
			else if(Settings.device == "sd_usb")
				anzahl_device = 2;
			else if(Settings.device == "all")
#ifndef VWII
				anzahl_device = 5;
#else
				anzahl_device = 2;
#endif

			// Ordner APPS, GC_APPS
			if(Settings.system == 1 || Settings.system == 0)
				anzahl_ordner = 1;
			else if(Settings.system == 2)
				anzahl_ordner = 2;

			for(int x = 0; x < anzahl_device; x++	)
			{
				for(int y = 0; y < anzahl_ordner; y++	)
				{
					string foldername, ordner;
#ifndef VWII
					if(Settings.system == 1)
						ordner = ":/apps/";
					else if(Settings.system == 0)
						ordner = ":/gc_apps/";
					else if(Settings.system == 2 && y == 0)
						ordner = ":/apps/";
					else if(Settings.system == 2 && y == 1)
						ordner = ":/gc_apps/";
#else
						ordner = ":/apps/";
#endif

					if(anzahl_device == 1)
						foldername = Settings.device + ordner + AvailableCategory.apps[category][a] + "/";
					else
					{
						if(x == 0)
							foldername = "sd1" + ordner + AvailableCategory.apps[category][a] + "/";
						else if(x == 1)
							foldername = "usb1" + ordner + AvailableCategory.apps[category][a] + "/";
#ifndef VWII
						else if(x == 2)
							foldername = "dvd" + ordner + AvailableCategory.apps[category][a] + "/";
						else if(x == 3)
							foldername = "gca" + ordner + AvailableCategory.apps[category][a] + "/";
						else if(x == 4)
							foldername = "gcb" + ordner + AvailableCategory.apps[category][a] + "/";
#endif
					}

					if(strcasecmp(vechomebrew_list_category[0][i].foldername.c_str(), foldername.c_str()) == 0)
					{

						vechomebrew_list_category[category].push_back(homebrew_list(
						vechomebrew_list_category[0][i].name,
						vechomebrew_list_category[0][i].info,
						vechomebrew_list_category[0][i].foldername,
						vechomebrew_list_category[0][i].icon,
						vechomebrew_list_category[0][i].pathboot,
						vechomebrew_list_category[0][i].arg,
						vechomebrew_list_category[0][i].force_reload
						));

						break;
					}
				}
			}
		}
	}
}

void copy_app_in_category() //Apps im Kategorie kopieren
{
	copy_app_in_category(Settings.current_category);
}
