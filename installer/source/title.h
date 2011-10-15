#ifndef _TITLE_H_
#define _TITLE_H_

/* Constants */
#define BLOCK_SIZE	1024

/* Prototypes */
s32 Title_FakesignTik(signed_blob *);
s32 Title_GetTicketViews(u64, tikview **, u32 *);

#endif
