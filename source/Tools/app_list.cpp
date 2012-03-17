
#include <unistd.h>
#include <fat.h>
#include <dirent.h>
#include <fstream>
#include <algorithm>

#include "main.h"
#include "Menus/menus.h"
#include "Tools/parser.h"
#include "Tools/load_channel.h"
#include "Tools/sort.h"

#include "libwiigui/gui.h"

#include "Prompts/prompts.h"
/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

void add(string device, string apps_path)
{
	/*
	char dirname[MAXPATHLEN];
	struct stat st;
	DIR_ITER* dir;
	char pathnameelf[200];
	char pathname[200];
	char pFilename[200];
	string pathboot;
	dir = diropen ((device + ":/" + apps_path).c_str());

	if(dir != NULL)
	{
		//get all dir names
		while( dirnext (dir, dirname, &st) != -1 )
		{
			sprintf(pathnameelf, (device + ":/" + apps_path + "%s/boot.elf").c_str() ,dirname);
			sprintf(pathname, (device + ":/" + apps_path + "%s/boot.dol").c_str() ,dirname);

			if(fopen(pathnameelf, "rb") != NULL) pathboot = pathnameelf;
			if(fopen(pathname, "rb") != NULL) pathboot = pathname;

			if((fopen(pathname, "rb") || fopen(pathnameelf, "rb")) && strstr(pathname, (apps_path + "./").c_str()) == 0 && strstr(pathname, (apps_path + "../").c_str()) == 0 )
			{
				sprintf(pFilename, (device + ":/" + apps_path + "%s/meta.xml").c_str() ,dirname);

				string line, quelltext, name, info, foldername, iconpath, arg;
				ifstream in(pFilename);
				while(getline(in, line))
					quelltext = quelltext + line + "\n";

				name = parser(quelltext, "<name>", "</name>");
				if(name == "\0") name = dirname;

				info = parser(quelltext, "<short_description>", "</short_description>");
				if(info == "\0") info = tr("<no description>");

				foldername = device + ":/" + apps_path + dirname + "/";
				transform(foldername.begin(), foldername.end(), foldername.begin(),::tolower);	// in kleinebuchstaben umwandeln

				u8 *tempicon;
				u8 *icon = NULL;

				iconpath = foldername + "icon.png";

				arg = parser(quelltext, "<arguments>", "</arguments>");

				size_t amount_read;
				FILE *fp = fopen(iconpath.c_str(),"r"); //open the png file
				if(fp)	//make sure the file exists
				{
					u32 filesize;
					fseek (fp , 0 , SEEK_END);
					filesize = ftell(fp); //find the file size
					rewind(fp);

					tempicon = new u8 [filesize]; //allocate memory for your image buffer
					if(tempicon)	//make sure memory allocated
					{
						amount_read = fread(tempicon,1,filesize,fp); //read file to buffer
						if(amount_read == filesize) icon = tempicon; //make sure it read everything
					}
				}
				fclose(fp); //close file

				vechomebrew_list_category[0].push_back(homebrew_list(name, info, foldername, icon, pathboot, arg));
			}
		}
		dirclose( dir );
	}*/

	DIR *pdir;
	struct dirent *pent;
//	struct stat statbuf;

	char pathnameelf[200];
	char pathname[200];
	char pathmeta[200];
	string pathboot;

	pdir=opendir((device + ":/" + apps_path).c_str());

	if(pdir != NULL)
	{
		while ((pent=readdir(pdir))!=NULL)
		{
			sprintf(pathnameelf, (device + ":/" + apps_path + "%s/boot.elf").c_str() ,pent->d_name);
			sprintf(pathname, (device + ":/" + apps_path + "%s/boot.dol").c_str() ,pent->d_name);

			if(fopen(pathnameelf, "rb") != NULL) pathboot = pathnameelf;
			if(fopen(pathname, "rb") != NULL) pathboot = pathname;

			if((fopen(pathname, "rb") || fopen(pathnameelf, "rb")) && strstr(pathname, (apps_path + "./").c_str()) == 0 && strstr(pathname, (apps_path + "../").c_str()) == 0 )
			{
				sprintf(pathmeta, (device + ":/" + apps_path + "%s/meta.xml").c_str() ,pent->d_name);

				string line, quelltext, name, info, foldername, iconpath, arg;
				ifstream in(pathmeta);
				while(getline(in, line))
					quelltext = quelltext + line + "\n";

				name = parser(quelltext, "<name>", "</name>");
				if(name == "\0") name = pent->d_name;

				info = parser(quelltext, "<short_description>", "</short_description>");
				if(info == "\0") info = tr("<no description>");

				foldername = device + ":/" + apps_path + pent->d_name + "/";
				transform(foldername.begin(), foldername.end(), foldername.begin(),::tolower);	// in kleinebuchstaben umwandeln

				u8 *tempicon;
				u8 *icon = NULL;

				iconpath = foldername + "icon.png";

				arg = parser(quelltext, "<arguments>", "</arguments>");

				size_t amount_read;
				FILE *fp = fopen(iconpath.c_str(),"r"); //open the png file
				if(fp)	//make sure the file exists
				{
					u32 filesize;
					fseek (fp , 0 , SEEK_END);
					filesize = ftell(fp); //find the file size
					rewind(fp);

					tempicon = new u8 [filesize]; //allocate memory for your image buffer
					if(tempicon)	//make sure memory allocated
					{
						amount_read = fread(tempicon,1,filesize,fp); //read file to buffer
						if(amount_read == filesize) icon = tempicon; //make sure it read everything
					}
				}
				fclose(fp); //close file

				vechomebrew_list_category[0].push_back(homebrew_list(name, info, foldername, icon, pathboot, arg));
			}
		}
		closedir(pdir);
	}
}

void app_list()
{
	vechomebrew_list_category[0].clear();

	if(Settings.device == "sd1" || Settings.device == "usb1" || Settings.device == "dvd")
	{
		if(Settings.system == 1)
			add(Settings.device, "apps/");
		else if(Settings.system == 0)
			add(Settings.device, "gc_apps/");
		else if(Settings.system == 2)
		{
			add(Settings.device, "apps/");
			add(Settings.device, "gc_apps/");
		}
	}
	else if(Settings.device == "sd_usb")
	{
		if(Settings.system == 1)
		{
			add("sd1", "apps/");
			add("usb1", "apps/");
		}
		else if(Settings.system == 0)
		{
			add("sd1", "gc_apps/");
			add("usb1", "gc_apps/");
		}
		else if(Settings.system == 2)
		{
			add("sd1", "apps/");
			add("sd1", "gc_apps/");
			add("usb1", "apps/");
			add("usb1", "gc_apps/");
		}
	}
	else if(Settings.device == "all")
	{
		if(Settings.system == 1)
		{
			add("sd1", "apps/");
			add("usb1", "apps/");
			add("dvd", "apps/");
		}
		else if(Settings.system == 0)
		{
			add("sd1", "gc_apps/");
			add("usb1", "gc_apps/");
			add("dvd", "gc_apps/");
		}
		else if(Settings.system == 2)
		{
			add("sd1", "apps/");
			add("sd1", "gc_apps/");
			add("usb1", "apps/");
			add("usb1", "gc_apps/");
			add("dvd", "apps/");
			add("dvd", "gc_apps/");
		}
	}

	// sortieren
	std::sort(vechomebrew_list_category[0].begin(),vechomebrew_list_category[0].end(), sort_name_a_z);

}

