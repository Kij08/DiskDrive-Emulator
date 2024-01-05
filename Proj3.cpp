#include <iostream>
#include "FileSystem.h"
#include "FileSystemChained.h"
#include "FileSystemIndex.h"

using namespace std;

int main(int argc, char** argv){
	string arg = argv[1];
	if(arg == "continuous"){
		FileSystem fs;
		fs.InitDD();
		int choice = 0;

		while(choice != 8){
			cout << "1) Display a file\n" 
			<< "2) Display the file table\n"
			<< "3) Display the free space bitmap\n"
			<< "4) Display a disk block\n"
			<< "5) Copy a file from the simulation to a file on the real system\n"
			<< "6) Copy a file from the real system to a file in the simulation\n"
			<< "7) Delete a file\n"
			<< "8) Exit\n";
			cout << "Input: ";
			cin >> choice;

			switch(choice){
				case 1:
				{
					cout << "Please enter file name: ";
					string fileName;
					cin >> fileName;
					fs.DisplayFile(fileName);
					break;
				}
				case 2:
					fs.DisplayFileTable();
					break;
				case 3: 
					fs.DisplayFreeSpaceBitmap();
					break;
				case 4:
				{
					cout << "Enter the index of the disk block: ";
					int index;
					cin >> index;
					fs.DisplayDiskBlock(index);
					break;
				}
				case 5:
				{
					cout << "Please enter file name: ";
					string fileName;
					cin >> fileName;
					fs.CpyToSystem(fileName);
					break;
				}
				case 6:
				{
					cout << "Please enter file name: ";
					string fileName;
					cin >> fileName;
					fs.CpyFromSystem(fileName);
					break;
				}
				case 7:
				{
					cout << "Please enter file name: ";
					string fileName;
					cin >> fileName;
					fs.Delete(fileName);
					break;
				}
				case 8:
					cout << "Exitting...\n";
					break;
				default:
					cout << "Unrecognised choice. Please select again.\n";
					break;
			}
		}

	}
	else if (arg == "chained") {
		FileSystemChained fs;
		fs.InitDD();
		int choice = 0;

		while (choice != 8) {
			cout << "1) Display a file\n"
				<< "2) Display the file table\n"
				<< "3) Display the free space bitmap\n"
				<< "4) Display a disk block\n"
				<< "5) Copy a file from the simulation to a file on the real system\n"
				<< "6) Copy a file from the real system to a file in the simulation\n"
				<< "7) Delete a file\n"
				<< "8) Exit\n";
			cout << "Input: ";
			cin >> choice;

			switch (choice) {
			case 1:
			{
				cout << "Please enter file name: ";
				string fileName;
				cin >> fileName;
				fs.DisplayFile(fileName);
				break;
			}
			case 2:
				fs.DisplayFileTable();
				break;
			case 3:
				fs.DisplayFreeSpaceBitmap();
				break;
			case 4:
			{
				cout << "Enter the index of the disk block: ";
				int index;
				cin >> index;
				fs.DisplayDiskBlock(index);
				break;
			}
			case 5:
			{
				cout << "Please enter file name: ";
				string fileName;
				cin >> fileName;
				fs.CpyToSystem(fileName);
				break;
			}
			case 6:
			{
				cout << "Please enter file name: ";
				string fileName;
				cin >> fileName;
				fs.CpyFromSystem(fileName);
				break;
			}
			case 7:
			{
				cout << "Please enter file name: ";
				string fileName;
				cin >> fileName;
				fs.Delete(fileName);
				break;
			}
			case 8:
				cout << "Exitting...\n";
				break;
			default:
				cout << "Unrecognised choice. Please select again.\n";
				break;
			}
		}

	}
	else if (arg == "indexed") {
		FileSystemIndex fs;
		fs.InitDD();
		int choice = 0;

		while (choice != 8) {
			cout << "1) Display a file\n"
				<< "2) Display the file table\n"
				<< "3) Display the free space bitmap\n"
				<< "4) Display a disk block\n"
				<< "5) Copy a file from the simulation to a file on the real system\n"
				<< "6) Copy a file from the real system to a file in the simulation\n"
				<< "7) Delete a file\n"
				<< "8) Exit\n";
			cout << "Input: ";
			cin >> choice;

			switch (choice) {
			case 1:
			{
				cout << "Please enter file name: ";
				string fileName;
				cin >> fileName;
				fs.DisplayFile(fileName);
				break;
			}
			case 2:
				fs.DisplayFileTable();
				break;
			case 3:
				fs.DisplayFreeSpaceBitmap();
				break;
			case 4:
			{
				cout << "Enter the index of the disk block: ";
				int index;
				cin >> index;
				fs.DisplayDiskBlock(index);
				break;
			}
			case 5:
			{
				cout << "Please enter file name: ";
				string fileName;
				cin >> fileName;
				fs.CpyToSystem(fileName);
				break;
			}
			case 6:
			{
				cout << "Please enter file name: ";
				string fileName;
				cin >> fileName;
				fs.CpyFromSystem(fileName);
				break;
			}
			case 7:
			{
				cout << "Please enter file name: ";
				string fileName;
				cin >> fileName;
				fs.Delete(fileName);
				break;
			}
			case 8:
				cout << "Exitting...\n";
				break;
			default:
				cout << "Unrecognised choice. Please select again.\n";
				break;
			}
		}

	}
	else{
		cout << "Unrecognised argument: " << argv[1] << "\n";
	}

	return 0;
}
