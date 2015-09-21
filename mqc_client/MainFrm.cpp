// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "mqc_clientView.h"
#include "MainFrm.h"
#include "ServerURLDlg.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

void CMainFrame::UpdateTitleBar()
{
    if (m_view.IsLoggedIn()) {
        m_sDomain = m_view.GetDomain();
        m_sProject = m_view.GetProject();
        if (!m_sDomain.IsEmpty() && !m_sProject.IsEmpty()) {
            //AtlMessageBox(m_hWnd, LPCTSTR(sDomain));
            CString sTitle = CString(_T("MQC Client - ")) + m_sDomain + CString(_T(":")) + m_sProject;
            SetWindowText(sTitle);
            return;
        }
    }

    TCHAR szText[2048] = {0};
    ::_tcscpy_s(szText, _T("MQC Client"));
    SetWindowText(szText);
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	//HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	//AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	//CreateSimpleStatusBar();

	m_hWndClient = m_view.Create(m_hWnd, rcDefault, GetServerURL(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);

	//UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UpdateTitleBar();

    SetTimer(1001, 500);
/*
	if (!::IsUserAnAdmin())
		UIEnable(ID_EDIT_SERVERURL, FALSE);
*/
	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document
	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
#if 0
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
#endif
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewServerURL(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CServerURLDlg dlg;
	::_tcscpy_s(dlg.m_szURL, GetServerURL());
	if (dlg.DoModal() == IDOK) {
		SetServerURL(dlg.m_szURL);
		CComPtr<IWebBrowser2> pWB2;
		HRESULT hr;
		hr = m_view.QueryControl ( &pWB2 );
		if (pWB2) {
			CComVariant v;  // empty variant
			pWB2->Navigate ( CComBSTR(dlg.m_szURL), &v, &v, &v, &v );
		}
		UpdateTitleBar();
	}
    return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    if (wParam == 1001) {   // timer to detect switch to logged in status
        if (m_view.IsLoggedIn()) {
            UpdateTitleBar();
            // state switch code
            KillTimer(wParam);
            SetTimer(1002, 500);
        }
    } else if (wParam == 1002) {   // timer to detect switch to logged out state and change of project
        if (m_view.IsLoggedIn()) {
            // check if user has changed the current project
            CString sDomain = m_view.GetDomain();
            CString sProject = m_view.GetProject();
            if (sDomain != m_sDomain || sProject != m_sProject)
                UpdateTitleBar();
        } else {
            // user has logged out - switch state
            UpdateTitleBar();
            // state switch code
            KillTimer(wParam);
            SetTimer(1001, 500);
        }
    }
    return 0;
}
