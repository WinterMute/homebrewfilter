
#include <dirent.h>

#include "main.h"
#include "video.h"
#include "Tools/save.h"

void Screenshot()
{
	int i = 0;
	bool stop = false;
	char temp[100];
	
	DIR *pDir;
	bool Exists = false;

	folder_exists();
	pDir = opendir(check_path(Settings.device_dat + ":/config/Homebrew Filter/Screenshots").c_str());

	if(pDir != NULL)
	{
		Exists = true;
		closedir (pDir);
	}
	else
		if(mkdir((Settings.device_dat + ":/config/Homebrew Filter/Screenshots").c_str(), 777) != -1)
			Exists = true;

	if(Exists)
	{
		while(!stop)
		{
			sprintf (temp, "%s/%i.png", check_path(Settings.device_dat + ":/config/Homebrew Filter/Screenshots").c_str(), i);
			FILE * f= fopen (temp,"r");
			if (!f)
				stop = true;
			else 
				i++;
				
			fclose(f); // dann ging's. Also wieder zu damit
		}
		
		TakeScreenshot(temp);
	}
}