#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include "FileSystemIndex.h"

using namespace std;


void FileSystemIndex::InitDD(){
	string bitmap = "11";
	for(int i = 2; i < 256; i++){
		bitmap.append("0");
	}
	dd.WriteToDisk(1, bitmap.c_str());
}

void FileSystemIndex::WriteToDisk(int index, const char block[512], bool del){
	if(del == false){
		dd.WriteToDisk(index, block);
	}	
	UpdateBitmap(index, del);
}


void FileSystemIndex::AddToFileAllocTable(string fileName, int indexBlock){ //16 char each entry
	string n = to_string(indexBlock);
	string entry = fileName + "." + n + "\n";
	

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

bool FileSystemIndex::GetFromFileAllocTable(std::string fileName, vector<int>& indexBlock, int& indexBlockNum){
	string table = dd.ReadFromDisk(0);
	int entry = table.find('\n');
	string substr = table.substr(0, entry);
	while(table != ""){
		int delim = substr.find('.');
		if(fileName == substr.substr(0, delim)){ //File name found
			//IndexBlock
			substr.erase(0, delim+1);
			delim = substr.find('\n');
			string detail = substr.substr(0, delim);
			indexBlockNum = stoi(detail);


			//Make vector here
			string indexBlockString = dd.ReadFromDisk(indexBlockNum);
			while(indexBlockString != ""){
				int delim = indexBlockString.find('\n');
				int num = stoi(indexBlockString.substr(0, delim));
				indexBlock.push_back(num);
				indexBlockString.erase(0, delim+1);
			}

			return true;
		}
		table.erase(0, entry+1);
		entry = table.find('\n');
		substr = table.substr(0, entry);
	}
	return false;
}

void FileSystemIndex::UpdateBitmap(int index, bool del){
	string bitmap = dd.ReadFromDisk(1);
	if(del == true){
	        bitmap.replace(index, 1, "0");
	}
	else{
		bitmap.replace(index, 1, "1");
	}
	dd.WriteToDisk(1, bitmap.c_str());
}

vector<int> FileSystemIndex::GetFreeSpace(int requiredSpace){
	vector<int> blockList;
	string bitmap = dd.ReadFromDisk(1);
	int count = 0;
	for(int i = 2; i < 512; i++){
		
		if(bitmap.at(i) == '0'){
			blockList.push_back(i);
			count++;
		}


		if(count == requiredSpace){
			return blockList;
		}
	}
	return blockList;
}

void FileSystemIndex::DisplayFile(string fileName){
	vector<int> indexBlock;
	int loc;
	if(GetFromFileAllocTable(fileName, indexBlock, loc)){
		for(int i = 0; i < indexBlock.size(); i++){
			cout << dd.ReadFromDisk(indexBlock.at(i));
		}
	}
	else{
		cerr << "File not found.";
	}
	cout << "\n";


}

void FileSystemIndex::DisplayFileTable(){
	cout << dd.ReadFromDisk(0);
	cout << "\n";
}

void FileSystemIndex::DisplayFreeSpaceBitmap(){
	cout << dd.ReadFromDisk(1);
	cout << "\n";
}

void FileSystemIndex::DisplayDiskBlock(int index){
	cout << dd.ReadFromDisk(index);
	cout << "\n";
}

void FileSystemIndex::CpyToSystem(string fileName){
	ofstream file(fileName.c_str());
	vector<int> indexBlock;
	int loc;
	//Get file stats from table
	GetFromFileAllocTable(fileName, indexBlock, loc);
	//Read each line into the opened file
	for(int i = 0; i < indexBlock.size(); i ++){
		file << dd.ReadFromDisk(indexBlock.at(i));
	}
	//profit
	file.close();
}

void FileSystemIndex::CpyFromSystem(string fileName){
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
	vector<int> storageBlocks;
	int i = 0;
	while(file >> noskipws >> ch){
		if(file.peek() != EOF){

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
	blocks[numBlocks][i] = '\0';

	storageBlocks = GetFreeSpace(numBlocks+1); //since array start at 0
	if(storageBlocks.empty()){
		cout << "Disk drive full.\n";
		return;
	}

	for(int j = 0; j < numBlocks+1; j++){
		WriteToDisk(storageBlocks.at(j), blocks[j], false);
	}

	vector<int> indexBlock = GetFreeSpace(1); //Get a bock for the index table
	if(indexBlock.empty()){
        	cout << "Disk Drive full";
		return;
	}

	char blockArrayBlock[512];
	int blockArrayPtr = 0;
	for(int i = 0; i < storageBlocks.size(); i++){
		string intBlock = to_string(storageBlocks.at(i));
		for(int j = 0; j < intBlock.length(); j++){
			blockArrayBlock[blockArrayPtr] = intBlock.at(j);
			blockArrayPtr++;
		}
		blockArrayBlock[blockArrayPtr] = '\n';
		blockArrayPtr++;

	}
	
	WriteToDisk(indexBlock.at(0), blockArrayBlock, false);

	AddToFileAllocTable(fileName, indexBlock.at(0));



	file.close();
}

void FileSystemIndex::Delete(string fileName){

	string table = dd.ReadFromDisk(0);
	string tableToWrite = table;
	int entry = table.find('\n');
	int prevEntry = 0;
	int indexLoc = 0;
	vector<int> indexBlock;
	string substr = table.substr(0, entry);
	while(table != ""){
	
		int delim = substr.find('.');
		if(fileName == substr.substr(0, delim)){ //File name found- entry+1 since entyr is the position of the last \n character
			GetFromFileAllocTable(fileName, indexBlock, indexLoc); //Get name to update bitmap
			//Clear from file table
			tableToWrite.erase(prevEntry, entry+1);
			dd.WriteToDisk(0, tableToWrite.c_str());
		}
		table.erase(0, entry+1);
		entry = table.find('\n');
		substr = table.substr(0, entry);
		prevEntry = entry;
	}

	for(int i = 0; i < indexBlock.size(); i++){
		UpdateBitmap(indexBlock.at(i), true);
	}
	UpdateBitmap(indexLoc, true);
	
}

