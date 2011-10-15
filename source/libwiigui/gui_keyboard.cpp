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

#define MAX_KEYBOARD_DISPLAY	35

static char tmptxt[MAX_KEYBOARD_DISPLAY +1];

static char * GetDisplayText(int pos, char * t)
{
	if(!t)
		return NULL;

	int len = strlen(t);

	if(len < MAX_KEYBOARD_DISPLAY)
		return t;

	strncpy(tmptxt, &t[pos], MAX_KEYBOARD_DISPLAY);
	tmptxt[MAX_KEYBOARD_DISPLAY] = 0;
	return &tmptxt[0];
}

/**
 * Constructor for the GuiKeyboard class.
 */

GuiKeyboard::GuiKeyboard(char * t, u32 max, bool Br)
{
	width = 540;
	height = 400;
	shift = 0;
	caps = 0;
	br = Br;
	selectable = true;
	focus = 0; // allow focus
	alignmentHor = ALIGN_CENTRE;
	alignmentVert = ALIGN_MIDDLE;
	strncpy(kbtextstr, t, max);
	kbtextstr[max] = 0;
	kbtextmaxlen = max;
	if(strlen(kbtextstr) > MAX_KEYBOARD_DISPLAY)
		letterPos = strlen(kbtextstr) - MAX_KEYBOARD_DISPLAY;
	else
		letterPos = 0;

	Key thekeys[4][13] = {
	{
		{'1','!'},
		{'2','@'},
		{'3','#'},
		{'4','$'},
		{'5','%'},
		{'6','^'},
		{'7','&'},
		{'8','*'},
		{'9','('},
		{'0',')'},
		{'\0','\0'}
	},
	{
		{'q','Q'},
		{'w','W'},
		{'e','E'},
		{'r','R'},
		{'t','T'},
		{'z','Z'},
		{'u','U'},
		{'i','I'},
		{'o','O'},
		{'p','P'},
		{'-','_'}
	},
	{
		{'a','A'},
		{'s','S'},
		{'d','D'},
		{'f','F'},
		{'g','G'},
		{'h','H'},
		{'j','J'},
		{'k','K'},
		{'l','L'},
		{';',':'},
		{'\'','"'}
	},

	{
		{'y','Y'},
		{'x','X'},
		{'c','C'},
		{'v','V'},
		{'b','B'},
		{'n','N'},
		{'m','M'},
		{',','<'},
		{'.','>'},
		{'/','?'},
		{'\0','\0'},
		{'\0','\0'},
		{'\0','\0'}
	}
	};
	memcpy(keys, thekeys, sizeof(thekeys));

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WPAD_BUTTON_A | WPAD_CLASSIC_BUTTON_A, PAD_BUTTON_A);

	keyTextbox = new GuiImageData(keyboard_textbox_png);
	keyTextboxImg = new GuiImage(keyTextbox);
	keyTextboxImg->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	keyTextboxImg->SetPosition(0, 0);
	keyTextboxImg->SetSelectable(false);
	this->Append(keyTextboxImg);

	kbText = new GuiText(GetDisplayText(letterPos, kbtextstr), 20, (GXColor){0, 0, 0, 0xff});
	kbText->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	kbText->SetPosition(0, 13);
	kbText->SetLetterPos(strlen(GetDisplayText(letterPos, kbtextstr)));
	kbText->SetSelectable(false);
	this->Append(kbText);

	textpointer = new GuiImageData(textpointer_img_png);
	textpointerImg = new GuiImage(textpointer);
	textpointerImg->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	textpointerImg->SetPosition(-kbText->GetTextWidth()/2 + kbText->GetTextWidthToPosition(kbText->GetLetterPos()) +1, 10);
	textpointerImg->SetSelectable(false);
	this->Append(textpointerImg);

	textpointerBtn = new GuiButton(keyTextbox->GetWidth(), keyTextbox->GetHeight());
	textpointerBtn->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	textpointerBtn->SetPosition(0, 0);
	textpointerBtn->SetTrigger(trigA);
	this->Append(textpointerBtn);

	key = new GuiImageData(keyboard_key_png);
	keyOver = new GuiImageData(keyboard_key_over_png);
	keyMedium = new GuiImageData(keyboard_mediumkey_png);
	keyMediumOver = new GuiImageData(keyboard_mediumkey_over_png);
	keyLarge = new GuiImageData(keyboard_largekey_png);
	keyLargeOver = new GuiImageData(keyboard_largekey_over_png);

//	keySoundOver = new GuiSound(button_over_pcm, button_over_pcm_size, SOUND_PCM);
//	keySoundClick = new GuiSound(button_click_pcm, button_click_pcm_size, SOUND_PCM);

	keyBrImg = new GuiImage(keyMedium);
	keyBrOverImg = new GuiImage(keyMediumOver);
	keyBrText = new GuiText(tr("Linebreak"), 20, (GXColor){0, 0, 0, 0xff});
	keyBr = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyBr->SetImage(keyBrImg);
	keyBr->SetImageOver(keyBrOverImg);
	keyBr->SetLabel(keyBrText);
//	keyBr->SetSoundOver(keySoundOver);
//	keyBr->SetSoundClick(keySoundClick);
	keyBr->SetTrigger(trigA);
	keyBr->SetPosition(0, 4*42+80);
	keyBr->SetEffectGrow();
	if(br)
		this->Append(keyBr);
	
	keyBackImg = new GuiImage(keyMedium);
	keyBackOverImg = new GuiImage(keyMediumOver);
	keyBackText = new GuiText("Back", 20, (GXColor){0, 0, 0, 0xff});
	keyBack = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyBack->SetImage(keyBackImg);
	keyBack->SetImageOver(keyBackOverImg);
	keyBack->SetLabel(keyBackText);
//	keyBack->SetSoundOver(keySoundOver);
//	keyBack->SetSoundClick(keySoundClick);
	keyBack->SetTrigger(trigA);
	keyBack->SetPosition(10*42+40, 0*42+80);
	keyBack->SetEffectGrow();
	this->Append(keyBack);

	keyCapsImg = new GuiImage(keyMedium);
	keyCapsOverImg = new GuiImage(keyMediumOver);
	keyCapsText = new GuiText("Caps", 20, (GXColor){0, 0, 0, 0xff});
	keyCaps = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyCaps->SetImage(keyCapsImg);
	keyCaps->SetImageOver(keyCapsOverImg);
	keyCaps->SetLabel(keyCapsText);
//	keyCaps->SetSoundOver(keySoundOver);
//	keyCaps->SetSoundClick(keySoundClick);
	keyCaps->SetTrigger(trigA);
	keyCaps->SetPosition(0, 2*42+80);
	keyCaps->SetEffectGrow();
	this->Append(keyCaps);

	keyShiftImg = new GuiImage(keyMedium);
	keyShiftOverImg = new GuiImage(keyMediumOver);
	keyShiftText = new GuiText("Shift", 20, (GXColor){0, 0, 0, 0xff});
	keyShift = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyShift->SetImage(keyShiftImg);
	keyShift->SetImageOver(keyShiftOverImg);
	keyShift->SetLabel(keyShiftText);
//	keyShift->SetSoundOver(keySoundOver);
//	keyShift->SetSoundClick(keySoundClick);
	keyShift->SetTrigger(trigA);
	keyShift->SetPosition(21, 3*42+80);
	keyShift->SetEffectGrow();
	this->Append(keyShift);

	keySpaceImg = new GuiImage(keyLarge);
	keySpaceOverImg = new GuiImage(keyLargeOver);
	keySpace = new GuiButton(keyLarge->GetWidth(), keyLarge->GetHeight());
	keySpace->SetImage(keySpaceImg);
	keySpace->SetImageOver(keySpaceOverImg);
//	keySpace->SetSoundOver(keySoundOver);
//	keySpace->SetSoundClick(keySoundClick);
	keySpace->SetTrigger(trigA);
	keySpace->SetPosition(0, 4*42+80);
	keySpace->SetAlignment(ALIGN_CENTRE, ALIGN_TOP);
	keySpace->SetEffectGrow();
	this->Append(keySpace);

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
	keyEmpty->SetPosition(10*42+40, 4*42+80);
	keyEmpty->SetEffectGrow();
	this->Append(keyEmpty);

	char txt[2] = { 0, 0 };

	for(int i=0; i<4; i++)
	{
		for(int j=0; j<11; j++)
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
				keyBtn[i][j]->SetPosition(j*42+21*i+40, i*42+80);
				keyBtn[i][j]->SetEffectGrow();
				this->Append(keyBtn[i][j]);
			}
		}
	}
}

/**
 * Destructor for the GuiKeyboard class.
 */
GuiKeyboard::~GuiKeyboard()
{
	delete kbText;
	delete keyTextbox;
	delete keyTextboxImg;
	delete textpointer;
	delete textpointerImg;
	delete textpointerBtn;
	delete keyCapsText;
	delete keyCapsImg;
	delete keyCapsOverImg;
	delete keyCaps;
	delete keyShiftText;
	delete keyShiftImg;
	delete keyShiftOverImg;
	delete keyShift;
	if(br)
	{
		delete keyBrText;
		delete keyBrImg;
		delete keyBrOverImg;
		delete keyBr;
	}
	delete keyBackText;
	delete keyBackImg;
	delete keyBackOverImg;
	delete keyBack;
	delete keySpaceImg;
	delete keySpaceOverImg;
	delete keySpace;
	delete keyEmptyText;
	delete keyEmptyImg;
	delete keyEmptyOverImg;
	delete keyEmpty;
	delete key;
	delete keyOver;
	delete keyMedium;
	delete keyMediumOver;
	delete keyLarge;
	delete keyLargeOver;
//	delete keySoundOver;
//	delete keySoundClick;
	delete trigA;

	for(int i=0; i<4; i++)
	{
		for(int j=0; j<11; j++)
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

void GuiKeyboard::Update(GuiTrigger * t)
{
	if(_elements.size() == 0 || (state == STATE_DISABLED && parentElement))
		return;

	for (u8 i = 0; i < _elements.size(); i++)
	{
		try	{ _elements.at(i)->Update(t); }
		catch (const std::exception& e) { }
	}

	bool update = false;
	
	if(keySpace->GetState() == STATE_CLICKED)
	{
		keySpace->SetState(STATE_SELECTED, t->chan);
		if(strlen(kbtextstr) < kbtextmaxlen)
		{
			for(int x = strlen(kbtextstr); x > kbText->GetLetterPos() + letterPos; x--)
				kbtextstr[x] = kbtextstr[x-1];
				
			kbtextstr[kbText->GetLetterPos() + letterPos] = ' ';
				
			if(strlen(kbtextstr) > MAX_KEYBOARD_DISPLAY)
				letterPos++;
			else
				kbText->SetLetterPos(kbText->GetLetterPos() +1);
				
			kbText->SetText(GetDisplayText(letterPos, kbtextstr));
			textpointerImg->SetPosition(-kbText->GetTextWidth()/2 + kbText->GetTextWidthToPosition(kbText->GetLetterPos()) +1, 10);
		}
	}
	else if(keyBack->GetState() == STATE_CLICKED)
	{
		keyBack->SetState(STATE_SELECTED, t->chan);
		if(kbText->GetLetterPos() > 0)
		{
			for(int i = kbText->GetLetterPos() + letterPos -1; i < (signed)strlen(kbtextstr) -1; i++)
				kbtextstr[i] = kbtextstr[i +1];
				
			kbtextstr[strlen(kbtextstr) -1] = 0;
			if(letterPos > 0)
			{
				letterPos--;
				kbText->SetText(GetDisplayText(letterPos, kbtextstr));
			}
			else
			{
				kbText->SetText(GetDisplayText(letterPos, kbtextstr));
				kbText->SetLetterPos(kbText->GetLetterPos()-1);
			}
			
			textpointerImg->SetPosition(-kbText->GetTextWidth()/2 + kbText->GetTextWidthToPosition(kbText->GetLetterPos()) +1, 10);
		}
	}
	else if(keyBr->GetState() == STATE_CLICKED)
	{
		keyBr->SetState(STATE_SELECTED, t->chan);
		if(strlen(kbtextstr) < kbtextmaxlen)
		{
			for(int x = strlen(kbtextstr); x > kbText->GetLetterPos() + letterPos; x--)
				kbtextstr[x] = kbtextstr[x-1];
				
			kbtextstr[kbText->GetLetterPos() + letterPos] = '¶';
				
			if(strlen(kbtextstr) > MAX_KEYBOARD_DISPLAY)
				letterPos++;
			else
				kbText->SetLetterPos(kbText->GetLetterPos() +1);
				
			kbText->SetText(GetDisplayText(letterPos, kbtextstr));
			textpointerImg->SetPosition(-kbText->GetTextWidth()/2 + kbText->GetTextWidthToPosition(kbText->GetLetterPos()) +1, 10);
		}
	}
	else if(keyEmpty->GetState() == STATE_CLICKED)
	{
		while (strlen(kbtextstr) > 0)
			kbtextstr[strlen(kbtextstr)-1] = 0;
			
		letterPos = 0;
		kbText->SetLetterPos(0);
		kbText->SetText(GetDisplayText(letterPos, kbtextstr));
		textpointerImg->SetPosition(0, 10);
		keyEmpty->SetState(STATE_SELECTED, t->chan);
	}
	else if(keyShift->GetState() == STATE_CLICKED)
	{
		shift ^= 1;
		keyShift->SetState(STATE_SELECTED, t->chan);
		update = true;
	}
	else if(keyCaps->GetState() == STATE_CLICKED)
	{
		caps ^= 1;
		keyCaps->SetState(STATE_SELECTED, t->chan);
		update = true;
	}
	else if(textpointerBtn->GetState() == STATE_CLICKED)
	{
		int pos = kbText->GetTextMaxWidth((int)t->wpad->ir.x-48 - (width - kbText->GetTextWidth()) /2);
		
		if(pos == 0)
			textpointerImg->SetPosition(-kbText->GetTextWidth()/2, 10);
		else
			textpointerImg->SetPosition(-kbText->GetTextWidth()/2 + pos + 1, 10);
		
		textpointerBtn->SetState(STATE_SELECTED, t->chan);
	}

	char txt[2] = { 0, 0 };

	startloop:

	for(int i=0; i<4; i++)
	{
		for(int j=0; j<11; j++)
		{
			if(keys[i][j].ch != '\0')
			{
				if(update)
				{
					if(shift || caps)
						txt[0] = keys[i][j].chShift;
					else
						txt[0] = keys[i][j].ch;

					keyTxt[i][j]->SetText(txt);
				}

				if(keyBtn[i][j]->GetState() == STATE_CLICKED)
				{
					keyBtn[i][j]->SetState(STATE_SELECTED, t->chan);
					if(strlen(kbtextstr) < kbtextmaxlen)
					{
						if(shift || caps)
						{
							for(int x = strlen(kbtextstr); x > kbText->GetLetterPos() + letterPos; x--)
								kbtextstr[x] = kbtextstr[x-1];
								
							kbtextstr[kbText->GetLetterPos() + letterPos] = keys[i][j].chShift;
								
							if(strlen(kbtextstr) > MAX_KEYBOARD_DISPLAY)
								letterPos++;
							else
								kbText->SetLetterPos(kbText->GetLetterPos() +1);
						}
						else
						{
							for(int x = strlen(kbtextstr); x > kbText->GetLetterPos() + letterPos; x--)
								kbtextstr[x] = kbtextstr[x-1];
								
							kbtextstr[kbText->GetLetterPos() + letterPos] = keys[i][j].ch;
								
							if(strlen(kbtextstr) > MAX_KEYBOARD_DISPLAY)
								letterPos++;
							else
								kbText->SetLetterPos(kbText->GetLetterPos() +1);
						}
					}
					kbText->SetText(GetDisplayText(letterPos, kbtextstr));
					textpointerImg->SetPosition(-kbText->GetTextWidth()/2 + kbText->GetTextWidthToPosition(kbText->GetLetterPos()) +1, 10);
					
					if(shift)
					{
						shift ^= 1;
						update = true;
						goto startloop;
					}
				}
			}
		}
	}
	this->ToggleFocus(t);

	if(focus) // only send actions to this window if it's in focus
	{
		if(textpointerBtn->GetState() == STATE_SELECTED)
		{
			bool changed = false;
			int pos = 0;
			if(t->Right())
			{
				changed = true;
				if(kbText->GetLetterPos() == MAX_KEYBOARD_DISPLAY && kbText->GetLetterPos() + (unsigned)letterPos < strlen(kbtextstr))
				{
					letterPos++;
					kbText->SetText(GetDisplayText(letterPos, kbtextstr));
				}
				pos = kbText->GetTextWidthToPosition(kbText->GetLetterPos() +1);
			}
			else if(t->Left())
			{
				changed = true;
				if(letterPos > 0 && kbText->GetLetterPos() == 0)
				{
					letterPos--;
					kbText->SetText(GetDisplayText(letterPos, kbtextstr));
				}
				pos = kbText->GetTextWidthToPosition(kbText->GetLetterPos() -1);
			}
			else if(t->Down())
				this->MoveSelectionVert(1);
			
			if(changed)
			{
				changed = false;
				if(pos == 0)
					textpointerImg->SetPosition(-kbText->GetTextWidth()/2, 10);
				else
					textpointerImg->SetPosition(-kbText->GetTextWidth()/2 + pos + 1, 10);
			}
		}
		else
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
}
