
// RealTimeViewDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "RealTimeView.h"
#include "RealTimeViewDlg.h"
#include "afxdialogex.h"
#include "house.h"
#include "fileio.h"
#include "alphasort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRTVDlg dialog

CString m_RTFontName = CString("Arial Bold");
CString m_RTFontThinName = CString("Calibri Bold");
int     m_RTFontSize = 220; // 110;220

CString JaVoteIni = "V:\\JaVote.Ini";
CString JaVoteDat = "V:\\JaVote.dat";

LEX m_JA_MemberSeatingList[MAX_SEATS];
unsigned short boardMap[MAX_SEATS];

CRTVDlg::CRTVDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_REALTIMEVIEW_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRTVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, (IDC_STATIC_BORDERTOP), m_BorderTop);
	DDX_Control(pDX, (IDC_STATIC_BORDERBOTTOM), m_BorderBottom);

	for (int i = 0; i < 144; i++) {
		DDX_Control(pDX, (IDC_STATIC_YES1 + i), m_MemberYes[i]);
		DDX_Control(pDX, (IDC_STATIC_NO1 + i), m_MemberNo[i]);
		DDX_Control(pDX, (IDC_STATIC_NAME1 + i), m_MemberName[i]);
	}
   for (int i = 0; i < 5; i++) {
      DDX_Control(pDX, (IDC_STATIC_G1 + i), m_MemberGroupBox[i]);
   }
}

BEGIN_MESSAGE_MAP(CRTVDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
   ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

#define BORDER_X 5
#define BORDER_Y 5
#define MAX_SCREEN_X 1920 // 1280 //640
#define MAX_SCREEN_Y 1080 // 720  //480
#define MIN_X 0
#define MIN_Y 0
#define MAX_X 1920 // 1280 //640
#define MAX_Y 1080 // 720  //480
#define MAX_ROWS 27
#define MAX_COLUMNS 5
#define MAX_FIELD_X (((MAX_X - MIN_X) - (BORDER_X * 2)) / MAX_COLUMNS)
#define MAX_FIELD_Y (((MAX_Y - MIN_Y) - (BORDER_Y * 2)) / MAX_ROWS)
#define MAX_RESULT_X (MAX_X / 4)


extern BOOL m_TestMode;

// CRTVDlg message handlers

BOOL CRTVDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

   if (m_TestMode) {
      m_ServerIPAddress = "192.168.10.125";
      //m_ServerIPAddress = "127.0.0.1";
   }
   else {
      m_ServerIPAddress = "192.168.10.2";
      ShowCursor(false);
   }

   // Try to get the address of the SetLayeredWindowAttributes function. It may not be available.
   HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));

   m_pSetLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes)GetProcAddress(hUser32, "SetLayeredWindowAttributes");

   pResultsViewDlg = new CRVDlg();
   pResultsViewDlg->Create(IDD_RESULTSVIEW_DIALOG);

   // Display the Message Board until the vote is open
   pResultsViewDlg->LoadDispOptions();
   //pResultsViewDlg->SetWindowPos(&CWnd::wndBottom, ::GetSystemMetrics(SM_CXSCREEN), 0, MAX_X, MAX_Y, 0); // SWP_NOMOVE | SWP_NOSIZE);
   pResultsViewDlg->SetWindowPos(&CWnd::wndBottom, 0, 0, MAX_X, MAX_Y, 0);
   int nResponse = pResultsViewDlg->ShowWindow(SW_SHOW);

   //MoveWindow(0, 0, MAX_SCREEN_X, MAX_SCREEN_Y);  // For debugging only

   //SetWindowPos(&CWnd::wndTop, ::GetSystemMetrics(SM_CXSCREEN), 0, MAX_X, MAX_Y, 0); // SWP_NOMOVE | SWP_NOSIZE);
   SetWindowPos(&CWnd::wndTop, 0, 0, MAX_X, MAX_Y, 0); // SWP_NOMOVE | SWP_NOSIZE);
   nResponse = ShowWindow(SW_SHOW);
   
   JaLoadParametersAndSeating();

   COLORREF TColor = RGB(255, 255, 255);
   COLORREF BColor = RGB(0, 0, 0);   // RGB(10, 10, 10);
   COLORREF YColor = RGB(0, 255, 0);
   COLORREF NColor = RGB(255, 0, 0);
   COLORREF GColor = RGB(0xdb, 0x88, 0x20);

   m_TextColor = TColor;
   m_BackColor = BColor;
   m_YesColor = YColor;
   m_NoColor = NColor;
   m_GBColor = GColor;

   m_RTFont.DeleteObject();
   m_RTFont.CreatePointFont(m_RTFontSize, m_RTFontName);
   m_RTFontThin.DeleteObject();
   m_RTFontThin.CreatePointFont(m_RTFontSize, m_RTFontThinName);

   int i, j;

   for (i = 0; i < MAX_COLUMNS; i++) {
      for (j = 0; j < MAX_ROWS; j++) {
         CString tmpStr = CString(m_JA_MemberSeatingList[j + (i * MAX_ROWS)].name);
         tmpStr.MakeUpper();

         // TBD: Remove when representative wakes up.
         CString ZZZ("ZZZ"); // = "ZZZ";
         if (tmpStr.Find(ZZZ) != -1) {
            tmpStr = CString("   ");
         }

         if (j + (i * MAX_ROWS) < 133) {
            m_MemberName[j + (i * MAX_ROWS)].SetTextColor(m_TextColor);
            m_MemberName[j + (i * MAX_ROWS)].SetBackColor(m_BackColor);
            CString space(" ");
            int n = tmpStr.Find(space);
            if ((n > 16) || (n == -1)) {
               m_MemberName[j + (i * MAX_ROWS)].SetFont(&m_RTFontThin);
            }
            else {
               m_MemberName[j + (i * MAX_ROWS)].SetFont(&m_RTFont);
            }
            m_MemberName[j + (i * MAX_ROWS)].SetWindowText(CString("  ") + tmpStr);
            m_MemberName[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (i * MAX_FIELD_X) + BORDER_X, MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 2, MAX_FIELD_X - BORDER_X, MAX_FIELD_Y);
            m_MemberName[j + (i * MAX_ROWS)].ShowWindow(TRUE);

            m_MemberYes[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (((i + 1) * MAX_FIELD_X) - (60 + BORDER_X)), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 3, 20, 20);
            m_MemberYes[j + (i * MAX_ROWS)].EnableWindow(FALSE);

            m_MemberNo[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (((i + 1) * MAX_FIELD_X) - (30 + BORDER_X)), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 3, 20, 20);
            m_MemberNo[j + (i * MAX_ROWS)].EnableWindow(FALSE);
         }
         else {
            // Special case for the Speaker

            int lLen = tmpStr.ReverseFind('.');
            int rLen = (tmpStr.GetLength() - lLen) - 1;

            m_MemberName[j + (i * MAX_ROWS)].SetTextColor(m_TextColor);
            m_MemberName[j + (i * MAX_ROWS)].SetBackColor(m_BackColor);
            m_MemberName[j + (i * MAX_ROWS)].SetFont(&m_RTFont);
            m_MemberName[j + (i * MAX_ROWS)].SetWindowText(CString("  ") + "SPEAKER"); // tmpStr.Left(lLen)); // "SPEAKER");
            m_MemberName[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (i * MAX_FIELD_X) + BORDER_X, MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 2, MAX_FIELD_X - BORDER_X, MAX_FIELD_Y);
            m_MemberName[j + (i * MAX_ROWS)].ShowWindow(TRUE);

            m_MemberYes[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (((i + 1) * MAX_FIELD_X) - (60 + BORDER_X)), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 3, 20, 20);
            m_MemberYes[j + (i * MAX_ROWS)].EnableWindow(FALSE);

            m_MemberNo[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (((i + 1) * MAX_FIELD_X) - (30 + BORDER_X)), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 3, 20, 20);
            m_MemberNo[j + (i * MAX_ROWS)].EnableWindow(FALSE);
            j++;
            m_MemberName[j + (i * MAX_ROWS)].SetTextColor(m_TextColor);
            m_MemberName[j + (i * MAX_ROWS)].SetBackColor(m_BackColor);
            m_MemberName[j + (i * MAX_ROWS)].SetFont(&m_RTFont);
            m_MemberName[j + (i * MAX_ROWS)].SetWindowText(CString("  ") + tmpStr.Right(rLen));
            m_MemberName[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (i * MAX_FIELD_X + BORDER_X), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 2, MAX_FIELD_X - BORDER_X, MAX_FIELD_Y);
            m_MemberName[j + (i * MAX_ROWS)].ShowWindow(TRUE);

            m_MemberYes[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (((i + 1) * MAX_FIELD_X) - (60 + BORDER_X)), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 3, 20, 20);
            m_MemberYes[j + (i * MAX_ROWS)].EnableWindow(FALSE);

            m_MemberNo[j + (i * MAX_ROWS)].MoveWindow(MIN_X + (((i + 1) * MAX_FIELD_X) - (30 + BORDER_X)), MIN_Y + (j * MAX_FIELD_Y) + BORDER_Y * 3, 20, 20);
            m_MemberNo[j + (i * MAX_ROWS)].EnableWindow(FALSE);
         }
      }
   }

   for (i = 0; i < 5; i++) {
      m_MemberGroupBox->SetBorderColor(m_GBColor);
      m_MemberGroupBox->SetBackColor(TRANS_BACK);
      m_MemberGroupBox[i].MoveWindow((i * MAX_FIELD_X) + BORDER_X, MIN_Y + BORDER_Y / 2, MAX_FIELD_X, MAX_Y - BORDER_Y);
   }

   ConnectToVM(); // Open socket to Voting Machine app

   JaReadCurrentVote(JaVoteDat);

   // Specify the timer interval for checking vote file.
   UINT    nElapse = 2000;
   SetTimer(1, nElapse, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRTVDlg::ConnectToVM()
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


void CRTVDlg::OnCancel()
{
   exit(0); // Just get the heck out.

   CDialog::OnCancel();
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRTVDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRTVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRTVDlg::SetMemberVote(int member, int vote)
{
   if (vote == 1) {
      m_MemberNo[member].ShowWindow(TRUE);
      m_MemberYes[member].ShowWindow(FALSE);
   }
   else if (vote == 2) {
      m_MemberYes[member].ShowWindow(TRUE);
      m_MemberNo[member].ShowWindow(FALSE);
   }
   else {
      m_MemberYes[member].ShowWindow(FALSE);
      m_MemberNo[member].ShowWindow(FALSE);
   }
}

void CRTVDlg::ShowResults()
{
   if (pResultsViewDlg) {
      pResultsViewDlg->LoadDispOptions();
   }

   pResultsViewDlg->DisplayResults(TRUE);

   pResultsViewDlg->ShowWindow(SW_SHOW);
   ShowWindow(SW_HIDE);

   // FadeOut();

}

void CRTVDlg::ShowRealTime()
{
   pResultsViewDlg->DisplayResults(FALSE);

   pResultsViewDlg->ShowWindow(SW_HIDE);
   ShowWindow(SW_SHOW);

   // FadeIn();
}

void CRTVDlg::UpdateCurrentVote(SockPacket votePacket)
{
   for (int i = 0; i < MAX_SEATS; i++) {
      if ((i != 68) && (i != 90)) { // 69 & 91 excluded from voting
         m_JA_MemberSeatingList[i].vote = votePacket.dataByte[i];
         SetMemberVote(boardMap[i], m_JA_MemberSeatingList[i].vote);
      }
   }
}

void CRTVDlg::FadeOut()
{
   m_Fading = TRUE;
   m_FadeOut = TRUE;

   m_FadeLevel = 255;
   // Calculate the timer interval required to complete the fade in the specified time.
   UINT    nElapse = 255 / (255 / 5);
   SetTimer(1, nElapse, NULL);
}


void CRTVDlg::FadeIn()
{
   m_Fading = TRUE;
   m_FadeOut = FALSE;

   m_FadeLevel = 0;

   // Calculate the timer interval required to complete the fade in the specified time.
   UINT    nElapse = 510 / (255 / 5);
   SetTimer(1, nElapse, NULL);
}

void CRTVDlg::OnTimer(UINT_PTR nIDEvent)
{

   if (m_Fading) {
      if (m_FadeOut) {
         if (m_FadeLevel >= 5) {
            m_FadeLevel -= 5;
            m_pSetLayeredWindowAttributes(GetSafeHwnd(), 0, m_FadeLevel, ULW_ALPHA);
         }
         else {
            // Reached minimum alpha. Kill the timer and destroy our window.
            // PostNcDestroy will delete this CFaderWnd instance.
            // TBD - reset timer to non fading interval
            KillTimer(nIDEvent);
         }
      }
      else {
         if (m_FadeLevel < 255) {
            m_FadeLevel += 5;
            m_pSetLayeredWindowAttributes(GetSafeHwnd(), 0, m_FadeLevel, ULW_ALPHA);
         }
         else {
            // Reached minimum alpha. Kill the timer and destroy our window.
            // PostNcDestroy will delete this CFaderWnd instance.
            // TBD - reset timer to non fading interval
            KillTimer(nIDEvent);
         }
      }
   }
   //else if (JaVoteFileChanged()) {
   //   JaReadCurrentVote(JaVoteDat);
   //}

   CDialog::OnTimer(nIDEvent);
}


void CRTVDlg::JaReadCurrentVote(CString voteFilename)
{
   FILE* voteFile;
   int count;
   unsigned char votes[MAX_VOTE_ARRAY];

   voteFile = fopen(voteFilename, "rb");

   if (voteFile != NULL) {
      count = fread(votes, 1, sizeof(votes), voteFile);
      fclose(voteFile);
   }
   else {
      TRACE("Unable to open Votes file!");
      return;
   }

   if (count != sizeof(votes)) {
      TRACE("Unable to read Votes file!");
      return;
   }

   //if (!JaReadFile((char *)voteFilename.GetString(), votes));

   for (int i = 0; i < MAX_SEATS; i++) {
      if ((i != 68) && (i != 90)) { // 69 & 91 excluded from voting
         m_JA_MemberSeatingList[i].vote = votes[i];
         SetMemberVote(boardMap[i], m_JA_MemberSeatingList[i].vote);
      }
   }
}

FILETIME CRTVDlg::JaGetFileTime(CString fName)
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

BOOL CRTVDlg::JaVoteFileChanged()
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


int CompareMembers(const void* member1, const void* member2)
{
   const char apostrophe = (const char)'\'';

   CString name1 = ((LEX*)member1)->name;
   CString name2 = ((LEX*)member2)->name;

   // Remove apostrophe's prior to comparison
   name1.Remove(apostrophe);
   name2.Remove(apostrophe);

   // Comparing name in lower case guarantees telephone sort
   name1.MakeLower();
   name2.MakeLower();

   return(strcoll(name1, name2));
}

void CRTVDlg::JaLoadParametersAndSeating(void)
{
   FILE* tmpFile;

   char section[32];
   char key[32];
   char name[51];
   char county[51];

   unsigned int i, j, seat;

   // Does JaVote.Ini file exist?
   tmpFile = fopen(JaVoteIni, "r+");
   if (tmpFile == NULL) {
      DWORD error = GetLastError();
      return;
   }
   else {
      fclose(tmpFile);
   }

   wsprintf(section, "Parameters");

   int k = 0;
   for (i = 0; i < MAX_SEATS; i++) {
      seat = i + 1;
      wsprintf(section, "MemberSeat_%03.3d", seat);

      wsprintf(key, " Name");
      GetPrivateProfileString(section, key, "Hornsworthy", name, 50, JaVoteIni);
      wsprintf(key, " County");
      GetPrivateProfileString(section, key, "Ramsey", county, 50, JaVoteIni);
      for (j = strlen(name); j < 30; j++) {
         strcat_s(name, " ");
      }

      if ((seat != 69) && (seat != 91)) {
         strncpy_s(m_JA_MemberSeatingList[k].name, name, 30);
         m_JA_MemberSeatingList[k].seat = seat;
         m_JA_MemberSeatingList[k].seatID = seat + 2000; // Window Id
         strncpy_s(m_JA_MemberSeatingList[k].county, county, 30);
         k++;
      }
   }
   m_JA_MemberSeatingList[k].seat = 0;

   setlocale(LC_COLLATE, "en-029"); // set locale to english-carribean

   qsort((char*)m_JA_MemberSeatingList, (MAX_SEATS - 3), sizeof(LEX), CompareMembers);

   // map members to board location
   for (i = 0; m_JA_MemberSeatingList[i].seat; i++) {
      boardMap[m_JA_MemberSeatingList[i].seat - 1] = i;
      m_JA_MemberSeatingList[i].name[NAME_PRINTABLE] = '\0';  // truncate the padded name for columnation on printout
   }

   boardMap[135] = 133;                                   // shame on you, Mr. Speaker
   //boardMap[134] = 132;                                  // shame on you, Mr. Speaker

   m_JA_MemberSeatingList[i].name[NAME_PRINTABLE] = '\0';
}

HBRUSH CRTVDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
   HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

   // TODO:  Change any attributes of the DC here

   // TODO:  Return a different brush if the default is not desired

   return (HBRUSH)GetStockObject(BLACK_BRUSH);

   return hbr;
}
