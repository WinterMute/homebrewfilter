
#include <string>
#include <algorithm>
#include "main.h"
#include "sort.h"
#include "Tools/copy_app_in_category.h"

using namespace std;

int _sort = -1;

void sort_apps()
{
	int category = Settings.current_category;

	for(int i = (signed)vechomebrew_list_category[category].size() -1; i >= 0; i--)
	{
		string folder = vechomebrew_list_category[category][i].foldername;
		if((signed)folder.find_last_of("/") != -1)
			folder.erase(folder.find_last_of("/"));
		if((signed)folder.find_last_of("/") != -1)
			folder.erase(0, folder.find_last_of("/") +1);
			
		vector<string>::iterator pos = find(AvailableCategory.apps[category].begin(), AvailableCategory.apps[category].end(), folder);
		if ( pos != AvailableCategory.apps[category].end() )
			AvailableCategory.apps[category].erase(pos);
			
		AvailableCategory.apps[category].insert(AvailableCategory.apps[category].begin(), folder);
	}
	
	copy_app_in_category();
	
	switch (_sort)
	{
		case NAME_A_Z:
			sort(vechomebrew_list_category[Settings.current_category].begin(), vechomebrew_list_category[Settings.current_category].end(), sort_name_a_z);
			break;
		case NAME_Z_A:
			sort(vechomebrew_list_category[Settings.current_category].begin(), vechomebrew_list_category[Settings.current_category].end(), sort_name_z_a);
			break;
		case FOLDERNAME_A_Z:
			sort(vechomebrew_list_category[Settings.current_category].begin(), vechomebrew_list_category[Settings.current_category].end(), sort_foldername_a_z);
			break;
		case FOLDERNAME_Z_A:
			sort(vechomebrew_list_category[Settings.current_category].begin(), vechomebrew_list_category[Settings.current_category].end(), sort_foldername_z_a);
			break;
		default:
			break;
	}
	_sort = -1;
}


bool sort_name_a_z(const homebrew_list& a, const homebrew_list& b)
{
	_sort = NAME_A_Z;
	string str1 = a.name;
	string str2 = b.name;
	
	transform(str1.begin(),str1.end(),str1.begin(),::tolower);	// in kleinebuchstaben umwandeln
	transform(str2.begin(),str2.end(),str2.begin(),::tolower);	// in kleinebuchstaben umwandeln
	
	return str1 < str2;
}


bool sort_name_z_a(const homebrew_list& a, const homebrew_list& b)
{ 
	_sort = NAME_Z_A;
	string str1 = a.name;
	string str2 = b.name;
	
	transform(str1.begin(),str1.end(),str1.begin(),::tolower);	// in kleinebuchstaben umwandeln
	transform(str2.begin(),str2.end(),str2.begin(),::tolower);	// in kleinebuchstaben umwandeln
	
	return str1 > str2;
}

bool sort_foldername_a_z(const homebrew_list& a, const homebrew_list& b)
{ 
	_sort = FOLDERNAME_A_Z;
	string str1 = a.foldername;
	string str2 = b.foldername;
	
	transform(str1.begin(),str1.end(),str1.begin(),::tolower);	// in kleinebuchstaben umwandeln
	transform(str2.begin(),str2.end(),str2.begin(),::tolower);	// in kleinebuchstaben umwandeln
	
	return str1 < str2;
}

bool sort_foldername_z_a(const homebrew_list& a, const homebrew_list& b)
{ 
	_sort = FOLDERNAME_Z_A;
	string str1 = a.foldername;
	string str2 = b.foldername;
	
	transform(str1.begin(),str1.end(),str1.begin(),::tolower);	// in kleinebuchstaben umwandeln
	transform(str2.begin(),str2.end(),str2.begin(),::tolower);	// in kleinebuchstaben umwandeln
	
	return str1 > str2;
}
