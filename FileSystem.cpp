#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include "FileSystem.h"

using namespace std;


void FileSystem::InitDD(){
	string bitmap = "11";
	for(int i = 2; i < 256; i++){
		bitmap.append("0");
	}
	dd.WriteToDisk(1, bitmap.c_str());
}

void FileSystem::WriteToDisk(int index, const char block[512], bool del){
	if(del == false){
		dd.WriteToDisk(index, block);
	}	
	UpdateBitmap(index, del);
}


void FileSystem::AddToFileAllocTable(string fileName, int startBlock, int numBlocks){ //16 char each entry
	string s = to_string(startBlock);
	string n = to_string(numBlocks);
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

bool FileSystem::GetFromFileAllocTable(std::string fileName, int& startBlock, int& numBlocks){
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

void FileSystem::UpdateBitmap(int index, bool del){
	string bitmap = dd.ReadFromDisk(1);
	if(del == true){
	        bitmap.replace(index, 1, "0");
	}
	else{
		bitmap.replace(index, 1, "1");
	}
	dd.WriteToDisk(1, bitmap.c_str());
}

int FileSystem::GetFreeSpace(int requiredSpace){
	string bitmap = dd.ReadFromDisk(1);
	int count = 0;
	int start = 2;
	for(int i = 2; i < 512; i++){
		
		if(bitmap.at(i) == '0'){
			count++;
		}
		else{
			count = 0;
			start = i+1;
		}


		if(count == requiredSpace){
			return start;
		}
	}
	return -1;
}

void FileSystem::DisplayFile(string fileName){
	int startBlock = 0;
	int numBlocks = 0;
	if(GetFromFileAllocTable(fileName, startBlock, numBlocks)){
		for(int i = 0; i < numBlocks; i++){
			cout << dd.ReadFromDisk(startBlock + i);
		}
	}
	else{
		cerr << "File not found.";
	}
	cout << "\n";


}

void FileSystem::DisplayFileTable(){
	cout << dd.ReadFromDisk(0);
	cout << "\n";
}

void FileSystem::DisplayFreeSpaceBitmap(){
	cout << dd.ReadFromDisk(1);
	cout << "\n";
}

void FileSystem::DisplayDiskBlock(int index){
	cout << dd.ReadFromDisk(index);
	cout << "\n";
}

void FileSystem::CpyToSystem(string fileName){
	ofstream file(fileName.c_str());
	int startBlock = 0;
	int numBlocks = 0;
	//Get file stats from table
	GetFromFileAllocTable(fileName, startBlock, numBlocks);
	//Read each line into the opened file
	for(int i = 0; i < numBlocks; i ++){
		file << dd.ReadFromDisk(startBlock + i);
	}
	//profit
	file.close();
}

void FileSystem::CpyFromSystem(string fileName){
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
	int startBlock = 0;
	int i = 0;
	while(file >> noskipws >> ch){
		if(file.peek() != EOF){
			/*if(blockToWrite.length() + gotLine.length() > 512){
				//Write block
				blocks[numBlocks] = blockToWrite;
				numBlocks += 1;
				blockToWrite = gotLine;
			}
			else{
				blockToWrite += gotLine;
			}*/
			if(i == 511){ //Don't write at 512 to save for cstring end string
				blocks[numBlocks][i] = '\0';
				numBlocks++;
				i = 0;
				continue;
			}
			if(numBlocks == 11){
				cout << "File to large.\n";
				return;
			}
			blocks[numBlocks][i] = ch;
			i++;
		}
	}
	blocks[numBlocks][511] = '\0';
	startBlock = GetFreeSpace(numBlocks+1); //since array start at 0
	if(startBlock < 0){
		cout << "Disk drive full.\n";
		return;
	}
	for(int j = 0; j < numBlocks+1; j++){
		WriteToDisk(startBlock + j, blocks[j], false);
	}
	
	AddToFileAllocTable(fileName, startBlock, numBlocks+1);

	file.close();
}

void FileSystem::Delete(string fileName){

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

	for(int i = startBlock; i < startBlock + numBlocks; i++){
		UpdateBitmap(i, true);
	}
	
}

