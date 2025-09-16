// JaVideoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JaVideo.h"
#include "JaVideoDlg.h"
#include "videothread.h"
#include "afxdialogex.h"
#include "house.h"

#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString JaVoteIni = "V:\\JaVote.Ini";
CString JaVoteDat = "V:\\JaVote.dat";
CString JaDispDat = "V:\\JaDisp.dat";

extern LEX m_JA_MemberSeatingList[MAX_SEATS];
extern unsigned short boardMap[MAX_SEATS];

extern BOOL m_TestMode;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
   CAboutDlg();

   // Dialog Data
#ifdef AFX_DESIGN_TIME
   enum { IDD = IDD_ABOUTBOX };
#endif

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

                                                       // Implementation
protected:
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CJaVideoDlg dialog



CJaVideoDlg::CJaVideoDlg(CWnd* pParent /*=NULL*/)
   : CDialogEx(IDD_JAVIDEO_DIALOG, pParent)
{
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJaVideoDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(JaVideoDlg)
   DDX_Control(pDX, IDC_JD_STATIC12, m_JD_Static12);
   DDX_Control(pDX, IDC_JD_STATIC11, m_JD_Static11);
   DDX_Control(pDX, IDC_JD_STATIC10, m_JD_Static10);
   DDX_Control(pDX, IDC_JD_STATIC9, m_JD_Static9);
   DDX_Control(pDX, IDC_JD_STATIC8, m_JD_Static8);
   DDX_Control(pDX, IDC_JD_STATIC7, m_JD_Static7);
   DDX_Control(pDX, IDC_JD_STATIC6, m_JD_Static6);
   DDX_Control(pDX, IDC_JD_STATIC5, m_JD_Static5);
   DDX_Control(pDX, IDC_JD_STATIC3, m_JD_Static3);
   DDX_Control(pDX, IDC_JD_STATIC4, m_JD_Static4);
   DDX_Control(pDX, IDC_JD_STATIC2, m_JD_Static2);
   DDX_Control(pDX, IDC_JD_STATIC1, m_JD_Static1);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CJaVideoDlg, CDialogEx)
   /*ON_WM_SYSCOMMAND()*/
   ON_WM_PAINT()
   ON_WM_TIMER()
   ON_WM_ERASEBKGND()
   /*ON_WM_QUERYDRAGICON()*/
END_MESSAGE_MAP()


// CJaVideoDlg message handlers

#define MIN_X 0
#define MIN_Y 50
#define MAX_X 1920 // 1280 //640
#define MAX_Y 1080 // 720  //480
#define MAX_FIELD_X MAX_X
#define MAX_FIELD_Y ((MAX_Y - MIN_Y) / 8)
#define MAX_RESULT_X (MAX_X / 4)

BOOL CJaVideoDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();

   ShowCursor(false);

   m_Watermark = "V:\\bitmaps\\ColorSeal.bmp";
   m_BackColor = TRANS_BACK;

   SetWaterMark(m_Watermark);
   ShowWindow(SW_SHOW);

   //LoadDispOptions();

   SetWindowPos(&CWnd::wndTop, 0, 0, MAX_X, MAX_Y, 0); // SWP_NOMOVE | SWP_NOSIZE);
   MoveWindow(0, 0, MAX_X, MAX_Y);
   Invalidate();

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);			// Set big icon
   SetIcon(m_hIcon, FALSE);		// Set small icon

   if (m_TestMode) {
      //m_ServerIPAddress = "127.0.0.1";
      m_ServerIPAddress = "192.168.10.125";
   }
   else {
      m_ServerIPAddress = "192.168.10.2";
   }

   JaLoadParametersAndSeating();

   pResultsDlg = new JaResults();
   pResultsDlg->Create(IDD_JADISPLAY_DIALOG);

   pRealTimeDlg = new JaRT();
   pRealTimeDlg->Create(IDD_JART_DIALOG);

   // Display the Message Board until the vote is open
   pResultsDlg->LoadDispOptions();
   pResultsDlg->SetWindowPos(&CWnd::wndBottom, ::GetSystemMetrics(SM_CXSCREEN), 0, MAX_X, MAX_Y, 0); // SWP_NOMOVE | SWP_NOSIZE);
   int nResponse = pResultsDlg->ShowWindow(SW_SHOW);

   pRealTimeDlg->SetWindowPos(&CWnd::wndTop, ::GetSystemMetrics(SM_CXSCREEN), 0, MAX_X, MAX_Y, 0); // SWP_NOMOVE | SWP_NOSIZE);
   nResponse = pRealTimeDlg->ShowWindow(SW_SHOW);

   ConnectToVM();   // Open socket to Voting Machine app

   if (m_TestMode) {
      ShowRealTime(); // Enable to debug RealTime
   }

   JaReadCurrentVote(JaVoteDat);

   // Specify the timer interval for checking vote file.
   UINT    nElapse = 2000;
   SetTimer(1, nElapse, NULL);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CJaVideoDlg::ConnectToVM()
{
   if (AfxSocketInit() == FALSE) {
      AfxMessageBox("Failed to Initialize Sockets");
      return;
   }

   if (m_Client.Create() == FALSE) {
      MessageBox("Failed to Create Socket");
      return;
   }

   if (m_Client.Connect(m_ServerIPAddress, 1002) == FALSE) {
      int error = GetLastError();
      MessageBox("Failed to Connect");
      return;
   }

   m_Client.pDlg = this;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJaVideoDlg::OnPaint()
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialogEx::OnPaint();
   }
}

void CJaVideoDlg::SetMemberVote(int member, int seat, int vote)
{
   seat--;

   if (pRealTimeDlg) {
      pRealTimeDlg->SetMemberVote(member, vote);
   }
}

void CJaVideoDlg::ShowResults()
{
   if (pResultsDlg) {
      pResultsDlg->LoadDispOptions();
   }

   pResultsDlg->DisplayResults(TRUE);

   if (pRealTimeDlg) {
      //BlockInput(false); // Restore keyboard and mouse input
      pRealTimeDlg->FadeOut();
   }
}


void CJaVideoDlg::ShowRealTime()
{
   pResultsDlg->DisplayResults(FALSE);

   if (pRealTimeDlg) {
      //BlockInput(true); // Block all keyboard and mouse input while Real Time is active (Doesn't work)
      pRealTimeDlg->FadeIn();
   }
}

void CJaVideoDlg::UpdateCurrentVote(SockPacket votePacket)
{
   for (int i = 0; i < MAX_SEATS; i++) {
      if ((i != 68) && (i != 90)) { // 69 & 91 excluded from voting
         m_JA_MemberSeatingList[i].vote = votePacket.dataByte[i];
         pRealTimeDlg->SetMemberVote(boardMap[i], m_JA_MemberSeatingList[i].vote);
      }
   }
}

void CJaVideoDlg::Shutdown()
{
   OnCancel();

   delete pResultsDlg;

   delete pRealTimeDlg;
}

void CJaVideoDlg::OnTimer(UINT_PTR nIDEvent)
{
   //if (JaVoteFileChanged()) {
   //   JaReadCurrentVote(JaVoteDat);
   //}

   //for (int i = 0; i < MAX_SEATS; i++) {
   //   if (pRealTimeDlg) {
   //      pRealTimeDlg->SetMemberVote(i, m_JA_MemberSeatingList[boardMap[i]].vote);
   //   }
   //}

   if (JaResultsFileChanged()) {
      LoadDispOptions();
   }
   
   CDialog::OnTimer(nIDEvent);
}

void CJaVideoDlg::OnCancel()
{
   CDialog::OnCancel();

   TRACE("OnCancel");

   delete pResultsDlg;

   delete pRealTimeDlg;

   TRACE("Before Exit");

   exit(0);
}

void CJaVideoDlg::SetNPHandle(HANDLE NP)
{
	NPHandle = NP;
}

void CJaVideoDlg::JaReadCurrentVote(CString voteFilename)
{
   FILE *voteFile;
   int count;
   unsigned char votes[MAX_VOTE_ARRAY];

   TRACE("Before open");

   voteFile = fopen(voteFilename, "rb");
   if (voteFile == NULL) {
      TRACE("No Vote file!");
      return;
   }

   count = fread(votes, 1, sizeof(votes), voteFile);

   fclose(voteFile);

   if (count != sizeof(votes)) {
      TRACE("Unable to read Vote file!");
      return;
   }

   for (int i = 0; i < MAX_SEATS; i++) {
      if ((i != 68) && (i != 90)) { // 69 & 91 excluded from voting
         m_JA_MemberSeatingList[i].vote = votes[i];
         pRealTimeDlg->SetMemberVote(boardMap[i], m_JA_MemberSeatingList[i].vote);
      }
   }
}

FILETIME CJaVideoDlg::JaGetFileTime(CString fName)
{
   HANDLE hFile = NULL;
   FILETIME ftCreate, ftAccess, ftWrite;

   ftWrite.dwHighDateTime = 0;
   ftWrite.dwLowDateTime = 0;

   hFile = CreateFile(fName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

   if (hFile != INVALID_HANDLE_VALUE) {
      // Retrieve the file times for the file.
      if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
         ftWrite.dwHighDateTime = 0;
         ftWrite.dwLowDateTime = 0;
      }

      CloseHandle(hFile);
   }

   return ftWrite;
}

BOOL CJaVideoDlg::JaVoteFileChanged()
{
   BOOL retVal = false;

   static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(JaVoteDat);

   if ((ftWrite.dwLowDateTime != 0) && (ftWrite.dwLowDateTime != ftLastWrite.dwLowDateTime)) {
      ftLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

BOOL CJaVideoDlg::JaResultsFileChanged()
{
   BOOL retVal = false;

   static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(JaDispDat);

   if ((ftWrite.dwLowDateTime != 0) && (ftWrite.dwLowDateTime != ftLastWrite.dwLowDateTime)) {
      ftLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}


BOOL CJaVideoDlg::LoadDispOptions()
{
   FILE *dispFile;

   int i;
   char tmpStr[256];

   if ((dispFile = fopen(JaDispDat, "r+")) == NULL) {
      return FALSE;
   }

   ClearAll();

   fgets(tmpStr, 256, dispFile);
   tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
   SetFont(tmpStr);

   fgets(tmpStr, 256, dispFile);
   tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
   SetFontSize(atoi(tmpStr));

   for (i = 0; i < 8; i++) {
      fgets(tmpStr, 256, dispFile);
      tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
      m_DisplayLine[i] = tmpStr;
   }

   for (i = 0; i < 4; i++) {
      fgets(tmpStr, 256, dispFile);
      tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
      m_ResultLine[i] = tmpStr;
   }

   fclose(dispFile);

   UpdateDisplay();

   return TRUE;
}

void CJaVideoDlg::SetFont(CString fontName)
{
   m_FontName = fontName;

   m_MyFont.DeleteObject();
   m_MyFont.CreatePointFont(m_FontSizePoints, m_FontName);
}

void CJaVideoDlg::SetFontSize(int fontSize)
{
   m_FontSize = fontSize;

   switch (fontSize) {
   case FontSizeLarge:
      m_FontSizePoints = 825; // 450;
      break;

   case FontSizeMedium:
      m_FontSizePoints = 800; // 375;
      break;

   case FontSizeSmall:
   default:
      m_FontSizePoints = 700; // 300;
      break;
   }

   m_MyFont.DeleteObject();
   m_MyFont.CreatePointFont(m_FontSizePoints, m_FontName);
}

void CJaVideoDlg::UpdateDisplay()
{
   // MoveWindow(::GetSystemMetrics(SM_CXSCREEN), 0, MAX_X, MAX_Y);
   //	MoveWindow(::GetSystemMetrics(SM_CXSCREEN) - 640, 0, MAX_X, MAX_Y);
   MoveWindow(0, 0, MAX_X, MAX_Y);

   COLORREF TColor = RGB(255, 255, 255);
   m_TextColor = TColor;

   m_JD_Static1.SetTextColor(m_TextColor);
   m_JD_Static1.SetBackColor(m_BackColor);
   m_JD_Static1.SetFont(&m_MyFont);
   m_JD_Static1.SetWindowText(m_DisplayLine[0]);
   m_JD_Static1.MoveWindow(MIN_X, MIN_Y, MAX_X, MAX_FIELD_Y);

   m_JD_Static2.SetTextColor(m_TextColor);
   m_JD_Static2.SetBackColor(m_BackColor);
   m_JD_Static2.SetFont(&m_MyFont);
   m_JD_Static2.SetWindowText(m_DisplayLine[1]);
   m_JD_Static2.MoveWindow(MIN_X, MAX_FIELD_Y + MIN_Y, MAX_X, MAX_FIELD_Y);

   m_JD_Static3.SetTextColor(m_TextColor);
   m_JD_Static3.SetBackColor(m_BackColor);
   m_JD_Static3.SetFont(&m_MyFont);
   m_JD_Static3.SetWindowText(m_DisplayLine[2]);
   m_JD_Static3.MoveWindow(0, MAX_FIELD_Y * 2 + MIN_Y, MAX_X, MAX_FIELD_Y);

   m_JD_Static4.SetTextColor(m_TextColor);
   m_JD_Static4.SetBackColor(m_BackColor);
   m_JD_Static4.SetFont(&m_MyFont);
   m_JD_Static4.SetWindowText(m_DisplayLine[3]);
   m_JD_Static4.MoveWindow(MIN_X, MAX_FIELD_Y * 3 + MIN_Y, MAX_X, MAX_FIELD_Y);

   m_JD_Static5.SetTextColor(m_TextColor);
   m_JD_Static5.SetBackColor(m_BackColor);
   m_JD_Static5.SetFont(&m_MyFont);
   m_JD_Static5.SetWindowText(m_DisplayLine[4]);
   m_JD_Static5.MoveWindow(MIN_X, MAX_FIELD_Y * 4 + MIN_Y, MAX_X, MAX_FIELD_Y);

   m_JD_Static6.SetTextColor(m_TextColor);
   m_JD_Static6.SetBackColor(m_BackColor);
   m_JD_Static6.SetFont(&m_MyFont);
   m_JD_Static6.SetWindowText(m_DisplayLine[5]);
   m_JD_Static6.MoveWindow(MIN_X, MAX_FIELD_Y * 5 + MIN_Y, MAX_X, MAX_FIELD_Y);

   m_JD_Static7.SetTextColor(m_TextColor);
   m_JD_Static7.SetBackColor(m_BackColor);
   m_JD_Static7.SetFont(&m_MyFont);
   m_JD_Static7.SetWindowText(m_DisplayLine[6]);
   m_JD_Static7.MoveWindow(MIN_X, MAX_FIELD_Y * 6 + MIN_Y, MAX_X, MAX_FIELD_Y);

   m_JD_Static8.SetTextColor(m_TextColor);
   m_JD_Static8.SetBackColor(m_BackColor);
   m_JD_Static8.SetFont(&m_MyFont);
   m_JD_Static8.SetWindowText(m_DisplayLine[7]);
   m_JD_Static8.MoveWindow(MIN_X, MAX_FIELD_Y * 7 + MIN_Y, MAX_X, MAX_FIELD_Y);

   m_JD_Static9.SetTextColor(m_TextColor);
   m_JD_Static9.SetBackColor(m_BackColor);
   m_JD_Static9.SetFont(&m_MyFont);
   m_JD_Static9.SetWindowText(m_ResultLine[0]);
   m_JD_Static9.MoveWindow(MIN_X, MAX_FIELD_Y * 7 + MIN_Y, MAX_RESULT_X, MAX_FIELD_Y);

   m_JD_Static10.SetTextColor(m_TextColor);
   m_JD_Static10.SetBackColor(m_BackColor);
   m_JD_Static10.SetFont(&m_MyFont);
   m_JD_Static10.SetWindowText(m_ResultLine[1]);
   m_JD_Static10.MoveWindow(MAX_RESULT_X, MAX_FIELD_Y * 7 + MIN_Y, MAX_RESULT_X, MAX_FIELD_Y);

   m_JD_Static11.SetTextColor(m_TextColor);
   m_JD_Static11.SetBackColor(m_BackColor);
   m_JD_Static11.SetFont(&m_MyFont);
   m_JD_Static11.SetWindowText(m_ResultLine[2]);
   m_JD_Static11.MoveWindow(MAX_RESULT_X * 2, MAX_FIELD_Y * 7 + MIN_Y, MAX_RESULT_X, MAX_FIELD_Y);

   m_JD_Static12.SetTextColor(m_TextColor);
   m_JD_Static12.SetBackColor(m_BackColor);
   m_JD_Static12.SetFont(&m_MyFont);
   m_JD_Static12.SetWindowText(m_ResultLine[3]);
   m_JD_Static12.MoveWindow(MAX_RESULT_X * 3, MAX_FIELD_Y * 7 + MIN_Y, MAX_RESULT_X, MAX_FIELD_Y);

   Invalidate();

   //	StoreDispOptions();
}

void CJaVideoDlg::ClearSix()
{
   for (int i = 0; i < 7; i++) {
      m_DisplayLine[i] = "";
   }

   COLORREF BColor = RGB(0, 0, 0);
   m_BackColor = BColor;

   UpdateDisplay();

   //m_BackColor = TRANS_BACK;

   Invalidate();
}

void CJaVideoDlg::ClearAll()
{
   int i;

   for (i = 0; i < 8; i++) {
      m_DisplayLine[i] = "";
   }

   for (i = 0; i < 4; i++) {
      m_ResultLine[i] = "";
   }

   COLORREF BColor = RGB(0, 0, 0);
   m_BackColor = BColor;

   UpdateDisplay();

   //m_BackColor = TRANS_BACK;

   Invalidate();
}

void CJaVideoDlg::SetLine(int i, char *text)
{
   m_DisplayLine[i] = text;

   m_BackColor = TRANS_BACK;
}

void CJaVideoDlg::SetResultLine(char *text1, char *text2, char *text3, char *text4)
{
   m_ResultLine[0] = text1;
   m_ResultLine[1] = text2;
   m_ResultLine[2] = text3;
   m_ResultLine[3] = text4;

   //m_BackColor = TRANS_BACK;
}

void CJaVideoDlg::ClearResults()
{
   int i;

   for (i = 0; i < 4; i++) {
      m_ResultLine[i] = "     ";
   }

   COLORREF BColor = RGB(0, 0, 0);
   m_BackColor = BColor;

   UpdateDisplay();

   //m_BackColor = TRANS_BACK;

   Invalidate();
}

BOOL CJaVideoDlg::OnEraseBkgnd(CDC* pDC)
{
   CRect rc;

   //GetClientRect(rc);

   //m_bmpBackground.Load(m_Watermark);
   //m_bmpBackground.DrawDIB(pDC, MIN_X, MIN_Y, rc.Width(), rc.Height());
   
   CRect rect;
   GetClientRect(&rect);
   CBrush myBrush(RGB(0, 0, 0));    // dialog background color
   CBrush *pOld = pDC->SelectObject(&myBrush);
   BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
   pDC->SelectObject(pOld);    // restore old brush
   return bRes;                       //TBD - Huh?
   //return CDialog::OnEraseBkgnd(pDC);


   //return TRUE; // tell Windows we handled it
}


void CJaVideoDlg::SetWaterMark(CString watermark)
{
   m_Watermark = "V:\\bitmaps\\" + watermark;
}