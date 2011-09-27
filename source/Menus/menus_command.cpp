
#include <algorithm>

#include "libwiigui/gui.h"
#include "main.h"
#include "menu.h"
#include "Tools/copy_app_in_category.h"
#include "Tools/erase.h"
#include "Tools/app_list.h"
#include "menus_command.h"

struct s_move move;

void Category_rename_move(string button)
{
	// Kategorie wechseln zwischen Alle und unkategorierten
	if(Settings.current_category == 0 && button == "A")
	{
		if(Settings.unassigned)
			Settings.unassigned = false;
		else
			Settings.unassigned = true;
			
		Settings.current_page = 1;
	}
	// Kategorie umbenennen
	else if(Settings.current_category != 0 && button == "A" && strcasecmp(Settings.code,"NULL") == 0)
	{
		char new_category_name[256];
		sprintf (new_category_name, Settings.category_name.c_str());
		OnScreenKeyboard(new_category_name, 256, false);
		
		if(strcasecmp(new_category_name,"NULL") != 0 )
			KategorieUmbenennen(Settings.category_name, new_category_name);
	}

	// Kategorie verschieben
	if(Settings.current_category != 0 && button == "B" && strcasecmp(Settings.code,"NULL") == 0)
	{
		string Kategorie1 = "NULL", Kategorie2;
		bool vor;
		MoveCategory(Settings.current_category, Kategorie1, vor, Kategorie2);
		if(Kategorie1 != "NULL")
		{
			KategorieVerschieben(Kategorie1, vor, Kategorie2);
			int nr = KategorieNr(Kategorie1);
			if(nr != -1)
			{
				Settings.current_category = nr;
				copy_app_in_category();
			}
		}
	}
}

void Next_Category()
{
	Settings.current_category++;
	if(Settings.current_category > (signed)AvailableCategory.categories.size() -1)
		Settings.current_category = 0;
		
	Settings.current_page = 1;
	Settings.unassigned = false;
	
	if(Settings.current_category != 0)
		copy_app_in_category();
	
	Settings.Apps_from = EFFECT_SLIDE_RIGHT;
	Settings.Apps_to = EFFECT_SLIDE_LEFT;
}

void Previous_Category()
{
	Settings.current_category--;
	if(Settings.current_category < 0)
		Settings.current_category = AvailableCategory.categories.size() -1;
	
	Settings.current_page = 1;
	Settings.unassigned = false;
	
	if(Settings.current_category != 0)
		copy_app_in_category();
	
	Settings.Apps_from = EFFECT_SLIDE_LEFT;
	Settings.Apps_to = EFFECT_SLIDE_RIGHT;
}

void AppEraseDelate(int choice, const char* name, const char* foldername)
{
	if(choice == 3)				// App Entfernen
	{
		if( RemoveApp(name) )
		{
			AppEntfernen(Settings.category_name, foldername);
			copy_app_in_category();
		}
	}
	if(choice == 4)				// App Löschen
	{
		if( eraseDir(name) )
		{
			DeleteDir(foldername);
			
			app_list();
			
			if(Settings.current_category != 0)
				copy_app_in_category();
			else
				copy_app_in_unassigned();
		}
	}
}

