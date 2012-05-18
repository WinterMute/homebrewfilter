#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>

#include "getios.h"
#include "title.h"
#include "video.h"
#include "wad.h"
#include "../../svnrev/svnrev.h"

#define round_up(x,n)	(-(-(x) & -(n)))

extern const u8  install_wad[];
extern const u32 install_wad_size;

u64 title_id;

/* 'WAD Header' structure */
typedef struct {
	/* Header length */
	u32 header_len;

	/* WAD type */
	u16 type;


	u16 padding;

	/* Data length */
	u32 certs_len;
	u32 crl_len;
	u32 tik_len;
	u32 tmd_len;
	u32 data_len;
	u32 footer_len;
} ATTRIBUTE_PACKED wadHeader;

/* Variables */
static u8 wadBuffer[BLOCK_SIZE] ATTRIBUTE_ALIGN(32);

void * startOfData;
void * endOfData;
void * internalPointer;

void mopen()
{
	u32 size = install_wad_size;
	startOfData = (void *) install_wad;
	endOfData = (void *) install_wad + size;
	internalPointer = startOfData;

	// Title ID
	wadHeader	*header   = NULL;
	signed_blob *p_tik    = NULL;
	tik         *tik_data = NULL;

	__Wad_ReadAlloc((void *)&header, 0, sizeof(wadHeader));

	u32 offset = 0;
	s32 ret;

	/* Ticket offset */
	offset += round_up(header->header_len, 64);
	offset += round_up(header->certs_len,  64);
	offset += round_up(header->crl_len,    64);

	/* Read ticket */
	ret = __Wad_ReadAlloc((void *)&p_tik, offset, header->tik_len);
	if (ret < 0)
		goto out;

	/* Ticket data */
	tik_data = (tik *)SIGNATURE_PAYLOAD(p_tik);

out:
	/* Free memory */
	if (header)
		free(header);
	if (p_tik)
		free(p_tik);

	title_id = tik_data->titleid;
}

int mseek(u32 offset, int origin)
{
	if (origin == SEEK_SET)
	{
		internalPointer = startOfData + offset;
	} else if (origin == SEEK_CUR)
	{
		internalPointer = internalPointer + offset;
	} else if (origin == SEEK_END) {
		internalPointer = endOfData - offset;
	} else
	{
		return -2;
	}

	if ((internalPointer<startOfData) || (internalPointer> endOfData))
	{
		return -1;
	} else
	{
		return 0;
	}
}

int mread(void * buf, int size, int count)
{
	memcpy(buf, internalPointer, size*count);
	//DCFlushRange(buf, size*count);
	return 0;
}


//-------------------------- INSTALL FROM MEMORY -------------------------------

s32 __Wad_ReadFile(void *outbuf, u32 offset, u32 len)
{
	s32 ret;

	/* Seek to offset */
	mseek(offset, SEEK_SET);

	/* Read data */
	ret = mread(outbuf, len, 1);
	if (ret < 0)
		return ret;

	return 0;
}

s32 __Wad_ReadAlloc(void **outbuf, u32 offset, u32 len)
{
	void *buffer = NULL;
	s32   ret;

	/* Allocate memory */
	buffer = memalign(32, len);
	if (!buffer)
		return -1;

	/* Read file */
	ret = __Wad_ReadFile(buffer, offset, len);
	if (ret < 0) {
		free(buffer);
		return ret;
	}

	/* Set pointer */
	*outbuf = buffer;

	return 0;
}

s32 __Wad_GetTitleID(wadHeader *header, u64 *tid)
{
	signed_blob *p_tik    = NULL;
	tik         *tik_data = NULL;

	u32 offset = 0;
	s32 ret;

	/* Ticket offset */
	offset += round_up(header->header_len, 64);
	offset += round_up(header->certs_len,  64);
	offset += round_up(header->crl_len,    64);

	/* Read ticket */
	ret = __Wad_ReadAlloc((void *)&p_tik, offset, header->tik_len);
	if (ret < 0)
		goto out;

	/* Ticket data */
	tik_data = (tik *)SIGNATURE_PAYLOAD(p_tik);

	/* Copy title ID */
	*tid = tik_data->titleid;

out:
	/* Free memory */
	if (p_tik)
		free(p_tik);

	return ret;
}

u64 GetTitleID()
{
	return title_id;
}

bool CheckAppFound(u64 title)
{

	char tmd[ISFS_MAXPATH];

	u32 high = (u32)(title >> 32);
	u32 low  = (u32)(title & 0xFFFFFFFF);

	sprintf(tmd, "/title/%08x/%08x/content/00000000.app", high, low);

	s32 check = ISFS_Open(tmd, ISFS_OPEN_READ);
	if(check == -102 || check > 0)
	{
		ISFS_Close(check);
		return true;
	}

	return false;
}

void __Wad_FixTicket(signed_blob *p_tik)
{
	u8 *data = (u8 *)p_tik;
	u8 *ckey = data + 0x1F1;

	if (*ckey > 1) {
		// Set common key
		*ckey = 0;

		// Fakesign ticket
		Title_FakesignTik(p_tik);
	}
}

s32 __Wad_Install(int startpos_x, int startpos_y)
{
	wadHeader   *header  = NULL;
	signed_blob *p_certs = NULL, *p_crl = NULL, *p_tik = NULL, *p_tmd = NULL;

	tmd *tmd_data  = NULL;

	u32 cnt, offset = 0;
	s32 ret;

	/* WAD header */
	ret = __Wad_ReadAlloc((void *)&header, offset, sizeof(wadHeader));
	if (ret >= 0)
		offset += round_up(header->header_len, 64);
	else
		goto err;

	/* WAD certificates */
	ret = __Wad_ReadAlloc((void *)&p_certs, offset, header->certs_len);
	if (ret < 0)
		goto err;
	else
		offset += round_up(header->certs_len, 64);

	/* WAD crl */
	if (header->crl_len) {
		ret = __Wad_ReadAlloc((void *)&p_crl, offset, header->crl_len);
		if (ret < 0)
			goto err;
		else
			offset += round_up(header->crl_len, 64);
	}

	/* WAD ticket */
	ret = __Wad_ReadAlloc((void *)&p_tik, offset, header->tik_len);
	if (ret < 0)
		goto err;
	else
		offset += round_up(header->tik_len, 64);

	/* WAD TMD */
	ret = __Wad_ReadAlloc((void *)&p_tmd, offset, header->tmd_len);
	if (ret < 0)
		goto err;
	else
		offset += round_up(header->tmd_len, 64);

	/* Fix ticket */
	__Wad_FixTicket(p_tik);

	printf(".....");

	/* Install ticket */
	ret = ES_AddTicket(p_tik, header->tik_len, p_certs, header->certs_len, p_crl, header->crl_len);
	if (ret < 0)
		goto err;

	printf(".....");
	/* Install title */
	ret = ES_AddTitleStart(p_tmd, header->tmd_len, p_certs, header->certs_len, p_crl, header->crl_len);
	if (ret < 0)
		goto err;

	printf(".....");
	/* Get TMD info */
	tmd_data = (tmd *)SIGNATURE_PAYLOAD(p_tmd);

	/* Install contents */
	for (cnt = 0; cnt < tmd_data->num_contents; cnt++)
	{
		tmd_content *content = &tmd_data->contents[cnt];

		u32 idx = 0, len;
		s32 cfd;

		printf(".....");
		/* Encrypted content size */
		len = round_up(content->size, 64);

		/* Install content */
		cfd = ES_AddContentStart(tmd_data->title_id, content->cid);
		if (cfd < 0) {
			ret = cfd;
			goto err;
		}

		/* Install content data */
		while (idx < len) {
			u32 size;

			/* Data length */
			size = (len - idx);
			if (size > BLOCK_SIZE)
				size = BLOCK_SIZE;

			/* Read data */
			ret = __Wad_ReadFile(&wadBuffer, offset, size);
			if (ret < 0)
				goto err;

			/* Install data */
			ret = ES_AddContentData(cfd, wadBuffer, size);
			if (ret < 0)
				goto err;

			/* Increase variables */
			idx    += size;
			offset += size;
		}

		/* Finish content installation */
		ret = ES_AddContentFinish(cfd);
		if (ret < 0)
			goto err;
	}

	printf(".....");

	/* Finish title install */
	ret = ES_AddTitleFinish();
	if (ret >= 0)
	{
		Con_FgColor(2, 1);
		printf("\t\t  SUCCESS");

		goto out;
	}

err:
	Con_FgColor(1, 1);
	printf("\n\t\t\tERROR! (ret = %d)\n", ret);

	/* Cancel install */
	ES_AddTitleCancel();

out:
	/* Free memory */
	if (header)
		free(header);
	if (p_certs)
		free(p_certs);
	if (p_crl)
		free(p_crl);
	if (p_tik)
		free(p_tik);
	if (p_tmd)
		free(p_tmd);

	return ret;
}

s32 __Wad_Uninstall(int startpos_x, int startpos_y)
{
	wadHeader *header   = NULL;
	tikview   *viewData = NULL;

	u64 tid;
	u32 viewCnt;
	s32 ret;

	startpos_x = startpos_x +4;
	printf("\x1b[%i;%iH", startpos_x, startpos_y);

	/* WAD header */
	ret = __Wad_ReadAlloc((void *)&header, 0, sizeof(wadHeader));
	if (ret < 0) {
		Con_FgColor(1, 1);
		printf(" ERROR! (ret = %d)", ret);
		goto out;
	}

	/* Get title ID */
	ret =  __Wad_GetTitleID(header, &tid);
	if (ret < 0) {
		Con_FgColor(1, 1);
		printf(" ERROR! (ret = %d)", ret);
		goto out;
	}

	Con_FgColor(7, 1);
	printf("Deleting tickets...");

	/* Get ticket views */
	ret = Title_GetTicketViews(tid, &viewData, &viewCnt);
	if (ret < 0)
	{
		Con_FgColor(1, 1);
		printf("\x1b[%i;%iH", startpos_x, startpos_y +34);
		printf(" ERROR! (ret = %d)", ret);
	}

	/* Delete tickets */
	if (ret >= 0) {
		u32 cnt;

		/* Delete all tickets */
		for (cnt = 0; cnt < viewCnt; cnt++) {
			ret = ES_DeleteTicket(&viewData[cnt]);
			if (ret < 0)
				break;
		}

		if (ret < 0)
		{
			Con_FgColor(1, 1);
			printf("\x1b[%i;%iH", startpos_x, startpos_y +35);
			printf("ERROR! (ret = %d)", ret);
		}
		else
		{
			Con_FgColor(2, 1);
			printf("\x1b[%i;%iH", startpos_x, startpos_y +42);
			printf("SUCCESS");
		}
	}

	Con_FgColor(7, 1);
	printf("\x1b[%i;%iH", startpos_x +1, startpos_y);
	printf("Deleting title contents...");

	/* Delete title contents */
	ret = ES_DeleteTitleContent(tid);
	if (ret < 0)
	{
		Con_FgColor(1, 1);
		printf("\x1b[%i;%iH", startpos_x +1, startpos_y +35);
		printf("ERROR! (ret = %d)", ret);
	}
	else
	{
		Con_FgColor(2, 1);
		printf("\x1b[%i;%iH", startpos_x +1, startpos_y +42);
		printf("SUCCESS");
	}

	Con_FgColor(7, 1);
	printf("\x1b[%i;%iH", startpos_x +2, startpos_y);
	printf("Deleting title...");

	/* Delete title */
	ret = ES_DeleteTitle(tid);
	if (ret < 0)
	{
		Con_FgColor(1, 1);
		printf("\x1b[%i;%iH", startpos_x +2, startpos_y +35);
		printf("ERROR! (ret = %d)", ret);
	}
	else
	{
		Con_FgColor(2, 1);
		printf("\x1b[%i;%iH", startpos_x +2, startpos_y +42);
		printf("SUCCESS");
	}

out:
	/* Free memory */
	if (header)
		free(header);

	return ret;
}

s32 Wad_InstallFromMemory(int startpos_x, int startpos_y)
{
	return __Wad_Install(startpos_x, startpos_y);
}

s32 Wad_UninstallFromMemory(int startpos_x, int startpos_y)
{
	return __Wad_Uninstall(startpos_x, startpos_y);
}
