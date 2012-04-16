
#include <string>

bool listIOS();
int nextIos();
int previousIos();
int SelectedIOS();
int SearchAppIOS(std::string foldername);
int GetAppIOS(std::string foldername);
int check_priiloader();
s32 GetTMD(u64 TicketID, signed_blob **Output, u32 *Length);
s32 NandReadFile(char *filepath, u8 **buffer, u32 *filesize);
int get_priiloader();
int get_bootmii();
int get_nandemu();
void set_priiloader(int choice);
void set_bootmii(int choice);
void set_nandemu(int choice);
