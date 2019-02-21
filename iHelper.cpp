#include "stdafx.h"
#include "iHelper.h"
#include <time.h>

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	HRESULT Hr = ::CoInitialize(NULL);
	if(FAILED(Hr)) return 0;

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
	CPaintManagerUI::SetResourceZip(MAKEINTRESOURCE(IDR_ZIPRES), _T("ZIPRES"));

	CMainDlg* pDlg = new CMainDlg();
	if(pDlg == NULL) return 0;
	pDlg->Create(NULL,_T("iHelper"),UI_WNDSTYLE_FRAME,WS_EX_APPWINDOW,0,0,800,600);
	pDlg->CenterWindow();
	pDlg->ShowWindow(SW_HIDE);
	CPaintManagerUI::MessageLoop();

	::CoUninitialize();

	return 0;
}

CMainDlg::CMainDlg()
{
	labTitle = NULL;
}

CMainDlg::~CMainDlg()
{
}

LPCTSTR CMainDlg::GetWindowClassName() const
{
	return _T("MainDlg");
}

void CMainDlg::InitWindow()
{
	labTitle = static_cast<CLabelUI*>(m_Manager.FindControl(_T("labTitle")));
	if (labTitle == NULL)
	{
		MessageBox(NULL,_T("Loading resources failed!"),_T("iHelper"),MB_OK|MB_ICONERROR);
		return;
	}
	trayIcon.CreateTrayIcon(GetHWND(),IDI_IHELPER,_T("iHelper"));
}

void CMainDlg::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

CDuiString CMainDlg::GetSkinFile()
{
	return _T("MainDlg.xml");
}

void CMainDlg::Notify(TNotifyUI& msg)
{
	CDuiString sCtrlName = msg.pSender->GetName();
	//OutputDebugString(msg.sType);
	if (_tcsicmp(msg.sType,DUI_MSGTYPE_CLICK) == 0)
	{
	}
	else if (_tcsicmp(msg.sType,DUI_MSGTYPE_SELECTCHANGED) == 0)
	{
	}
	return WindowImplBase::Notify(msg);
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	trayIcon.DeleteTrayIcon();
	bHandled = FALSE;
	PostQuitMessage(0);
	return 0;
}


LRESULT CMainDlg::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CMainDlg::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 关闭窗口，退出程序
	if(uMsg == WM_DESTROY)
	{
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	else if(uMsg == UIMSG_TRAYICON)
	{
		UINT uIconMsg = (UINT)lParam;
		if(uIconMsg == WM_LBUTTONUP) {
			BOOL bVisible = IsWindowVisible(m_hWnd);
			::ShowWindow(m_hWnd, !bVisible ?  SW_SHOW : SW_HIDE);
		}
	}
	bHandled = FALSE;
	return 0;
}
