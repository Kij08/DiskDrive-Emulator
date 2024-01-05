#include <vector>
#include "DiskDrive.h"

class FileSystemIndex {

private:
	DiskDrive dd;

	void AddToFileAllocTable(std::string fileName, int indexBlock);
	bool GetFromFileAllocTable(std::string fileName, std::vector<int>& indexBlock, int& indexBlockNum);
	void UpdateBitmap(int index, bool del);
	std::vector<int> GetFreeSpace(int requiredSpace);
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
