
#ifndef _THEME_H_
#define _THEME_H_

#include <string>
using namespace std;

struct STheme
{
	// image
	const u8*	apps_list;
	const u8*	apps_list_hover;
	const u8*	apps_grid;
	const u8*	apps_grid_hover;
	const u8*	apps_next;
	const u8*	apps_next_hover;
	const u8*	apps_previous;
	const u8*	apps_previous_hover;
	const u8*	apps_plus;
	const u8*	apps_plus_hover;
	const u8*	apps_minus;
	const u8*	apps_minus_hover;
	const u8*	background;
	const u8*	button;
	const u8*	button_focus;
	const u8*	button_small;
	const u8*	button_small_focus;
	const u8*	button_square;
	const u8*	button_square_focus;
	const u8*	button_tiny;
	const u8*	button_tiny_focus;
	const u8*	dialog_background;
	const u8*	throbber;
	const u8*	choice;
	const u8*	choice_large;
	const u8*	sd_active;
	const u8*	sd_inactive;
	const u8*	usb_active;
	const u8*	usb_inactive;
	const u8*	sd_usb_active;
	const u8*	sd_usb_inactive;
	const u8*	dvd_active;
	const u8*	dvd_inactive;
	const u8*	all_active;
	const u8*	all_inactive;
	const u8*	wii_active;
	const u8*	wii_inactive;
	const u8*	gc_active;
	const u8*	gc_inactive;
	const u8*	wii_gc_active;
	const u8*	wii_gc_inactive;
	const u8*	power_active;
	const u8*	power_inactive;
	const u8*	settings_active;
	const u8*	settings_inactive;
	const u8*	normal_active;
	const u8*	normal_inactive;
	const u8*	grid_active;
	const u8*	grid_inactive;
	const u8*	network_active;
	const u8*	network_inactive;
	const u8*	cancel_active;
	const u8*	cancel_inactive;
	const u8*	edit_active;
	const u8*	edit_inactive;
	const u8*	recycle_bin;
	const u8*	progress;
	const u8*	player_point;
	const u8*	player_grab;
	const u8*	bg_options;
	const u8*	bg_options_entry;
	const u8*	scrollbar;
	const u8*	scrollbar_arrowdown;
	const u8*	scrollbar_arrowdown_over;
	const u8*	scrollbar_arrowup;
	const u8*	scrollbar_arrowup_over;
	
	// color
	int		category_1;				// Alle
	int		category_2;
	int		category_3;
	int		page_1;					// Seite x von x
	int		page_2;
	int		page_3;
	int		owner_1;				// hamachi-mp
	int		owner_2;
	int		owner_3;
	int		title_1;				// Einstellungen
	int		title_2;
	int		title_3;
	int		program_name_1;			// Homebrew Filter
	int		program_name_2;	
	int		program_name_3;	
	int		apps_1;					// Apps
	int		apps_2;
	int		apps_3;
	int		text_1;					// text
	int		text_2;
	int		text_3;
	int		button_small_text_1;	// Kategorei erstellen. löschen, Themes
	int		button_small_text_2;
	int		button_small_text_3;
	int		button_tiny_text_1;		// Ok, Zurück, Abbrechen
	int		button_tiny_text_2;
	int		button_tiny_text_3;
	int		progressbar_color1_1;	// Ladebalken oben
	int		progressbar_color1_2;
	int		progressbar_color1_3;	
	int		progressbar_color2_1;	// Ladebalken unten
	int		progressbar_color2_2;
	int		progressbar_color2_3;	
};
extern struct STheme Theme;

void DefaultTheme();
void theme(string path);
bool theme_folder_exists(string theme);

#endif
