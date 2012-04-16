
#include <string> 
#include <vector>
using namespace std;

class TextLine
{
public:
	void text(string text, int FontSize, int maxWidth);
	void list(string text);
	
	int text_up();
	int text_down(int number);
	std::vector<string> line;
	int textScrollPos;
	
};