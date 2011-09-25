
enum
{
	NAME_A_Z,
	NAME_Z_A,
	FOLDERNAME_A_Z,
	FOLDERNAME_Z_A,
};

/****************************************************************************
 * Sortierfunktionen
 ***************************************************************************/

void sort_apps();
bool sort_name_a_z(const homebrew_list& a, const homebrew_list& b);
bool sort_name_z_a(const homebrew_list& a, const homebrew_list& b);
bool sort_foldername_a_z(const homebrew_list& a, const homebrew_list& b);
bool sort_foldername_z_a(const homebrew_list& a, const homebrew_list& b);

