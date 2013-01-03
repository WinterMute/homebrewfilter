
#include <sstream>
#include <cstring>
#include <algorithm>
#include "main.h"

string Zeilenumbruch = "\n";
CategoryAvailable AvailableCategory;

// einlesen
void AvailableCategoryLoad(string pfad)
{
#if defined(STBOOT) || defined(STBOOTVWII)
	string line, quelltext, temp;
	ifstream in(pfad.c_str());
	while(getline(in, line))
		quelltext = quelltext + line;

	int Anzahl = 0;
	for (int i = 0; (unsigned)i < quelltext.size(); i++)
	{
		temp = quelltext[i];
		if(strcmp(temp.c_str(),"[") == 0)
			Anzahl++;
	}

	AvailableCategory.categories.clear();
	AvailableCategory.categories.push_back(tr(Settings.category_name_all));
	// alle Kategorien durchlaufen
	for(int i=1; i < Anzahl +1; i++)
	{
		AvailableCategory.apps[i].clear();
		// Kategorie durchsuchen
		temp = quelltext.erase(0,quelltext.find("[") +1);
		AvailableCategory.categories.push_back(quelltext.substr(0, quelltext.find("]")));
		temp = quelltext.erase(0,quelltext.find("]") +1);
		if((signed)temp.find("[") != -1)
			temp.erase(temp.find("["));

		// alle Apps auflisten
		while((signed)temp.find("*") != -1)
		{
			string temp2 = temp.erase(0,temp.find("*") +1);
			temp2 = temp2.erase(temp.find("*"));
			transform(temp2.begin(), temp2.end(), temp2.begin(),::tolower);	// in kleinebuchstaben umwandeln
			AvailableCategory.apps[i].push_back(temp2);
			temp.erase(0,temp.find("*") +1);
		}
	}
#else
	s32 fd;
	static fstats filestats ATTRIBUTE_ALIGN(32);
	static u8 filearray[1024] ATTRIBUTE_ALIGN(32);

	fd = ISFS_Open(pfad.c_str(), ISFS_OPEN_READ);
	if (fd <= 0)
		ISFS_Close(fd);

	ISFS_GetFileStats(fd, &filestats);

	ISFS_Read(fd, filearray, filestats.file_length);

	ISFS_Close(fd);

	string line, source, temp;
	istringstream in((char*)filearray);
	while(getline(in, line))
		source += line;

	int anzahl = 0;
	for (int i = 0; (unsigned)i < source.size(); i++)
	{
		temp = source[i];
		if(strcmp(temp.c_str(),"[") == 0)
			anzahl++;
	}

	AvailableCategory.categories.clear();
	AvailableCategory.categories.push_back(tr(Settings.category_name_all));

	// alle Kategorien durchlaufen
	for(int i=1; i < anzahl +1; i++)
	{
		AvailableCategory.apps[i].clear();
		// Kategorie durchsuchen
		temp = source.erase(0,source.find("[") +1);
		AvailableCategory.categories.push_back(source.substr(0, source.find("]")));
		temp = source.erase(0,source.find("]") +1);
		if((signed)temp.find("[") != -1)
			temp.erase(temp.find("["));

		// alle Apps auflisten
		while((signed)temp.find("*") != -1)
		{
			string temp2 = temp.erase(0,temp.find("*") +1);
			temp2 = temp2.erase(temp.find("*"));
			transform(temp2.begin(), temp2.end(), temp2.begin(),::tolower);	// in kleinebuchstaben umwandeln
			AvailableCategory.apps[i].push_back(temp2);
			temp.erase(0,temp.find("*") +1);
		}
	}
#endif
}

// speichern
void AvailableCategorySave(string pfad)
{
#if defined(STBOOT) || defined(STBOOTVWII)
	ofstream out(pfad.c_str());
	// alle Kategorien durchlaufen auer "Alle"

	for(int i = 1; i < (signed)AvailableCategory.categories.size(); i++)
	{
		// Kategorie speichern
		out << "[" << AvailableCategory.categories[i] << "]" << endl;
		// alle Apps auflisten und speichern
		for(int x = 0; x < (signed)AvailableCategory.apps[i].size(); x++)
			out << "*" << AvailableCategory.apps[i][x] << "*" << endl;
		// Zeilenumbruch nach Kategorie
		out << endl;
	}
#else
	ISFS_Delete(pfad.c_str());
	ISFS_CreateFile(pfad.c_str(), 0, 3, 3, 3);
	s32 file = ISFS_Open(pfad.c_str(), ISFS_OPEN_RW);
	if (file > 0)
	{
		stringstream save_category;

		// alle Kategorien durchlaufen auer "Alle"
		for(int i = 1; i < (signed)AvailableCategory.categories.size(); i++)
		{
			// Kategorie speichern
			save_category << "[" << AvailableCategory.categories[i] << "]" << endl;
			// alle Apps auflisten und speichern
			for(int x = 0; x < (signed)AvailableCategory.apps[i].size(); x++)
				save_category << "*" << AvailableCategory.apps[i][x] << "*" << endl;
			// Zeilenumbruch nach Kategorie
			save_category << endl;
		}

		char *pbuf = NULL;
		unsigned int psize = save_category.str().size();

		pbuf = (char*)memalign( 32, sizeof(char) *psize );
		memset( pbuf, 0, sizeof(char) *psize );

		char text[psize];
		sprintf(text, "%s", save_category.str().c_str());
		strcpy(pbuf, text);

		ISFS_Write(file, pbuf, sizeof(char) *psize);
	}
	ISFS_Close(file);
#endif
}

int KategorieNr(string Kategorie)
{
    int nr = -1;
    for(int i = 1; i < (signed)AvailableCategory.categories.size(); i++)
	{
		if(AvailableCategory.categories[i] == Kategorie)
		{
			// ersetzen
			nr = i;
			break;
		}
	}
    return nr;
}

void KategorieEinfuegen(string Kategorie)
{
	bool found = false;
	for(int i = 0; i < (signed)AvailableCategory.categories.size(); i++)
	{
		if(AvailableCategory.categories[i] == Kategorie)
		{
			found = true;
			break;
		}
	}
	if(!found)
		AvailableCategory.categories.push_back(Kategorie);
}

void KategorieEntfernen(string Kategorie)
{
    int del_nr = 0;

	if(AvailableCategory.categories[KategorieNr(Kategorie)] == Kategorie)
		del_nr = KategorieNr(Kategorie);

	vector<string>::iterator pos = find(AvailableCategory.categories.begin(), AvailableCategory.categories.end(), Kategorie);
	if ( pos != AvailableCategory.categories.end() )
	{
		AvailableCategory.categories.erase(pos);
		for(int i = del_nr; i < (signed)AvailableCategory.categories.size() +1; i++)
			AvailableCategory.apps[i] = AvailableCategory.apps[i +1];
	}
}

void KategorieUmbenennen(string suchen, string ersetzen)
{
    if(KategorieNr(suchen) != -1)
		AvailableCategory.categories[KategorieNr(suchen)] = ersetzen;
}

void KategorieVerschieben(string Kategorie1, bool vor, string Kategorie2)
{
	int a = KategorieNr(Kategorie1);
	if(a != -1 && Kategorie1 != Kategorie2)
	{
		// apps temporr speichen und Kategorie entfernen
		vector<string> temp_apps = AvailableCategory.apps[a];
		KategorieEntfernen(Kategorie1);

		vector<string>::iterator pos = find(AvailableCategory.categories.begin(), AvailableCategory.categories.end(), Kategorie2);
		if(vor)
			AvailableCategory.categories.insert(pos, Kategorie1);
		else
		{
			for(int x = 0; x < (signed)AvailableCategory.categories.size(); x++)
			{
				if(AvailableCategory.categories[x] == Kategorie2)
				{
					if(x+1 < (signed)AvailableCategory.categories.size())
					{
						pos = find(AvailableCategory.categories.begin(), AvailableCategory.categories.end(), AvailableCategory.categories[x +1]);
						AvailableCategory.categories.insert(pos, Kategorie1);
					}
					else
						AvailableCategory.categories.push_back(Kategorie1);
					break;
				}
			}
		}
		// apps verschieben
		a = KategorieNr(Kategorie1);
		for(int i = AvailableCategory.categories.size(); i >= a; i--)
			AvailableCategory.apps[i +1] = AvailableCategory.apps[i];
		// apps leeren
		AvailableCategory.apps[a].clear();
		// apps befllen
		for(int i = 0; i < (signed)temp_apps.size(); i++)
			AvailableCategory.apps[a].push_back(temp_apps[i]);
	}
}

void AppEinfuegen(string Kategorie, string App)
{
	if((signed)App.find_last_of("/") != -1)
		App.erase(App.find_last_of("/"));
	if((signed)App.find_last_of("/") != -1)
		App.erase(0,App.find_last_of("/") +1);

	// kategorie auswhlen

	int i = KategorieNr(Kategorie);
	if(i != -1)
	{
		// berprfen, ob App in Kategorie ist
		bool found = false;
		for(int x = 0; x < (signed)AvailableCategory.apps[i].size(); x++)
		{
			if(AvailableCategory.apps[i][x] == App)
				found = true;
		}
		if(!found)
			AvailableCategory.apps[i].insert(AvailableCategory.apps[i].begin(), App);
	}
}

void AppEntfernen(string Kategorie, string App)
{
	if((signed)App.find_last_of("/") != -1)
		App.erase(App.find_last_of("/"));
	if((signed)App.find_last_of("/") != -1)
		App.erase(0,App.find_last_of("/") +1);
	std::transform(App.begin(),App.end(),App.begin(),::tolower);	// in kleinebuchstaben umwandeln

	// Kategorie auswhlen
	int i = KategorieNr(Kategorie);
	if(i != -1)
	{
		// berprfen, ob App in Kategorie ist
		vector<string>::iterator pos = find(AvailableCategory.apps[i].begin(), AvailableCategory.apps[i].end(), App);
		if ( pos != AvailableCategory.apps[i].end() )
			AvailableCategory.apps[i].erase(pos);
	}
}

void AppVerschieben(string Kategorie, string AppOrdner1, bool vor, string AppOrdner2)
{
	if((signed)AppOrdner1.find_last_of("/") != -1)
		AppOrdner1.erase(AppOrdner1.find_last_of("/"));
	if((signed)AppOrdner1.find_last_of("/") != -1)
		AppOrdner1.erase(0,AppOrdner1.find_last_of("/") +1);

	if((signed)AppOrdner2.find_last_of("/") != -1)
		AppOrdner2.erase(AppOrdner2.find_last_of("/"));
	if((signed)AppOrdner2.find_last_of("/") != -1)
		AppOrdner2.erase(0,AppOrdner2.find_last_of("/") +1);

	// Kategorie auswhlen
	int i = KategorieNr(Kategorie);
	if(i != -1 && AppOrdner1 != AppOrdner2)
	{
		// berprfen, ob App in Kategorie ist
		vector<string>::iterator pos = find(AvailableCategory.apps[i].begin(), AvailableCategory.apps[i].end(), AppOrdner1);
		if ( pos != AvailableCategory.apps[i].end() )
			AvailableCategory.apps[i].erase(pos);

		if(vor)
		{
		    pos = find(AvailableCategory.apps[i].begin(), AvailableCategory.apps[i].end(), AppOrdner2);
            AvailableCategory.apps[i].insert(pos, AppOrdner1);
        }
        else
        {
			for(int x = 0; x < (signed)AvailableCategory.apps[i].size(); x++)
			{
				if(AvailableCategory.apps[i][x] == AppOrdner2)
				{
					if(x+1 < (signed)AvailableCategory.apps[i].size())
					{
						pos = find(AvailableCategory.apps[i].begin(), AvailableCategory.apps[i].end(), AvailableCategory.apps[i][x +1]);
						AvailableCategory.apps[i].insert(pos, AppOrdner1);
					}
					else
						AvailableCategory.apps[i].push_back(AppOrdner1);
					break;
				}
			}
		}
	}
}
