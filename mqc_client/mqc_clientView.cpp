// mqc_clientView.cpp : implementation of the CMqc_clientView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "mqc_clientView.h"

extern bool g_fMQCControlLoaded;

// get a child window with a specific window text
static HWND _GetChildWindow(HWND hWnd, LPCTSTR lpszText)
{
    HWND hWndChild = ::GetWindow(hWnd, GW_CHILD);
    while (hWndChild) {
        TCHAR szText[1024] = {0};
        if (::GetWindowText(hWndChild, szText, sizeof(szText)/sizeof(szText[0])) > 0 &&
            ::_tcscmp(szText, lpszText) == 0)
            break;
        hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
    }
    return hWndChild;
}

// get a child window with a specific window class name
static HWND _GetChildWindowByClass(HWND hWnd, LPCTSTR lpszClassName, bool fRecurse)
{
    HWND hWndChild = ::GetWindow(hWnd, GW_CHILD);
    while (hWndChild) {
        TCHAR szClassName[1024] = {0};
        if (::GetClassName(hWndChild, szClassName, sizeof(szClassName)/sizeof(szClassName[0])) > 0 &&
            ::_tcscmp(szClassName, lpszClassName) == 0)
            return hWndChild;
        hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
    }

    // recurse
    hWndChild = ::GetWindow(hWnd, GW_CHILD);
    while (hWndChild) {
        HWND hWnd = _GetChildWindowByClass(hWndChild, lpszClassName, fRecurse);
        if (hWnd)
            return hWnd;
        hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
    }

    return NULL;
}

bool CMqc_clientView::IsLoggedIn()
{
    HWND hWnd = GetProjectCombobox();
    if (hWnd)
        return !::IsWindowVisible(hWnd); // if combobox is not visible, user IS logged in
    return false;
}

CString CMqc_clientView::GetProject()
{
    HWND hWnd = GetProjectCombobox();
    if (hWnd) {
        return GetComboboxText(hWnd);
    }
    return CString();
}

CString CMqc_clientView::GetDomain()
{
    HWND hWnd = GetDomainCombobox();
    if (hWnd) {
        return GetComboboxText(hWnd);
    }
    return CString();
}

CString CMqc_clientView::GetComboboxText(HWND hWndCombobox)
{
    HWND hWnd = ::GetWindow(hWndCombobox, GW_CHILD);
    if (hWnd) {
        TCHAR szText[256] = {0};
        ::GetWindowText(hWnd, szText, _countof(szText));
        return CString(szText);
    }
    return CString();
}

HWND CMqc_clientView::GetProjectCombobox()
{
    return _GetChildWindowByClass(m_hWnd, _T("TcxMRUEdit"), true);
}

HWND CMqc_clientView::GetDomainCombobox()
{
    HWND hWnd = _GetChildWindowByClass(m_hWnd, _T("TcxMRUEdit"), true);
    if (hWnd) {
        return ::GetWindow(hWnd, GW_HWNDNEXT);
    }
    return NULL;
}

BOOL CMqc_clientView::PreTranslateMessage(MSG* pMsg)
{
	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
	   (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return FALSE;

	// give HTML page a chance to translate this message
	return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
}

LRESULT CMqc_clientView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    SetTimer(1001, 1000);   // timer to detect SpiderWindow. Once detected the timer will be stopped
    bHandled = FALSE;
    return 0;
}


LRESULT CMqc_clientView::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    if (wParam == 1001) {   // SpiderWindow detection timer id
        HWND hWndSpider = GetSpiderWindow();
        if (hWndSpider) {
            HWND hWndControlFrame = _GetChildWindowByClass(hWndSpider, _T("TTdFrameX"), false);
            if (hWndControlFrame && ::IsWindowVisible(hWndControlFrame)) {
                g_fMQCControlLoaded = true; // yes MQC control was successfully created!
                KillTimer(wParam);  // we no longer need the timer
            }
        }
    }
    return 0;
}

// returns SpiderWindow child window handle, if found. Or return NULL
HWND CMqc_clientView::GetSpiderWindow()
{
    HWND hWndShellEmbedding = _GetChildWindowByClass(m_hWnd, _T("Shell Embedding"), false);
    if (!hWndShellEmbedding) return NULL;
    
    HWND hWndShellDoc = _GetChildWindowByClass(hWndShellEmbedding, _T("Shell DocObject View"), false);
    if (!hWndShellDoc) return NULL;
    
    HWND hWndIE = _GetChildWindowByClass(hWndShellDoc, _T("Internet Explorer_Server"), false);
    if (!hWndIE) return NULL;
    
    HWND hWndSpider = _GetChildWindowByClass(hWndIE, _T("SpiderWindow"), false);
    return hWndSpider;
}

