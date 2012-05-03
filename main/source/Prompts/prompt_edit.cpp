
#include <unistd.h>
#include <fstream>

#include "libwiigui/gui.h"
#include "main.h"
#include "Tools/textline.h"
#include "Prompts/prompts.h"

/*** Extern variables ***/
extern GuiWindow * mainWindow;

/*** Extern functions ***/
extern void ResumeGui();
extern void HaltGui();
extern void HaltResumeGui();

/****************************************************************************
 * MetaEdit
 *
 * meta bearbeiten
 ***************************************************************************/
bool
MetaEdit(string dir)
{
	int choice = -1;
	bool changed = false;

	dir += "meta.xml";
	string line, quelltext;

	ifstream in(dir.c_str());
	while(getline(in, line))
		quelltext += line + "\n";

	GuiWindow promptWindow(520,360);
	promptWindow.SetAlignment(ALIGN_CENTRE, ALIGN_MIDDLE);
	promptWindow.SetPosition(0, -10);
	GuiImageData btnOutline(Theme.button_small);
	GuiImageData btnOutlineOver(Theme.button_small_focus);
	GuiTrigger trigA;
	trigA.SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	GuiImageData dialogBox(Theme.dialog_background);
	GuiImage dialogBoxImg(&dialogBox);

	TextLine meta;
	meta.text(quelltext, 18, 440);

    int i = 0;
    int y = 90;
	int place = 25;

	int number = 7;
	int startline = 0;

	GuiText upTxt("c", 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	upTxt.SetFont(symbol_ttf, symbol_ttf_size);
	upTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	upTxt.SetPosition(0, y -20);

	GuiText downTxt("d", 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
	downTxt.SetFont(symbol_ttf, symbol_ttf_size);
	downTxt.SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	downTxt.SetPosition(0, y + (place * (number-1)) + 15);

	GuiButton * Entrie[number];
	GuiText * EntrieTxt[number];

	for(i=0; i < number && i < (signed)meta.line.size(); i++)
	{
		EntrieTxt[i] = new GuiText(meta.line[i].c_str(), 18, (GXColor) {Theme.text_1, Theme.text_2, Theme.text_3, 255});
		EntrieTxt[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
		Entrie[i] = new GuiButton(440, 18);
		Entrie[i]->SetLabel(EntrieTxt[i]);
		Entrie[i]->SetAlignment(ALIGN_LEFT, ALIGN_TOP);
		Entrie[i]->SetPosition(40, y);
		Entrie[i]->SetTrigger(&trigA);
		y += place;
	}

	GuiText titleTxt("meta.xml", 22, (GXColor){Theme.text_1, Theme.text_2, Theme.text_3, 255});
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

	if((signed)meta.line.size() >= number)
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

	while(choice == -1)
	{
		usleep(100);

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_UP | WPAD_CLASSIC_BUTTON_UP) || PAD_ButtonsDown(0) & PAD_BUTTON_UP)
		{
			startline = meta.text_up();

			for(int x=0; x < i; x++)
				EntrieTxt[x]->SetText(meta.line[x + startline].c_str());

			HaltResumeGui();
		}

		if(WPAD_ButtonsDown(0) & (WPAD_BUTTON_DOWN | WPAD_CLASSIC_BUTTON_DOWN) || PAD_ButtonsDown(0) & PAD_BUTTON_DOWN)
		{
			startline = meta.text_down(number);

			for(int x=0; x < i; x++)
				EntrieTxt[x]->SetText(meta.line[x + startline].c_str());

			HaltResumeGui();
		}

		if(back.GetState() == STATE_CLICKED)
			choice = 0;

		for(int x=0; x < i; x++)
		{
			if(Entrie[x]->GetState() == STATE_CLICKED)
			{
				Entrie[x]->ResetState();

				string temp = meta.line[x + startline];
				while((signed)temp.find("\n") != -1)
					temp.replace(temp.find("\n"), 1, "¶");

				char new_text[256];
				sprintf (new_text, "%s", temp.c_str());
				OnScreenKeyboard(new_text, 256, true);

				mainWindow->SetState(STATE_DISABLED);
				promptWindow.SetState(STATE_DEFAULT);

				if(strcasecmp(new_text,"NULL") != 0 )
				{
					changed = true;
					meta.line[x + startline] = new_text;
					while((signed)meta.line[x + startline].find("¶") != -1)
						meta.line[x + startline].replace(meta.line[x + startline].find("¶"), 1, "\n");

					EntrieTxt[x]->SetText(meta.line[x + startline].c_str());

					quelltext.clear();
					for(int a = 0; a < (signed)meta.line.size(); a++)
						quelltext += meta.line[a];

					meta.text(quelltext, 18, 440);
					for(int x=0; x < i; x++)
						EntrieTxt[x]->SetText(meta.line[x + startline].c_str());
				}

				break;
			}
		}
	}

	if(changed)
	{
		ofstream datei(dir.c_str());
		datei << quelltext;
	//	for(int i = 0; i < (signed)meta.line.size(); i++)
	//		datei << meta.line[i];
	}

	HaltGui();
	mainWindow->Remove(&promptWindow);
	mainWindow->SetState(STATE_DEFAULT);
	ResumeGui();
	return choice;
}

