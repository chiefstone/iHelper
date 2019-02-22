#ifndef _IHELPER_H_
#define _IHELPER_H_
#include <windows.h>
#include <objbase.h>
#include <Winnls.h>
#include "resource.h"
#pragma comment(lib,"Kernel32.lib")
#pragma once


class CMainDlg : public WindowImplBase
{
public:
	CMainDlg();
	~CMainDlg();

	void GuardStartup();
	void CheckOption1();
	void CheckOption2();

	int Run();
	void Start();
	void Stop();

public:
	LPCTSTR GetWindowClassName() const;
	virtual void InitWindow();
	virtual void OnFinalMessage(HWND hWnd);
	virtual CDuiString GetSkinFile();
	
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	void Notify(TNotifyUI& msg);

private:
	CLabelUI* labTitle;
	CLabelUI* labOption1;
	CEditUI* editOption1;
	CButtonUI* btnOption1;
	CLabelUI* labOption2;
	CEditUI* editOption2;
	CButtonUI* btnOption2;

	CTrayIcon trayIcon;
	CDuiString sAppPath;		//程序安装目录
	CDuiString sProfile;		//配置文件路径
	CDuiString sHelper;			//守护程序路径
	CDuiString sOption1;		//开机时启动的程序
	CDuiString sOption2;		//关闭程序时自动关机

	BOOL bScheduleRuning;
	HANDLE hScheduleThread;
	DWORD dwScheduleThreadId;
	static DWORD WINAPI ScheduleThread(LPVOID lpParameter);
};

#endif