
#include "main.h"
#include <mxml.h>
//#include <zlib.h>
#include <dirent.h>
#include "Tools/save.h"

extern GuiImageData * pointer;

struct STheme Theme;

void
DefaultTheme()
{
	// image
	Theme.apps_list					= apps_list_png;
	Theme.apps_list_hover			= apps_list_hover_png;
	Theme.apps_grid					= apps_grid_png;
	Theme.apps_grid_hover			= apps_grid_hover_png;
	Theme.apps_next					= apps_next_png;
	Theme.apps_next_hover			= apps_next_hover_png;
	Theme.apps_previous				= apps_previous_png;
	Theme.apps_previous_hover		= apps_previous_hover_png;
	Theme.apps_plus					= apps_plus_png;
	Theme.apps_plus_hover			= apps_plus_hover_png;
	Theme.apps_minus				= apps_minus_png;
	Theme.apps_minus_hover			= apps_minus_hover_png;
	Theme.background				= background_png;
	Theme.button					= button_png;
	Theme.button_focus				= button_focus_png;
	Theme.button_small				= button_small_png;
	Theme.button_small_focus		= button_small_focus_png;
	Theme.button_square				= button_square_png;
	Theme.button_square_focus		= button_square_focus_png;
	Theme.button_tiny				= button_tiny_png;
	Theme.button_tiny_focus			= button_tiny_focus_png;
	Theme.dialog_background			= dialog_background_png;
	Theme.throbber					= throbber_png;
	Theme.choice					= choice_png;
	Theme.sd_active					= sd_active_png;
	Theme.sd_inactive				= sd_inactive_png;
	Theme.usb_active				= usb_active_png;
	Theme.usb_inactive				= usb_inactive_png;
	Theme.sd_usb_active				= sd_usb_active_png;
	Theme.sd_usb_inactive			= sd_usb_inactive_png;
	Theme.wii_active				= wii_active_png;
	Theme.wii_inactive				= wii_inactive_png;
	Theme.gc_active					= gc_active_png;
	Theme.gc_inactive				= gc_inactive_png;
	Theme.wii_gc_active				= wii_gc_active_png;
	Theme.wii_gc_inactive			= wii_gc_inactive_png;
	Theme.power_active				= power_active_png;
	Theme.power_inactive			= power_inactive_png;
	Theme.settings_active			= settings_active_png;
	Theme.settings_inactive			= settings_inactive_png;
	Theme.normal_active				= normal_active_png;
	Theme.normal_inactive			= normal_inactive_png;
	Theme.grid_active				= grid_active_png;
	Theme.grid_inactive				= grid_inactive_png;
	Theme.network_active			= network_active_png;
	Theme.network_inactive			= network_inactive_png;
	Theme.cancel_active				= cancel_active_png;
	Theme.cancel_inactive			= cancel_inactive_png;
	Theme.edit_active				= edit_active_png;
	Theme.edit_inactive				= edit_inactive_png;
	Theme.recycle_bin				= recycle_bin_png;
	Theme.progress					= progress_png;
	Theme.player_point				= player_point_png;
	Theme.player_grab				= player_grab_png;
	Theme.bg_options				= bg_options_png;
	Theme.bg_options_entry			= bg_options_entry_png;
	Theme.scrollbar					= scrollbar_png;
	Theme.scrollbar_arrowdown		= scrollbar_arrowdown_png;
	Theme.scrollbar_arrowdown_over	= scrollbar_arrowdown_over_png;
	Theme.scrollbar_arrowup			= scrollbar_arrowup_png;
	Theme.scrollbar_arrowup_over	= scrollbar_arrowup_over_png;
	
	// color
	Theme.category_1				= 0;
	Theme.category_2				= 0;
	Theme.category_3				= 0;
	Theme.page_1					= 255;
	Theme.page_2					= 255;
	Theme.page_3					= 255;
	Theme.owner_1					= 255;
	Theme.owner_2					= 255;
	Theme.owner_3					= 255;
	Theme.title_1					= 0;
	Theme.title_2					= 0;
	Theme.title_3					= 0;
	Theme.program_name_1			= 255;
	Theme.program_name_2			= 255;
	Theme.program_name_3			= 255;
	Theme.apps_1					= 0;
	Theme.apps_2					= 0;
	Theme.apps_3					= 0;
	Theme.text_1					= 0;
	Theme.text_2					= 0;
	Theme.text_3					= 0;
	Theme.button_small_text_1		= 0;
	Theme.button_small_text_2		= 0;
	Theme.button_small_text_3		= 0;
	Theme.progressbar_color1_1		= 255;
	Theme.progressbar_color1_2		= 255;
	Theme.progressbar_color1_3		= 255;
	Theme.progressbar_color2_1		= 80;
	Theme.progressbar_color2_2		= 160;
	Theme.progressbar_color2_3		= 255;
}

const u8* load_data(const u8* image, string path)
{
	u8 *tempicon;
	
	size_t amount_read;
	FILE *fp = fopen(path.c_str(),"r"); //open the png file
	if(fp) { //make sure the file exists

		u32 filesize;

		fseek (fp , 0 , SEEK_END);
		filesize = ftell(fp); //find the file size
		rewind(fp);

		tempicon = new u8 [filesize]; //allocate memory for your image buffer
		if(tempicon) { //make sure memory allocated

			amount_read = fread(tempicon,1,filesize,fp); //read file to buffer
			if(amount_read == filesize)
				image = tempicon; //make sure it read everything
		}
	}
	fclose(fp); //close file
	return image;
}

int split(string to_share, int share)
{
	int number = 0;
	string temp;
	
	for(int i=0; i < share; i++)
	{
		temp = to_share.substr(0, to_share.find(","));
		to_share.erase(0, to_share.find(",") +1);
		
		if(i == share -1)
		{
			number = atoi(temp.c_str());
			break;
		}
	}
	
	return number;
}

void theme(string path)
{
	// image
	Theme.apps_list					= load_data(Theme.apps_list					, path + "apps_list.png");
	Theme.apps_list_hover			= load_data(Theme.apps_list_hover			, path + "apps_list_hover.png");
	Theme.apps_grid					= load_data(Theme.apps_grid					, path + "apps_grid.png");
	Theme.apps_grid_hover			= load_data(Theme.apps_grid_hover			, path + "apps_grid_hover.png");
	Theme.apps_next					= load_data(Theme.apps_next					, path + "apps_next.png");
	Theme.apps_next_hover			= load_data(Theme.apps_next_hover			, path + "apps_next_hover.png");
	Theme.apps_previous				= load_data(Theme.apps_previous				, path + "apps_previous.png");
	Theme.apps_previous_hover		= load_data(Theme.apps_previous_hover		, path + "apps_previous_hover.png");
	Theme.apps_plus					= load_data(Theme.apps_plus					, path + "apps_plus.png");
	Theme.apps_plus_hover			= load_data(Theme.apps_plus_hover			, path + "apps_plus_hover.png");
	Theme.apps_minus				= load_data(Theme.apps_minus				, path + "apps_minus.png");
	Theme.apps_minus_hover			= load_data(Theme.apps_minus_hover			, path + "apps_minus_hover.png");
	Theme.background				= load_data(Theme.background				, path + "background.png");
	Theme.button					= load_data(Theme.button					, path + "button.png");
	Theme.button_focus				= load_data(Theme.button_focus				, path + "button_focus.png");
	Theme.button_small				= load_data(Theme.button_small				, path + "button_small.png");
	Theme.button_small_focus		= load_data(Theme.button_small_focus		, path + "button_small_focus.png");
	Theme.button_square				= load_data(Theme.button_square				, path + "button_square.png");
	Theme.button_square_focus		= load_data(Theme.button_square_focus		, path + "button_square_focus.png");
	Theme.button_tiny				= load_data(Theme.button_tiny				, path + "button_tiny.png");
	Theme.button_tiny_focus			= load_data(Theme.button_tiny_focus			, path + "button_tiny_focus.png");
	Theme.dialog_background			= load_data(Theme.dialog_background			, path + "dialog_background.png");
	Theme.throbber					= load_data(Theme.throbber					, path + "throbber.png");
	Theme.choice					= load_data(Theme.choice					, path + "choice.png");
	Theme.sd_active					= load_data(Theme.sd_active					, path + "sd_active.png");
	Theme.sd_inactive				= load_data(Theme.sd_inactive				, path + "sd_inactive.png");
	Theme.usb_active				= load_data(Theme.usb_active				, path + "usb_active.png");
	Theme.usb_inactive				= load_data(Theme.usb_inactive				, path + "usb_inactive.png");
	Theme.sd_usb_active				= load_data(Theme.sd_usb_active				, path + "sd_usb_active.png");
	Theme.sd_usb_inactive			= load_data(Theme.sd_usb_inactive			, path + "sd_usb_inactive.png");
	Theme.wii_active				= load_data(Theme.wii_active				, path + "wii_active.png");
	Theme.wii_inactive				= load_data(Theme.wii_inactive				, path + "wii_inactive.png");
	Theme.gc_active					= load_data(Theme.gc_active					, path + "gc_active.png");
	Theme.gc_inactive				= load_data(Theme.gc_inactive				, path + "gc_inactive.png");
	Theme.wii_gc_active				= load_data(Theme.wii_gc_active				, path + "wii_gc_active.png");
	Theme.wii_gc_inactive			= load_data(Theme.wii_gc_inactive			, path + "wii_gc_inactive.png");
	Theme.power_active				= load_data(Theme.power_active				, path + "power_active.png");
	Theme.power_inactive			= load_data(Theme.power_inactive			, path + "power_inactive.png");
	Theme.settings_active			= load_data(Theme.settings_active			, path + "settings_active.png");
	Theme.settings_inactive			= load_data(Theme.settings_inactive			, path + "settings_inactive.png");
	Theme.normal_active				= load_data(Theme.normal_active				, path + "normal_active.png");
	Theme.normal_inactive			= load_data(Theme.normal_inactive			, path + "normal_inactive.png");
	Theme.grid_active				= load_data(Theme.grid_active				, path + "grid_active.png");
	Theme.grid_inactive				= load_data(Theme.grid_inactive				, path + "grid_inactive.png");
	Theme.network_active			= load_data(Theme.network_active			, path + "network_active.png");
	Theme.network_inactive			= load_data(Theme.network_inactive			, path + "network_inactive.png");
	Theme.cancel_active				= load_data(Theme.cancel_active				, path + "cancel_active.png");
	Theme.cancel_inactive			= load_data(Theme.cancel_inactive			, path + "cancel_inactive.png");
	Theme.edit_active				= load_data(Theme.edit_active				, path + "edit_active.png");
	Theme.edit_inactive				= load_data(Theme.edit_inactive				, path + "edit_inactive.png");
	Theme.recycle_bin				= load_data(Theme.recycle_bin				, path + "recycle_bin.png");
	Theme.progress					= load_data(Theme.progress					, path + "progress.png");
	Theme.player_point				= load_data(Theme.player_point				, path + "player_point.png");
	Theme.player_grab				= load_data(Theme.player_grab				, path + "player_grab.png");
	Theme.bg_options				= load_data(Theme.bg_options				, path + "bg_options.png");
	Theme.bg_options_entry			= load_data(Theme.bg_options_entry			, path + "bg_options_entry.png");
	Theme.scrollbar					= load_data(Theme.scrollbar					, path + "scrollbar.png");
	Theme.scrollbar_arrowdown		= load_data(Theme.scrollbar_arrowdown		, path + "scrollbar_arrowdown.png");
	Theme.scrollbar_arrowdown_over	= load_data(Theme.scrollbar_arrowdown_over	, path + "scrollbar_arrowdown_over.png");
	Theme.scrollbar_arrowup			= load_data(Theme.scrollbar_arrowup			, path + "scrollbar_arrowup.png");
	Theme.scrollbar_arrowup_over	= load_data(Theme.scrollbar_arrowup_over	, path + "scrollbar_arrowup_over.png");

	// color
	mxml_node_t *tree;
	mxml_node_t *data;

	FILE *fp = fopen((path + "settings.xml").c_str(), "rb");
	if (fp == NULL)
	{
		fclose(fp);
	}
	else
	{
		fseek (fp , 0, SEEK_END);
		long settings_size = ftell (fp);
		rewind (fp);
		
		if (settings_size > 0)
		{
			tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
			fclose(fp);
			
			data = mxmlFindElement(tree, tree, "#color#", NULL, NULL, MXML_DESCEND);
			
			if (mxmlElementGetAttr(data,"category")) {
				Theme.category_1 = split(mxmlElementGetAttr(data,"category"), 1);
				Theme.category_2 = split(mxmlElementGetAttr(data,"category"), 2);
				Theme.category_3 = split(mxmlElementGetAttr(data,"category"), 3);
			}
			
			if (mxmlElementGetAttr(data,"page")) {
				Theme.page_1 = split(mxmlElementGetAttr(data,"page"), 1);
				Theme.page_2 = split(mxmlElementGetAttr(data,"page"), 2);
				Theme.page_3 = split(mxmlElementGetAttr(data,"page"), 3);
			}
			
			if (mxmlElementGetAttr(data,"owner")) {
				Theme.owner_1 = split(mxmlElementGetAttr(data,"owner"), 1);
				Theme.owner_2 = split(mxmlElementGetAttr(data,"owner"), 2);
				Theme.owner_3 = split(mxmlElementGetAttr(data,"owner"), 3);
			}
			
			if (mxmlElementGetAttr(data,"title")) {
				Theme.title_1 = split(mxmlElementGetAttr(data,"title"), 1);
				Theme.title_2 = split(mxmlElementGetAttr(data,"title"), 2);
				Theme.title_3 = split(mxmlElementGetAttr(data,"title"), 3);
			}
			
			if (mxmlElementGetAttr(data,"program_name")) {
				Theme.program_name_1 = split(mxmlElementGetAttr(data,"program_name"), 1);
				Theme.program_name_2 = split(mxmlElementGetAttr(data,"program_name"), 2);
				Theme.program_name_3 = split(mxmlElementGetAttr(data,"program_name"), 3);
			}
			
			if (mxmlElementGetAttr(data,"apps")) {
				Theme.apps_1 = split(mxmlElementGetAttr(data,"apps"), 1);
				Theme.apps_2 = split(mxmlElementGetAttr(data,"apps"), 2);
				Theme.apps_3 = split(mxmlElementGetAttr(data,"apps"), 3);
			}
			
			if (mxmlElementGetAttr(data,"text")) {
				Theme.text_1 = split(mxmlElementGetAttr(data,"text"), 1);
				Theme.text_2 = split(mxmlElementGetAttr(data,"text"), 2);
				Theme.text_3 = split(mxmlElementGetAttr(data,"text"), 3);
			}
			
			if (mxmlElementGetAttr(data,"button_small_text")) {
				Theme.button_small_text_1 = split(mxmlElementGetAttr(data,"button_small_text"), 1);
				Theme.button_small_text_2 = split(mxmlElementGetAttr(data,"button_small_text"), 2);
				Theme.button_small_text_3 = split(mxmlElementGetAttr(data,"button_small_text"), 3);
			}
			
			if (mxmlElementGetAttr(data,"button_tiny_text")) {
				Theme.button_tiny_text_1 = split(mxmlElementGetAttr(data,"button_tiny_text"), 1);
				Theme.button_tiny_text_2 = split(mxmlElementGetAttr(data,"button_tiny_text"), 2);
				Theme.button_tiny_text_3 = split(mxmlElementGetAttr(data,"button_tiny_text"), 3);
			}
			
			if (mxmlElementGetAttr(data,"progressbar_color1")) {
				Theme.progressbar_color1_1 = split(mxmlElementGetAttr(data,"progressbar_color1"), 1);
				Theme.progressbar_color1_2 = split(mxmlElementGetAttr(data,"progressbar_color1"), 2);
				Theme.progressbar_color1_3 = split(mxmlElementGetAttr(data,"progressbar_color1"), 3);
			}
			
			if (mxmlElementGetAttr(data,"progressbar_color2")) {
				Theme.progressbar_color2_1 = split(mxmlElementGetAttr(data,"progressbar_color2"), 1);
				Theme.progressbar_color2_2 = split(mxmlElementGetAttr(data,"progressbar_color2"), 2);
				Theme.progressbar_color2_3 = split(mxmlElementGetAttr(data,"progressbar_color2"), 3);
			}
			
			mxmlDelete(data);
			mxmlDelete(tree);
		}
		else
		{
			fclose(fp);
			unlink((path + "settings.xml").c_str());
		}
	}
}

bool theme_folder_exists(string theme)
{
	// theme speicher ordner erstellen
	DIR *pDir;

	folder_exists();
	pDir = opendir(check_path(Settings.device_dat + ":/config/HBF/Themes").c_str());

	if(pDir != NULL)
	{
		pDir = opendir(check_path(Settings.device_dat + ":/config/HBF/Themes/" + theme).c_str());
		if(pDir != NULL)
		{
			closedir (pDir);
			return true;
		}
		else
		{
			if (mkdir((Settings.device_dat + ":/config/HBF/Themes/" + theme).c_str(), 0777) != -1)
			{
				closedir (pDir);
				return true;
			}
		}
	}
	else
	{
		if (mkdir((Settings.device_dat + ":/config/HBF/Themes").c_str(), 0777) != -1)
		{
			if (mkdir((Settings.device_dat + ":/config/HBF/Themes/" + theme).c_str(), 0777) != -1)
			{
				closedir (pDir);
				return true;
			}
		}
	}
	closedir (pDir);
	return false;
}