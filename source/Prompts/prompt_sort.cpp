
#include <unistd.h>

#include "libwiigui/gui.h"
#include "main.h"
#include "Tools/sort.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();

int sortPrompt()
{
	bool stop = false;
	int device = -1;

	GuiWindow promptWindow(520,360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiTrigger trigA;
	GuiTrigger trigB;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigB.SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);

	GuiText titleTxt(tr("Sort"), 26, (GXColor){Theme.title_1, Theme.title_2, Theme.title_3, 255});
	titleTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	titleTxt.SetPosition(0,40);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

 	// Buttons data
	GuiImageData btn(Theme.button_small);
	GuiImage btn1Img(&btn);
	GuiImage btn2Img(&btn);
	GuiImage btn3Img(&btn);
	GuiImage btn4Img(&btn);
	GuiImage backImg(&btn);
	
	// Buttons over data
	GuiImageData btn_over(Theme.button_small_focus);
	GuiImage btn1ImgOver(&btn_over);
	GuiImage btn2ImgOver(&btn_over);
	GuiImage btn3ImgOver(&btn_over);
	GuiImage btn4ImgOver(&btn_over);
	GuiImage backImgOver(&btn_over);

	GuiText btn1Txt(tr("Name ( A - Z )"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton btn1(btn.GetWidth(), btn.GetHeight());
	btn1.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	btn1.SetPosition(-120, 90);
	btn1.SetLabel(&btn1Txt);
	btn1.SetImage(&btn1Img);
	btn1.SetImageOver(&btn1ImgOver);
	btn1.SetTrigger(&trigA);
	
	GuiText btn2Txt(tr("Name ( Z - A )"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton btn2(btn.GetWidth(), btn.GetHeight());
	btn2.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	btn2.SetPosition(120, 90);
	btn2.SetLabel(&btn2Txt);
	btn2.SetImage(&btn2Img);
	btn2.SetImageOver(&btn2ImgOver);
	btn2.SetTrigger(&trigA);
	
	GuiText btn3Txt(tr("Folder ( A - Z )"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton btn3(btn.GetWidth(), btn.GetHeight());
	btn3.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	btn3.SetPosition(-120, 180);
	btn3.SetLabel(&btn3Txt);
	btn3.SetImage(&btn3Img);
	btn3.SetImageOver(&btn3ImgOver);
	btn3.SetTrigger(&trigA);
	
	GuiText btn4Txt(tr("Folder ( Z - A )"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton btn4(btn.GetWidth(), btn.GetHeight());
	btn4.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	btn4.SetPosition(120, 180);
	btn4.SetLabel(&btn4Txt);
	btn4.SetImage(&btn4Img);
	btn4.SetImageOver(&btn4ImgOver);
	btn4.SetTrigger(&trigA);
	
	GuiText backTxt(tr("Back"), 22, (GXColor){Theme.button_small_text_1, Theme.button_small_text_2, Theme.button_small_text_3, 255});
	GuiButton back(btn.GetWidth(), btn.GetHeight());
	back.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	back.SetPosition(0, 270);
	back.SetLabel(&backTxt);
	back.SetImage(&backImg);
	back.SetImageOver(&backImgOver);
	back.SetTrigger(&trigA);
	back.SetTrigger(&trigB);

	promptWindow.Append(&dialogBoxImg);
	promptWindow.Append(&titleTxt);
	promptWindow.Append(&btn1);
	promptWindow.Append(&btn2);
	promptWindow.Append(&btn3);
	promptWindow.Append(&btn4);
	promptWindow.Append(&back);

	HaltGui();
	mainWindow->SetState(STATE_DISABLED);
	mainWindow->Append(&promptWindow);
	mainWindow->ChangeFocus(&promptWindow);
	ResumeGui();

	while(!stop)
	{
		usleep(100);

		if(btn1.GetState() == STATE_CLICKED)
		{
			device = NAME_A_Z;
			stop = true;
		}
		
		if(btn2.GetState() == STATE_CLICKED)
		{
			device = NAME_Z_A;
			stop = true;
		}
			
		if(btn3.GetState() == STATE_CLICKED)
		{
			device = FOLDERNAME_A_Z;
			stop = true;
		}
			
		if(btn4.GetState() == STATE_CLICKED)
		{
			device = FOLDERNAME_Z_A;
			stop = true;
		}
			
		if(back.GetState() == STATE_CLICKED)
			stop = true;
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	return device;
}

