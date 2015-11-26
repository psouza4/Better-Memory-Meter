
////////////////////////////////////////////////////////////////////
//
//  Include standard libraries
//
#include <windows.h>
#include <stdio.h>
#include <psapi.h>
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Include definitions for resources
//
#include "resource.h"
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Macro declarations
//
#define BETTERMEMORYMETER_ID			0xFF
#define BETTERMEMORYMETER_EVENT			(WM_APP+1)
#define IDM_EXIT                        701
#define IDM_ABOUT                       702
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Global data
//
HINSTANCE hInst;
HICON hApplicationIcon;
HICON hTrayIcon;
LPTSTR szClassName			= "CLS-ZSC-BMM";
LPTSTR szAppName			= "Better Memory Meter";
UINT bOperatingMode			= 0;
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Function prototypes
//
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK MainCBProc(HWND, UINT, WPARAM, LPARAM);
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Application entry point
//
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HWND hwndMain;
	WNDCLASS wc;
	int rc;

	hInst = hInstance;
	hApplicationIcon = LoadIcon(hInst, MAKEINTRESOURCE(224)); // Custom icon resource

	wc.style = 0;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = hApplicationIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szClassName;

	rc = (RegisterClass (&wc) == 0) ? 1 : 0;

	if (rc)
		return rc;

	hwndMain = CreateWindow(szClassName, szAppName,	
		WS_CLIPCHILDREN | WS_SIZEBOX | WS_POPUP | WS_MINIMIZEBOX | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
		-100, -100, 0, 0, NULL, NULL, hInstance, NULL);
	
	if (!IsWindow (hwndMain))
		return 0x10;

	// Available under Windows NT and Windows 2000, this will adjust
	// the current priority class of the process to 'below normal'.
	SetPriorityClass(GetCurrentProcess( ), 0x00007800);

	ShowWindow(hwndMain, nCmdShow);
	UpdateWindow(hwndMain);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}

	return (int)msg.wParam;
}
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  This is a hack which redirects the primary window to a dialog,
//  thus alleviating the painful task of creating each control
//  manually with CreateWindow().
//
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_TIMER:
			KillTimer(hWnd, 43000);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_MAIN), hWnd, MainCBProc);
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		case WM_CREATE:
			SetTimer(hWnd, 43000, 100, NULL);
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Centers a dialog (window) horizontally and vertically according
//  to the available working area (desktop size, minus Windows Start
//  menu area).
//
VOID CenterDialog(HWND hWnd)
{
	RECT r1, r2;
	GetClientRect(hWnd, &r1);
	SystemParametersInfo(SPI_GETWORKAREA, 0, &r2, 0);
	SetWindowPos(hWnd, NULL, (r2.right - r1.right) / 2, (r2.bottom - r1.bottom) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Displays GetLastError() message, using FormatMessage to give a
//  textual representation of the error.  Call with a NULL hWnd to
//  make it system-owned.
//
VOID DoError(HWND hWnd)
{
	LPVOID lpMsgBuf;
	DWORD dwErrorCode = GetLastError( );
	char errstr[100];
	
	SetLastError(0);
	
	sprintf(errstr, "Error %lu", dwErrorCode);

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	while (((LPTSTR)lpMsgBuf)[strlen((const LPTSTR)lpMsgBuf)-1] == '\n' ||
		((LPTSTR)lpMsgBuf)[strlen((const LPTSTR)lpMsgBuf)-1] == '\r')
		((LPTSTR)lpMsgBuf)[strlen((const LPTSTR)lpMsgBuf)-1] = 0;
	MessageBox(hWnd, (LPTSTR)lpMsgBuf, errstr, 16);
	LocalFree(lpMsgBuf);
}
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Handles system tray messages, including adding the icon,
//  destroying the icon, providing tool-tip information, and 
//  identifying the message value to submit to the application when
//  the user interacts with the icon.
//
BOOL TrayMessage(HWND hWnd, DWORD dwMessage, HICON hIcon, LPTSTR szString)
{
    BOOL bOK;

	NOTIFYICONDATA nid;

	nid.cbSize				= sizeof(NOTIFYICONDATA);
	nid.hWnd				= hWnd;
	nid.uID					= BETTERMEMORYMETER_ID;

	nid.uFlags				= NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage	= BETTERMEMORYMETER_EVENT;
	nid.hIcon				= hIcon;

    strcpy(nid.szTip, szString);

	bOK = Shell_NotifyIcon(dwMessage, &nid);

	if (hIcon)
	    DestroyIcon(hIcon);

	return bOK;
}
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Creates the pop-up menu available when a user right-clicks the
//  system tray icon.
//
void DoPopupMenu(HWND hwnd)
{
    HMENU hMenu = CreatePopupMenu( );

    if (hMenu)
    {
        AppendMenu(hMenu,  MF_STRING, IDM_ABOUT, "&About...");
        AppendMenu(hMenu,  MF_SEPARATOR, 0, NULL);
        AppendMenu(hMenu,  MF_STRING, IDM_EXIT,  "E&xit Better Memory Meter");

      	// Avoids a bug in Windows 95 (see MSDN knowledge base article Q135788)
	    POINT pt;
	    GetCursorPos(&pt);
	    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
    }
}
//
////////////////////////////////////////////////////////////////////


BOOL SetPrivilege(
    HANDLE hToken,          // access token handle
    LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
    BOOL bEnablePrivilege   // to enable or disable privilege
    ) 
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if ( !LookupPrivilegeValue( 
			NULL,            // lookup privilege on local system
			lpszPrivilege,   // privilege to lookup 
			&luid ) )        // receives LUID of privilege
		return FALSE; 

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL) )
		return FALSE; 

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
		return FALSE;

	return TRUE;
}

DWORD MemoryUsedBySuperfetch()
{
	// Counts memory used by:
	//     AudioEndpointBuilder		Windows Audio Endpoint Builder
	//     CscService				Offline Files
	//     hidserv					Human Interface Device Access
	//     Netman					Network Connections
	//     SysMain					Superfetch
	//     TrkWks					Distributed Link Tracking Client
	DWORD dwAmountUsed = 0;

	SC_HANDLE hServiceManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hServiceManager != 0)
	{
		SC_HANDLE hService = OpenService(hServiceManager, "SysMain", SERVICE_ALL_ACCESS);

		if (hService != 0)
		{
			DWORD dNumBytesRequired = 0;
			SERVICE_STATUS_PROCESS service_status;

			int iRetVal = QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)(&(service_status)), sizeof(SERVICE_STATUS_PROCESS), &dNumBytesRequired);

			if (iRetVal != 0)
			{
				PROCESS_MEMORY_COUNTERS_EX process_memory_info;
				process_memory_info.cb = sizeof(PROCESS_MEMORY_COUNTERS_EX);

				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, service_status.dwProcessId);

				if (hProcess != 0)
				{
					if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS *)&process_memory_info, sizeof(PROCESS_MEMORY_COUNTERS_EX)) != 0)
						dwAmountUsed = process_memory_info.PrivateUsage;

					CloseHandle(hProcess);
				}
			}

			CloseServiceHandle(hService);

		}

		CloseServiceHandle(hServiceManager);
	}

	return dwAmountUsed;
}



////////////////////////////////////////////////////////////////////
//
//  Updates the tray icon at timed intervals depending on how much
//  memory is available on the system.
//
BOOL bDoingIt = FALSE;
VOID UpdateSystemTrayIcon(HWND hWnd)
{
	DWORD percent_avail;
	MEMORYSTATUSEX mem_status;
	CHAR szText[2048];

	// Needs to be set for GlobalMemoryStatusEx() version control
	mem_status.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&mem_status);
	
	// Calculate how much memory is available
	DWORD dMemoryUsedBySuperfetch = MemoryUsedBySuperfetch();
	percent_avail = (mem_status.ullAvailPhys + dMemoryUsedBySuperfetch) / (mem_status.ullTotalPhys / 100);
	if (percent_avail > 99)
		percent_avail = 99;

	// Choose the correct icon resource based on how much memory is available
	// Use this	version to use whatever version is available

	// Use this version to force it to convert to 16-color mode
	hTrayIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(225 + percent_avail), IMAGE_ICON, 16, 16, LR_VGACOLOR | LR_DEFAULTCOLOR);

	// Produce a textual memory availability representation for the tool-tip description of the
	// system tray icon
	if (mem_status.ullTotalPhys > 1610612736LL) // 1.5 GB+ goes in GB range
	{
 		sprintf(szText, "    %.1f GB total\n - %.1f GB used\n= %.1f GB avail (%d%%)",
			((double)(mem_status.ullTotalPhys))/(1024.0*1024.0*1024.0),
			((double)(mem_status.ullTotalPhys-(mem_status.ullAvailPhys + dMemoryUsedBySuperfetch)))/(1024.0*1024.0*1024.0),
			((double)(mem_status.ullAvailPhys+dMemoryUsedBySuperfetch))/(1024.0*1024.0*1024.0),
			percent_avail);
	}
	else
	{
 		sprintf(szText, "    %.1f MB total\n - %.1f MB used\n= %.1f MB avail (%d%%)",
			((double)(mem_status.ullTotalPhys))/(1024.0*1024.0),
			((double)(mem_status.ullTotalPhys-(mem_status.ullAvailPhys + dMemoryUsedBySuperfetch)))/(1024.0*1024.0),
			((double)(mem_status.ullAvailPhys+dMemoryUsedBySuperfetch))/(1024.0*1024.0),
			percent_avail);
	}

	// Update the system tray with this icon and text
	TrayMessage(hWnd, NIM_MODIFY, hTrayIcon, szText);

	// Clean up memory
	DestroyIcon(hTrayIcon);
	return;
}
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//  Main (and only) dialog message processing callback.
//
BOOL CALLBACK MainCBProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Integrated shell (I.E. 4 and later) Active Desktop feature that allows you to
	// restore the system tray icon on a shell restart.  Required for if/when explorer
	// crashes or else the system tray icon won't redraw itself.
    static UINT s_uiTaskbarCreated = RegisterWindowMessage("TaskbarCreated"); 


	// Reinstall tray icon whenever the shell restarts
	if (message == s_uiTaskbarCreated)
	{
		TrayMessage(hWnd, NIM_ADD, hApplicationIcon, "Better Memory Meter v2.00");
		return FALSE;
	}

	switch (message)
	{
		// Messages from buttons, menus, and other controls
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDM_ABOUT:
					// Restore the window
					ShowWindow(hWnd, SW_SHOW);
					ShowWindow(GetParent(hWnd), SW_SHOW);
					SetForegroundWindow(hWnd);
					ShowWindow(hWnd, SW_RESTORE);
					ShowWindow(GetParent(hWnd), SW_RESTORE);
					break;

				case IDM_EXIT:
					// Exit application (show the window first, to avoid
					// problems with application redraw)
					ShowWindow(hWnd, SW_SHOW);
					ShowWindow(GetParent(hWnd), SW_SHOW);
					DestroyWindow(hWnd);
					break;

				case IDC_OK:
					ShowWindow(hWnd, SW_HIDE);
					ShowWindow(GetParent(hWnd), SW_HIDE);
					break;
			}
			break;

		// Timer called to auto-hide the window after a few moments
		case WM_TIMER:
			if (bOperatingMode == 0)
			{
				bOperatingMode++;
				ShowWindow(hWnd, SW_HIDE);
				ShowWindow(GetParent(hWnd), SW_HIDE);
				KillTimer(hWnd, 45678);
				SetTimer(hWnd, 45678, 1200, NULL);
			}
			UpdateSystemTrayIcon(hWnd);
			break;


		// Upon application close, clean up any available timers, remove the
		// system tray icon, and destroy the dialog (window)
		case WM_DESTROY:
		case WM_CLOSE:
			KillTimer(hWnd, 45678);
            TrayMessage(hWnd, NIM_DELETE, NULL, "");
			EndDialog(hWnd, TRUE);
			break;


		// If the window is resizing...
		case WM_SIZE:
		case WM_SIZING:
			// ...particularly if it's being minimized...
			if (wParam == SIZE_MINIMIZED)
			{
				// Hide the window
				ShowWindow(hWnd, SW_HIDE);
				ShowWindow(GetParent(hWnd), SW_HIDE);
			}
			break;


		// When we receive a message from the system tray icon...
        case BETTERMEMORYMETER_EVENT:
		    switch (lParam)
		    {
				// ...and that message is 'double-clicked'...
                case WM_LBUTTONDBLCLK:
                {
					// Restore the window (make it visible)
					ShowWindow(hWnd, SW_SHOW);
					ShowWindow(GetParent(hWnd), SW_SHOW);
					SetForegroundWindow(hWnd);
					ShowWindow(hWnd, SW_RESTORE);
					ShowWindow(GetParent(hWnd), SW_RESTORE);
					break;
				}

				// ...and that message is 'right-clicked'...
                case WM_RBUTTONUP:
					// Display the pop-up menu
                    DoPopupMenu(hWnd);
                    break;
			}
			break;


		// Application startup...
		case WM_INITDIALOG:
			SendMessage(hWnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hApplicationIcon);
			CenterDialog(hWnd);
			TrayMessage(hWnd, NIM_ADD, hApplicationIcon, "Better Memory Meter v2.00");

			// Minimize the main window after a few moments
			SetTimer(hWnd, 45678, 1, NULL);

			HANDLE hToken;
			OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
			SetPrivilege(hToken, "SeDebugPrivilege", TRUE);
			break;
	}

	return FALSE;
}
//
////////////////////////////////////////////////////////////////////

