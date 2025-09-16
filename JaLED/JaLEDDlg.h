
// JaLEDDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CJaLEDDlg dialog
class CJaLEDDlg : public CDialogEx
{
// Construction
public:
	CJaLEDDlg(CWnd* pParent = NULL);	// standard constructor
   HANDLE OpenCOMPort(CString comPort);

   BOOLEAN DisableScreenSaver();
   BOOLEAN ClearScreen();
   BOOLEAN MoveTo(unsigned char x, unsigned char y);
   BOOLEAN SetTextColor(short color);
   BOOLEAN SetTextSize(unsigned char size);
   BOOLEAN SetTextFontId(short id);
   BOOLEAN SendDisplayString(char *dispStr);

   BOOLEAN DisableScreenSaver2();
   BOOLEAN ClearScreen2();
   BOOLEAN MoveTo2(unsigned char x, unsigned char y);
   BOOLEAN SetTextColor2(short color);
   BOOLEAN SetTextSize2(unsigned char size);
   BOOLEAN SetTextFontId2(short id);
   BOOLEAN ScreenMode2(unsigned char mode);
   BOOLEAN SendDisplayString2(char *dispStr);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JALED_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedButton1();

   CString COMPort5;
   CString COMPort6;
   HANDLE  comHandle5;
   HANDLE  comHandle6;
   afx_msg void OnBnClickedCancel();
   afx_msg void OnBnClickedOk();
   afx_msg void OnBnClickedButton2();
   afx_msg void OnBnClickedButton3();
   bool Initialize(void);

   CEdit FontID;
   afx_msg void OnBnClickedButton4();
};
