
#include <fstream>
#include <string>
#include "main.h"
#include "http.h"
#include "Tools/parser.h"

using namespace std;
bool newrev = false;
bool readrev = true;

string CheckNewVersions()
{
	string revs = "error";
	struct block file = downloadfile("http://hamachi-mp.bplaced.net/Downloads/wii/Homebrew_Filter/version.txt");
	if (file.data != NULL)
	{
		revs = (char*)file.data;
		free(file.data);
	}
	return revs;
}

string NewVersionsText()
{
	string text = "error";
	struct block file = downloadfile("http://hamachi-mp.bplaced.net/Downloads/wii/Homebrew_Filter/version_text.txt");
	if (file.data != NULL)
	{
		text = (char*)file.data;
		free(file.data);
	}
	return text;
}
struct block file;

string new_update(string rev, string filename)
{
	char url[100];
	if(rev == "Beta")
		sprintf(url, "http://hamachi-mp.bplaced.net/Downloads/wii/Homebrew_Filter/Beta/%s", filename.c_str());
	else
		sprintf(url, "http://hamachi-mp.bplaced.net/Downloads/wii/Homebrew_Filter/rev%s/%s", rev.c_str(), filename.c_str());

	file = downloadfile(url);
	if (file.data && file.size > 0)
		return filename;
	else
	{
		if(file.data)
			free(file.data);
		return "NULL";
	}
}

string new_theme(string themename, string filename)
{
	char url[100];
	sprintf(url, "http://hbf.hamachi-mp.bplaced.net/Themes/%s/%s", themename.c_str(), filename.c_str());

	file = downloadfile(url);
	if (file.data && file.size > 0)
		return filename;
	else
	{
		if(file.data)
			free(file.data);
		return "NULL";
	}
}

void update(string filename)
{
	if (file.data && file.size > 0)
	{
		FILE * data = fopen((Settings.device_dat + ":/config/Homebrew Filter/"+ filename).c_str(), "wb");
		if(data)
		{
			fwrite(file.data, 1, file.size, data);
			fclose(data);
		}
	}
	if(file.data)
		free(file.data);
}

bool CheckRev()
{
	if(newrev)
		return newrev;
		
	if(Settings.checkrev > SvnRev())
		newrev = true;
	
	return newrev;
}
