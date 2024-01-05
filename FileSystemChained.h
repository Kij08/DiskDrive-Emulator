#include "DiskDrive.h"

class FileSystemChained {

private:
	class DiskDrive dd;
	void AddToFileAllocTable(std::string fileName, int startBlock, int numBlocks);
	bool GetFromFileAllocTable(std::string fileName, int& startBlock, int& numBlocks);
	void UpdateBitmap(int index, bool del);
	int GetFreeSpace(int requiredSpace);
	void WriteToDisk(int index, const char block[512], bool del);
public:
	void InitDD();
	void DisplayFile(std::string fileName);
	void DisplayFileTable();
	void DisplayFreeSpaceBitmap();
	void DisplayDiskBlock(int index);
	void CpyToSystem(std::string fileName);
	void CpyFromSystem(std::string fileName);
	void Delete(std::string fileName);
};

