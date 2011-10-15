
void menu();
int menu_main(int);
int menu_install_uninstall(int);
int menu_install();
int menu_uninstall();
int menu_copyright();

enum
{
	MENU_EXIT = -1,
	MENU_MAIN,
	MENU_INSTALL,
	MENU_UNINSTALL,
	MENU_INSTALLING,
	MENU_UNINSTALLING,
	MENU_COPYRIGHT
};
