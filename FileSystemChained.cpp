#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include "FileSystemChained.h"


using namespace std;


void FileSystemChained::InitDD(){
	string bitmap = "11";
	for(int i = 2; i < 256; i++){
		bitmap.append("0");
	}
	dd.WriteToDisk(1, bitmap.c_str());
}

void FileSystemChained::WriteToDisk(int index, const char block[512], bool del){
	if(del == false){
		dd.WriteToDisk(index, block);
	}	
	UpdateBitmap(index, del);
}


void FileSystemChained::AddToFileAllocTable(string fileName, int startBlock, int length){ 
	string s = to_string(startBlock);
	string n = to_string(length);
	string entry = fileName + "." + s + "." + n + "\n";
	

	char newTable[512];
	string table = dd.ReadFromDisk(0);
	if(table.length() == 0){
		WriteToDisk(0, entry.c_str(), false);
	}
	else {
		string appended = table.append(entry);
		WriteToDisk(0, appended.c_str(), false);
	}
}

bool FileSystemChained::GetFromFileAllocTable(std::string fileName, int& startBlock, int& numBlocks){
	string table = dd.ReadFromDisk(0);
	int entry = table.find('\n');
	string substr = table.substr(0, entry);
	while(table != ""){
		int delim = substr.find('.');
		if(fileName == substr.substr(0, delim)){ //File name found
			//Start block
			substr.erase(0, delim+1);
			delim = substr.find('.');
			string detail = substr.substr(0, delim);
			startBlock = stoi(detail);

			//NumBlocks
			substr.erase(0, delim+1);
			numBlocks = stoi(substr);

			return true;
		}
		table.erase(0, entry+1);
		entry = table.find('\n');
		substr = table.substr(0, entry);
	}
	return false;
}

void FileSystemChained::UpdateBitmap(int index, bool del){
	string bitmap = dd.ReadFromDisk(1);
	if(del == true){
	        bitmap.replace(index, 1, "0");
	}
	else{
		bitmap.replace(index, 1, "1");
	}
	dd.WriteToDisk(1, bitmap.c_str());
}

int FileSystemChained::GetFreeSpace(int requiredSpace){
	string bitmap = dd.ReadFromDisk(1);
	int start = 2;
	for(int i = 2; i < 512; i++){
		
		if(bitmap.at(i) == '0'){
			return i;
		}
	}
	return -1;
}

void FileSystemChained::DisplayFile(string fileName){
	int nextBlock = 0;
	int numBlocks = 0;
	if(GetFromFileAllocTable(fileName, nextBlock, numBlocks)){
		for(int i = 0; i < numBlocks; i++){
			//Read block
			string rawBlock = dd.ReadFromDisk(nextBlock);
			string cleanBlock = rawBlock.substr(3, string::npos);
			cout << cleanBlock;
			
			if(i == numBlocks-1){
				break;
			}
			//Get next block
			string len = rawBlock.substr(0, 1);
			int length = stoi(len);
			string blockPointer = rawBlock.substr(1, length);
			nextBlock = stoi(blockPointer);
		}	
	}
	else{
		cerr << "File not found.";
	}
	cout << "\n";


}

void FileSystemChained::DisplayFileTable(){
	cout << dd.ReadFromDisk(0);
	cout << "\n";
}

void FileSystemChained::DisplayFreeSpaceBitmap(){
	cout << dd.ReadFromDisk(1);
	cout << "\n";
}

void FileSystemChained::DisplayDiskBlock(int index){
	cout << dd.ReadFromDisk(index);
	cout << "\n";
}

void FileSystemChained::CpyToSystem(string fileName){
	ofstream file(fileName.c_str());
	int nextBlock = 0;
	int numBlocks = 0;
	//Get file stats from table
	GetFromFileAllocTable(fileName, nextBlock, numBlocks);
	//Read each line into the opened file
	for(int i = 0; i < numBlocks; i ++){
       		string rawBlock = dd.ReadFromDisk(nextBlock);
		string cleanBlock = rawBlock.substr(3, string::npos);
		cout << cleanBlock;

		if(i == numBlocks-1){
			break;
		}

        	//Find next block
	        string len = rawBlock.substr(0, 1);
		int length = stoi(len);
		string blockPointer = rawBlock.substr(1, length);
		nextBlock = stoi(blockPointer);
	}
	//profit
	file.close();
}

void FileSystemChained::CpyFromSystem(string fileName){
	ifstream file(fileName.c_str());

	if(!file.is_open()){
		cerr << "File not found.\n";
		return;
	}

	char ch;
	string gotLine;
	string blockToWrite;
	char blocks[10][512];
	int numBlocks = 0;
	int allocatedBlock = 0;
	int startBlock = 0;
	int i = 0;
	while(file >> noskipws >> ch){
		if(file.peek() != EOF){    
			if(i < 4){
				blocks[numBlocks][i] = '-';
			}
			if(i == 511){ //Don't write at 512 to save for cstring end string
				blocks[numBlocks][i] = '\0';
				numBlocks++;
				i = 0;
				continue;
			}
			/*else if(i > 507){
	               		i++; //Leave space open for block allocation
	        	        continue;
			}*/
			if(numBlocks == 11){
				cout << "File to large.\n";
				return;
			}
			blocks[numBlocks][i] = ch;
			i++;
		}
	}
	blocks[numBlocks][511] = '\0';
	int prevAllocated = 0;
	for(int j = 0; j < numBlocks+1; j++){
		allocatedBlock = GetFreeSpace(numBlocks+1); //since array start at 0
		
		if(j == 0){
			startBlock = allocatedBlock;
		}
		if(allocatedBlock < 0){
			cout << "Disk drive full\n";
			return;
		}
	       	string nextBlock = to_string(allocatedBlock);
        	for(int k = 1; (k < nextBlock.size()+1 && j != 0); k++){
       	        	blocks[j-1][k] = nextBlock.at(k-1);
        	}
		WriteToDisk(allocatedBlock, blocks[j], false);
		if(j != 0){
			blocks[j-1][0] = char(nextBlock.size()+48);
			WriteToDisk(prevAllocated, blocks[j-1], false);
		}
		prevAllocated = allocatedBlock;
	}
	
	AddToFileAllocTable(fileName, startBlock, numBlocks+1);

	file.close();
}

void FileSystemChained::Delete(string fileName){

	string table = dd.ReadFromDisk(0);
	string tableToWrite = table;
	int entry = table.find('\n');
	int prevEntry = 0;
	int startBlock;
	int numBlocks;
	string substr = table.substr(0, entry);
	while(table != ""){
	
		int delim = substr.find('.');
		if(fileName == substr.substr(0, delim)){ //File name found- entry+1 since entyr is the position of the last \n character
			GetFromFileAllocTable(fileName, startBlock, numBlocks); //Get name to update bitmap
			//Clear from file table
			tableToWrite.erase(prevEntry, entry+1);
			dd.WriteToDisk(0, tableToWrite.c_str());
		}
		table.erase(0, entry+1);
		entry = table.find('\n');
		substr = table.substr(0, entry);
		prevEntry = entry;
	}
	
	for(int i = 0; i < numBlocks; i++){
		UpdateBitmap(startBlock, true);
		
		if(i == numBlocks-1){
			break;
		}

        	//Find next block
		string rawBlock = dd.ReadFromDisk(startBlock);
		string len = rawBlock.substr(0, 1);
		int length = stoi(len);
		string blockPointer = rawBlock.substr(1, length);
		startBlock = stoi(blockPointer);
	}
	
}

