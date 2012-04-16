
#include <string>
using namespace std;

string CheckNewVersions();
string NewVersionsText();
string new_update(string rev, string filename);
string new_theme(string themename, string filename);
void update(string filename);
bool CheckRev();
void CounterUpdate(string revnumber);
