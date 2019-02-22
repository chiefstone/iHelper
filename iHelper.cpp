#include "stdafx.h"
#include "iHelper.h"
#include <time.h>
#include <TlHelp32.h>
#include <Shlwapi.h>
#include <Psapi.h>
#include <Dbt.h>
#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"Version.lib")
#pragma comment(lib,"Psapi.lib")

HANDLE m_hMutex = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	HRESULT Hr = ::CoInitialize(NULL);
	if(FAILED(Hr)) return 0;
	m_hMutex = CreateMutex(NULL,FALSE,_T("Global\\iHelper"));
	if (m_hMutex==NULL) return 0;

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
	
	if (m_hMutex!=NULL)
	ReleaseMutex(m_hMutex);
	::CoUninitialize();

	return 0;
}

CMainDlg::CMainDlg()
{
	labTitle = NULL;
	labOption1 = NULL;
	editOption1 = NULL;
	btnOption1 = NULL;
	labOption2 = NULL;
	editOption2 = NULL;
	btnOption2 = NULL;
	bScheduleRuning = FALSE;
	hScheduleThread = NULL;
	dwScheduleThreadId = 0;
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
	labOption1 = static_cast<CLabelUI*>(m_Manager.FindControl(_T("labOption1")));
	editOption1 = static_cast<CEditUI*>(m_Manager.FindControl(_T("editOption1")));
	btnOption1 = static_cast<CButtonUI*>(m_Manager.FindControl(_T("btnOption1")));
	labOption2 = static_cast<CLabelUI*>(m_Manager.FindControl(_T("labOption2")));
	editOption2 = static_cast<CEditUI*>(m_Manager.FindControl(_T("editOption2")));
	btnOption2 = static_cast<CButtonUI*>(m_Manager.FindControl(_T("btnOption2")));
	if (labTitle == NULL
		|| labOption1 == NULL || editOption1 == NULL || btnOption1 == NULL
		|| labOption2 == NULL || editOption2 == NULL || btnOption2 == NULL)
	{
		MessageBox(NULL,_T("Loading resources failed!"),_T("iHelper"),MB_OK|MB_ICONERROR);
		return;
	}
	
	TCHAR szBuffer[MAX_PATH];
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szName[_MAX_FNAME];
	GetModuleFileName(NULL, szBuffer, _MAX_PATH);
	sHelper = szBuffer;
	_tsplitpath(szBuffer, szDrive, szDir, szName, NULL);
	sAppPath.Append(szDrive);
	sAppPath.Append(szDir);
	sAppPath.Append(szName);
	sProfile = sAppPath+_T(".ini");
	if(!PathFileExists(sProfile.GetData())){
		WritePrivateProfileString(_T("iHelper"), _T("Option1"), _T("C:\\Windows\\notepad.exe"), sProfile);
	}
	GetPrivateProfileString(_T("iHelper"), _T("Option1"), _T(""), szBuffer, MAX_PATH, sProfile);
	sOption1 = szBuffer;
	GetPrivateProfileString(_T("iHelper"), _T("Option2"), _T(""), szBuffer, MAX_PATH, sProfile);
	sOption2 = szBuffer;

	trayIcon.CreateTrayIcon(GetHWND(),IDI_IHELPER,_T("iHelper"));

	if(GetSystemDefaultLangID() == 0x0804){
		labOption1->SetText(_T("开机时启动的程序(C:\\app.exe)："));
		btnOption1->SetText(_T("浏览..."));
		labOption2->SetText(_T("关闭程序时自动关机(app.exe)："));
		btnOption2->SetText(_T("保存"));
	}
	editOption1->SetText(sOption1);
	editOption2->SetText(sOption2);

	Start();
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
		if (_tcsicmp(sCtrlName,_T("btnOption1")) == 0){
			CDuiString sBuffer = editOption1->GetText();
			if(sBuffer.GetLength() > 0){
				sOption1 = sBuffer;
				WritePrivateProfileString(_T("iHelper"), _T("Option1"), sBuffer, sProfile);
			}
		}
		else if (_tcsicmp(sCtrlName,_T("btnOption2")) == 0){
			CDuiString sBuffer = editOption2->GetText();
			if(sBuffer.GetLength() > 0){
				sOption2 = sBuffer;
				WritePrivateProfileString(_T("iHelper"), _T("Option2"), sBuffer, sProfile);
			}
		}
	}
	return WindowImplBase::Notify(msg);
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	Stop();
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
			if(IsWindowVisible(m_hWnd)){
				::ShowWindow(m_hWnd, SW_HIDE);
			}else{
				::ShowWindow(m_hWnd, SW_SHOW);
				::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
		}
	}
	bHandled = FALSE;
	return 0;
}

void CMainDlg::CheckOption1()
{
	PROCESSENTRY32 pe32;
	bool bFindTarget = false;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap != INVALID_HANDLE_VALUE && Process32First(hProcessSnap,&pe32))
	{
		do
		{
			if(pe32.th32ProcessID > 0)
			{
				if (sOption2.CompareNoCase(pe32.szExeFile)==0)
				{
					bFindTarget = true;
				}
			}
		}
		while(Process32Next(hProcessSnap,&pe32));
	}
	CloseHandle(hProcessSnap);

	if(!bFindTarget){
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		DWORD dwProcessId = 0;
		memset(&si,0,sizeof(si));
		memset(&pi,0,sizeof(pi));
		si.cb = sizeof(si);
		si.wShowWindow = SW_NORMAL;
		CreateProcess(NULL, (LPWSTR)(LPCTSTR)sOption1.GetData(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	}
}

void CMainDlg::CheckOption2()
{
	PROCESSENTRY32 pe32;
	bool bFindTarget = false;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap != INVALID_HANDLE_VALUE && Process32First(hProcessSnap,&pe32))
	{
		do
		{
			if(pe32.th32ProcessID > 0)
			{
				if (sOption2.CompareNoCase(pe32.szExeFile)==0)
				{
					bFindTarget = true;
				}
			}
		}
		while(Process32Next(hProcessSnap,&pe32));
	}
	CloseHandle(hProcessSnap);

	if(!bFindTarget){
		HANDLE hToken;
		TOKEN_PRIVILEGES tkp;
		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(GetVersionEx( &osvi ) != 0){
			if(OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken ) ){
				LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
				tkp.PrivilegeCount = 1;
				tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
				AdjustTokenPrivileges( hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL,0);
				ExitWindowsEx(EWX_SHUTDOWN|EWX_FORCEIFHUNG, 0);
			}
		}
	}
}

void CMainDlg::GuardStartup()
{
	HKEY hKey;
	DWORD dwType = REG_SZ;
	LPTSTR lpKey = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	LONG result = RegOpenKeyEx(HKEY_CURRENT_USER,lpKey,0,KEY_ALL_ACCESS,&hKey);
	if (result != ERROR_SUCCESS)
	{
		//Debug(_T("RegOpenKeyEx %s Failure."),lpKey);
		return;
	}

	DWORD dwLength = sHelper.GetLength()*sizeof(TCHAR);
	DWORD dwData = dwLength+1;
	LPBYTE lpData = new BYTE[dwData];
	memset(lpData,0,dwData);
	result = RegQueryValueEx(hKey,_T("GWDaemon"),0,&dwType,lpData,&dwData);
	if ((result != ERROR_SUCCESS)|| (dwData != dwLength+1))
	{
		//如果键不存在或者键长度与目前的值长度不匹配则添加新键
		RegSetValueEx(hKey, _T("iHelper"), 0, REG_SZ, (LPBYTE)sHelper.GetData(), sHelper.GetLength()*sizeof(TCHAR));
	}
	else
	{
		//如果键的内容与当前值不匹配同样进行更改
		if (memcmp(lpData,sHelper.GetData(),dwLength)!=0)
		{
			RegSetValueEx(hKey, _T("iHelper"), 0, REG_SZ, (LPBYTE)sHelper.GetData(), sHelper.GetLength()*sizeof(TCHAR));
		}
	}
	delete[] lpData;
	RegCloseKey (hKey);
}

void CMainDlg::Start()
{
	GuardStartup();
	if (sOption1.GetLength()>0){
		CheckOption1();
	}
	bScheduleRuning = TRUE;
	hScheduleThread = CreateThread(NULL,0,&CMainDlg::ScheduleThread, this,  0,&dwScheduleThreadId);
}

void CMainDlg::Stop()
{
	if(bScheduleRuning){
		bScheduleRuning = FALSE;
		WaitForSingleObject(hScheduleThread, 10000);
	}
	CloseHandle(hScheduleThread);
	hScheduleThread = NULL;
	//PostThreadMessage(dwThreadId, WM_QUIT, 0, 0);
}

DWORD WINAPI CMainDlg::ScheduleThread(LPVOID lpParameter)
{
	BOOL bRet = FALSE;
	time_t tLastProtect = 0;
	time_t tLastUpgrade = 0;
	CMainDlg * pThis = (CMainDlg *)lpParameter;
	if(pThis==NULL)
		return 0;

	while(pThis->bScheduleRuning)
	{
		time_t tNow = time(NULL);

		//保护应用运行 10秒
		if(pThis->sOption2.GetLength() > 0 && difftime(tNow,tLastProtect) > 10)
		{
			pThis->CheckOption2();
			tLastProtect = tNow;
		}
		Sleep(1000);
	}

	pThis->Stop();

	return 0;
}
