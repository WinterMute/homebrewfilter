
#include <dirent.h>
#include "FreeTypeGX.h"
#include "filelist.h"
#include "main.h"
#include "Tools/save.h"

FreeTypeGX * fontSystem = NULL;
static FT_Byte * MainFont = (FT_Byte *) font_ttf;
static u32 MainFontSize = font_ttf_size;
static float scale = 1;

void ClearFontData()
{
    if(fontSystem)
        delete fontSystem;
    fontSystem = NULL;

	if(MainFont != (FT_Byte *) font_ttf)
    {
        if(MainFont != NULL)
            delete [] MainFont;
        MainFont = (FT_Byte *) font_ttf;
        MainFontSize = font_ttf_size;
    }
}

bool SetupDefaultFont(const char *path)
{
    bool result = false;
    FILE *pfile = NULL;
	
    ClearFontData();

    if(path)
        pfile = fopen(path, "rb");

    if(pfile)
    {
        fseek(pfile, 0, SEEK_END);
        MainFontSize = ftell(pfile);
        rewind(pfile);

        MainFont = new (std::nothrow) FT_Byte[MainFontSize];
        if(!MainFont)
        {
            MainFont = (FT_Byte *) font_ttf;
            MainFontSize = font_ttf_size;
        }
        else
        {
            fread(MainFont, 1, MainFontSize, pfile);
            result = true;
        }
        fclose(pfile);
    }

    fontSystem = new FreeTypeGX(MainFont, MainFontSize);

    return result;
}

void SetFontScale(float Scale)
{
	scale = Scale;
}

float GetFontScale()
{
	return scale;
}

void SetFont()
{
	SetupDefaultFont((check_path(Settings.device_dat + ":/config/HBF/fonts/") + Options.font).c_str());
	SetFontScale( (float)51 / (float)fontSystem->getWidth(charToWideChar("a"), 100) );	// 51 -> breite von "a" mit standart schrift
}

bool font_folder_exists()
{
	// font speicher ordner erstellen
	if(folder_exists())
	{
		DIR *pDir;

		pDir = opendir(check_path(Settings.device_dat + ":/config/HBF/Fonts").c_str());

		if(pDir != NULL)
		{
			closedir (pDir);
			return true;
		}
		else
		{
			if (mkdir((Settings.device_dat + ":/config/HBF/Fonts").c_str(), 0777) != -1)
			{
				closedir (pDir);
				return true;
			}
		}
		closedir (pDir);
	}
	return false;
}