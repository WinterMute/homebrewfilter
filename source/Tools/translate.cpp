
#include <dirent.h>
#include <string> 
#include <vector>
#include <fstream>

#include "main.h"
#include "menu.h"
#include "Tools/save.h"
using namespace std;

std::vector<string> quelltext;

void ini_Open(string filename)
{
	quelltext.clear();
	string line;
	ifstream in(filename.c_str());
	while(getline(in, line))
	{
		while((signed)line.find("\r") != -1)
			line.erase(line.find("\r"), 1);
				
		quelltext.push_back(line);
	}
}

string ini_GetString(string key)
{
	string value;
	int i = 0;

	for (i = 0; i < (signed)quelltext.size(); i++)
	{
		if((signed)quelltext[i].find(key) != -1)
		{
			// leerzeichen vor dem ersten buchstaben löschen
			while(quelltext[i].substr(0, 1) == " ")
				quelltext[i].erase(0, 1);

			// leerzeichen nach dem letzen buchstaben löschen
			while(quelltext[i].substr(quelltext[i].find("=") -1, 1) == " ")
				quelltext[i].erase(quelltext[i].find("=") -1, 1);

			// überprüfen, ob vorhandener key und gesuchter key übereinstimmen
			if(key == quelltext[i].substr(0, quelltext[i].find("=")))
				break;
		}
	}

	if((signed)quelltext.size() != i)
	{
		value = quelltext[i].substr(quelltext[i].find("=") +1);
		while(value.substr(0, 1) == " ")
			value.erase(0, 1);
    }
	return value;   
}

const char* tr(string translate)
{
	string GetString = ini_GetString(translate);
	if(GetString != "")
		translate = GetString;

	return translate.c_str();
}

void translate()
{
	if(CONF_GetLanguage() == JAPANESE)
		ini_Open(check_path(Settings.device_dat + ":/config/Homebrew Filter/languages/") + "japanese.lang");
	else if(CONF_GetLanguage() == ENGLISH)
		ini_Open(check_path(Settings.device_dat + ":/config/Homebrew Filter/languages/") + "english.lang");
	else if(CONF_GetLanguage() == GERMAN)
		ini_Open(check_path(Settings.device_dat + ":/config/Homebrew Filter/languages/") + "german.lang");
	else if(CONF_GetLanguage() == FRENCH)
		ini_Open(check_path(Settings.device_dat + ":/config/Homebrew Filter/languages/") + "french.lang");
	else if(CONF_GetLanguage() == SPANISH)
		ini_Open(check_path(Settings.device_dat + ":/config/Homebrew Filter/languages/") + "spanish.lang");
}

bool language_folder_exists()
{
	// font speicher ordner erstellen
	if(folder_exists())
	{
		DIR *pDir;

		pDir = opendir(check_path(Settings.device_dat + ":/config/Homebrew Filter/Languages").c_str());

		if(pDir != NULL)
		{
			closedir (pDir);
			return true;
		}
		else
		{
			if (mkdir((Settings.device_dat + ":/config/Homebrew Filter/Languages").c_str(), 0777) != -1)
			{
				closedir (pDir);
				return true;
			}
		}
		closedir (pDir);
	}
	return false;
}