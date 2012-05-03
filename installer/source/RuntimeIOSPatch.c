#include <gccore.h>
#include <ogc/machine/processor.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "RuntimeIOSPatch.h"

#define MEM_PROT 0xD8B420A

const u8 isfs_permissions_old[] = { 0x42, 0x8B, 0xD0, 0x01, 0x25, 0x66 };
const u8 isfs_permissions_patch[] = { 0x42, 0x8B, 0xE0, 0x01, 0x25, 0x66 };
const u8 setuid_old[] = { 0xD1, 0x2A, 0x1C, 0x39 };
const u8 setuid_patch[] = { 0x46, 0xC0 };
const u8 es_identify_old[] = { 0x28, 0x03, 0xD1, 0x23 };
const u8 es_identify_patch[] = { 0x00, 0x00 };
const u8 hash_old[] = { 0x20, 0x07, 0x4B, 0x0B };
const u8 hash_patch[] = { 0x00 };
const u8 addticket_vers_check[] = { 0xD2, 0x01, 0x4E, 0x56 };
const u8 addticket_patch[] = { 0xE0 };

u32 apply_patch(const char *name, const u8 *old, u32 old_size, const u8 *patch, u32 patch_size, u32 patch_offset)
{
//	printf("Applying patch %s.....", name);
	u8 *ptr = (u8 *) 0x93400000;
	u32 i, found = 0;
	u8 *start;

	while ((u32) ptr < (0x94000000 - old_size))
	{
		if(!memcmp(ptr, old, old_size))
		{
			found++;
			start = ptr + patch_offset;
			for (i = 0; i < patch_size; i++)
				*(start + i) = patch[i];
			ptr += patch_size;
			DCFlushRange((u8 *) (((u32) start) >> 5 << 5), (patch_size >> 5 << 5) + 64);
		}
		ptr++;
	}
//	if(found)
//		printf("Patched\n");
//	else
//		printf("\n");

	return found;
}

u32 runtimePatchApply()
{
	u32 count = 0;
	write16(MEM_PROT, 0);
	count += apply_patch("Trucha", hash_old,
		sizeof(hash_old), hash_patch, sizeof(hash_patch), 1);
	count += apply_patch("ES_Identify", es_identify_old,
		sizeof(es_identify_old), es_identify_patch, sizeof(es_identify_patch),
		2);
	count += apply_patch("NAND Permissions", isfs_permissions_old,
		sizeof(isfs_permissions_old), isfs_permissions_patch,
		sizeof(isfs_permissions_patch), 0);
	count += apply_patch("add ticket patch", addticket_vers_check,
		sizeof(addticket_vers_check), addticket_patch, sizeof(addticket_patch),
		0);
	count += apply_patch("ES_SetUID", setuid_old, sizeof(setuid_old),
		setuid_patch, sizeof(setuid_patch), 0);
	write16(MEM_PROT, 1);
	return count;
}
