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

protected:
	void Notify(TNotifyUI& msg);

private:
	CLabelUI* labTitle;
	CRichEditUI* editUpper;
	CRichEditUI* editLower;
	CRichEditUI* editNumber;
	CRichEditUI* editSymbol;
	CCheckBoxUI* chkUpper;
	CCheckBoxUI* chkLower;
	CCheckBoxUI* chkNumber;
	CCheckBoxUI* chkSymbol;
	CLabelUI* labLength;
	CRichEditUI* editLength;
	CLabelUI* labCount;
	CRichEditUI* editCount;
	CRichEditUI* editPassword;
	CButtonUI* btnGenerator;
};

#endif