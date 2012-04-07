
#include <unistd.h>
#include <algorithm>

#include "libwiigui/gui.h"
#include "main.h"
#include "menu.h"
#include "menus.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

extern bool runaway;

/****************************************************************************
 * MenuSettings
 ***************************************************************************/
int MenuSettingsDisplay()
{
	int menu = MENU_NONE;

	int top		= Settings.top;
	int bottom	= Settings.bottom;
	int left	= Settings.left;
	int right	= Settings.right;

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	GuiTrigger trigHome;
	trigHome.SetButtonOnlyTrigger(-1, WPAD_BUTTON_HOME | WPAD_CLASSIC_BUTTON_HOME, 0);

	GuiText titleTxt(tr("Display"), 30, (GXColor){Theme.title_1, Theme.title_2, Theme.title_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,30);

	// Buttons Data
	GuiImageData textBtn(keyboard_textbox_png);

	GuiImageData btn(Theme.button_small);
	GuiImageData btn_over(Theme.button_small_focus);

	GuiImageData plusBtnData(Theme.scrollbar_arrowup);
	GuiImageData plusBtnOverData(Theme.scrollbar_arrowup_over);
	GuiImageData minusBtnData(Theme.scrollbar_arrowdown);
	GuiImageData minusBtnOverData(Theme.scrollbar_arrowdown_over);

	// Buttons
	GuiImage backBtnImg(&btn);
	GuiImage backBtnImgOver(&btn_over);
	GuiImage saveBtnImg(&btn);
	GuiImage saveBtnImgOver(&btn_over);

	GuiImage plusTopBtnImg(&plusBtnData);
	GuiImage plusTopBtnImgOver(&plusBtnOverData);
	GuiImage minusTopBtnImg(&minusBtnData);
	GuiImage minusTopBtnImgOver(&minusBtnOverData);
	GuiImage plusBottomBtnImg(&plusBtnData);
	GuiImage plusBottomBtnImgOver(&plusBtnOverData);
	GuiImage minusBottomBtnImg(&minusBtnData);
	GuiImage minusBottomBtnImgOver(&minusBtnOverData);
	GuiImage plusLeftBtnImg(&plusBtnData);
	GuiImage plusLeftBtnImgOver(&plusBtnOverData);
	GuiImage minusLeftBtnImg(&minusBtnData);
	GuiImage minusLeftBtnImgOver(&minusBtnOverData);
	GuiImage plusRightBtnImg(&plusBtnData);
	GuiImage plusRightBtnImgOver(&plusBtnOverData);
	GuiImage minusRightBtnImg(&minusBtnData);
	GuiImage minusRightBtnImgOver(&minusBtnOverData);


	char number[4];
	int y = 130;

	// oben
	GuiImage textTopBtnImg(&textBtn);
	textTopBtnImg.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	textTopBtnImg.SetPosition(0, y);

	GuiText topTxt("g                                       g", 30, (GXColor){0, 0, 0, 255});
	topTxt.SetFont(symbol_ttf, symbol_ttf_size);
	topTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	topTxt.SetPosition(0, y +3);

	GuiButton plusTopBtn(plusBtnData.GetWidth(), plusBtnData.GetHeight());
	plusTopBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	plusTopBtn.SetPosition(80, y);
	plusTopBtn.SetImage(&plusTopBtnImg);
	plusTopBtn.SetImageOver(&plusTopBtnImgOver);
	plusTopBtn.SetTrigger(&trigA);

	sprintf(number, "%i", top);
	GuiText topNr(number, 30, (GXColor){0, 0, 0, 255});
	topNr.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	topNr.SetPosition(0, y +10);

	GuiButton minusTopBtn(minusBtnData.GetWidth(), minusBtnData.GetHeight());
	minusTopBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	minusTopBtn.SetPosition(-80, y);
	minusTopBtn.SetImage(&minusTopBtnImg);
	minusTopBtn.SetImageOver(&minusTopBtnImgOver);
	minusTopBtn.SetTrigger(&trigA);

	y += 50;

	// unten
	GuiImage textBottomBtnImg(&textBtn);
	textBottomBtnImg.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	textBottomBtnImg.SetPosition(0, y);

	GuiText bottomTxt("h                                       h", 30, (GXColor){0, 0, 0, 255});
	bottomTxt.SetFont(symbol_ttf, symbol_ttf_size);
	bottomTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	bottomTxt.SetPosition(0, y +7);

	GuiButton plusBottomBtn(plusBtnData.GetWidth(), plusBtnData.GetHeight());
	plusBottomBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	plusBottomBtn.SetPosition(80, y);
	plusBottomBtn.SetImage(&plusBottomBtnImg);
	plusBottomBtn.SetImageOver(&plusBottomBtnImgOver);
	plusBottomBtn.SetTrigger(&trigA);

	sprintf(number, "%i", bottom);
	GuiText bottomNr(number, 30, (GXColor){0, 0, 0, 255});
	bottomNr.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	bottomNr.SetPosition(0, y +10);

	GuiButton minusBottomBtn(minusBtnData.GetWidth(), minusBtnData.GetHeight());
	minusBottomBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	minusBottomBtn.SetPosition(-80, y);
	minusBottomBtn.SetImage(&minusBottomBtnImg);
	minusBottomBtn.SetImageOver(&minusBottomBtnImgOver);
	minusBottomBtn.SetTrigger(&trigA);

	y += 50;

	// links
	GuiImage textLeftBtnImg(&textBtn);
	textLeftBtnImg.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	textLeftBtnImg.SetPosition(0, y);

	GuiText lefttTxt("e                                      e", 30, (GXColor){0, 0, 0, 255});
	lefttTxt.SetFont(symbol_ttf, symbol_ttf_size);
	lefttTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	lefttTxt.SetPosition(0, y +5);

	GuiButton plusLeftBtn(plusBtnData.GetWidth(), plusBtnData.GetHeight());
	plusLeftBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	plusLeftBtn.SetPosition(80, y);
	plusLeftBtn.SetImage(&plusLeftBtnImg);
	plusLeftBtn.SetImageOver(&plusLeftBtnImgOver);
	plusLeftBtn.SetTrigger(&trigA);

	sprintf(number, "%i", left);
	GuiText leftNr(number, 30, (GXColor){0, 0, 0, 255});
	leftNr.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	leftNr.SetPosition(0, y +10);

	GuiButton minusLeftBtn(minusBtnData.GetWidth(), minusBtnData.GetHeight());
	minusLeftBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	minusLeftBtn.SetPosition(-80, y);
	minusLeftBtn.SetImage(&minusLeftBtnImg);
	minusLeftBtn.SetImageOver(&minusLeftBtnImgOver);
	minusLeftBtn.SetTrigger(&trigA);

	y += 50;

	// rechts
	GuiImage textRightBtnImg(&textBtn);
	textRightBtnImg.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	textRightBtnImg.SetPosition(0, y);

	GuiText rightTxt("f                                      f", 30, (GXColor){0, 0, 0, 255});
	rightTxt.SetFont(symbol_ttf, symbol_ttf_size);
	rightTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	rightTxt.SetPosition(0, y +5);

	GuiButton plusRightBtn(plusBtnData.GetWidth(), plusBtnData.GetHeight());
	plusRightBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	plusRightBtn.SetPosition(80, y);
	plusRightBtn.SetImage(&plusRightBtnImg);
	plusRightBtn.SetImageOver(&plusRightBtnImgOver);
	plusRightBtn.SetTrigger(&trigA);

	sprintf(number, "%i", right);
	GuiText rightNr(number, 30, (GXColor){0, 0, 0, 255});
	rightNr.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	rightNr.SetPosition(0, y +10);

	GuiButton minusRightBtn(minusBtnData.GetWidth(), minusBtnData.GetHeight());
	minusRightBtn.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	minusRightBtn.SetPosition(-80, y);
	minusRightBtn.SetImage(&minusRightBtnImg);
	minusRightBtn.SetImageOver(&minusRightBtnImgOver);
	minusRightBtn.SetTrigger(&trigA);

	GuiText backBtnTxt(tr("Stop"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton backBtn(btn.GetWidth(), btn.GetHeight());
	backBtn.SetAlignment(ALIGN_RIGHT, ALIGN_BOTTOM);
	backBtn.SetPosition(-100, -35);
	backBtn.SetLabel(&backBtnTxt);
	backBtn.SetImage(&backBtnImg);
	backBtn.SetImageOver(&backBtnImgOver);
	backBtn.SetTrigger(&trigA);
	backBtn.SetTrigger(&trigHome);
	backBtn.SetEffectGrow();

	GuiText saveBtnTxt(tr("Save"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton saveBtn(btn.GetWidth(), btn.GetHeight());
	saveBtn.SetAlignment(ALIGN_LEFT, ALIGN_BOTTOM);
	saveBtn.SetPosition(100, -35);
	saveBtn.SetLabel(&saveBtnTxt);
	saveBtn.SetImage(&saveBtnImg);
	saveBtn.SetImageOver(&saveBtnImgOver);
	saveBtn.SetTrigger(&trigA);
	saveBtn.SetEffectGrow();

	HaltGui();
	GuiWindow w(screenwidth, screenheight);
	w.Append(&titleTxt);

	w.Append(&textTopBtnImg);
	w.Append(&topTxt);
	w.Append(&plusTopBtn);
	w.Append(&topNr);
	w.Append(&minusTopBtn);

	w.Append(&textBottomBtnImg);
	w.Append(&bottomTxt);
	w.Append(&plusBottomBtn);
	w.Append(&bottomNr);
	w.Append(&minusBottomBtn);

	w.Append(&textLeftBtnImg);
	w.Append(&lefttTxt);
	w.Append(&plusLeftBtn);
	w.Append(&leftNr);
	w.Append(&minusLeftBtn);

	w.Append(&textRightBtnImg);
	w.Append(&rightTxt);
	w.Append(&plusRightBtn);
	w.Append(&rightNr);
	w.Append(&minusRightBtn);

	w.Append(&backBtn);
	w.Append(&saveBtn);

	mainWindow->Append(&w);

	ResumeGui();

	int display = DISPLAY_NONE;
	bool add = -1;
	int temp_number = 0;

	while(menu == MENU_NONE)
	{
		usleep(100);

		// oben
		if(plusTopBtn.GetState() == STATE_CLICKED || minusTopBtn.GetState() == STATE_CLICKED)
		{
			if(plusTopBtn.GetState() == STATE_CLICKED)
				add = true;
			else
				add = false;

			plusTopBtn.ResetState();
			minusTopBtn.ResetState();
			display = DISPLAY_TOP;
			temp_number = top;
		}

		// unten
		if(plusBottomBtn.GetState() == STATE_CLICKED || minusBottomBtn.GetState() == STATE_CLICKED)
		{
			if(plusBottomBtn.GetState() == STATE_CLICKED)
				add = true;
			else
				add = false;

			plusBottomBtn.ResetState();
			minusBottomBtn.ResetState();
			display = DISPLAY_BOTTOM;
			temp_number = bottom;
		}

		// links
		if(plusLeftBtn.GetState() == STATE_CLICKED || minusLeftBtn.GetState() == STATE_CLICKED)
		{
			if(plusLeftBtn.GetState() == STATE_CLICKED)
				add = true;
			else
				add = false;

			plusLeftBtn.ResetState();
			minusLeftBtn.ResetState();
			display = DISPLAY_LEFT;
			temp_number = left;
		}

		// rechts
		if(plusRightBtn.GetState() == STATE_CLICKED || minusRightBtn.GetState() == STATE_CLICKED)
		{
			if(plusRightBtn.GetState() == STATE_CLICKED)
				add = true;
			else
				add = false;

			plusRightBtn.ResetState();
			minusRightBtn.ResetState();
			display = DISPLAY_RIGHT;
			temp_number = right;
		}

		if(display != DISPLAY_NONE)
		{
			while(WPAD_ButtonsHeld(0) & (WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A) || PAD_ButtonsHeld(0) & PAD_BUTTON_A)
			{
				if(add)
					temp_number++;
				else
					temp_number--;

				sprintf(number, "%i", temp_number);

				switch (display)
				{
					case DISPLAY_TOP:
						top = temp_number;
						topNr.SetText(number);
						break;

					case DISPLAY_BOTTOM:
						bottom = temp_number;
						bottomNr.SetText(number);
						break;

					case DISPLAY_LEFT:
						left = temp_number;
						leftNr.SetText(number);
						break;

					case DISPLAY_RIGHT:
						right = temp_number;
						rightNr.SetText(number);
						break;
				}

				stretch(top, bottom, left, right);

				if(temp_number == 0)
					usleep(500000);
				else
					usleep(100000);
			}

			if(add && display == DISPLAY_TOP)
				plusTopBtn.SetState(STATE_SELECTED);
			else if(!add && display == DISPLAY_TOP)
				minusTopBtn.SetState(STATE_SELECTED);
			else if(add && display == DISPLAY_BOTTOM)
				plusBottomBtn.SetState(STATE_SELECTED);
			else if(!add && display == DISPLAY_BOTTOM)
				minusBottomBtn.SetState(STATE_SELECTED);
			else if(add && display == DISPLAY_LEFT)
				plusLeftBtn.SetState(STATE_SELECTED);
			else if(!add && display == DISPLAY_LEFT)
				minusLeftBtn.SetState(STATE_SELECTED);
			else if(add && display == DISPLAY_RIGHT)
				plusRightBtn.SetState(STATE_SELECTED);
			else if(!add && display == DISPLAY_RIGHT)
				minusRightBtn.SetState(STATE_SELECTED);

			display = DISPLAY_NONE;
		}

		if(backBtn.GetState() == STATE_CLICKED)
		{
			Options.temp_last_setting = 1;
			stretch(Settings.top, Settings.bottom, Settings.left, Settings.right);
			menu = MENU_SETTINGS_FILE;
		}
		if(saveBtn.GetState() == STATE_CLICKED)
		{
			Options.temp_last_setting = 1;
			Settings.top	= top;
			Settings.bottom	= bottom;
			Settings.left	= left;
			Settings.right	= right;

			menu = MENU_SETTINGS_FILE;
		}

		if(runaway == true)
		{
			Options.temp_last_setting = 1;
			stretch(Settings.top, Settings.bottom, Settings.left, Settings.right);
			menu = MENU_SETTINGS_FILE;
		}
	}

	HaltGui();
	mainWindow->Remove(&w);
	return menu;
}

