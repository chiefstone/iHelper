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

	TCHAR GetRandChar(CDuiString& sTemplate, UINT nIncrement=0);

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
	CTrayIcon trayIcon;
};

#endif