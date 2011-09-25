
#include <string.h>

#include "parser.h"

string parser(string src, const char *suche1, const char *suche2)
{
	int strpos1 = src.find(suche1);
	if(strpos1 == -1) return "\0";
	else{
		int strpos2 = src.find(suche2);
		if(strpos2 == -1) return "\0";
		else{
			int laenge = strpos2 - strpos1 - strlen(suche1);
			return src.substr(strpos1 + strlen(suche1), laenge);
		}
	}
}
