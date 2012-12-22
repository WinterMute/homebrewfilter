#ifndef _RUNTIMEIOSPATCH_H_
#define _RUNTIMEIOSPATCH_H_

#define HAVE_AHBPROT ((*(vu32*)0xcd800064 == 0xFFFFFFFF) ? 1 : 0)

#ifdef __cplusplus
extern "C" {
#endif

u32 runtimePatchApply(bool wii, bool sciifii, bool vwii);

#ifdef __cplusplus
}
#endif

#endif
