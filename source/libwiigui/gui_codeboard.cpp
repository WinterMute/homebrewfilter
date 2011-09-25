/****************************************************************************
 * libwiigui
 *
 * Tantric 2009
 *
 * gui_keyboard.cpp
 *
 * GUI class definitions
 ***************************************************************************/

#include "gui.h"
#include "Tools/translate.h"

/**
 * Constructor for the GuiKeyboard class.
 */
#include "Prompts/prompts.h"
GuiCodeboard::GuiCodeboard(char * t, u32 max)
{
	width = 540;
	height = 400;
	selectable = true;
	focus = 0; // allow focus
	alignmentHor = ALIGN_CENTRE;
	alignmentVert = ALIGN_MIDDLE;
	strncpy(kbtextstr, t, max);
	kbtextstr[max] = 0;
	kbtextmaxlen = max;

	Key thekeys[4][3] = {
	{
		{'1'},
		{'2'},
		{'3'}
	},
	{
		{'4'},
		{'5'},
		{'6'}
	},
	{
		{'7'},
		{'8'},
		{'9'}
	},
	{
		{'\0'},
		{'0'},
		{'\0'}
	}
	};
	memcpy(keys, thekeys, sizeof(thekeys));

	for(int i = 0; i < 4; i++)
	{
		keyTextbox[i] = new GuiImageData(keyboard_key_png);
		keyTextboxImg[i] = new GuiImage(keyTextbox[i]);
		keyTextboxImg[i]->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
		if(i == 0)
			keyTextboxImg[i]->SetPosition(-75, 0);
		else if(i == 1)
			keyTextboxImg[i]->SetPosition(-25, 0);
		else if(i == 2)
			keyTextboxImg[i]->SetPosition(25, 0);
		else if(i == 3)
			keyTextboxImg[i]->SetPosition(75, 0);
		this->Append(keyTextboxImg[i]);
		
		kbText[i] = new GuiText("\0", 20, (GXColor){0, 0, 0, 0xff});
		kbText[i]->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
		kbText[i]->SetPosition(keyTextboxImg[i]->GetLeft() + keyTextboxImg[i]->GetWidth()/2, 13);
		this->Append(kbText[i]);
	}
	
	key = new GuiImageData(keyboard_key_png);
	keyOver = new GuiImageData(keyboard_key_over_png);
	keyMedium = new GuiImageData(keyboard_mediumkey_png);
	keyMediumOver = new GuiImageData(keyboard_mediumkey_over_png);

//	keySoundOver = new GuiSound(button_over_pcm, button_over_pcm_size, SOUND_PCM);
//	keySoundClick = new GuiSound(button_click_pcm, button_click_pcm_size, SOUND_PCM);
	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);
	trigB = new GuiTrigger;
	trigB->SetButtonOnlyTrigger(-1, WPAD_BUTTON_B | WPAD_CLASSIC_BUTTON_B, PAD_BUTTON_B);

	int distance = 40;
	int pos = (width-(3*key->GetWidth()+2*distance))/2;
	
	keyBackImg = new GuiImage(keyMedium);
	keyBackOverImg = new GuiImage(keyMediumOver);
	keyBackText = new GuiText("i", 40, (GXColor){0, 0, 0, 0xff});
	keyBackText->SetFont(symbol_ttf, symbol_ttf_size);
	keyBack = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyBack->SetImage(keyBackImg);
	keyBack->SetImageOver(keyBackOverImg);
	keyBack->SetLabel(keyBackText);
//	keyBack->SetSoundOver(keySoundOver);
//	keyBack->SetSoundClick(keySoundClick);
	keyBack->SetTrigger(trigA);
	keyBack->SetTrigger(trigB);
	keyBack->SetPosition(pos-keyMedium->GetWidth()+key->GetWidth(), 3*(key->GetWidth() + 20)+80);
	keyBack->SetEffectGrow();
	this->Append(keyBack);

	keyEmptyImg = new GuiImage(keyMedium);
	keyEmptyOverImg = new GuiImage(keyMediumOver);
	keyEmptyText = new GuiText(tr("Empty"), 20, (GXColor){0, 0, 0, 0xff});
	keyEmpty = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyEmpty->SetImage(keyEmptyImg);
	keyEmpty->SetImageOver(keyEmptyOverImg);
	keyEmpty->SetLabel(keyEmptyText);
//	keyEmpty->SetSoundOver(keySoundOver);
//	keyEmpty->SetSoundClick(keySoundClick);
	keyEmpty->SetTrigger(trigA);
	keyEmpty->SetPosition(pos+2*(key->GetWidth()+distance), 3*(key->GetWidth() + 20)+80);
	keyEmpty->SetEffectGrow();
	this->Append(keyEmpty);

	char txt[2] = { 0 };
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<3; j++)
		{
			if(keys[i][j].ch != '\0')
			{
				txt[0] = keys[i][j].ch;
				keyImg[i][j] = new GuiImage(key);
				keyImgOver[i][j] = new GuiImage(keyOver);
				keyTxt[i][j] = new GuiText(txt, 20, (GXColor){0, 0, 0, 0xff});
				keyTxt[i][j]->SetAlignment(ALIGN_CENTRE, ALIGN_BOTTOM);
				keyTxt[i][j]->SetPosition(0, -10);
				keyBtn[i][j] = new GuiButton(key->GetWidth(), key->GetHeight());
				keyBtn[i][j]->SetImage(keyImg[i][j]);
				keyBtn[i][j]->SetImageOver(keyImgOver[i][j]);
//				keyBtn[i][j]->SetSoundOver(keySoundOver);
//				keyBtn[i][j]->SetSoundClick(keySoundClick);
				keyBtn[i][j]->SetTrigger(trigA);
				keyBtn[i][j]->SetLabel(keyTxt[i][j]);
				keyBtn[i][j]->SetPosition(pos+j*(key->GetWidth()+distance), i*(key->GetWidth() + 20)+80);
				keyBtn[i][j]->SetEffectGrow();
				this->Append(keyBtn[i][j]);
			}
		}
	}
}

/**
 * Destructor for the GuiKeyboard class.
 */
GuiCodeboard::~GuiCodeboard()
{
	for(int i = 0; i < 4; i++)
	{
		delete kbText[i];
		delete keyTextbox[i];
		delete keyTextboxImg[i];
	}
	delete keyBackText;
	delete keyBackImg;
	delete keyBackOverImg;
	delete keyBack;
	delete keyEmptyText;
	delete keyEmptyImg;
	delete keyEmptyOverImg;
	delete keyEmpty;
	delete key;
	delete keyOver;
	delete keyMedium;
	delete keyMediumOver;
//	delete keySoundOver;
//	delete keySoundClick;
	delete trigA;

	for(int i=0; i<4; i++)
	{
		for(int j=0; j<3; j++)
		{
			if(keys[i][j].ch != '\0')
			{
				delete keyImg[i][j];
				delete keyImgOver[i][j];
				delete keyTxt[i][j];
				delete keyBtn[i][j];
			}
		}
	}
}

void GuiCodeboard::Update(GuiTrigger * t)
{
	if(_elements.size() == 0 || (state == STATE_DISABLED && parentElement))
		return;

	for (u8 i = 0; i < _elements.size(); i++)
	{
		try	{ _elements.at(i)->Update(t); }
		catch (const std::exception& e) { }
	}

	bool update = false;

	if(keyBack->GetState() == STATE_CLICKED)
	{
		kbtextstr[strlen(kbtextstr)-1] = 0;
		kbText[strlen(kbtextstr)]->SetText("\0");
		keyBack->SetState(STATE_SELECTED, t->chan);
	}
	else if(keyEmpty->GetState() == STATE_CLICKED)
	{
		while (strlen(kbtextstr) > 0)
		{
			kbtextstr[strlen(kbtextstr)-1] = 0;
			kbText[strlen(kbtextstr)]->SetText("\0");
		}
		keyEmpty->SetState(STATE_SELECTED, t->chan);
	}

	char txt[2] = { 0 };

	for(int i=0; i<4; i++)
	{
		for(int j=0; j<3; j++)
		{
			if(keys[i][j].ch != '\0')
			{
				if(update)
				{
					txt[0] = keys[i][j].ch;
					
					keyTxt[i][j]->SetText(txt);
				}
				
				if(keyBtn[i][j]->GetState() == STATE_CLICKED)
				{
					if((signed)strlen(kbtextstr) < kbtextmaxlen)
						kbtextstr[strlen(kbtextstr)] = keys[i][j].ch;
						
					int letter = strlen(kbtextstr) -1;
					string buffer = kbtextstr;
					kbText[letter]->SetText(buffer.substr(letter, 1).c_str());
					keyBtn[i][j]->SetState(STATE_SELECTED, t->chan);
				}
			}
		}
	}

	this->ToggleFocus(t);

	if(focus) // only send actions to this window if it's in focus
	{
		// pad/joystick navigation
		if(t->Right())
			this->MoveSelectionHor(1);
		else if(t->Left())
			this->MoveSelectionHor(-1);
		else if(t->Down())
			this->MoveSelectionVert(1);
		else if(t->Up())
			this->MoveSelectionVert(-1);
	}
}
