
#include <unistd.h>
#include <algorithm>

#include "libwiigui/gui.h"
#include "main.h"
#include "menu.h"
#include "menus_command.h"
#include "Prompts/prompts.h"
#include "Tools/copy_app_in_category.h"
#include "Tools/sort.h"
#include "Network/network.h"
#include "Network/update.h"
#include "Network/tcp.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;
extern bool boothomebrew;
extern bool boot_buffer;
extern struct s_move move;
extern GuiImageData * pointer;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern void HaltResumeGui();

bool grab = false;
// slide temporär aktivieren
bool temp_slide = true;

/****************************************************************************
 * MenuSettings
 ***************************************************************************/
int MenuMain()
{
	int menu = MENU_NONE;
	string	button;
	bool	networkswitch = true;
	int		networtwaittime = 0;
	Settings.total_page = 1;
	bool vor = false;
	
	int app_pos = -1;
	int apps_row = 3;
	int apps_numers = Options.apps * apps_row;

	const u8* temp_apps_btn				= NULL;
	const u8* temp_apps_btnOver			= NULL;
	const u8* temp_normal_grid_inactive	= NULL;
	const u8* temp_normal_grid_active	= NULL;
	const u8* temp_sd_usb_active		= NULL;
	const u8* temp_sd_usb_inactive		= NULL;
	const u8* temp_wii_gc_active		= NULL;
	const u8* temp_wii_gc_inactive		= NULL;

	/************************************************************************
	 * icons zuweisen
	 ***********************************************************************/
	
	// normal / grid ansicht
	if(Settings.grid)
	{
		temp_apps_btn = Theme.apps_grid;
		temp_apps_btnOver = Theme.apps_grid_hover;
		
		// symbol für normale ansicht
		temp_normal_grid_inactive = Theme.grid_inactive;
		temp_normal_grid_active = Theme.grid_active;
	}
	else if(!Settings.grid)
	{
		temp_apps_btn = Theme.apps_list;
		temp_apps_btnOver = Theme.apps_list_hover;
		
		// symbol für grid ansicht
		temp_normal_grid_inactive = Theme.normal_inactive;
		temp_normal_grid_active = Theme.normal_active;
		
		apps_numers = Options.apps;
		apps_row = 1;
	}
	
	// sd / usb ansicht
	if(Settings.device == "sd1")
	{
		temp_sd_usb_active = Theme.sd_active;
		temp_sd_usb_inactive = Theme.sd_inactive;
	}
	else if(Settings.device == "usb1")
	{
		temp_sd_usb_active = Theme.usb_active;
		temp_sd_usb_inactive = Theme.usb_inactive;
	}
	else if(Settings.device == "sd_usb")
	{
		temp_sd_usb_active = Theme.sd_usb_active;
		temp_sd_usb_inactive = Theme.sd_usb_inactive;
	}
	
	// wii / gc ansicht
	if(Settings.system == 1)
	{
		temp_wii_gc_active = Theme.wii_active;
		temp_wii_gc_inactive = Theme.wii_inactive;
	}
	else if(Settings.system == 0)
	{
		temp_wii_gc_active = Theme.gc_active;
		temp_wii_gc_inactive = Theme.gc_inactive;
	}
	else if(Settings.system == 2)
	{
		temp_wii_gc_active = Theme.wii_gc_active;
		temp_wii_gc_inactive = Theme.wii_gc_inactive;
	}
	
	GuiTrigger trigA;
	GuiTrigger trigB;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigB.SetSimpleTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);
	
	// Seite der zuletzt gestarteten app
	if(Settings.last_app_pos != -1)
	{
		Settings.current_page = Settings.last_app_pos/apps_numers +1;
		app_pos = Settings.last_app_pos;
		Settings.last_app_pos = -1;
	}

	if(Settings.unassigned)
	{
		vechomebrew_list_choice = vechomebrew_list_unassigned;
		// Kategoriename
		Settings.category_name = tr(Settings.category_name_all);
		Settings.category_name += " - ";
		Settings.category_name += tr("unassigned");
	}
	else
	{
		vechomebrew_list_choice = vechomebrew_list_category[Settings.current_category];
		// Kategoriename
		Settings.category_name = AvailableCategory.categories[Settings.current_category];
	}

	GuiText categoryTxt(Settings.category_name.c_str(), 28, (GXColor){Theme.category_1, Theme.category_2, Theme.category_3, 255});
	categoryTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	categoryTxt.SetPosition(0,30);
	
	GuiButton categoryBtn(categoryTxt.GetTextWidth(), 28);
	categoryBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	categoryBtn.SetPosition(0, 30);
	categoryBtn.SetTrigger(&trigA);		
	categoryBtn.SetTrigger(&trigB);		

	GuiImageData LogoHomebrewFilterImgData(LogoHomebrewFilter_png);
	GuiImage LogoHomebrewFilterImg(&LogoHomebrewFilterImgData);
	LogoHomebrewFilterImg.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	LogoHomebrewFilterImg.SetPosition(30, 420);
	LogoHomebrewFilterImg.SetScale(0.65);
	
	GuiText ownerTxt("hamachi-mp", 18, (GXColor){Theme.owner_1, Theme.owner_2, Theme.owner_3, 255});
	ownerTxt.SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	ownerTxt.SetPosition(-35,430);

	// button data
	GuiImageData apps_next(Theme.apps_next);
	GuiImageData apps_next_hover(Theme.apps_next_hover);
	GuiImageData apps_previous(Theme.apps_previous);
	GuiImageData apps_previous_hover(Theme.apps_previous_hover);
	GuiImageData apps_plus(Theme.apps_plus);
	GuiImageData apps_plus_hover(Theme.apps_plus_hover);
	GuiImageData apps_minus(Theme.apps_minus);
	GuiImageData apps_minus_hover(Theme.apps_minus_hover);
	GuiImageData btnOutline(Theme.button_small);
	GuiImageData btnOutlineOver(Theme.button_small_focus);
	GuiImageData apps_btn(temp_apps_btn);
	GuiImageData apps_btnOver(temp_apps_btnOver);
	GuiImageData sd_usb_BtnImgData(temp_sd_usb_inactive);
	GuiImageData sd_usb_BtnImgDataOver(temp_sd_usb_active);
	GuiImageData wii_gc_BtnImgData(temp_wii_gc_inactive);
	GuiImageData wii_gc_BtnImgDataOver(temp_wii_gc_active);
	GuiImageData power_BtnImgData(Theme.power_inactive);
	GuiImageData power_BtnImgDataOver(Theme.power_active);
	GuiImageData settings_BtnImgData(Theme.settings_inactive);
	GuiImageData settings_BtnImgDataOver(Theme.settings_active);
	GuiImageData normal_grid_BtnImgData(temp_normal_grid_inactive);
	GuiImageData normal_grid_BtnImgDataOver(temp_normal_grid_active);
	GuiImageData network_BtnImgData(Theme.network_inactive);
	GuiImageData network_BtnImgDataOver(Theme.network_active);
	
	// button
	GuiImage rightBtnImg(&apps_next);
	GuiImage leftBtnImg(&apps_previous);
	GuiImage plusBtnImg(&apps_plus);
	GuiImage minusBtnImg(&apps_minus);
	GuiImage settingBtnImg(&btnOutline);
	GuiImage sd_usb_BtnImg(&sd_usb_BtnImgData);
	GuiImage wii_gc_BtnImg(&wii_gc_BtnImgData);
	GuiImage power_BtnImg(&power_BtnImgData);
	GuiImage settings_BtnImg(&settings_BtnImgData);
	GuiImage normal_grid_BtnImg(&normal_grid_BtnImgData);
	GuiImage network_BtnImg(&network_BtnImgData);
	GuiImage * appsBtnImg[vechomebrew_list_choice.size()];
	for(int i=0; i < (signed)vechomebrew_list_choice.size(); i++)
		appsBtnImg[i] = new GuiImage(&apps_btn);

	// button over
	GuiImage rightBtnImgOver(&apps_next_hover);
	GuiImage leftBtnImgOver(&apps_previous_hover);
	GuiImage plusBtnImgOver(&apps_plus_hover);
	GuiImage minusBtnImgOver(&apps_minus_hover);
	GuiImage settingBtnImgOver(&btnOutlineOver);
	GuiImage sd_usb_BtnImgOver(&sd_usb_BtnImgDataOver);
	GuiImage wii_gc_BtnImgOver(&wii_gc_BtnImgDataOver);
	GuiImage power_BtnImgOver(&power_BtnImgDataOver);
	GuiImage settings_BtnImgOver(&settings_BtnImgDataOver);
	GuiImage normal_grid_BtnImgOver(&normal_grid_BtnImgDataOver);
	GuiImage network_BtnImgOver(&network_BtnImgDataOver);
	GuiImage * appsBtnImgOver[vechomebrew_list_choice.size()];
	for(int i=0; i < (signed)vechomebrew_list_choice.size(); i++)
		appsBtnImgOver[i] = new GuiImage(&apps_btnOver);

	GuiButton rightBtn(apps_next.GetWidth(), apps_next.GetHeight());
	rightBtn.SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	rightBtn.SetPosition(-30, 175);
	if(Options.apps == 5)
		rightBtn.SetPosition(-30, 180);
	rightBtn.SetImage(&rightBtnImg);
	rightBtn.SetImageOver(&rightBtnImgOver);
	rightBtn.SetTrigger(&trigA);
	rightBtn.SetEffectGrow();

	GuiButton leftBtn(apps_previous.GetWidth(), apps_previous.GetHeight());
	leftBtn.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	leftBtn.SetPosition(30, 175);
	if(Options.apps == 5)
		leftBtn.SetPosition(30, 180);
	leftBtn.SetImage(&leftBtnImg);
	leftBtn.SetImageOver(&leftBtnImgOver);
	leftBtn.SetTrigger(&trigA);
	leftBtn.SetEffectGrow();

	GuiButton plusBtn(apps_plus.GetWidth(), apps_plus.GetHeight());
	plusBtn.SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	plusBtn.SetPosition(-54, 20);
	plusBtn.SetImage(&plusBtnImg);
	plusBtn.SetImageOver(&plusBtnImgOver);
	plusBtn.SetTrigger(&trigA);
	plusBtn.SetEffectGrow();

	GuiButton minusBtn(apps_minus.GetWidth(), apps_minus.GetHeight());
	minusBtn.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	minusBtn.SetPosition(54, 20);
	minusBtn.SetImage(&minusBtnImg);
	minusBtn.SetImageOver(&minusBtnImgOver);
	minusBtn.SetTrigger(&trigA);
	minusBtn.SetEffectGrow();

	// sd / usb
	GuiButton sd_usb_Btn(sd_usb_BtnImgData.GetWidth(), sd_usb_BtnImgData.GetHeight());
	sd_usb_Btn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	sd_usb_Btn.SetPosition(-135, 355);
	if(Options.apps == 5)
		sd_usb_Btn.SetPosition(-135, 375);
	sd_usb_Btn.SetImage(&sd_usb_BtnImg);
	sd_usb_Btn.SetImageOver(&sd_usb_BtnImgOver);
	sd_usb_Btn.SetTrigger(&trigA);
	sd_usb_Btn.SetEffectGrow();

	// wii / gc
	GuiButton wii_gc_Btn(wii_gc_BtnImgData.GetWidth(), wii_gc_BtnImgData.GetHeight());
	wii_gc_Btn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	wii_gc_Btn.SetPosition(-185, 355);
	if(Options.apps == 5)
		wii_gc_Btn.SetPosition(-185, 375);
	wii_gc_Btn.SetImage(&wii_gc_BtnImg);
	wii_gc_Btn.SetImageOver(&wii_gc_BtnImgOver);
	wii_gc_Btn.SetTrigger(&trigA);
	wii_gc_Btn.SetEffectGrow();

	// normal / grid ansicht
	GuiButton normal_grid_Btn(normal_grid_BtnImgData.GetWidth(), normal_grid_BtnImgData.GetHeight());
	normal_grid_Btn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	normal_grid_Btn.SetPosition(0, 355);
	if(Options.apps == 5)
		normal_grid_Btn.SetPosition(0, 375);
	normal_grid_Btn.SetImage(&normal_grid_BtnImg);
	normal_grid_Btn.SetImageOver(&normal_grid_BtnImgOver);
	normal_grid_Btn.SetTrigger(&trigA);
	normal_grid_Btn.SetEffectGrow();
	
	// settings
	GuiButton settings_Btn(settings_BtnImgData.GetWidth(), settings_BtnImgData.GetHeight());
	settings_Btn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	settings_Btn.SetPosition(135, 355);
	if(Options.apps == 5)
		settings_Btn.SetPosition(135, 375);
	settings_Btn.SetImage(&settings_BtnImg);
	settings_Btn.SetImageOver(&settings_BtnImgOver);
	settings_Btn.SetTrigger(&trigA);
	settings_Btn.SetEffectGrow();

	// power
	GuiButton power_Btn(power_BtnImgData.GetWidth(), power_BtnImgData.GetHeight());
	power_Btn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	power_Btn.SetPosition(185, 355);
	if(Options.apps == 5)
		power_Btn.SetPosition(185, 375);
	power_Btn.SetImage(&power_BtnImg);
	power_Btn.SetImageOver(&power_BtnImgOver);
	power_Btn.SetTrigger(&trigA);
	power_Btn.SetEffectGrow();

	// network ansicht
	GuiButton network_Btn(network_BtnImgData.GetWidth(), network_BtnImgData.GetHeight());
	network_Btn.SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	network_Btn.SetPosition(-50, 362);
	if(Options.apps == 5)
		network_Btn.SetPosition(-50, 382);
	if(IsNetworkInit())
		network_Btn.SetImage(&network_BtnImgOver);
	else
		network_Btn.SetImage(&network_BtnImg);
	network_Btn.SetImageOver(&network_BtnImgOver);
	network_Btn.SetTrigger(&trigA);
	network_Btn.SetEffectGrow();

	// alle button der kategorie erstellen
	GuiWindow Apps(screenwidth, screenheight);
	GuiButton * AppsBtn[vechomebrew_list_choice.size()];
	
	int x = screenwidth / 2 - (apps_btn.GetWidth() * apps_row) / 2;
	int y = 70;
	if(Options.apps == 5)
		y = 55;
	int app_grid = 0;
	
	// position der apps
	vector<int> pos_x;
	vector<int> pos_y;

	// auflisten der apps
	int anzahl_pro_seite = 0;
	for(int i=0; i < (signed)vechomebrew_list_choice.size(); i++)
	{
		if(anzahl_pro_seite == apps_numers)		// Apps pro Seite
		{
			y = 70;
			if(Options.apps == 5)
				y = 55;
			anzahl_pro_seite=1;
			Settings.total_page++;
		}
		else
			anzahl_pro_seite++;
			
		// device symbol anzeigen
		GuiImage * viewdevice = NULL;
		
		if((Options.device_icon == 1 || Options.device_icon == 3) && Settings.device == "sd_usb")
		{
			bool icon = false;
			if(strncmp(vechomebrew_list_choice[i].foldername.c_str(), "sd", 2) == 0)
			{
				viewdevice = new GuiImage(new GuiImageData(Theme.sd_inactive));
				icon = true;
			}
			else if(strncmp(vechomebrew_list_choice[i].foldername.c_str(), "usb", 3) == 0)
			{
				viewdevice = new GuiImage(new GuiImageData(Theme.usb_inactive));
				icon = true;
			}
			
			if(icon)
			{
				viewdevice->SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
				viewdevice->SetPosition(15, -10);
				viewdevice->SetScale(0.75);
			}
		}
		
		GuiImage * viewicon = new GuiImage(new GuiImageData(vechomebrew_list_choice[i].icon));
		GuiText * AppsBtnTxt1 = NULL;
		GuiText * AppsBtnTxt2 = NULL;
		GuiText * AppsBtnTxt3 = NULL;
		
		// Grid Ansicht
		if(Settings.grid)
		{
			if(vechomebrew_list_choice[i].icon == NULL)
			{
				AppsBtnTxt1 = new GuiText(vechomebrew_list_choice[i].name.c_str(), 20, (GXColor){Theme.apps_1, Theme.apps_2, Theme.apps_3, 255});
				AppsBtnTxt1->SetMaxWidth(100, SCROLL_HORIZONTAL);
			}
			viewicon->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
		}
		// Normale Ansicht
		else
		{
			AppsBtnTxt1 = new GuiText(vechomebrew_list_choice[i].name.c_str(), 20, (GXColor){Theme.apps_1, Theme.apps_2, Theme.apps_3, 255});
			AppsBtnTxt2 = new GuiText("....................................................", 24, (GXColor){Theme.apps_1, Theme.apps_2, Theme.apps_3, 100});
			AppsBtnTxt3 = new GuiText(tr(vechomebrew_list_choice[i].info.c_str()), 20, (GXColor){Theme.apps_1, Theme.apps_2, Theme.apps_3, 255});
			
			viewicon->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
			viewicon->SetPosition(10, 0);
			AppsBtnTxt1->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
			AppsBtnTxt2->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
			AppsBtnTxt3->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
			AppsBtnTxt1->SetPosition(160, 10);
			AppsBtnTxt1->SetMaxWidth(265, DOTTED);
			AppsBtnTxt2->SetPosition(160, 17);
			AppsBtnTxt2->SetMaxWidth(260, DOTTED);
			AppsBtnTxt3->SetPosition(160, 40);
			AppsBtnTxt3->SetMaxWidth(265, DOTTED);
		}
		
		AppsBtn[i] = new GuiButton(apps_btn.GetWidth(), apps_btn.GetHeight());
		AppsBtn[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
		AppsBtn[i]->SetPosition(x, y);
		AppsBtn[i]->SetLabel(AppsBtnTxt1, 0);
		AppsBtn[i]->SetLabel(AppsBtnTxt2, 1);
		AppsBtn[i]->SetLabel(AppsBtnTxt3, 2);
		AppsBtn[i]->SetImage(appsBtnImg[i]);
		AppsBtn[i]->SetImageOver(appsBtnImgOver[i]);
		AppsBtn[i]->SetTrigger(&trigA);
		AppsBtn[i]->SetTrigger(&trigB);
		AppsBtn[i]->SetIcon(viewicon, 0);
		AppsBtn[i]->SetIcon(viewdevice, 1);
		if(i >= apps_numers)
		{
			AppsBtn[i]->SetVisible(false);
			AppsBtn[i]->SetSelectable(false);
		}
		
		pos_x.push_back(x);
		pos_y.push_back(y);
		
		app_grid++;
		
		// wenn zeile voll, zur nächsten gehen
		if(app_grid == apps_row)
		{
			x = screenwidth / 2 - (apps_btn.GetWidth() * apps_row) / 2;
			if(Options.apps == 4)
				y += 70;
			else if(Options.apps == 5)
				y += 63;
			app_grid = 0;
		}
		else
			x += 144;
	}
	
	if(Settings.current_page > Settings.total_page)
		Settings.current_page = Settings.total_page;
		
	// wenn nicht seite 1
	if(Settings.current_page != 1)
	{
		for(int i=0; i < (signed)vechomebrew_list_choice.size(); i++)
		{
			AppsBtn[i]->SetVisible(false);
			AppsBtn[i]->SetSelectable(false);
		}
		
		for(int i = 0; i < apps_numers; i++)
		{
			if((Settings.current_page-1)*apps_numers+i < (signed)vechomebrew_list_choice.size())
			{
				AppsBtn[(Settings.current_page-1)*apps_numers+i]->SetVisible(true);
				AppsBtn[(Settings.current_page-1)*apps_numers+i]->SetSelectable(true);
			}
		}
	}

	// move button
	GuiButton * AppsBtnMove = NULL;
	GuiImageData * icon_dataMove;
	GuiImage * ViewIconMove;
	GuiText * AppsBtnTxtMove1 = new GuiText("", 20, (GXColor){Theme.apps_1, Theme.apps_2, Theme.apps_3, 255});
	GuiText * AppsBtnTxtMove2 = new GuiText("", 24, (GXColor){Theme.apps_1, Theme.apps_2, Theme.apps_3, 100});
	GuiText * AppsBtnTxtMove3 = new GuiText("", 20, (GXColor){Theme.apps_1, Theme.apps_2, Theme.apps_3, 255});
	GuiImage * appsBtnImgMove = new GuiImage(&apps_btnOver);
	AppsBtnMove = new GuiButton(apps_btn.GetWidth(), apps_btn.GetHeight());
	AppsBtnMove->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	AppsBtnMove->SetLabel(AppsBtnTxtMove1, 0);
	AppsBtnMove->SetLabel(AppsBtnTxtMove2, 1);
	AppsBtnMove->SetLabel(AppsBtnTxtMove3, 2);
	AppsBtnMove->SetImage(appsBtnImgMove);
	AppsBtnMove->SetVisible(false);

	// platzhalter
	GuiButton * AppsBtnFree = NULL;
	GuiImage * appsBtnImgFree = new GuiImage(&apps_btn);
	appsBtnImgFree->SetAlpha(100);
	AppsBtnFree = new GuiButton(apps_btn.GetWidth(), apps_btn.GetHeight());
	AppsBtnFree->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	AppsBtnFree->SetImage(appsBtnImgFree);
	AppsBtnFree->SetVisible(false);

	// page
	char buffer[20];
	sprintf(buffer, tr("Page %i of %i"), Settings.current_page, Settings.total_page);	
	GuiText * pageTxt = new GuiText(buffer, 20, (GXColor){Theme.page_1, Theme.page_2, Theme.page_3, 255});
	pageTxt->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	pageTxt->SetPosition(0,430);

	// new rev
	GuiText newRevTxt(tr("New Rev is available for download"), 16, (GXColor){Theme.page_1, Theme.page_2, Theme.page_3, 255});
	newRevTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	newRevTxt.SetPosition(0,415);

	if(Options.slide_effect > 0 && temp_slide)
		Apps.SetEffect(Settings.Apps_from | EFFECT_SLIDE_IN, Options.slide_effect * 50);
	
	// slide temporär aktivieren
	temp_slide = true;

	HaltGui();
	Apps.Append(&categoryTxt);
	for(int i=0; i < (signed)vechomebrew_list_choice.size(); i++)
		Apps.Append(AppsBtn[i]);
	Apps.Append(AppsBtnFree);
	Apps.Append(AppsBtnMove);
		
	GuiWindow w(screenwidth, screenheight);
	w.Append(&categoryBtn);
	w.Append(&LogoHomebrewFilterImg);
	w.Append(&ownerTxt);
	w.Append(&leftBtn);
	w.Append(&rightBtn);
	w.Append(&minusBtn);
	w.Append(&plusBtn);
	w.Append(&sd_usb_Btn);
	w.Append(&wii_gc_Btn);
	w.Append(&normal_grid_Btn);
	w.Append(&settings_Btn);
	w.Append(&power_Btn);
	w.Append(&network_Btn);
	w.Append(pageTxt);
	if(CheckRev() && Options.newrevtext)
		w.Append(&newRevTxt);

	mainWindow->Append(&Apps);
	mainWindow->Append(&w);

	mainWindow->ChangeFocus(&Apps);
	ResumeGui();

	// letzte app markieren
	if(app_pos != -1)
	{
	//	mainWindow->ChangeFocus(&Apps);
		for(int i=0; i < (signed)vechomebrew_list_choice.size(); i++)
		{
			if(i == app_pos)
				AppsBtn[i]->SetState(STATE_SELECTED);
			else
				AppsBtn[i]->ResetState();
		}
	}
	
	bool next_page = false;
	bool previous_page = false;
	
	while(menu == MENU_NONE)
	{
		usleep(100);
		
		if(mainWindow->GetState() != STATE_DISABLED)
		{
			// Sortieren
			if(WPAD_ButtonsDown(0) & WPAD_BUTTON_1 && Settings.current_category != 0)
			{
				switch (sortPrompt())
				{
					case NAME_A_Z:
						sort(vechomebrew_list_category[Settings.current_category].begin(), vechomebrew_list_category[Settings.current_category].end(), sort_name_a_z);
						break;
					case NAME_Z_A:
						sort(vechomebrew_list_category[Settings.current_category].begin(), vechomebrew_list_category[Settings.current_category].end(), sort_name_z_a);
						break;
					case FOLDERNAME_A_Z:
						sort(vechomebrew_list_category[Settings.current_category].begin(), vechomebrew_list_category[Settings.current_category].end(), sort_foldername_a_z);
						break;
					case FOLDERNAME_Z_A:
						sort(vechomebrew_list_category[Settings.current_category].begin(), vechomebrew_list_category[Settings.current_category].end(), sort_foldername_z_a);
						break;
					default:
						break;
				}
				sort_apps();
				menu = MENU_MAIN;
			}
			
			// Netzwerksymbol anzeigen
			if(IsNetworkInit())
			{
				network_Btn.SetImage(&network_BtnImgOver);
				ResumeTcpThread();
				if(boot_buffer)
					menu = MENU_EXIT;
			}
			else if(!IsNetworkInit() || IsNetworkError())
				network_Btn.SetImage(&network_BtnImg);
			else
			{
				networtwaittime++;
				if(networkswitch && networtwaittime == 10000)
				{
					networtwaittime = 0;
					networkswitch = false;
					network_Btn.SetImage(&network_BtnImgOver);
				}
				else if(!networkswitch && networtwaittime == 10000)
				{
					networtwaittime = 0;
					networkswitch = true;
					network_Btn.SetImage(&network_BtnImg);
				}
			}

			// Beenden
			if(PowerOff != -1)
				menu = MENU_EXIT;
			
			// ansicht wechseln
			if(((WPAD_ButtonsDown(0) & WPAD_BUTTON_2) &! (WPAD_ButtonsDown(0) & WPAD_BUTTON_1)) || normal_grid_Btn.GetState() == STATE_CLICKED)
			{
				Settings.current_page = 1;
				if(Settings.grid)
					Settings.grid = false;
				else
					Settings.grid = true;
					
				menu = MENU_MAIN;
			}
			
			// wenn A gedrückt
			if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A) || PAD_ButtonsDown(0) & PAD_BUTTON_A)
				button = "A";
				
			// wenn B gedrückt
			if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B) || PAD_ButtonsDown(0) & PAD_BUTTON_B)
				button = "B";
			
			// Settings
			if(settings_Btn.GetState() == STATE_CLICKED)
			{
				menu = MENU_SETTINGS;
			}
			
			// SD, USB
			else if(sd_usb_Btn.GetState() == STATE_CLICKED || Settings.sd_insert == -1 || Settings.sd_insert == 2 || Settings.usb_insert == -1 || Settings.usb_insert == 2)
			{
				int device = -1;
				if(sd_usb_Btn.GetState() == STATE_CLICKED)
					device = devicePrompt();
				
				if(device == 1)
					Settings.device = "sd1";
				else if(device == 2)
					Settings.device = "usb1";
				else if(device == 3)
					Settings.device = "sd_usb";
					
				if(device != -1 || Settings.sd_insert == -1 || Settings.sd_insert == 2 || Settings.usb_insert == -1 || Settings.usb_insert == 2)
				{
					check_device();
					Settings.current_page = 1;
					menu = MENU_MAIN;
				}
				else
					sd_usb_Btn.ResetState();
			}
			
			// Wii, GC
			else if(wii_gc_Btn.GetState() == STATE_CLICKED)
			{
				int choice = systemPrompt();
				
				if(choice == 0)
					Settings.system = 1;
				else if(choice == 1)
					Settings.system = 0;
				else if(choice == 2)
					Settings.system = 2;
					
				if(choice != -1)
				{
					check_device();
					Settings.current_page = 1;
					menu = MENU_MAIN;
				}
				else
					wii_gc_Btn.ResetState();
			}
			
			// Power button
			else if(power_Btn.GetState() == STATE_CLICKED || WPAD_ButtonsDown(0) & (WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME) || PAD_ButtonsDown(0) & PAD_BUTTON_START)
			{
				power_Btn.ResetState();
				if(endPrompt() == MENU_EXIT)
					menu = MENU_EXIT;
			}
			
			// Netzwerk button
			else if(network_Btn.GetState() == STATE_CLICKED)
			{
				network_Btn.ResetState();
				ResumeNetworkThread();
			}
			
			// eine Seite weiter bzw zurück
			else if((WPAD_ButtonsDown(0) & (WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT) && !Options.navigation) || 
					(WPAD_ButtonsDown(0) & (WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS) && Options.navigation) || 
					PAD_ButtonsDown(0) & PAD_BUTTON_RIGHT || rightBtn.GetState() == STATE_CLICKED ||
					(WPAD_ButtonsDown(0) & (WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT) && !Options.navigation) || 
					(WPAD_ButtonsDown(0) & (WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS) && Options.navigation) || 
					PAD_ButtonsDown(0) & PAD_BUTTON_LEFT || leftBtn.GetState() == STATE_CLICKED)
			{
				// eine Seite weiter
				if((WPAD_ButtonsDown(0) & (WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT) && !Options.navigation) || 
					(WPAD_ButtonsDown(0) & (WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS) && Options.navigation) || 
					PAD_ButtonsDown(0) & PAD_BUTTON_RIGHT || rightBtn.GetState() == STATE_CLICKED)
					previous_page = false;
					
				// eine Seite zurück
				if((WPAD_ButtonsDown(0) & (WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT) && !Options.navigation) || 
					(WPAD_ButtonsDown(0) & (WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS) && Options.navigation) || 
					PAD_ButtonsDown(0) & PAD_BUTTON_LEFT || leftBtn.GetState() == STATE_CLICKED)
					next_page = false;
					
				int page = Settings.current_page;
				bool change = false;
				if(next_page || rightBtn.GetState() == STATE_CLICKED || (WPAD_ButtonsDown(0) & (WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS) && Options.navigation))
				{
					rightBtn.ResetState();
					Settings.current_page++;
					if(Settings.current_page > Settings.total_page)
						Settings.current_page = 1;
					
					Settings.Apps_from = EFFECT_SLIDE_RIGHT;
					Settings.Apps_to = EFFECT_SLIDE_LEFT;
					
					next_page = false;
					change = true;
				}
				
				if(previous_page || leftBtn.GetState() == STATE_CLICKED || (WPAD_ButtonsDown(0) & (WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS) && Options.navigation))
				{
					leftBtn.ResetState();
					Settings.current_page--;
					if(Settings.current_page == 0)
						Settings.current_page = Settings.total_page;
					
					Settings.Apps_from = EFFECT_SLIDE_LEFT;
					Settings.Apps_to = EFFECT_SLIDE_RIGHT;
					
					previous_page = false;
					change = true;
				}
				
				if(change)
				{
					HaltResumeGui(); // -> um nur eine Seite zu blättern

					// alten apps ausblenden
					if(Options.slide_effect > 0 && Settings.Apps_to != 0)
					{	
						Apps.SetEffect(Settings.Apps_to | EFFECT_SLIDE_OUT, Options.slide_effect * 50);
						while(Apps.GetEffect() > 0)
							usleep(100);
					}
					
					for(int i = 0; i < apps_numers; i++)
					{
						// vorherige seite ausblenden
						if((page-1)*apps_numers+i < (signed)vechomebrew_list_choice.size())
						{
							AppsBtn[(page-1)*apps_numers+i]->SetVisible(false);
							AppsBtn[(page-1)*apps_numers+i]->SetSelectable(false);
							AppsBtn[(page-1)*apps_numers+i]->ResetState();
						}
						// aktuelle seite einblenden
						if((Settings.current_page-1)*apps_numers+i < (signed)vechomebrew_list_choice.size())
						{
							AppsBtn[(Settings.current_page-1)*apps_numers+i]->SetVisible(true);
							AppsBtn[(Settings.current_page-1)*apps_numers+i]->SetSelectable(true);
							AppsBtn[(Settings.current_page-1)*apps_numers+i]->ResetState();
						}
						// zu verschiebene app ausblenden und leeres feld einblenden
						if(grab && (Settings.current_page-1)*apps_numers+i == move.nr)
						{
							AppsBtn[move.nr]->SetVisible(false);
							AppsBtn[move.nr]->SetSelectable(false);
						}
					}
					
					// felder in ursprungsposition setzen
					for(int i=0; i < (signed)vechomebrew_list_choice.size(); i++)
						AppsBtn[i]->SetPosition(pos_x[i], pos_y[i]);
						
					// beim seitenwechsel, leeres feld positionieren
					if(Settings.current_page == move.page)
						AppsBtnFree->SetPosition(pos_x[move.nr], pos_y[move.nr]);
					else
						AppsBtnFree->SetPosition(-1000, -1000);
						
					// app 1 auf der aktuellen seite markieren, außer beim verschieben
					if(!grab)
						AppsBtn[(Settings.current_page-1)*apps_numers]->SetState(STATE_SELECTED);
						
					// neue apps einblenden
					if(Options.slide_effect > 0)
						Apps.SetEffect(Settings.Apps_from | EFFECT_SLIDE_IN, Options.slide_effect * 50);
					
					char buffer[100];
					sprintf(buffer, tr("Page %i of %i"), Settings.current_page, Settings.total_page);	
					pageTxt->SetText(buffer);
				}
				
			}
			// eine Kategorie weiter
			else if((WPAD_ButtonsDown(0) & (WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS) && !Options.navigation) || 
					(WPAD_ButtonsDown(0) & (WPAD_BUTTON_RIGHT | WPAD_CLASSIC_BUTTON_RIGHT) && Options.navigation) || 
					PAD_ButtonsDown(0) & PAD_TRIGGER_R || plusBtn.GetState() == STATE_CLICKED)
			{
				Next_Category();
				menu = MENU_MAIN;
			}
			
			// eine Kategorie zurück
			else if((WPAD_ButtonsDown(0) & (WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS) && !Options.navigation) || 
					(WPAD_ButtonsDown(0) & (WPAD_BUTTON_LEFT | WPAD_CLASSIC_BUTTON_LEFT) && Options.navigation) || 
					PAD_ButtonsDown(0) & PAD_TRIGGER_L || minusBtn.GetState() == STATE_CLICKED)
			{
				Previous_Category();
				menu = MENU_MAIN;
			}
			
			// Kategorie umbenennen / verschieben
			else if(categoryBtn.GetState() == STATE_CLICKED)
			{
				categoryBtn.ResetState();
				Category_rename_move(button);
				temp_slide = false;
				menu = MENU_MAIN;
			}
			
			// für die grid ansicht, navigation der einzelnen zellen
			previous_page = false;
			next_page = false;
			for(int i = 0; i< apps_numers/apps_row; i++)
			{
				if(Apps.GetSelected() == i * apps_row + 1 + (Settings.current_page -1) * apps_numers)
					previous_page = true;
				
				if(Apps.GetSelected() == (i+1) * apps_row + (Settings.current_page -1) * apps_numers || Apps.GetSelected() == (signed)vechomebrew_list_choice.size())
					next_page = true;
			}

			// list apps
			for(int i = (Settings.current_page-1)*apps_numers; i < Settings.current_page*apps_numers && i < (signed)vechomebrew_list_choice.size(); i++)
			{
				// show apps info
				if(AppsBtn[i]->GetState() == STATE_CLICKED && button == "A")
				{
					AppsBtn[i]->SetState(STATE_SELECTED);
					again:
					
					int choice;
					if(Options.quick_start == 0)
						choice = AppInfo(vechomebrew_list_choice[i].name.c_str(), vechomebrew_list_choice[i].foldername, vechomebrew_list_choice[i].icon);
					else
						choice = 1;
					
					if(choice == 1)				// App starten
					{
						Settings.forwarder_path = vechomebrew_list_choice[i].pathboot;
						Settings.forwarder_arg = vechomebrew_list_choice[i].arg.c_str();
						boothomebrew = true;
						menu = MENU_EXIT;
					}
					if(choice == 2)				// App Einfügen
					{
						string app_in_kategorie = AddApp(vechomebrew_list_choice[i].name.c_str());
						if( app_in_kategorie != "NULL" )
						{
							AppEinfuegen(app_in_kategorie, vechomebrew_list_choice[i].foldername.c_str());
							copy_app_in_unassigned();
							temp_slide = false;
							menu = MENU_MAIN;
						}
					}
					if(choice == 3 || choice == 4)	
					{
						AppEraseDelate(choice, vechomebrew_list_choice[i].name.c_str(), vechomebrew_list_choice[i].foldername.c_str());
						temp_slide = false;
						menu = MENU_MAIN;
					}
					if(choice == 5)	
					{
						MetaEdit(vechomebrew_list_choice[i].foldername);
						temp_slide = false;
						goto again;
					}

					mainWindow->ChangeFocus(&Apps);
					for(int x=0; x < (signed)vechomebrew_list_choice.size(); x++)
					{
						if(x == i)
							AppsBtn[x]->SetState(STATE_SELECTED);
						else
							AppsBtn[x]->ResetState();
					}
				}
				
				// move apps
				if(AppsBtn[i]->GetState() == STATE_CLICKED && button == "B" && !grab)
				{
					AppsBtn[i]->ResetState();
					if(Settings.current_category != 0 && strcasecmp(Settings.code,"NULL") == 0 && WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
					{
						pointer = new GuiImageData(Theme.player_grab);
						grab = true;
						move.nr = i;
						move.page = Settings.current_page;
						
						// Werte Move Button übergeben
						icon_dataMove = new GuiImageData(vechomebrew_list_choice[i].icon);
						ViewIconMove = new GuiImage(icon_dataMove);
						
						// Grid Ansicht
						if(Settings.grid)
						{
							AppsBtnTxtMove1->SetText("");
							if(vechomebrew_list_choice[i].icon == NULL)
							{
								AppsBtnTxtMove1->SetText(vechomebrew_list_choice[i].name.c_str());
								AppsBtnTxtMove1->SetMaxWidth(100, SCROLL_HORIZONTAL);
							}
							ViewIconMove->SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
						}
						// Normale Ansicht
						else
						{
							AppsBtnTxtMove1->SetText(vechomebrew_list_choice[i].name.c_str());
							AppsBtnTxtMove2->SetText("....................................................");
							AppsBtnTxtMove3->SetText(tr(vechomebrew_list_choice[i].info.c_str()));
							
							ViewIconMove->SetAlignment(ALIGN_LEFT, ALIGN_MIDDLE);
							ViewIconMove->SetPosition(10, 0);
							AppsBtnTxtMove1->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
							AppsBtnTxtMove2->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
							AppsBtnTxtMove3->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
							AppsBtnTxtMove1->SetPosition(160, 10);
							AppsBtnTxtMove1->SetMaxWidth(265, DOTTED);
							AppsBtnTxtMove2->SetPosition(160, 17);
							AppsBtnTxtMove2->SetMaxWidth(260, DOTTED);
							AppsBtnTxtMove3->SetPosition(160, 40);
							AppsBtnTxtMove3->SetMaxWidth(265, DOTTED);
						}
						
						AppsBtnMove->SetIcon(ViewIconMove);
						AppsBtnMove->SetVisible(true);
						
						// button ausblenden
						AppsBtn[i]->SetVisible(false);
						
						// leeres feld einblenden
						AppsBtnFree->SetPosition(pos_x[move.nr], pos_y[move.nr]);
						AppsBtnFree->SetVisible(true);
						
						move.nr_selected = move.nr;
					}
				}
				
				if(AppsBtn[i]->GetState() == STATE_SELECTED && i != move.nr)
				{
					move.nr_selected = i;
					move.new_app = true;
				}
			}
			
			if(grab)
			{
				int x = userInput[0].wpad->ir.x - AppsBtnMove->GetWidth()/2;
				int y = userInput[0].wpad->ir.y - AppsBtnMove->GetHeight()/2;
				AppsBtnMove->SetPosition(x, y);
				
				if(move.new_app)
				{	
					move.new_app = false;
					
					// leeres button feld anzeigen
					int i = 0;
					if(	pos_x[move.nr_selected] == AppsBtn[move.nr_selected]->GetLeft() && pos_y[move.nr_selected] == AppsBtn[move.nr_selected]->GetTop() )
						i = move.nr_selected;
					else
					{
						if(move.nr < move.nr_selected)
							i = move.nr_selected -1;
						else if(move.nr > move.nr_selected)
							i = move.nr_selected +1;
					}
					AppsBtnFree->SetPosition(pos_x[i], pos_y[i]);
					
					for(i = 0; i < apps_numers && (Settings.current_page-1)*apps_numers+i  < (signed)vechomebrew_list_choice.size(); i++)
					{
						int x = (Settings.current_page-1)*apps_numers+i;
						// buttons 1 kleiner button 2
						if(move.nr < move.nr_selected)
						{
							if(x > move.nr && x < move.nr_selected +1)
							{
								if(x == move.nr_selected && (pos_x[move.nr_selected] != AppsBtn[move.nr_selected]->GetLeft() || pos_y[move.nr_selected] != AppsBtn[move.nr_selected]->GetTop()))
									AppsBtn[x]->SetPosition(pos_x[x], pos_y[x]);
								else if(x == (Settings.current_page-1) * apps_numers)
									AppsBtn[x]->SetPosition(-1000, -1000);
								else
									AppsBtn[x]->SetPosition(pos_x[x -1], pos_y[x -1]);
							}
							else
								AppsBtn[x]->SetPosition(pos_x[x], pos_y[x]);
						}
						// buttons 1 größer button 2
						else if(move.nr > move.nr_selected)
						{
							if(x < move.nr && x >= move.nr_selected)
							{
								if(x == move.nr_selected && (pos_x[move.nr_selected] != AppsBtn[move.nr_selected]->GetLeft() || pos_y[move.nr_selected] != AppsBtn[move.nr_selected]->GetTop()))
									AppsBtn[x]->SetPosition(pos_x[x], pos_y[x]);
								else if(x +1 == Settings.current_page * apps_numers)
									AppsBtn[x]->SetPosition(-1000, -1000);
								else
									AppsBtn[x]->SetPosition(pos_x[x +1], pos_y[x +1]);
							}
							else
								AppsBtn[x]->SetPosition(pos_x[x], pos_y[x]);
						}
						AppsBtn[x]->ResetState();
					}
					
					if(move.nr < move.nr_selected)
					{
						if(pos_x[move.nr_selected] != AppsBtn[move.nr_selected]->GetLeft() || pos_y[move.nr_selected] != AppsBtn[move.nr_selected]->GetTop())
							vor = false;
						else
							vor = true;
					}
					else if(move.nr > move.nr_selected)
					{
						if(pos_x[move.nr_selected] != AppsBtn[move.nr_selected]->GetLeft() || pos_y[move.nr_selected] != AppsBtn[move.nr_selected]->GetTop())
							vor = true;
						else
							vor = false;
					}
				}
				
				// wenn b losgelassen wurde
				if(WPAD_ButtonsUp(0) & WPAD_BUTTON_B)
				{
					grab = false;
					pointer = new GuiImageData(Theme.player_point);
					AppsBtnMove->SetVisible(false);
					AppsBtnFree->SetVisible(false);
					
					// verschieben
					if(AppsBtnFree->GetState() == STATE_SELECTED)
					{
						string AppOrdner1 = vechomebrew_list_choice[move.nr].foldername;
						string AppOrdner2 = vechomebrew_list_choice[move.nr_selected].foldername;
						
						AppVerschieben(Settings.category_name, AppOrdner1, vor, AppOrdner2);
						copy_app_in_category();
						// slide temporär deaktivieren
						temp_slide = false;
						menu = MENU_MAIN;
					}
					// nicht verschieben und button anzeigen
					else
					{
						// felder in ursprungsposition setzen
						for(int i=0; i < (signed)vechomebrew_list_choice.size(); i++)
							AppsBtn[i]->SetPosition(pos_x[i], pos_y[i]);
						if(Settings.current_page == move.page)
							AppsBtn[move.nr]->SetVisible(true);
					}
				}
			}
		}
	}

	if(Options.slide_effect > 0 && Settings.Apps_to != 0 && temp_slide)
	{	
		Apps.SetEffect(Settings.Apps_to | EFFECT_SLIDE_OUT, Options.slide_effect * 50);
		while(Apps.GetEffect() > 0)
			usleep(100);
	}
	HaltGui();
	mainWindow->Remove(&Apps);
	mainWindow->Remove(&w);
	return menu;
}