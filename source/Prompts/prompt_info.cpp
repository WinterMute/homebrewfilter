
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

/****************************************************************************
 * WindowPrompt
 *
 * Displays a prompt window to user, with information, an error message, or
 * presenting a user with a choice
 ***************************************************************************/
void
infoPrompt()
{
	bool stop = false;

	GuiWindow promptWindow(520,360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiImageData btnOutline(Theme.button_tiny);
	GuiImageData btnOutlineOver(Theme.button_tiny_focus);
	GuiTrigger trigA;
	GuiTrigger trigB;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigB.SetSimpleTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

    int i = 0;
    int x = 30;
    int y = 40;
	GuiText * Entrie[20];

    Entrie[i] = new GuiText(tr("Credits"), 28, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
    Entrie[i]->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
    Entrie[i]->SetPosition(0, y);
    i++;
    y += 50;

    Entrie[i] = new GuiText(tr("Coder:"), 24, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
    Entrie[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
    Entrie[i]->SetPosition(x, y);
    i++;

    Entrie[i] = new GuiText("hamachi-mp", 22, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
    Entrie[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
    Entrie[i]->SetPosition(x+200, y);
    i++;
    y += 32;

    Entrie[i] = new GuiText(tr("Designer:"), 24, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
    Entrie[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
    Entrie[i]->SetPosition(x, y);
    i++;

    Entrie[i] = new GuiText("Black.Pearl", 22, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
    Entrie[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
    Entrie[i]->SetPosition(x+200, y);
    i++;
    y += 50;

    Entrie[i] = new GuiText(tr("Special thanks to:"), 22, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
    Entrie[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
    Entrie[i]->SetPosition(x,y);
	i++;
	y += 32;
	
	Entrie[i] = new GuiText("- Dimok", 20, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
    Entrie[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
    Entrie[i]->SetPosition(x+50,y);
    i++;
	y += 32;

	Entrie[i] = new GuiText("- ichfly ", 20, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
    Entrie[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
    Entrie[i]->SetPosition(x+50,y);
    i++;
	y += 32;

	Entrie[i] = new GuiText("- all the translators", 20, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
    Entrie[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
    Entrie[i]->SetPosition(x+50,y);
    i++;


    int CreditEntries = i;

	char Rev[50];
    sprintf(Rev, "Rev. %i", SvnRev());
	
	GuiText RevTxt(Rev, 20, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	RevTxt.SetAlignment(ALIGN_LEFT, ALIGN_TOP);
	RevTxt.SetPosition(30,24);
 	
	char RunsIos[50];
	if((*(volatile unsigned int*)HW_ARMIRQMASK)&&(*(volatile unsigned int*)HW_ARMIRQFLAG))
		sprintf(RunsIos, "IOS %i Rev. %i (HW_AHBPROT)", IOS_GetVersion(), IOS_GetRevision());
	else
		sprintf(RunsIos, "IOS %i Rev. %i", IOS_GetVersion(), IOS_GetRevision());
	
	GuiText RunsIosTxt(RunsIos, 14, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	RunsIosTxt.SetAlignment(ALIGN_RIGHT, ALIGN_TOP);
	RunsIosTxt.SetPosition(-30,24);
 	
	GuiText backTxt(tr("Back"), 24, (GXColor){Theme.button_tiny_text_1, Theme.button_tiny_text_2, Theme.button_tiny_text_3, 255});
	GuiImage backImg(&btnOutline);
	GuiImage backImgOver(&btnOutlineOver);
	GuiButton back(btnOutline.GetWidth(), btnOutline.GetHeight());
	back.SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
	back.SetPosition(0, -15);
	back.SetLabel(&backTxt);
	back.SetImage(&backImg);
	back.SetImageOver(&backImgOver);
	back.SetTrigger(&trigA);
	back.SetTrigger(&trigB);

	promptWindow.Append(&dialogBoxImg);
	
	for(int i = 0; i < CreditEntries; i++)
        promptWindow.Append(Entrie[i]);
		
    promptWindow.Append(&RevTxt);
    promptWindow.Append(&RunsIosTxt);
	promptWindow.Append(&back);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(!stop)
	{
		usleep(100);

		if(back.GetState() == STATE_CLICKED)
			stop = true;
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
}

