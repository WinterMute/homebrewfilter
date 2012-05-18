
void menu();
int menu_main(int);
int menu_install_uninstall(int);
int menu_install();
int menu_uninstall();
int menu_copyright();
int menu_reinstall();

enum
{
	MENU_EXIT = -1,
	MENU_MAIN,
	MENU_INSTALL,
	MENU_REINSTALL,
	MENU_UNINSTALL,
	MENU_INSTALLING,
	MENU_REINSTALLING,
	MENU_UNINSTALLING,
	MENU_COPYRIGHT
};
