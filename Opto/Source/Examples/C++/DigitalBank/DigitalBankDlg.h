// DigitalBankDlg.h : header file
//
//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#include "CancelDlg.h"
#include "O22SIOMM.h"

#if !defined(AFX_DIGITALBANKDLG_H__B1192D09_FBE7_11D2_9707_080009ABC65D__INCLUDED_)
#define AFX_DIGITALBANKDLG_H__B1192D09_FBE7_11D2_9707_080009ABC65D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ID_OPEN_TIMER 1
#define ID_SCAN_TIMER 2

/////////////////////////////////////////////////////////////////////////////
// CDigitalBankDlg dialog

class CDigitalBankDlg : public CDialog
{
// Construction
public:
	CDigitalBankDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDigitalBankDlg)
	enum { IDD = IDD_DIGITALBANK_DIALOG };
	CEdit	m_StatusEdit;
	CButton	m_WriteStatesBtn;
	CButton	m_WriteOnBtn;
	CButton	m_WriteOffBtn;
	CEdit	m_WriteStatesEdit;
	CEdit	m_WriteOnEdit;
	CEdit	m_WriteOffEdit;
	CButton	m_ClearOffLatchBtn;
	CButton	m_ClearOnLatchBtn;
	CEdit	m_OffLatchEdit;
	CEdit	m_OnLatchEdit;
	CEdit	m_PointsStatesEdit;
	CEdit	m_PortEdit;
	CEdit	m_IpAddressEdit;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDigitalBankDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
  UINT  m_nOpenTimerId;
  UINT  m_nScanTimerId;
  CCancelDlg m_dlgCancel;
  CFont m_CourierFont;
  O22SnapIoMemMap m_Brain;

  void HandleResult(int nResult);

  void StartScanning();
  void StopScanning();

	// Generated message map functions
	//{{AFX_MSG(CDigitalBankDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConnectBtn();
	afx_msg void OnDisconnectBtn();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClearOnLatchBtn();
	afx_msg void OnClearOffLatchBtn();
	afx_msg void OnWriteOnBtn();
	afx_msg void OnWriteOffBtn();
	afx_msg void OnWriteStatesBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIGITALBANKDLG_H__B1192D09_FBE7_11D2_9707_080009ABC65D__INCLUDED_)
