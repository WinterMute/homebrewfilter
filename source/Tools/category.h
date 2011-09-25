
#include <string>
#include <vector>
using namespace std;

class CategoryAvailable
{
public:
    vector<string>	categories;
	vector<string>	apps[99];
};
extern CategoryAvailable AvailableCategory;

void AvailableCategoryLoad(string pfad);
void AvailableCategorySave(string pfad);

int KategorieNr(string Kategoriename);

void KategorieEinfuegen(string Kategorie);
void KategorieEntfernen(string Kategorie);
void KategorieUmbenennen(string suchen, string ersetzen);
void KategorieVerschieben(string Kategorie1, bool vor, string Kategorie2);

void AppEinfuegen(string Kategorie, string AppOrdner);
void AppEntfernen(string Kategorie, string AppOrdner);
void AppVerschieben(string Kategorie, string AppOrdner1, bool vor, string AppOrdner2);
