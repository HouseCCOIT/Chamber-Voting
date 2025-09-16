// JaMBDlg.h : header file
//

#if !defined(AFX_JAMBDLG_H__A0F351D9_5088_11D2_9F70_00104B9A2A27__INCLUDED_)
#define AFX_JAMBDLG_H__A0F351D9_5088_11D2_9F70_00104B9A2A27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "JaDisplay.h"
#include "JaStatic.h"
#include "ClientSocket.h"
#include "VM_Sock.h"

/////////////////////////////////////////////////////////////////////////////
// CJaMBDlg dialog

extern BOOL m_JA_MessageBoardActive;

class CJaMBDlg : public CDialog
{
   // Construction
public:
   bool LoadMBOptions();
   int m_DisplayLength;
   CJaMBDlg(CWnd* pParent = NULL); // standard constructor

// Dialog Data
    //{{AFX_DATA(CJaMBDlg)
   enum { IDD = IDD_JAMB_DIALOG };
   CComboBox	m_ComboWatermark;
   CComboBox	m_ComboFontSize;
   CEdit	      m_Edit8;
   CEdit	      m_Edit7;
   CEdit	      m_Edit6;
   CEdit	      m_Edit5;
   CEdit	      m_Edit4;
   CEdit	      m_Edit3;
   CEdit	      m_Edit2;
   CEdit	      m_Edit1;
   CComboBox	m_ComboFont;
   CComboBox	m_ComboOrders;
   CComboBox	m_ComboActions;
   CComboBox	m_ComboFiles;
   CEdit	      m_EditNumber;
   CCheckListBox	m_LockList;
   BOOL        m_UpdateConsentCalendar;
   BOOL        m_UpdateCalendar;
   BOOL        m_SwitchingDisplays;
   CString     m_OrdersOfBusiness;
   CString     m_FilesResolutions;
   UINT        m_FileNumber;
   CString     m_Actions;
   int         m_radioSelected;
   int         m_ActiveDisplay;
   int		   m_RadioSummary;
   int		   m_FontSize;
   //}}AFX_DATA

   CString  m_DisplayLine[2][8];

   CString m_OB[2];
   CString m_FR[2];
   CString m_FI[2];
   UINT    m_FN[2];
   CString m_AC[2];

   void ConnectToVM();
   CClientSocket m_Client;
   CString	m_ServerIPAddress;
   void SendVMMessage(SockPacket* packet);

   //{{AFX_VIRTUAL(CJaMBDlg)


protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   HICON m_hIcon;

   // Generated message map functions
   //{{AFX_MSG(CJaMBDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
   afx_msg void OnPaint();
   afx_msg HCURSOR OnQueryDragIcon();
   afx_msg void OnSelchangeComboMbOrderOfBusiness();
   afx_msg void OnSelchangeComboMbFilesResolutions();
   afx_msg void OnSelchangeComboMbActions();
   afx_msg void OnChangeEditMbNumber();
   afx_msg void OnDeltaposSpinMbNumber(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnRadioMbDisplay1();
   afx_msg void OnRadioMbDisplay2();
   afx_msg void OnKillfocusEditMbEdit1();
   afx_msg void OnKillfocusEditMbEdit2();
   afx_msg void OnKillfocusEditMbEdit3();
   afx_msg void OnKillfocusEditMbEdit4();
   afx_msg void OnKillfocusEditMbEdit5();
   afx_msg void OnKillfocusEditMbEdit6();
   afx_msg void OnKillfocusEditMbEdit7();
   afx_msg void OnKillfocusEditMbEdit8();
   afx_msg void OnCheckConsentCalendar();
   afx_msg void OnCheckCalendar();
   afx_msg void OnSelchangeComboMbConsentCalendar();
   afx_msg void OnSelchangeComboMbCalendar();
   afx_msg void OnButtonTransmitDisplay();
   afx_msg void OnButtonAddConsentCalendar();
   afx_msg void OnButtonAddCalendar();
   afx_msg void OnButtonClearConsentCalendar();
   afx_msg void OnButtonClearCalendar();
   afx_msg void OnButtonClear();
   virtual void OnCancel();
   afx_msg void OnRadioLockSummary();
   afx_msg void OnRadioLockFull();
   afx_msg void OnButtonClearRemotes();
   afx_msg void OnClose();
   virtual void OnOK();
   afx_msg void OnDropdownComboMbFilesResolutions();
   afx_msg void OnSelchangeComboMbFont();
   afx_msg void OnSelchangeComboMbFontsize();
   afx_msg void OnSelchangeComboMbWatermark();
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

   void UpdateDisplay();
   void SwitchDisplay();

   FILETIME JaGetFileTime(CString fName);
   BOOL JaSetFileToCurrentTime(CString fName);
   BOOL JaCallInFileChanged();
   BOOL JaRemoteFileChanged();
   BOOL JaLockFileChanged();
   void JaReadLocks();
   void JaReadRemotes();
   void JaReadCallIns();
   void EnableDisableLists();
   void JaWriteLocks(CString lockFilename);
   void JaWriteRemotes(CString remoteFilename);
   void JaWriteCallIns(CString callInFilename);
   void JaWriteAllFiles();

private:
   void StoreMBOptions();
   CString m_Watermark;
   int m_FontSizePoints;

   enum {
      FontSizeUnknown,
      FontSizeSmall,
      FontSizeMedium,
      FontSizeLarge
   } FONTSIZE;

   CFont m_MyFont;
   CString m_FontName;
   //JaResults *pDisplayDlg;
public:
   afx_msg void OnButtonPrintLocks();
   afx_msg void OnBnClickedCheckNoWeb();
   CButton m_CheckNoWeb;
   CCheckListBox m_RemoteList;
   CCheckListBox m_CallInList;
   afx_msg void OnSelchangeLockList();
   afx_msg void OnCheckBoxChangeLockList();
   afx_msg void OnSelchangeRemoteList();
   afx_msg void OnCheckBoxChangeRemoteList();
   afx_msg void OnRadioRemoteFull();
   afx_msg void OnRadioRemoteSummary();
   afx_msg void OnButtonPrintRemotes();
   afx_msg void OnSelchangeCallinList();
   afx_msg void OnCheckBoxChangeCallinList();
   afx_msg void OnRadioCallinFull();
   afx_msg void OnRadioCallinSummary();
   afx_msg void OnButtonPrintCallins();
   CButton m_LocksAll;
   CButton m_RemotesAll;
   CButton m_CallInsAll;
   afx_msg void OnBnClickedCheckLocksAll();
   afx_msg void OnBnClickedCheckRemotesAll();
   afx_msg void OnBnClickedCheckCallinsAll();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   afx_msg void OnDblclkCallinList();
   afx_msg void OnDblclkRemoteList();
   afx_msg void OnDblclkLockList();
   CButton m_RadioRemoteFull;
   CButton m_RadioCallinFull;
   CButton m_RadioLocksFull;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JAMBDLG_H__A0F351D9_5088_11D2_9F70_00104B9A2A27__INCLUDED_)
