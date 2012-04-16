
struct s_move
{
	int page;
	int nr;	
	int nr_selected;
	bool new_app; 
};
extern struct s_move move;

void Category_rename_move(string button);
void Next_Category();
void Previous_Category();
void AppEraseDelate(int choice, const char* kurzname, const char* foldername);
