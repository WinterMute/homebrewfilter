
#include <stdio.h>
#include <string.h>
#include <dirent.h>

void DeleteDir(const char* dirname){
    DIR *hdir;
    struct dirent *entry;
     
    hdir = opendir(dirname);
    do
    {
        entry = readdir(hdir);
        if (entry && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char erase[500];
            sprintf(erase, "%s/%s", dirname, entry->d_name);
            if(remove(erase) == -1)
                DeleteDir(erase);
			remove(erase);
        }  
    } while (entry);
    closedir(hdir);
	remove(dirname);
}