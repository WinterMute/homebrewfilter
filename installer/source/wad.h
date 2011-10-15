#ifndef _WAD_H_
#define _WAD_H_

#ifdef __cplusplus
extern "C" {
#endif

void mopen();
u64 GetTitleID();
bool CheckAppFound(u64 title);

s32 __Wad_ReadAlloc(void **outbuf, u32 offset, u32 len);
s32 Wad_InstallFromMemory(int startpos_x, int startpos_y);
s32 Wad_UninstallFromMemory(int startpos_x, int startpos_y);

#ifdef __cplusplus
}
#endif

#endif
