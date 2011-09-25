
#include "libwiigui/gui.h"
#include "textline.h"
#include "Prompts/prompts.h"
#include "Tools/FontSystem.h"

void TextLine::text(string text, int size, int maxWidth)
{
	textScrollPos = 0;
	line.clear();
	
	while((signed)text.find("\r\n") != -1)
		text.replace(text.find("\r\n"), 2, "\n");

	int FontSize = size * GetFontScale();
	int txtlen = text.length();
	int i = 0;
	int ch = 0;
	int lastSpace = -1;
	int lastSpaceIndex = -1;
	wchar_t *wtext = charToWideChar(text.c_str());
	wchar_t *textDynRow = new wchar_t[maxWidth];
	
	while(ch < txtlen)
	{
		textDynRow[i] = wtext[ch];
		textDynRow[i+1] = 0;

		int curTxtWidth = fontSystem->getWidth(textDynRow, FontSize);

		if((wtext[ch] == ' ' || wtext[ch] == '\n') && i >= 0)
		{
			lastSpace = ch;
			lastSpaceIndex = i;
		}

		if(wtext[ch] == ' ' || wtext[ch] == '\n' || ch == txtlen-1 || curTxtWidth >= maxWidth)
		{
			if(curTxtWidth >= maxWidth || wtext[ch] == '\n')
			{
				if(lastSpace >= 0)
				{
					while(text.substr(lastSpaceIndex, 1) != " " && text.substr(lastSpaceIndex, 1) != "\n")
						lastSpaceIndex++;
						
					line.push_back(text.substr(0, lastSpaceIndex +1));
						
					text.erase(0, lastSpaceIndex +1);
					
					ch = lastSpace; // go backwards to the last space
					lastSpace = -1; // we have used this space
					lastSpaceIndex = -1;
				}
				
				i = -1;
			}
			else if(ch == txtlen-1)
			{
				line.push_back(text);
			}
		}
		
		ch++;
		i++;
	}
}

void TextLine::list(string text)
{
	textScrollPos = 0;
	
	text.erase(text.rfind("end"));

	while(1)
	{
		if((signed)text.find("\n") == -1)
			break;
			
		line.push_back(text.substr(0, text.find("\n") -1));
		text.erase(0, text.find("\n") +1);
	}
}

int TextLine::text_up()
{
	textScrollPos--;
	if(textScrollPos < 0)
		textScrollPos = 0;
	
	return textScrollPos;
}

int TextLine::text_down(int number)
{
	textScrollPos++;
	int temp = line.size() - number;
	
	if(temp < 0)
		textScrollPos = 0;
	else if(textScrollPos > temp)
		textScrollPos = temp;
	
	return textScrollPos;
}