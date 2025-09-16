
// JaVideoDlg.h : header file
//

#include "JaStatic.h"
#include "JaResults.h"
#include "JaRT.h"
#include "ClientSocket.h"
#include "VM_Sock.h"

#pragma once


// CJaVideoDlg dialog
class CJaVideoDlg : public CDialogEx
{
   // Construction
public:
   CJaVideoDlg(CWnd* pParent = NULL);	// standard constructor

   void SetWaterMark(CString watermark);
   void UpdateDisplay();
   void SetMemberVote(int member, int seat, int vote);
   void ClearResults();
   void SetResultLine(char *text1, char *text2, char *text3, char *text4);
   void SetLine(int i, char *text);
   void ClearAll();
   void ClearSix();
   void SetFontSize(int fontSize);
   void SetFont(CString fontName);

   void ConnectToVM();
   CClientSocket m_Client;
   CString	m_ServerIPAddress;

   void ShowResults();
   void ShowRealTime();
   void UpdateCurrentVote(SockPacket votePacket);
   void Shutdown();
   void SetNPHandle(HANDLE NP);
   HANDLE NPHandle;

   // Dialog Data
   //{{AFX_DATA(JaDisplay)
   enum { IDD = IDD_JAVIDEO_DIALOG };
   CJaStatic	m_JD_Static12;
   CJaStatic	m_JD_Static11;
   CJaStatic	m_JD_Static10;
   CJaStatic	m_JD_Static9;
   CJaStatic	m_JD_Static8;
   CJaStatic	m_JD_Static7;
   CJaStatic	m_JD_Static6;
   CJaStatic	m_JD_Static5;
   CJaStatic	m_JD_Static3;
   CJaStatic	m_JD_Static4;
   CJaStatic	m_JD_Static2;
   CJaStatic	m_JD_Static1;
   //}}AFX_DATA

protected:
   virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
   void JaReadCurrentVote(CString voteFilename);
   FILETIME JaGetFileTime(CString fName);
   BOOL JaVoteFileChanged();
   BOOL JaResultsFileChanged();
   BOOL LoadDispOptions();

   // Implementation
protected:
   HICON m_hIcon;

   // Generated message map functions
   virtual BOOL OnInitDialog();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   /*afx_msg void OnSysCommand(UINT nID, LPARAM lParam);*/
   afx_msg void OnPaint();
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   virtual void OnCancel();
   /*afx_msg HCURSOR OnQueryDragIcon();*/
   DECLARE_MESSAGE_MAP()

private:
   CFont m_SFont;

   int m_TextColor;
   CString m_Watermark;
   CDIBitmap m_bmpBackground;
   int m_BackColor;
   CString m_ResultLine[5];
   CString m_DisplayLine[9];
   int m_ActiveDisplay;

   enum {
      FontSizeUnknown,
      FontSizeSmall,
      FontSizeMedium,
      FontSizeLarge
   } FONTSIZE;

   int m_FontSize;
   int m_FontSizePoints;
   CFont m_MyFont;
   CString m_FontName;

   JaResults * pResultsDlg;
   JaRT *pRealTimeDlg;
};
