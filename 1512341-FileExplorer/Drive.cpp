#include "stdafx.h"
#include "Drive.h"

Drive::Drive()
{
	_numberOfDrive = 0;
}

Drive::~Drive()
{
	for (int i = 0; i <= _numberOfDrive; i++)
	{
		delete[] _driveLetter[i];
		delete[] _volumeLabel[i];
		delete[] _driveType[i];
	}
	delete[] _driveLetter;
	delete[] _volumeLabel;
	delete[] _driveType;
	delete[] _iconPosition;
	_numberOfDrive = 0;
}
int Drive::getNumberOfDrive()
{
	return _numberOfDrive;
}
void Drive::getSystemDrives()
{
	TCHAR str[MAX_LOADSTRING];
	long temp = GetLogicalDriveStrings(MAX_LOADSTRING, str);
	int i = 0;
	while (!(str[i] == '\0' && str[i + 1] == '\0'))
	{
		if (str[i] == '\0')
			_numberOfDrive++;
		i++;
	}
	_numberOfDrive++;
	_driveLetter = new TCHAR*[_numberOfDrive];
	_volumeLabel = new TCHAR*[_numberOfDrive];
	_driveType = new TCHAR*[_numberOfDrive];
	_iconPosition = new int[_numberOfDrive];
	for (int i = 0; i < _numberOfDrive; i++)
	{
		_driveLetter[i] = new TCHAR[MAX_LOADSTRING];
		_volumeLabel[i] = new TCHAR[MAX_LOADSTRING];
		_driveType[i] = new TCHAR[MAX_LOADSTRING];
	}
	//Lấy từng kí tự ổ đĩa
	int index = 0, k;
	for (int j = 0; j < _numberOfDrive; j++)
	{
		k = 0;
		while (str[index] != 0)
		{
			_driveLetter[j][k++] = str[index++];
		}
		_driveLetter[j][k] = '\0';
		index++;
	}

	// Phân loại các ổ địa C,D,E,...
	int nType;
	for (int i = 0; i < _numberOfDrive; ++i)
	{
		StrCpy(str, L"");
		//Determines whether a disk drive is a removable, fixed, CD-ROM, RAM disk, or network drive
		nType = GetDriveType(_driveLetter[i]);
		switch (nType)
		{
		case DRIVE_FIXED:
			StrCpy(_driveType[i], L"Local Disk");
			break;
		case DRIVE_REMOVABLE:
			StrCpy(_driveType[i], L"Removable Drive");
			break;
		case DRIVE_REMOTE:
			StrCpy(_driveType[i], L"Network Drive");
			break;
		case DRIVE_CDROM:
			StrCpy(_driveType[i], L"CD-ROM");
			break;
		default:
			break;
		}
		GetVolumeInformation(_driveLetter[i], str, MAX_LOADSTRING, NULL, NULL, NULL, NULL, 0);
		StrCpy(_volumeLabel[i], str);
		if (wcslen(str) == 0)
		{
			switch (nType)
			{
			case DRIVE_CDROM:
				if (wcslen(_volumeLabel[i]) == 0)
					StrCpy(_volumeLabel[i], L"CD-ROM");
				else
					StrCpy(_volumeLabel[i], str);
				_iconPosition[i] = IDI_CD;
			default:
				break;
			}
		}
		if (!StrCmp(_volumeLabel[i], L"DATA"))
			_iconPosition[i] = IDI_DATA;
		else if (!StrCmp(_volumeLabel[i], L"OS"))
			_iconPosition[i] = IDI_OS;

		StrCat(_volumeLabel[i], _T(" ("));
		StrNCat(_volumeLabel[i], _driveLetter[i], 3);
		StrCat(_volumeLabel[i], _T(")"));
	}
}
TCHAR* Drive::getDriveLetter(int i)
{
	return _driveLetter[i];
}
TCHAR* Drive::getVolumeLabel(int i)
{
	return _volumeLabel[i];
}
TCHAR* Drive::getDriveType(int i)
{
	return _driveType[i];
}
int Drive::getIcon(int i)
{
	return _iconPosition[i];
}
//LPWSTR Drive::getFreeSpace(int i)
//{
//	return _driveSize[i]->getFreeSpace();
//}
//LPWSTR Drive::getTotalSize(int i)
//{
//	return _driveSize[i]->getTotalSize();
//}