

struct SSettingsNeek2o {
	char type[8];
	char neeknandpath[64];
	char neekdipath[64];
	char nandbootdelay[4];
	char neekbootdelay[4];
	char nandbootapplication[64];
	char neekbootapplication[64];
	char neekbootchannel[24];
	char neekselection[2];
	char nanddisc[16];
	char nandfolder[64];
	char nandbootdescription[128];
	char neekbootdescription[128];
	char nandbootargument[128];
	char neekbootargument[128];
};
#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

bool FileExist (char *fn);
bool DirExist (char *path);
u8 *ReadFile2Buffer (char *path, size_t *filesize, int *err);
bool IsNandFolder (char *path);
s32 adjust_nandpath (void);
void create_dipath(void);
s32 set_neek_channel(void);
void xml_set_default(void);
s32 get_nand_folder(char* nandpath);
#ifdef __cplusplus
}
#endif  //__cplusplus
