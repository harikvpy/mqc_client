// mqc_clientView.h : interface of the CMqc_clientView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CMqc_clientView : public CWindowImpl<CMqc_clientView, CAxWindow>
{
private:
    // returns handle to the top level spider window
    HWND GetSpiderWindow();
    // given a handle to a window of class TcxMRUEdit, returns 
    // the selected item's text
    CString GetComboboxText(HWND hWnd);
public:
	DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg);

    // return true if the user is fully logged into a domain/project
    bool IsLoggedIn();
    // return the project selection combo box handle
    HWND GetProjectCombobox();
    // returns the domain selection combo box handle
    HWND GetDomainCombobox();
    // returns the selected domain text
    CString GetDomain();
    // returns the selected Project text
    CString GetProject();

	BEGIN_MSG_MAP(CMqc_clientView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
