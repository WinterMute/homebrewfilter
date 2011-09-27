
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"
#include "Tools/textline.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern void HaltResumeGui();

/****************************************************************************
 * WindowPrompt
 *
 * Displays a prompt window to user, with information, an error message, or
 * presenting a user with a choice
 ***************************************************************************/
void
revtext(const char *msg)
{
	bool stop = false;

	GuiWindow promptWindow(520,360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiImageData btnOutline(Theme.button_small);
	GuiImageData btnOutlineOver(Theme.button_small_focus);
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

	TextLine revtext;
	revtext.text(msg, 18, 400);

    int i = 0;
    int y = 90;
	int place = 25;

	int number = 7;

	GuiText upTxt("c", 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	upTxt.SetFont(symbol_ttf, symbol_ttf_size);
	upTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	upTxt.SetPosition(0, y -20);
	
	GuiText downTxt("d", 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	downTxt.SetFont(symbol_ttf, symbol_ttf_size);
	downTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	downTxt.SetPosition(0, y + (place * (number-1)) + 15);
	
	GuiText * Entrie[number];
	
	for(i=0; i < number && i < (signed)revtext.line.size(); i++)
	{
		Entrie[i] = new GuiText(revtext.line[i].c_str(), 20, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
		Entrie[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
		Entrie[i]->SetPosition(50, y);
		y += place;
	}

	GuiText titleTxt(tr("Info"), 26, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,40);

	GuiText backTxt(tr("OK"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiImage backImg(&btnOutline);
	GuiImage backImgOver(&btnOutlineOver);
	GuiButton back(btnOutline.GetWidth(), btnOutline.GetHeight());

	back.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	back.SetPosition(0, -25);

	back.SetLabel(&backTxt);
	back.SetImage(&backImg);
	back.SetImageOver(&backImgOver);
	back.SetTrigger(&trigA);
	back.SetState(STATE_SELECTED);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	
	for(int x=0; x < i; x++)
		promptWindow.Append(Entrie[x]);
	
	if((signed)revtext.line.size() >= number)
	{
		promptWindow.Append(&upTxt);
		promptWindow.Append(&downTxt);
	}
	
	promptWindow.Append(&back);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(!stop)
	{
		usleep(100);

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_UP | WPAD_CLASSIC_BUTTON_UP) || PAD_ButtonsDown(0) & PAD_BUTTON_UP)
		{
			int z = revtext.text_up();
			
			for(int x=0; x < i; x++)
				Entrie[x]->SetText(revtext.line[x + z].c_str());
		

			HaltResumeGui();
		}
		
		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN) || PAD_ButtonsDown(0) & PAD_BUTTON_DOWN)
		{
			int z = revtext.text_down(number);
			
			for(int x=0; x < i; x++)
				Entrie[x]->SetText(revtext.line[x + z].c_str());


			HaltResumeGui();
		}

		if(back.GetState() == STATE_CLICKED)
			stop = true;
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
}
