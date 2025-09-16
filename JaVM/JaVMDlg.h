// JaVMDlg.h : header file
//

#if !defined(AFX_JAVMDLG_H__CD7E4B86_484F_11D2_9F60_00104B9A2A27__INCLUDED_)
#define AFX_JAVMDLG_H__CD7E4B86_484F_11D2_9F60_00104B9A2A27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "JaStatic.h"
#include "VM_Sock.h"
#include "RemoteSocketServer.h"
#include "VideoSocketServer.h"
#include "MBSocketServer.h"
#include "VMSocketServer.h"

/////////////////////////////////////////////////////////////////////////////
// CJaVMDlg dialog

class CJaVMDlg : public CDialog
{
   // Construction
public:
   CJaVMDlg(CWnd* pParent = NULL); // standard constructor
   void SetMemberVote(int Member, int seat, int vote);
   void AddMsg(SockRemotePacket* SockData);
   void SendSockRemoteMsg(SockVMPacket* SockData);
   void SendSockRemoteMsgToRTS(SockRemotePacket* SockPacket);
   void SendCurrentVoteMsg(SockPacket* vmPacket);
   void SendVideoMsg(SockPacket* videoPacket);
   void SendMBMsg(SockPacket* mbPacket);
   void JaReadLocks();
   void JaReadRemotes();
   void JaReadCallIns();

   FILETIME JaGetFileTime(CString fName);
   BOOL JaLockFileChanged();
   BOOL JaRemoteFileChanged();
   BOOL JaCallInFileChanged();

   CRemoteSocketServer* VMSockRemote;
   CMBSocketServer* VMSockMB;
   CVideoSocketServer* VMSockVideo;
   CVMSocketServer* VMSockVote;

   // Dialog Data
   //{{AFX_DATA(CJaVMDlg)
   enum { IDD = IDD_JAVM_DIALOG };
   CButton	m_CheckOpen;
   CJaStatic	m_MemberSeat[150];
   //}}AFX_DATA


   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CJaVMDlg)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
                                                       //}}AFX_VIRTUAL

protected:
   HICON m_hIcon;


   // Generated message map functions
   //{{AFX_MSG(CJaVMDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnPaint();
   virtual void OnOK();
   virtual void OnCancel();
   afx_msg void OnCheckOpen();
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnMove(int x, int y);

   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()

private:
   CFont m_SFont;

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JAVMDLG_H__CD7E4B86_484F_11D2_9F60_00104B9A2A27__INCLUDED_)
