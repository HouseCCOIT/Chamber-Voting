
// JaLCDDlg.h : header file
//

#pragma once


// CJaLCDDlg dialog
class CJaLCDDlg : public CDialogEx
{
// Construction
public:
	CJaLCDDlg(CWnd* pParent = NULL);	// standard constructor
   HANDLE OpenCOMPort(CString comPort);
   BOOLEAN SendDisplayString(char *dispStr);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JALCD_DIALOG };
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
   CString COMPort5;
   CString COMPort6;
   HANDLE  comHandle5;
   HANDLE  comHandle6;

   afx_msg void OnBnClickedButtonStartLedtest();
   afx_msg void OnBnClickedButtonYea();
   afx_msg void OnBnClickedButtonNay();
   afx_msg void OnBnClickedButtonReset();
   afx_msg void OnBnClickedButtonOn();
   afx_msg void OnBnClickedButtonOff();
   afx_msg void OnBnClickedButtonOn2();
};
