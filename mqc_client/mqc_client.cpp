// mqc_client.cpp : main source file for mqc_client.exe
//

#include "stdafx.h"

#include "resource.h"

#include "mqc_clientView.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#include "ServerURLDlg.h"

CAppModule _Module;

bool g_fMQCControlLoaded = false;

LPCTSTR ADMIN_INITCOMPLETE_ENTRY = _T("AdminInitComplete");
LPCTSTR MQC_URL_ENTRY = _T("ServerURL");

// returns the MQC server URL saved in registry. If a URL is not saved, 
// returns an empty string.
CString GetServerURL()
{
	CRegKey key;
   if (key.Open(HKEY_CURRENT_USER, _T("Software\\MQCClient"), KEY_READ) == ERROR_SUCCESS) {
		TCHAR szServerURL[1024] = {0};
        ULONG cURL = _countof(szServerURL);
        if (key.QueryStringValue(MQC_URL_ENTRY, szServerURL, &cURL) == ERROR_SUCCESS)
			return szServerURL;
		key.Close();
   }

   if (key.Open(HKEY_LOCAL_MACHINE, _T("Software\\MQCClient"), KEY_READ) == ERROR_SUCCESS) {
		TCHAR szServerURL[1024] = {0};
        ULONG cURL = _countof(szServerURL);
        if (key.QueryStringValue(MQC_URL_ENTRY, szServerURL, &cURL) == ERROR_SUCCESS)
			return szServerURL;
		key.Close();
   }

   return _T("");
}

// Saves the given URL to registry so that subsequent calls to GetServerURL()
// will return this URL.
void SetServerURL(LPCTSTR lpszURL)
{
	CRegKey key;
   if (key.Create(HKEY_CURRENT_USER, _T("Software\\MQCClient")) == ERROR_SUCCESS) {
        key.SetStringValue(MQC_URL_ENTRY, lpszURL);
		key.Close();
   }
}

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

    // check if program has already been run in administrator mode and MQC ActiveX control
    // was successfully initialized
    bool fInited = false;
    ATL::CRegKey key;
    if (key.Open(HKEY_LOCAL_MACHINE, _T("Software\\MQCClient"), KEY_READ) == ERROR_SUCCESS) {
        DWORD dwDef = 0;
        if (key.QueryDWORDValue(ADMIN_INITCOMPLETE_ENTRY, dwDef) == ERROR_SUCCESS)
            fInited = (dwDef != 0);
            
        key.Close();
    }
    
    if (!fInited && !::IsUserAnAdmin()) {
        // display message asking user to run the program as an administrator
        ::AtlMessageBox(HWND_DESKTOP, _T("Seems like the first time you're running this program on this computer!\n\nPlease run this program as an administrator to download and register the necessary ActiveX controls. ")
                                      _T("This is a one off step and once completed, the program can be run from a normal user context."));
        return 1;
    }

	if (GetServerURL().GetLength() == 0) {
        // display message telling user that URL is not specified
        ::AtlMessageBox(HWND_DESKTOP, _T("Please enter Mercury Quality Center web access URL in the following dialog and continue. ")
                                      _T("This is a one off step and upon successful initialization of the Quality Center interface, it will be saved to the registry during program exit for use in subsequent runs.")); 
        CServerURLDlg dlg;
        if (dlg.DoModal() == IDCANCEL)
            return 1;
		SetServerURL(dlg.m_szURL);
    }
    
	AtlAxWinInit();

	int nRet = Run(lpstrCmdLine, nCmdShow);

    if (!fInited && g_fMQCControlLoaded) {
        if (key.Create(HKEY_LOCAL_MACHINE, _T("Software\\MQCClient")) == ERROR_SUCCESS) {
            // save initialization settings to registry
            DWORD dwDef = g_fMQCControlLoaded ? 1 : 0;
            fInited = key.SetDWORDValue(ADMIN_INITCOMPLETE_ENTRY, dwDef);
            key.Close();
        }
    }
    
	_Module.Term();
	::CoUninitialize();

	return nRet;
}
