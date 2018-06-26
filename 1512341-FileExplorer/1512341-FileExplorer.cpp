// 1512341_FileExplorer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "1512341-FileExplorer.h"

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND g_listView;
HWND g_treeView;
HWND g_hWnd;
RECT rcClient;
vector<TCHAR*>g_nameFile;
vector<TCHAR*>g_nameFolder;
int x, y, size;
int parentWidth;
int parentHeight, nWidth, nHeight;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void createTreeView(int x, int y, int height, int width, HWND hWnd);
void loadOrExecSelected(HWND g_listView);
void loadDirItemToLisview(HWND hWnd, HWND g_listView, LPCWSTR path);
LPWSTR getDateModified(FILETIME ftLastWrite);
LPCWSTR getPathForListView(HWND m_hListView, int iItem);
LPCWSTR getPathForTreeView(HTREEITEM hItem);
void preLoad(HTREEITEM hItem);
void loadChild(HTREEITEM &hWnd, LPCWSTR path, BOOL showHiddenSystem);
void preLoadExpanding(HTREEITEM hPrev, HTREEITEM hCurSel);
void expand(HWND g_treeView, HTREEITEM hPrev);
void loadChildForListView(LPCWSTR path, Drive *drive);
void loadDesktop();
char *convertByteToStringSize(DWORD fileSizeLow);
bool findNameItem(vector<TCHAR*>g_nameFile, char *name);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY1512341FILEEXPLORER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY1512341FILEEXPLORER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FILE_EXPLORER));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_MY1512341FILEEXPLORER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_FILE_EXPLORER));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable


	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
void loadMyComputerToTreeView(HWND g_treeView, Drive *drive)
{
	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = nullptr;
	tvInsert.hInsertAfter = TVI_ROOT;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvInsert.item.pszText = L"Desktop";
	tvInsert.item.lParam = (LPARAM)_T("Desktop");
	//tvInsert.item.iImage = IDI_DEKSTOP;
	//tvInsert.item.iSelectedImage = IDI_DEKSTOP;
	HTREEITEM hDesktop = TreeView_InsertItem(g_treeView, &tvInsert);

	tvInsert.hParent = hDesktop;
	tvInsert.hInsertAfter = TVI_LAST;
	//tvInsert.item.iImage = IDI_COMPUTER;
	//tvInsert.item.iSelectedImage = IDI_COMPUTER;
	tvInsert.item.pszText = _T("My Computer");
	tvInsert.item.lParam = (LPARAM)_T("MyComputer");
	HTREEITEM hMyComputer = TreeView_InsertItem(g_treeView, &tvInsert);

	for (int i = 0; i < drive->getNumberOfDrive(); i++)
	{
		tvInsert.hParent = hMyComputer; //Them
		tvInsert.item.iImage = drive->getIcon(i);
		tvInsert.item.iSelectedImage = drive->getIcon(i);
		tvInsert.item.pszText = drive->getVolumeLabel(i);
		tvInsert.item.lParam = (LPARAM)drive->getDriveLetter(i);
		HTREEITEM hDrive = TreeView_InsertItem(g_treeView, &tvInsert);

		if (drive->getIcon(i) == IDI_DATA || drive->getIcon(i) == IDI_OS)
		{
			tvInsert.hParent = hDrive; //Them
			tvInsert.item.pszText = _T("PreLoad"); //Them
			tvInsert.item.lParam = (LPARAM)_T("PreLoad");
			TreeView_InsertItem(g_treeView, &tvInsert);
		}
		TreeView_Expand(g_treeView, hMyComputer, TVE_EXPAND);
		TreeView_SelectItem(g_treeView, hMyComputer);
	}

}
void loadMyComputerToListView(HWND g_listView, Drive *drive)
{
	LV_ITEM lv;
	for (int i = 0; i < drive->getNumberOfDrive(); i++)
	{
		//Nạp cột đầu tiên cũng là thông tin chính
		lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lv.iItem = i;
		lv.iImage = 0;
		lv.iSubItem = 0;
		lv.pszText = drive->getVolumeLabel(i);
		lv.lParam = (LPARAM)drive->getDriveLetter(i);
		ListView_InsertItem(g_listView, &lv);
		//Nạp các cột còn lại (Type, Size, Free Space)
		lv.mask = LVIF_TEXT;

		//Cột đầu tiên là Type
		lv.iSubItem = 1;
		lv.pszText = drive->getDriveType(i);
		ListView_SetItem(g_listView, &lv);
	}
}
void createTreeView(int x, int y, int height, int width, HWND hWnd)
{

	g_treeView = CreateWindowEx(0, WC_TREEVIEW, L"TREE VIEW", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_SIZEBOX | WS_VSCROLL | WS_TABSTOP | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, x, y, width, height, hWnd, (HMENU)IDM_TREE_VIEW, hInst, nullptr);

}
void createListView(int x, int y, int width, int height, HWND hWnd)
{
	g_listView = CreateWindow(WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | WS_HSCROLL | WS_VSCROLL, rcClient.left + LEFT_WINDOW_WIDTH + SPLITTER_BAR_WIDTH,
		0,
		rcClient.right - (rcClient.left + LEFT_WINDOW_WIDTH + SPLITTER_BAR_WIDTH),
		(rcClient.bottom - rcClient.top) - (TOP_POS + BOTTOM_POS),
		hWnd,
		NULL, hInst, NULL);
	LVCOLUMN lvCol1;

	lvCol1.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol1.fmt = LVCFMT_LEFT;

	lvCol1.cx = 130;
	lvCol1.pszText = _T("Name");
	ListView_InsertColumn(g_listView, 0, &lvCol1);

	LVCOLUMN lvCol2;
	lvCol2.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol2.fmt = LVCFMT_LEFT | LVCF_WIDTH;
	lvCol2.cx = 130;
	lvCol2.pszText = _T("Size");
	ListView_InsertColumn(g_listView, 1, &lvCol2);

	LVCOLUMN lvCol3;
	lvCol3.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol3.fmt = LVCFMT_CENTER;
	lvCol3.cx = 130;
	lvCol3.pszText = _T("Type");
	ListView_InsertColumn(g_listView, 2, &lvCol3);

	LVCOLUMN lvCol4;
	lvCol4.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol4.fmt = LVCFMT_CENTER;
	lvCol4.cx = 339;
	lvCol4.pszText = _T("Data modified");
	ListView_InsertColumn(g_listView, 3, &lvCol4);
}
LPNMLISTVIEW pnmv;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	
	Drive *drive = new Drive();
	NMHDR* notifyMess = nullptr;
	drive->getSystemDrives();
	LPNMTREEVIEW lpnmTree;
	int iPos;
	LPCWSTR path;
	switch (message)
	{
	case WM_CREATE:
		INITCOMMONCONTROLSEX icex;

		// Ensure that the common control DLL is loaded. 
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES;
		InitCommonControlsEx(&icex);

		GetClientRect(hWnd, &rcClient);
		parentWidth = rcClient.right - rcClient.left;
		parentHeight = rcClient.bottom - rcClient.top;
		nHeight = rcClient.bottom;
		nWidth = (rcClient.right - rcClient.left) * 2 / 3 + 1;
		x = rcClient.right / 3;
		y = 0;

		createTreeView(0, 0, parentHeight, parentWidth / 4, hWnd);
		createListView(parentWidth / 4, y, nWidth, nHeight, hWnd);

		loadMyComputerToListView(g_listView, drive);
		loadMyComputerToTreeView(g_treeView, drive);
		SetFocus(g_treeView);

		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			MessageBox(hWnd, TEXT("1512341-Thang Vỹ Nam\n University of Science - HCMUS"), TEXT("Information"), MB_OK);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &rcClient);
		size = rcClient.right / 4;
		if (size > 220)
			size = 220;
		parentHeight = rcClient.bottom - rcClient.top;
		MoveWindow(g_treeView, 0, 0, size, parentHeight, TRUE);
		MoveWindow(g_listView, size, 0, rcClient.right - size, rcClient.bottom, TRUE);
		break;
	case WM_NOTIFY:
		notifyMess = (NMHDR*)lParam;
		lpnmTree = (LPNMTREEVIEW)notifyMess;
		switch (notifyMess->code)
		{

		case TVN_ITEMEXPANDING:
			preLoadExpanding(lpnmTree->itemOld.hItem, lpnmTree->itemNew.hItem);
			break;
		case NM_DBLCLK:
			if (notifyMess->hwndFrom == g_listView)
				loadOrExecSelected(g_listView);
			break;
		case NM_RCLICK:
			if (MessageBox(hWnd, L"Are you sure you want to permanently delete this file?", L"Delete", MB_YESNO) == IDYES);
			{
				int iPos = ListView_GetNextItem(g_listView, -1, LVNI_SELECTED);
				while (iPos != -1) {
					path = getPathForListView(g_listView, iPos);
					ListView_DeleteItem(g_listView, iPos);
					wstring wStr((wstring)path);
					const wchar_t *str = wStr.c_str();
					size_t size = wcslen(str) * 2 + 2;
					char * StartPoint = new char[size];
					size_t c_size;
					wcstombs_s(&c_size, StartPoint, size, str, size);
					if (findNameItem(g_nameFile, StartPoint)) // nếu đây là tập tin 
						remove(StartPoint);
					else // nếu đây là folder
						RemoveDirectory(path);
					iPos = ListView_GetNextItem(g_listView, iPos, LVNI_SELECTED);
				}
			}
			break;
		case TVN_SELCHANGED:
			expand(g_treeView, TreeView_GetNextItem(g_treeView, nullptr, TVGN_CARET));
			ListView_DeleteAllItems(g_listView);
			loadChildForListView(getPathForTreeView(TreeView_GetNextItem(g_treeView, NULL, TVGN_CARET)), drive);
		default:
			break;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
LPWSTR getDateModified(FILETIME ftLastWrite)
{
	SYSTEMTIME stUTC, stLocal;
	FileTimeToSystemTime(&ftLastWrite, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	TCHAR *buffer = new TCHAR[50];
	wsprintf(buffer, _T("%02d/%02d/%04d %02d:%02d %s"),
		stLocal.wDay, stLocal.wMonth, stLocal.wYear,
		(stLocal.wHour>12) ? (stLocal.wHour / 12) : (stLocal.wHour),
		stLocal.wMinute,
		(stLocal.wHour>12) ? (_T("PM")) : (_T("AM")));

	return buffer;
}
void loadDirItemToLisview(HWND hWnd, HWND g_listView, LPCWSTR path)
{
	TCHAR buffer[10240];
	StrCpy(buffer, path);

	if (wcslen(path) == 3)
		StrCat(buffer, _T("*"));
	else
		StrCat(buffer, _T("\\*"));
	WIN32_FIND_DATA fd;
	HANDLE hFile;
	BOOL bFound = true;
	LV_ITEM lv, lv2;
	TCHAR * folder_path = nullptr;
	int itemIndexFolder = 0;
	int fileSizeCount = 0;
	hFile = FindFirstFileW(buffer, &fd);
	bFound = TRUE;
	TCHAR * file_path = nullptr;
	int itemIndexFile = 0;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	while (bFound)
	{
		if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN))
		{
			file_path = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(file_path, path);

			if (wcslen(path) != 3)
				StrCat(file_path, _T("\\"));

			StrCat(file_path, fd.cFileName);

			//Cột thứ nhất là tên hiển thị của tập tin
			lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv.iItem = itemIndexFile;
			lv.iSubItem = 0;
			lv.pszText = fd.cFileName;
			lv.lParam = (LPARAM)file_path;
			lv.iImage = 0;
			ListView_InsertItem(g_listView, &lv);

			//Cột thứ hai là Size
			DWORD fileSizeLow = fd.nFileSizeLow; //The low-order DWORD value of the file size, in bytes
			ListView_SetItemText(g_listView, itemIndexFile, 1, (TCHAR*)convertByteToStringSize(fileSizeLow));
			//Cột thứ ba là Type
			ListView_SetItemText(g_listView, itemIndexFile, 2, L"File");
			//Cột thứ tư là Date Modified	
			ListView_SetItemText(g_listView, itemIndexFile, 3, getDateModified(fd.ftLastWriteTime));
			g_nameFile.push_back(file_path);
			++itemIndexFile;
		}
		else
		{
			folder_path = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(folder_path, path);

			if (wcslen(path) != 3)
				StrCat(folder_path, _T("\\"));

			StrCat(folder_path, fd.cFileName);
			lv2.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lv2.iItem = itemIndexFolder;
			lv2.iSubItem = 0;
			lv2.pszText = fd.cFileName;
			lv2.lParam = (LPARAM)folder_path;
			lv2.iImage = 0;
			ListView_InsertItem(g_listView, &lv2);

			//Cột thứ ba là cột Type
			ListView_SetItemText(g_listView, itemIndexFolder, 2, _T("Folder"));

			//Cột thứ tư là cột Date modified
			ListView_SetItemText(g_listView, itemIndexFolder, 3, getDateModified(fd.ftLastWriteTime));
			++itemIndexFolder;
			g_nameFolder.push_back(folder_path);
		}

		bFound = FindNextFileW(hFile, &fd);
	}//while
}
void loadOrExecSelected(HWND g_listView)
{
	LPCWSTR filePath = getPathForListView(g_listView, ListView_GetSelectionMark(g_listView));

	WIN32_FIND_DATA fd;

	//Retrieves attributes for a specified file or directory.
	if (GetFileAttributesEx(filePath, GetFileExInfoStandard, &fd) != 0)
	{
		//Check whether it's folder or directory
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//Delete and reload item in Listview
			ListView_DeleteAllItems(g_listView);
			loadDirItemToLisview(g_hWnd, g_listView, filePath);
		}
		else
		{
			ShellExecute(NULL, _T("open"), filePath, NULL, NULL, SW_SHOWNORMAL);
		}
	}
}
HTREEITEM getMyComputer(HWND g_treeView)
{
	return TreeView_GetChild(g_treeView, TreeView_GetRoot(g_treeView));
}
LPCWSTR getPathForTreeView(HTREEITEM hItem)
{
	TVITEMEX tv;
	tv.mask = TVIF_PARAM;
	tv.hItem = hItem;
	TreeView_GetItem(g_treeView, &tv);
	return (LPCWSTR)tv.lParam;
}
LPCWSTR getPathForListView(HWND m_hListView, int iItem)
{
	LVITEM lv;
	lv.mask = LVIF_PARAM;
	lv.iItem = iItem;
	lv.iSubItem = 0;
	ListView_GetItem(m_hListView, &lv);
	return (LPCWSTR)lv.lParam;
}
void preLoad(HTREEITEM hItem)
{
	TCHAR buffer[1024];
	StrCpy(buffer, getPathForTreeView(hItem));

	if (wcslen(buffer) == 3) //Nếu quét các ổ đĩa
	{
		if (StrStr(buffer, _T("A:")) || StrStr(buffer, _T("B:"))) //Đĩa mềm hổng làm 
			return;
	}
	else
		StrCat(buffer, _T("\\"));

	StrCat(buffer, _T("*"));

	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);

	if (hFile == INVALID_HANDLE_VALUE)
		return;

	BOOL bFound = true;

	//Trong khi còn tìm thấy file hay thư mục
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& (StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			TV_INSERTSTRUCT tvInsert;
			tvInsert.hParent = hItem;
			tvInsert.hInsertAfter = TVI_LAST;
			tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
			tvInsert.item.pszText = NULL;
			tvInsert.item.lParam = (LPARAM)_T("PreLoad");
			TreeView_InsertItem(g_treeView, &tvInsert);
			bFound = FALSE;
		}
		else
			bFound = FindNextFileW(hFile, &fd);
	}//while
}

void loadChild(HTREEITEM &hWnd, LPCWSTR path, BOOL showHiddenSystem = FALSE)
{
	TCHAR buffer[MAX_LOADSTRING];
	StrCpy(buffer, path);

	StrCat(buffer, _T("\\*"));

	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = hWnd;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvInsert.item.iImage = 0;
	tvInsert.item.iSelectedImage = 0;

	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);
	BOOL bFound = 1;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	TCHAR * folderPath;
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM)
			&& (StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			tvInsert.item.pszText = fd.cFileName;
			folderPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];

			StrCpy(folderPath, path);
			if (wcslen(path) != 3)
				StrCat(folderPath, _T("\\"));
			StrCat(folderPath, fd.cFileName);

			tvInsert.item.lParam = (LPARAM)folderPath;
			HTREEITEM hItem = TreeView_InsertItem(g_treeView, &tvInsert);
			//Preload
			preLoad(hItem);
		}

		bFound = FindNextFileW(hFile, &fd);
	}//while
}
void preLoadExpanding(HTREEITEM hPrev, HTREEITEM hCurSel)
{
	if (hCurSel == getMyComputer(g_treeView)) //Nếu là My Computer không nạp
		return;

	HTREEITEM hCurSelChild = TreeView_GetChild(g_treeView, hCurSel);

	if (!StrCmp(getPathForTreeView(hCurSelChild), _T("PreLoad")))
	{
		TreeView_DeleteItem(g_treeView, hCurSelChild);
		loadChild(hCurSel, getPathForTreeView(hCurSel));
	}
}
void expand(HWND g_treeView, HTREEITEM hPrev)
{
	TreeView_Expand(g_treeView, hPrev, TVE_EXPAND);
}
void loadChildForListView(LPCWSTR path, Drive *drive)
{
	if (path == nullptr)
		return;
	if (!StrCmp(path, _T("Desktop")))
		loadDesktop(); // load only MYCOMPUTER
	else
		if (!StrCmp(path, _T("MyComputer")))
			loadMyComputerToListView(g_listView, drive);
		else
			loadDirItemToLisview(g_hWnd, g_listView, path);
}
void loadDesktop()
{
	ListView_DeleteAllItems(g_listView);

	LV_ITEM lv;
	lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lv.iItem = 0;
	lv.iSubItem = 0;
	lv.pszText = _T("My Computer");
	lv.iImage = 0;
	lv.lParam = (LPARAM)_T("MyComputer");
	ListView_InsertItem(g_listView, &lv);
}
char* convertByteToStringSize(DWORD fileSizeLow)
{
	int nType = 0; //Bytes
	int nRight;

	if (fileSizeLow >= 1024)
	{

		nRight = fileSizeLow % 1024;

		while (nRight > 99)
			nRight /= 10;

		fileSizeLow /= 1024;
		++nType;
	}
	else
		nRight = 0;

	TCHAR *buffer = new TCHAR[11];
	TCHAR *temp = new TCHAR[15];
	_itow_s(fileSizeLow, buffer, 11, 10);
	int k = 0;
	int j = 0;
	bool check = false;
	if (_tcslen(buffer) > 3)
	{
		check = true;

		for (int i = _tcslen(buffer) - 1; i >= 0; i--)
		{
			if (k == 3)
			{
				temp[j++] = ',';
				k = 0;
			}

			temp[j++] = buffer[i];
			k++;
		}
	}
	if (!check)
	{
		if (nType > 0)
		{
			_itow_s(fileSizeLow, buffer, 11, 10);
			StrCat(buffer, _T("   KB"));
		}
		else
		{
			_itow_s(1, buffer, 11, 10);
			StrCat(buffer, _T("   KB"));
		}
	}
	else
	{
		temp[j] = '\0';
		StrCat(temp, _T("   KB"));
	}
	if (!check)
		return (char*)buffer;
	return _strrev((char*)temp);
}
void convertWStringToCharPtr(_In_ std::wstring input, _Out_ char * outputString)
{
	size_t outputSize = input.length() + 1; // +1 for null terminator
	outputString = new char[outputSize];
	size_t charsConverted = 0;
	const wchar_t * inputW = input.c_str();
	wcstombs_s(&charsConverted, outputString, outputSize, inputW, input.length());
}
bool findNameItem(vector<TCHAR*>g_nameFile, char *name)
{
	for (int i = 0; i < g_nameFile.size(); i++)
	{
		char * temp = new char[_tcslen((g_nameFile.at(i)))];
		for (int j = 0; j < _tcslen((g_nameFile.at(i))); j++)
		{
			temp[j] = g_nameFile.at(i)[j];
		}
		temp[_tcslen((g_nameFile.at(i)))] = '\0';
		if (strcmp(name, temp) == 0)
			return true;
	}
	return false;
}
