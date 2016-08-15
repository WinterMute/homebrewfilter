
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"
#include "Menus/menus.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern void HaltResumeGui();

/****************************************************************************
 * App verschieben
 ***************************************************************************/
void MoveCategory(int moveCategoryNr, string &Kategorie1, bool &vor, string &Kategorie2)
{
	bool stop = false;
	int CategoryInt = 1;
	vor = true;

	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	GuiTrigger trigB;
	trigB.SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);

	GuiWindow promptWindow(520, 360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);
	GuiImageData btnOutline(Theme.button_small);
	GuiImageData btnOutlineOver(Theme.button_small_focus);

	GuiText zeile1Txt(AvailableCategory.categories[moveCategoryNr].c_str(), 24, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	zeile1Txt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	zeile1Txt.SetPosition(0, -140);

	GuiText zeile2Txt(tr("Move"), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	zeile2Txt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	zeile2Txt.SetPosition(0, -70);

	GuiText zeile3_1Txt("<                                                     >", 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	zeile3_1Txt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	zeile3_1Txt.SetPosition(0, -30);

	GuiText zeile3_2Txt(tr("before"), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	zeile3_2Txt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	zeile3_2Txt.SetPosition(0, -30);

	GuiText zeile4Txt("c", 30, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	zeile4Txt.SetFont(symbol_ttf, symbol_ttf_size);
	zeile4Txt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	zeile4Txt.SetPosition(0, 10);

	GuiText zeile5Txt(AvailableCategory.categories[1].c_str(), 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	zeile5Txt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	zeile5Txt.SetPosition(0, 35);

	GuiText zeile6Txt("d", 30, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	zeile6Txt.SetFont(symbol_ttf, symbol_ttf_size);
	zeile6Txt.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	zeile6Txt.SetPosition(0, 60);

	GuiText okTxt(tr("Move"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiImage okImg(&btnOutline);
	GuiImage okImgOver(&btnOutlineOver);
	GuiButton ok(btnOutline.GetWidth(), btnOutline.GetHeight());
	ok.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	ok.SetPosition(-120, -25);
	ok.SetLabel(&okTxt);
	ok.SetImage(&okImg);
	ok.SetImageOver(&okImgOver);
	ok.SetTrigger(&trigA);

	GuiText backTxt(tr("Stop"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiImage backImg(&btnOutline);
	GuiImage backImgOver(&btnOutlineOver);
	GuiButton back(btnOutline.GetWidth(), btnOutline.GetHeight());
	back.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	back.SetPosition(120, -25);
	back.SetLabel(&backTxt);
	back.SetImage(&backImg);
	back.SetImageOver(&backImgOver);
	back.SetTrigger(&trigA);
	back.SetTrigger(&trigB);
	back.SetState(STATE_SELECTED);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&zeile1Txt);
	promptWindow.Append(&zeile2Txt);
	promptWindow.Append(&zeile3_1Txt);
	promptWindow.Append(&zeile3_2Txt);
	promptWindow.Append(&zeile4Txt);
	promptWindow.Append(&zeile5Txt);
	promptWindow.Append(&zeile6Txt);
	promptWindow.Append(&ok);
	promptWindow.Append(&back);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(!stop)
	{
		usleep(100);

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_PLUS | WPAD_CLASSIC_BUTTON_PLUS) || PAD_ButtonsDown(0) & PAD_TRIGGER_R
		   || WUPC_ButtonsDown(0) & WPAD_CLASSIC_BUTTON_PLUS)
		{
			vor = false;
			zeile3_2Txt.SetText(tr("after"));
		}

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_MINUS | WPAD_CLASSIC_BUTTON_MINUS) || PAD_ButtonsDown(0) & PAD_TRIGGER_L
		   || WUPC_ButtonsDown(0) & WPAD_CLASSIC_BUTTON_MINUS)
		{
			vor = true;
			zeile3_2Txt.SetText(tr("before"));
		}

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_UP | WPAD_CLASSIC_BUTTON_UP) || PAD_ButtonsDown(0) & PAD_BUTTON_UP
		   || WUPC_ButtonsDown(0) & WPAD_CLASSIC_BUTTON_UP)
		{
			CategoryInt--;

			if ( CategoryInt < 1 )
				CategoryInt = AvailableCategory.categories.size() -1;

			zeile5Txt.SetText(AvailableCategory.categories[CategoryInt].c_str());
			HaltResumeGui();
		}

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN) || PAD_ButtonsDown(0) & PAD_BUTTON_DOWN
		   || WUPC_ButtonsDown(0) & WPAD_CLASSIC_BUTTON_DOWN)
		{
			CategoryInt++;

			if (CategoryInt == (signed)AvailableCategory.categories.size())
				CategoryInt = 1;

			zeile5Txt.SetText(AvailableCategory.categories[CategoryInt].c_str());
			HaltResumeGui();
		}

		if(ok.GetState() == STATE_CLICKED)
		{
			Kategorie1 = AvailableCategory.categories[moveCategoryNr];
			Kategorie2 = AvailableCategory.categories[CategoryInt];
			stop = true;
		}

		if(back.GetState() == STATE_CLICKED)
			stop = true;
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
}
