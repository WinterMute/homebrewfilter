#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <dirent.h>
#include <wiiuse/wpad.h>
#include <sys/unistd.h>
#include <malloc.h>
#include "uneek_fs.h"
#include "../xprintf.h"
#include "armboot.h"
//#include "../build/kernel_bin.h"

//#define KERNEL_SIZE 168920

void DoMini(u8* kbuf, size_t kernel_size)
{
//temporary test
//copy kernel.bin


		kernel_size +=3;
		kernel_size &= 0xFFFFFFFC;
		//memcpy((void*)0x91000000,kernel_bin,KERNEL_SIZE);
		//DCFlushRange( (void *)0x91000000, KERNEL_SIZE );


//        char* mini = (char*) memalign( 32, armboot_bin_size +KERNEL_SIZE + 4 );
        u8* mini = (u8*) memalign( 32, armboot_size +kernel_size + 4 );

        if( !mini )
        {
                return;
        }
        xprintf( "mini buffer: %p\n", mini );

//		memcpy( mini, kernel_bin,KERNEL_SIZE);
//        DCFlushRange( mini, KERNEL_SIZE );
//        memcpy( mini+KERNEL_SIZE+4, armboot_bin, armboot_bin_size );
//        DCFlushRange( mini+KERNEL_SIZE+4, armboot_bin_size );

		memcpy( mini, kbuf,kernel_size);
        DCFlushRange( mini, kernel_size );
        free(kbuf);
        memcpy( mini+kernel_size+4, armboot, armboot_size );
        DCFlushRange( mini+kernel_size+4, armboot_size );

        xprintf( "armboot.bin copied\n" );
        *(u32*)0xc150f000 = 0x424d454d;
        asm volatile( "eieio" );

        // physical address for armboot.bin.  ( virtualToPhysical() )
//        *(u32*)0xc150f004 = MEM_VIRTUAL_TO_PHYSICAL( mini+KERNEL_SIZE+4 );
        *(u32*)0xc150f004 = MEM_VIRTUAL_TO_PHYSICAL( mini+kernel_size+4 );

        asm volatile( "eieio" );

        xprintf( "physical memory address: %08x\n", MEM_VIRTUAL_TO_PHYSICAL( mini ) );
        xprintf( "loading bootmii IOS\n" );

// pass position of kernel.bin to mini
		*(u32*)0x8132FFF0 = MEM_VIRTUAL_TO_PHYSICAL( mini );
        asm volatile( "eieio" );
		DCFlushRange((void*)0x8132FFF0,4);

// pass length of kernel.bin to mini
		*(u32*)0x8132FFF4 = kernel_size;
        asm volatile( "eieio" );
		DCFlushRange((void*)0x8132FFF4,4);

        IOS_ReloadIOS( 0xfe );

        xprintf( "well shit.  this shouldnt happen\n" );

        free( mini );
}

