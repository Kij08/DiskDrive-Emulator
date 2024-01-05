#include <stdio.h>
#include <string.h>
#include "DiskDrive.h"

const char* DiskDrive::ReadFromDisk(int index){
	return drive[index];
}

void DiskDrive::WriteToDisk(int index, const char block[512]){
	strncpy(drive[index], block, 512);
}

void DiskDrive::EraseBlock(int index){
	//:memset(drive[index];
}
