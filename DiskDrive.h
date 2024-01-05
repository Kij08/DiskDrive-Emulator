#pragma once

class DiskDrive{
private:
	char drive[256][512];
public:
	const char* ReadFromDisk(int index);
	void WriteToDisk(int index, const char block[512]);
	void EraseBlock(int index);
};
