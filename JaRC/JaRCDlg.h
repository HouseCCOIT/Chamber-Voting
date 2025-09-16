// JaRCDlg.h : header file
//

#if !defined(AFX_JARCDLG_H__BAA9CF1C_26E5_41AD_AF9A_0C12CBD41433__INCLUDED_)
#define AFX_JARCDLG_H__BAA9CF1C_26E5_41AD_AF9A_0C12CBD41433__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CJaRCDlg dialog

class CJaRCDlg : public CDialog
{
// Construction
public:
	BOOLEAN StoreOptions();
	BOOLEAN LoadOptions();
	int m_Input;
	int m_SetID;
	HANDLE m_ComHandle;
	CJaRCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CJaRCDlg)
	enum { IDD = IDD_JARC_DIALOG };
	CSliderCtrl	m_SliderBrightness;
	CSliderCtrl	m_SliderContrast;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJaRCDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CJaRCDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnReleasedcaptureSliderContrast(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSliderBrightness(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioPowerOff();
	afx_msg void OnRadioInputComponent();
	afx_msg void OnRadioPowerOn();
	afx_msg void OnRadioInputRgb();
	afx_msg void OnRadioInputSvideo();
	afx_msg void OnRadioInputVideo();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	HANDLE m_ComHandle2;
	CString m_ComPort2;
	BOOLEAN InitComPort2();
	BOOLEAN GetResponse();
	int m_Power;
	BOOLEAN SendCommand(char cmd2, char setID, char data);
	CString m_ComPort;
	int m_BrightnessLevel;
	int m_ContrastLevel;
	BOOLEAN InitComPort();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JARCDLG_H__BAA9CF1C_26E5_41AD_AF9A_0C12CBD41433__INCLUDED_)
