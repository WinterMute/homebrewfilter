/******************************************************************************************
***											***
*** nswitch - Simple neek/realnand switcher to embed in a channel			***
***											***
*** Copyright (C) 2011	OverjoY								***
*** 											***
*** This program is free software; you can redistribute it and/or			***
*** modify it under the terms of the GNU General Public License				***
*** as published by the Free Software Foundation version 2.				***
***											***
*** This program is distributed in the hope that it will be useful,			***
*** but WITHOUT ANY WARRANTY; without even the implied warranty of			***
*** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the			***
*** GNU General Public License for more details.					***
***											***
*** You should have received a copy of the GNU General Public License			***
*** along with this program; if not, write to the Free Software				***
*** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. 	***
***											***
*******************************************************************************************/

#include <gccore.h>
#include <fat.h>
#include <sdcard/wiisd_io.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#include "armboot.h"

#define le32(i) (((((u32) i) & 0xFF) << 24) | ((((u32) i) & 0xFF00) << 8) | \
                ((((u32) i) & 0xFF0000) >> 8) | ((((u32) i) & 0xFF000000) >> 24))

enum
{
    SD = 0,
    USB1,
    USB2,
    USB3,
    USB4,
    MAXDEV
};

static const char dev[MAXDEV][6] =
{
    "sd",
    "usb1",
    "usb2",
    "usb3",
    "usb4"
};

typedef struct _PR
{
    u8 state;
    u8 chs_st[3];
    u8 type;
    u8 chs_e[3];
    u32 lba;
    u32 bc;
} __attribute__((__packed__)) _pr;

typedef struct _MBR
{
    u8 ca[446];
    _pr part[4];
    u16 sig;
} __attribute__((__packed__)) _mbr;

bool check_neek2o() {

	if(fopen("sd1:/sneek/kernel.bin", "rb") || fopen("usb1:/sneek/kernel.bin", "rb"))
		return true;
	
	return false;

}

int boot_neek2o() {

	u32 b2;
	ES_GetBoot2Version( &b2 );
	if( b2 < 5 )
	{
		int tr = 10;
		bool USBMounted = false, SDMounted = false;
		while( tr )
		{
			if( __io_usbstorage.startup() && __io_usbstorage.isInserted() )
			{
				_mbr mbr;
				char buffer[512];

				__io_usbstorage.readSectors( 0, 1, &mbr );
				for( tr = 0; tr < 4; ++tr)
				{
					if(mbr.part[tr].type == 0)
					continue;

					__io_usbstorage.readSectors( le32( mbr.part[tr].lba ), 1, buffer );

					if( *( (u16 *)( buffer + 0x1FE ) ) == 0x55AA )
					{
						if( memcmp( buffer + 0x36, "FAT", 3 ) == 0 || memcmp( buffer + 0x52, "FAT", 3 ) == 0 )
						{
							fatMount( dev[USB1+tr], &__io_usbstorage, le32( mbr.part[tr].lba ), 8, 64 );
							USBMounted = true;
						}
						else
						{
							continue;
						}
					}
				}
				break;
			}
			usleep( 150000 );
		}

		if( __io_wiisd.startup() || !__io_wiisd.isInserted() )
			if( fatMount( dev[SD], &__io_wiisd, 0, 8, 64 ) )
				SDMounted = true;


		FILE *f = NULL;
		long fsize;
		size_t fres;
		char path[40];
		if( USBMounted )
		{
			for( tr = USB1; tr < MAXDEV; ++tr )
			{
				sprintf( path, "%s:/sneek/kernel.bin", dev[tr] );
				if( !f )
					f = fopen( path, "rb" );
			}
		}

		if( !f && SDMounted )
			f = fopen( "sd:/sneek/kernel.bin", "rb" );


		if( f )
		{
			fseek( f , 0 , SEEK_END );
			fsize = ftell( f );
			rewind( f );
			fres = fread ( (void *)0x91000000, 1, fsize, f );
			DCFlushRange( (void *)0x91000000, fsize );
		}
		else
		{
			fclose( f );

			for( tr = SD; tr < MAXDEV; ++tr )
			{
				char d[40];
				sprintf( d, "%s:", dev[tr] );
				fatUnmount( d );
			}
			__io_usbstorage.shutdown();
			__io_wiisd.shutdown();
			SYS_ResetSystem( SYS_RETURNTOMENU, 0, 0 );
		}

		fclose( f );

		for( tr = SD; tr < MAXDEV; ++tr )
		{
			char d[40];
			sprintf( d, "%s:", dev[tr] );
			fatUnmount( d );
		}
		__io_usbstorage.shutdown();
		__io_wiisd.shutdown();

		/*** Boot mini from mem code by giantpune ***/
		void *mini = memalign( 32, armboot_size );
		if( !mini )
			return 0;

		memcpy( mini, armboot, armboot_size );
		DCFlushRange( mini, armboot_size );

		*(u32*)0xc150f000 = 0x424d454d;
		asm volatile( "eieio" );

		*(u32*)0xc150f004 = MEM_VIRTUAL_TO_PHYSICAL( mini );
		asm volatile( "eieio" );

		IOS_ReloadIOS( 0xfe );

		free( mini );

		return 0;
	}
	else
	{
		SYS_ResetSystem( SYS_RESTART,0,0 );
		return 0;
	}

}
