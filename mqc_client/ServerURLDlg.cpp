#include "stdafx.h"
#include "resource.h"

#include "ServerURLDlg.h"

CServerURLDlg::CServerURLDlg()
	: CDialogImpl<CServerURLDlg>()
{
	m_szURL[0] = _T('\0');
}

LRESULT CServerURLDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetDlgItemText(IDC_URL, m_szURL);
	CenterWindow(GetParent());
	return TRUE;
}

LRESULT CServerURLDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    m_szURL[0] = _T('0');
    if (wID == IDOK) {
        GetDlgItemText(IDC_URL, m_szURL, sizeof(m_szURL)/sizeof(m_szURL[0]));
        if (::_tcslen(m_szURL) == 0) {
            return 0;
        }

		CString sURL = m_szURL;
		sURL.MakeLower();
		if (sURL.Find(_T("http://")) != 0 && sURL.Find(_T("https://")) != 0) {
            AtlMessageBox(m_hWnd, _T("Please enter the fully qualified URL beginning with \"http://\" or \"https://\""));
			return 0;
		}

    }
	EndDialog(wID);
	return 0;
}
