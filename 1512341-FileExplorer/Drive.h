#pragma once
#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#define IDI_CD                          134
#define IDI_OS                          135
#define IDI_DATA                        136
#define MAX_LOADSTRING 100
class Drive
{
private:
	int _numberOfDrive;
	TCHAR** _driveLetter;
	TCHAR** _volumeLabel;
	TCHAR** _driveType;
	int *_iconPosition;
public:
	Drive();
	~Drive();
	int getNumberOfDrive();
	TCHAR* getDriveLetter(int);
	TCHAR* getVolumeLabel(int);
	TCHAR* getDriveType(int);
	int getIcon(int);
	void getSystemDrives();
	//chưa làm size 
	LPWSTR getFreeSpace(int);
	LPWSTR getTotalSize(int);
};

