/****************************************************************************
 * WiiMC
 * Tantric 2009-2010
 *
 * fileop.h
 *
 * File operations
 ****************************************************************************/

#ifndef _FILEOP_H_
#define _FILEOP_H_

#include <gccore.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <unistd.h>

#define MAX_DEVICES 10

using namespace std;

typedef struct
{
	char name[100];
	char mount[10];
	int type;
	DISC_INTERFACE* interface;
	sec_t sector;
} DEVICE_STRUCT;

extern DEVICE_STRUCT part[2][MAX_DEVICES];

void MountAllDevices();
void UnmountAllDevices();

bool DVD_Inserted();
void check_dvd();

/*
//!USB FAT Controls
int USBDevice_Init();
void USBDevice_deInit();
*/
bool USBDevice_Inserted();
void check_usb();
/*
//!SD FAT Controls
int SDCard_Init();
void SDCard_deInit();
*/
bool SDCard_Inserted();
void check_sd();

void check_device();
string check_path(string old_path);

#endif
