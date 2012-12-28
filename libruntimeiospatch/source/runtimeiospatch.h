#ifndef _RUNTIMEIOSPATCH_H_
#define _RUNTIMEIOSPATCH_H_

#define HAVE_AHBPROT ((*(vu32*)0xcd800064 == 0xFFFFFFFF) ? 1 : 0)

#ifdef __cplusplus
extern "C" {
#endif

bool have_ahbprot();
u32 IosPatch_AHBPROT(bool verbose);
u32 IosPatch_RUNTIME(bool wii, bool sciifii, bool vwii, bool verbose);
u32 IosPatch_FULL(bool wii, bool sciifii, bool vwii, bool verbose, int IOS);
int get_libruntimeiospatch_version();

#ifdef __cplusplus
}
#endif

#endif
