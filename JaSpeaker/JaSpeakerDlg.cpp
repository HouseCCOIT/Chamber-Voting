
// JaSpeakerDlg.cpp : implementation file
//

#include <sys/types.h>
#include <sys/stat.h>
#include "stdafx.h"
#include "trace.h"
#include "alphasort.h"

#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <strsafe.h>

#include "JaSpeaker.h"
#include "JaSpeakerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString StationLockFile = "V:\\JaLock.dat";
CString RemoteVotesFile = "V:\\JaRemoteVotes.dat";
CString CallInVotesFile = "V:\\JaCallInVotes.dat";
CString DebateQueueFile = "V:\\JaDQ.dat";

char remoteVotes[MAX_VOTE_ARRAY];
char callInVotes[MAX_VOTE_ARRAY];
char debateQueue[MAX_VOTE_ARRAY];

static char mister_speaker[] = "Spk. ";
CString JaVoteIni = "V:\\JaVote.Ini";
CString JaVoteDat = "V:\\JaVote.dat"; // V:
CString JaLockDat = "V:\\JaLock.dat";
CString JaDispDat = "V:\\JaDisp.dat";

short boardMap[MAX_SEATS];                    // vectors absolute vote station numbers to display board locations
LEX m_JA_MemberSeatingList[MAX_SEATS];

unsigned char votesLast[MAX_VOTE_ARRAY];
unsigned char votes[MAX_VOTE_ARRAY];
unsigned char locks[MAX_VOTE_ARRAY];

// CJaSpeakerDlg dialog

CJaSpeakerDlg::CJaSpeakerDlg(CWnd* pParent /*=NULL*/)
   : CDialogEx(IDD_JASPEAKER_DIALOG, pParent)
{
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CJaSpeakerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);

   for (int i = 0; i < MAX_SEATS; i++) {
      DDX_Control(pDX, (IDC_STATIC_001 + i), m_MemberSeat[i]);
   }
   //{{AFX_DATA_MAP(JaDisplay)
   DDX_Control(pDX, IDC_JD_STATIC14, m_JD_Static14);
   DDX_Control(pDX, IDC_JD_STATIC13, m_JD_Static13);
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

   DDX_Control(pDX, IDC_JD_STATIC_DATETIME, m_JD_Static_DateTime);
   DDX_Control(pDX, IDC_STATIC_SPEAKER_LABEL, m_JD_Static_SpeakerLabel);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CJaSpeakerDlg, CDialogEx)
   //ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   //ON_WM_QUERYDRAGICON()
   ON_WM_TIMER()
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

extern BOOL m_TestMode;

// CJaSpeakerDlg message handlers

BOOL CJaSpeakerDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);			// Set big icon
   SetIcon(m_hIcon, FALSE);		// Set small icon

                                 // TODO: Add extra initialization here
   LONG style = -1811939260;
   LONG exstyle = 327680;
   ::SetWindowLong(this->m_hWnd, GWL_STYLE, style);
   ::SetWindowLong(this->m_hWnd, GWL_EXSTYLE, exstyle);
   ::SetWindowPos(this->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

   ShowCursor(false);

   MoveWindow(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
   ModifyStyle(WS_EX_CLIENTEDGE, 0, SWP_DRAWFRAME);

   if (m_TestMode) {
      //m_ServerIPAddress = "192.168.10.140";
      m_ServerIPAddress = "127.0.0.1";
   }
   else {
      m_ServerIPAddress = "192.168.10.2";
   }

   JaLoadParametersAndSeating();
   JaReadCallIns();
   JaReadRemotes();

   m_SFont.CreatePointFont(128, LPCTSTR("Arial"));

   //CenterWindow();
   //SetCursorPos(::GetSystemMetrics(SM_CXSCREEN) / 2, ::GetSystemMetrics(SM_CYSCREEN) / 2);

   COLORREF bgColor = RGB(10, 10, 10);
   //SetBackgroundColor(bgColor);

   for (int i = 0; i < MAX_SEATS; i++) {
      CString nameStr = m_JA_MemberSeatingList[i].name;
      CString countyStr = m_JA_MemberSeatingList[i].county;
      unsigned int seat = m_JA_MemberSeatingList[i].seat;

      nameStr.MakeUpper();

      // TBD: Remove when representative wakes up.
      CString ZZZ("ZZZ"); // = "ZZZ";
      if (nameStr.Find(ZZZ) != -1) {
         nameStr = CString("   ");
      }

      if ((seat != 0)) { // && (i != 68) && (i != 91)) {
         char tmp[10];
         int tokenPos = 0;
         _itoa(seat, tmp, 10);
         m_MemberSeat[seat - 1].SetSeat(CString(tmp));
         if (seat != 136) {
            m_MemberSeat[seat - 1].SetName(nameStr.TrimRight()); //.Tokenize(_T(" "), tokenPos));
         }
         else {
            m_MemberSeat[seat - 1].SetName(nameStr.TrimRight());
         }
         m_MemberSeat[seat - 1].SetCounty(countyStr);

         if (remoteVotes[seat - 1]) {
            m_MemberSeat[seat - 1].SetRorC("R");
         }
         else if (callInVotes[seat - 1]) {
            m_MemberSeat[seat - 1].SetRorC("C");
         }
         else {
            m_MemberSeat[seat - 1].SetRorC(" ");
         }
      }
      else {
         //m_MemberSeat[68].SetName("Sergeant");
         //m_MemberSeat[68].SetCounty("at Arms");
         // In honor of Melissa Hortman's legacy
         m_MemberSeat[68].SetName("HORTMAN");
         m_MemberSeat[68].SetCounty("Hennepin");
      }
      m_MemberSeat[i].SetBackColor(RGB(200, 200, 200));
      //m_MemberSeat[i].SetTextColor(RGB(128, 128, 128));
      //m_MemberSeat[i].SetFont(&m_SFont);
   }

   m_JD_Static_DateTime.SetTextColor(m_TextColor);
   m_JD_Static_DateTime.SetBackColor(m_BackColor);
   m_JD_Static_DateTime.SetFont(&m_MyFont);
   m_JD_Static_DateTime.SetWindowText("");
   m_JD_Static_DateTime.Invalidate(true);

   m_JD_Static_SpeakerLabel.SetTextColor(m_TextColor);
   m_JD_Static_SpeakerLabel.SetBackColor(m_BackColor);
   m_JD_Static_SpeakerLabel.SetFont(&m_MyFont);
   m_JD_Static_SpeakerLabel.SetWindowText("Speaker");
   m_JD_Static_SpeakerLabel.Invalidate(true);

   for (int i = 0; i < 138; i++) {
      votesLast[i] = 0;
   }

   LoadDispOptions();

   ConnectToVM();

   JaReadCurrentVote();

   // Specify the timer interval for checking vote file.
   UINT    nElapse = 1000;
   SetTimer(1, nElapse, NULL);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CJaSpeakerDlg::ConnectToVM()
{
   if (AfxSocketInit() == FALSE) {
      AfxMessageBox("Failed to Initialize Sockets");
      return;
   }

   if (m_Client.Create() == FALSE) {
      MessageBox("Failed to Create Socket");
      return;
   }

   if (m_Client.Connect(m_ServerIPAddress, 1004) == FALSE) {
      int error = GetLastError();
      MessageBox("Failed to Connect");
      return;
   }

   m_Client.pDlg = this;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJaSpeakerDlg::OnPaint()
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

void CJaSpeakerDlg::JaLoadParametersAndSeating(void)
{
   FILE *tmpFile;

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
         //if (seat == 136) {
         //   strcpy_s(m_JA_MemberSeatingList[k].name, mister_speaker);
         //   strncat(m_JA_MemberSeatingList[k].name, "\t", 1);
         //   strncat(m_JA_MemberSeatingList[k].name, name, 25);
         //}
         //else {
            strncpy_s(m_JA_MemberSeatingList[k].name, name, 30);
         //}
         m_JA_MemberSeatingList[k].seat = seat;
         m_JA_MemberSeatingList[k].seatID = seat + 2000; // Window Id
         strncpy_s(m_JA_MemberSeatingList[k].county, county, 30);
         k++;
      }
   }
   m_JA_MemberSeatingList[k].seat = 0;

   setlocale(LC_COLLATE, "en-029"); // set locale to english-carribean

   qsort((char *)m_JA_MemberSeatingList, (MAX_SEATS - 3), sizeof(LEX), CompareMembers);

   // map members to board location
   for (i = 0; m_JA_MemberSeatingList[i].seat; i++) {
      boardMap[m_JA_MemberSeatingList[i].seat - 1] = i;
      m_JA_MemberSeatingList[i].name[NAME_PRINTABLE] = '\0';  // truncate the padded name for columnation on printout
   }

   boardMap[135] = 133;                                   // shame on you, Mr. Speaker
   //boardMap[134] = 132;                                  // shame on you, Mr. Speaker

   m_JA_MemberSeatingList[i].name[NAME_PRINTABLE] = '\0';

   RefreshNames();
}

// GetLastWriteTime - Retrieves the last-write time and converts
//                    the time to a string
//
// Return value - TRUE if successful, FALSE otherwise
// hFile      - Valid file handle
// ftWrite    - Pointer to buffer to receive file time

BOOL GetLastWriteTime(HANDLE hFile, FILETIME *ftWrite)
{
   FILETIME ftCreate, ftAccess; //, ftWrite;

   // Retrieve the file times for the file.
   if (!GetFileTime(hFile, &ftCreate, &ftAccess, ftWrite)) {
      return false;
   }
   
   return true;
}

// OnTimer is scheduled to run once per second.
// The toggle boolean prevents the files from being accessed more frequently than 
// once every two seconds, which is the ganularity of the OS file write time stamp.
// The bail boolean prevents the timer from re-entrancy.

void CJaSpeakerDlg::OnTimer(UINT_PTR nIDEvent)
{
   static bool bail = false;
   static bool toggle = false; 

// Testing block. Uncomment to test
   //UINT i = rand() % MAX_SEATS;

   //if (m_JA_MemberSeatingList[i].vote == 0) {
   //   m_JA_MemberSeatingList[i].vote = rand() % 2 + 1;
   //   if (m_JA_MemberSeatingList[i].vote == 1) {
   //	   m_MemberSeat[i].SetBackColor(RGB(0, 255, 0));
   //   }
   //   else {
   //	   m_MemberSeat[i].SetBackColor(RGB(255, 0, 0));
   //   }
   //}

   if (bail) goto BailOut;

	bail = true;

   if (toggle) {
      toggle = false;

      if (JaRemoteFileChanged()) {
         JaReadRemotes();
      }

      if (JaCallInFileChanged()) {
         JaReadCallIns();
      }

      if (JaDQFileChanged()) {
         JaReadDebateQueue();
      }

      if (JaLockFileChanged()) {
         JaReadLocks();
      }

      //if (JaVoteFileChanged()) {
      //   JaReadCurrentVote();
      //}

      if (JaResultsFileChanged()) {
         LoadDispOptions();
      }
   }
   else {
      toggle = true;
   }

   // Update Speakers clock once per second.
   char   tmpDate[80];
   __time64_t  tmpTime = time(&tmpTime);
   struct tm *newTime;
   static struct tm oldTime;

   newTime = localtime(&tmpTime);

   if (newTime->tm_min != oldTime.tm_min) {
      oldTime = *newTime;

      strftime(tmpDate, 80, " %m/%d/%Y %I:%M%p", newTime);

      m_JD_Static_DateTime.SetTextColor(m_TextColor);
      m_JD_Static_DateTime.SetBackColor(m_BackColor);
      m_JD_Static_DateTime.SetFont(&m_MyFont);
      m_JD_Static_DateTime.SetWindowText(CString(tmpDate));
      m_JD_Static_DateTime.Invalidate(true);

      m_JD_Static_SpeakerLabel.SetTextColor(m_TextColor);
      m_JD_Static_SpeakerLabel.SetBackColor(m_BackColor);
      m_JD_Static_SpeakerLabel.SetFont(&m_MyFont);
      m_JD_Static_SpeakerLabel.SetWindowText("Speaker");
      m_JD_Static_SpeakerLabel.Invalidate(true);
   }

   bail = false;

BailOut:
   CDialog::OnTimer(nIDEvent);
}

BOOL CJaSpeakerDlg::OnEraseBkgnd(CDC* pDC)
{
   CRect rect;
   GetClientRect(&rect);
   CBrush myBrush(RGB(0, 0, 0));    // dialog background color
   CBrush *pOld = pDC->SelectObject(&myBrush);
   BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
   pDC->SelectObject(pOld);    // restore old brush
   return bRes;                       //TBD - Huh?
   //return CDialog::OnEraseBkgnd(pDC);
}


BOOL CJaSpeakerDlg::LoadDispOptions()
{
   FILE *dispFile;

   int i;
   char tmpStr[256];

   if ((dispFile = fopen(JaDispDat, "r+")) == NULL) {
      return FALSE;
   }

   // ClearAll();
   ClearSix();

   // Get font name
   fgets(tmpStr, 256, dispFile);
   tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
   SetFont("Lucida Console");           // Hard code font for now

   // Get font size
   fgets(tmpStr, 256, dispFile);
   tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
                                        // Ignore font size setting. Speakers display has it's own setting.

   for (i = 0; i < 7; i++) {
      fgets(tmpStr, 256, dispFile);
      tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
      m_DisplayLine[i] = tmpStr;
   }

   // Let the Results display in real time
   //for (i = 0; i < 4; i++) {
   //	fgets(tmpStr, 256, dispFile);
   //	tmpStr[strlen(tmpStr) - 1] = '\0';   // Eliminate newline
   //	m_ResultLine[i] = tmpStr;
   //}

   fclose(dispFile);

   UpdateDisplay();

   return TRUE;
}

void CJaSpeakerDlg::SetFont(CString fontName)
{
   m_FontName = fontName;
   m_FontSizePoints = 100; // Speakers display default

   m_MyFont.DeleteObject();
   m_MyFont.CreatePointFont(m_FontSizePoints, m_FontName);
}


void CJaSpeakerDlg::UpdateDisplay()
{
   char absentText[10];
   int yea, nay, absent;

   COLORREF TColor = RGB(255, 255, 255);
   m_TextColor = TColor;

   m_JD_Static1.SetTextColor(m_TextColor);
   m_JD_Static1.SetBackColor(m_BackColor);
   m_JD_Static1.SetFont(&m_MyFont);
   m_JD_Static1.SetWindowText(m_DisplayLine[0]);
   m_JD_Static1.Invalidate(true);

   m_JD_Static2.SetTextColor(m_TextColor);
   m_JD_Static2.SetBackColor(m_BackColor);
   m_JD_Static2.SetFont(&m_MyFont);
   m_JD_Static2.SetWindowText(m_DisplayLine[1]);
   m_JD_Static2.Invalidate(true);

   m_JD_Static3.SetTextColor(m_TextColor);
   m_JD_Static3.SetBackColor(m_BackColor);
   m_JD_Static3.SetFont(&m_MyFont);
   m_JD_Static3.SetWindowText(m_DisplayLine[2]);
   m_JD_Static3.Invalidate(true);

   m_JD_Static4.SetTextColor(m_TextColor);
   m_JD_Static4.SetBackColor(m_BackColor);
   m_JD_Static4.SetFont(&m_MyFont);
   m_JD_Static4.SetWindowText(m_DisplayLine[3]);
   m_JD_Static4.Invalidate(true);

   m_JD_Static5.SetTextColor(m_TextColor);
   m_JD_Static5.SetBackColor(m_BackColor);
   m_JD_Static5.SetFont(&m_MyFont);
   m_JD_Static5.SetWindowText(m_DisplayLine[4]);
   m_JD_Static5.Invalidate(true);

   m_JD_Static6.SetTextColor(m_TextColor);
   m_JD_Static6.SetBackColor(m_BackColor);
   m_JD_Static6.SetFont(&m_MyFont);
   m_JD_Static6.SetWindowText(m_DisplayLine[5]);
   m_JD_Static5.Invalidate(true);

   m_JD_Static7.SetTextColor(m_TextColor);
   m_JD_Static7.SetBackColor(m_BackColor);
   m_JD_Static7.SetFont(&m_MyFont);
   m_JD_Static7.SetWindowText(m_DisplayLine[6]);
   m_JD_Static7.Invalidate(true);

   m_JD_Static8.SetTextColor(m_TextColor);
   m_JD_Static8.SetBackColor(m_BackColor);
   m_JD_Static8.SetFont(&m_MyFont);
   m_JD_Static8.SetWindowText(m_DisplayLine[7]);
   m_JD_Static8.Invalidate(true);

   m_JD_Static9.SetTextColor(m_TextColor);
   m_JD_Static9.SetBackColor(m_BackColor);
   m_JD_Static9.SetFont(&m_MyFont);
   m_JD_Static9.SetWindowText(m_ResultLine[0]);
   m_JD_Static9.Invalidate(true);

   m_JD_Static10.SetTextColor(m_TextColor);
   m_JD_Static10.SetBackColor(m_BackColor);
   m_JD_Static10.SetFont(&m_MyFont);
   m_JD_Static10.SetWindowText(m_ResultLine[1]);
   m_JD_Static10.Invalidate(true);

   m_JD_Static11.SetTextColor(m_TextColor);
   m_JD_Static11.SetBackColor(m_BackColor);
   m_JD_Static11.SetFont(&m_MyFont);
   m_JD_Static11.SetWindowText(m_ResultLine[2]);
   m_JD_Static11.Invalidate(true);

   m_JD_Static12.SetTextColor(m_TextColor);
   m_JD_Static12.SetBackColor(m_BackColor);
   m_JD_Static12.SetFont(&m_MyFont);
   m_JD_Static12.SetWindowText(m_ResultLine[3]);
   m_JD_Static12.Invalidate(true);

   m_JD_Static13.SetTextColor(m_TextColor);
   m_JD_Static13.SetBackColor(m_BackColor);
   m_JD_Static13.SetFont(&m_MyFont);
   m_JD_Static13.SetWindowText("Not Voting");
   m_JD_Static13.Invalidate(true);

   m_JD_Static14.SetTextColor(m_TextColor);
   m_JD_Static14.SetBackColor(m_BackColor);
   m_JD_Static14.SetFont(&m_MyFont);

   // Calculate absentees
   yea = atoi(m_ResultLine[1]);
   nay = atoi(m_ResultLine[3]);
   absent = 134 - (yea + nay);
   _itoa(absent, absentText, 10);

   m_JD_Static14.SetWindowText(absentText);
   m_JD_Static14.Invalidate(true);
}

void CJaSpeakerDlg::ClearSix()
{
   for (int i = 0; i < 7; i++) {
      m_DisplayLine[i] = "";
   }

   COLORREF BColor = RGB(0, 0, 0);
   m_BackColor = BColor;

   UpdateDisplay();

   Invalidate();
}

void CJaSpeakerDlg::ClearAll()
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

   Invalidate();
}

void CJaSpeakerDlg::SetLine(int i, char *text)
{
   m_DisplayLine[i] = text;
}

void CJaSpeakerDlg::SetResultLine(char *text1, char *text2, char *text3, char *text4)
{
   m_ResultLine[0] = text1;
   m_ResultLine[1] = text2;
   m_ResultLine[2] = text3;
   m_ResultLine[3] = text4;
}

void CJaSpeakerDlg::ClearResults()
{
   int i;

   for (i = 0; i < 4; i++) {
      m_ResultLine[i] = "     ";
   }

   COLORREF BColor = RGB(0, 0, 0);
   m_BackColor = BColor;

   UpdateDisplay();

   Invalidate();
}

void CJaSpeakerDlg::RefreshNames()
{
   for (int i = 0; i < MAX_SEATS; i++) {
      CString nameStr = m_JA_MemberSeatingList[i].name;
      CString countyStr = m_JA_MemberSeatingList[i].county;
      unsigned int seat = m_JA_MemberSeatingList[i].seat;

      nameStr.MakeUpper();

      // TBD: Remove when representative wakes up.
      CString ZZZ("ZZZ"); // = "ZZZ";
      if (nameStr.Find(ZZZ) != -1) {
         nameStr = CString("   ");
      }

      // TBD 12252024
      if ((seat != 0)) { //&& (i != 68) && (i != 90)) { // 69 & 91 excluded from display
         char tmp[10];
         int tokenPos = 0;
         _itoa(seat, tmp, 10);
         m_MemberSeat[seat - 1].SetSeat(CString(tmp));
         if (seat != 136) {
            m_MemberSeat[seat - 1].SetSpeaker("Representative");
            m_MemberSeat[seat - 1].SetName(nameStr.TrimRight()); //.Tokenize(_T(" "), tokenPos));
         }
         else {
            m_MemberSeat[seat - 1].SetSpeaker("Speaker");
            m_MemberSeat[seat - 1].SetName(nameStr.TrimRight());
         }
         m_MemberSeat[seat - 1].SetCounty(countyStr);
      }
   }
}

void CJaSpeakerDlg::UpdateCurrentVote(SockPacket votePacket)
{
   //for (int i = 0; i < MAX_SEATS; i++) {
   //   if ((i != 68) && (i != 90)) { // 69 & 91 excluded from voting
   //      m_JA_MemberSeatingList[i].vote = votePacket.dataByte[i];
   //      SetMemberVote(boardMap[i], m_JA_MemberSeatingList[i].vote);
   //   }
   //}

   char buffer1[4];
   char buffer2[4];

   for (int i = 0; i < MAX_SEATS; i++) {

      m_JA_MemberSeatingList[i].vote = votePacket.dataByte[i];

      if (locks[i] == 0) {
         if (m_JA_MemberSeatingList[i].vote == 2) {
            m_MemberSeat[i].SetBackColor(RGB(0, 255, 0));
         }
         else if (m_JA_MemberSeatingList[i].vote == 1) {
            m_MemberSeat[i].SetBackColor(RGB(255, 0, 0));
         }
         else if (i == 68) {
            m_MemberSeat[i].SetBackColor(RGB(255, 255, 0));
         }
         else if (debateQueue[i]) {
            m_MemberSeat[i].SetBackColor(RGB(0, 0, 255));
         }
         else {
            m_MemberSeat[i].SetBackColor(RGB(200, 200, 200));
         }
      }
      else {
         m_MemberSeat[i].SetBackColor(RGB(255, 255, 0));
      }

      votesLast[i] = votePacket.dataByte[i];
   }

   SetResultLine("Yeas", _itoa(votePacket.dataByte[136], buffer1, 10), "Nays", _itoa(votePacket.dataByte[137], buffer2, 10));

   UpdateDisplay();
}

FILETIME CJaSpeakerDlg::JaGetFileTime(CString fName)
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

BOOL CJaSpeakerDlg::JaVoteFileChanged()
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

BOOL CJaSpeakerDlg::JaLockFileChanged()
{
   BOOL retVal = false;

   static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(JaLockDat);

   if ((ftWrite.dwLowDateTime != 0) && (ftWrite.dwLowDateTime != ftLastWrite.dwLowDateTime)) {
      ftLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

BOOL CJaSpeakerDlg::JaResultsFileChanged()
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

BOOL CJaSpeakerDlg::JaRemoteFileChanged()
{
   BOOL retVal = false;

   static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(RemoteVotesFile);

   if ((ftWrite.dwLowDateTime != 0) && (ftWrite.dwLowDateTime != ftLastWrite.dwLowDateTime)) {
      ftLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

BOOL CJaSpeakerDlg::JaCallInFileChanged()
{
   BOOL retVal = false;

   static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(CallInVotesFile);

   if ((ftWrite.dwLowDateTime != 0) && (ftWrite.dwLowDateTime != ftLastWrite.dwLowDateTime)) {
      ftLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

BOOL CJaSpeakerDlg::JaDQFileChanged()
{
   BOOL retVal = false;

   static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(DebateQueueFile);

   if ((ftWrite.dwLowDateTime != 0) && (ftWrite.dwLowDateTime != ftLastWrite.dwLowDateTime)) {
      ftLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

void CJaSpeakerDlg::JaReadCurrentVote()
{
   FILE* voteFile;
   int count;
   static unsigned char votes[MAX_VOTE_ARRAY];
   char buffer1[4];
   char buffer2[4];

   //RefreshNames();

   TRACE("Open Vote file");

   voteFile = fopen(JaVoteDat, "rb");

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

   for (int i = 0; i < MAX_SEATS; i++) {

      m_JA_MemberSeatingList[i].vote = votes[i];

      if (locks[i] == 0) {
         if (m_JA_MemberSeatingList[i].vote == 2) {
            m_MemberSeat[i].SetBackColor(RGB(0, 255, 0));
         }
         else if (m_JA_MemberSeatingList[i].vote == 1) {
            m_MemberSeat[i].SetBackColor(RGB(255, 0, 0));
         }
         else if (i == 68) {
            m_MemberSeat[i].SetBackColor(RGB(255, 255, 0));
         }
         else if (debateQueue[i]) {
            m_MemberSeat[i].SetBackColor(RGB(0, 0, 255));
         }
         else {
            m_MemberSeat[i].SetBackColor(RGB(200, 200, 200));
         }
      }
      else {
         m_MemberSeat[i].SetBackColor(RGB(255, 255, 0));
      }

      votesLast[i] = votes[i];
   }

   SetResultLine("Yeas", _itoa(votes[136], buffer1, 10), "Nays", _itoa(votes[137], buffer2, 10));

   UpdateDisplay();
}

void CJaSpeakerDlg::JaReadLocks()
{
   FILE* lockFile;
   int count;

   TRACE("Open Locks file");

   lockFile = fopen(JaLockDat, "rb");
   if (lockFile != NULL) {
      count = fread(locks, 1, sizeof(locks), lockFile);
      fclose(lockFile);
   }

   if (count != sizeof(locks)) {
      TRACE("Unable to read Locks file!");
      return;
   }
}

void CJaSpeakerDlg::JaReadRemotes()
{
   FILE* remotesFile;
   int count;

   remotesFile = fopen(RemoteVotesFile, "rb");

   if (remotesFile != NULL) {
      count = fread(remoteVotes, 1, sizeof(remoteVotes), remotesFile);
      fclose(remotesFile);
   }
   else {
      TRACE("Unable to open Remote Votes file!");
      return;
   }

   if (count != sizeof(remoteVotes)) {
      TRACE("Unable to read Remote Votes file!");
      return;
   }

   for (int i = 0; i < MAX_SEATS; i++) {
      if (remoteVotes[i]) {
         m_MemberSeat[i].SetRorC("R");
      }
      else if (callInVotes[i]) {
         m_MemberSeat[i].SetRorC("C");
      }
      else {
         m_MemberSeat[i].SetRorC(" ");
      }
   }
}

void CJaSpeakerDlg::JaReadCallIns()
{
   FILE* callInsFile;
   int count;

   callInsFile = fopen(CallInVotesFile, "rb");

   if (callInsFile != NULL) {
      count = fread(callInVotes, 1, sizeof(callInVotes), callInsFile);
      fclose(callInsFile);
   }
   else {
      TRACE("Unable to open CallIn Votes file!");
      return;
   }

   if (count != sizeof(callInVotes)) {
      TRACE("Unable to read CallIn Votes file!");
      return;
   }

   for (int i = 0; i < MAX_SEATS; i++) {
      if (remoteVotes[i]) {
         m_MemberSeat[i].SetRorC("R");
      }
      else if (callInVotes[i]) {
         m_MemberSeat[i].SetRorC("C");
      }
      else {
         m_MemberSeat[i].SetRorC(" ");
      }
   }
}

void CJaSpeakerDlg::JaReadDebateQueue()
{
   FILE* DQFile;
   int count;

   DQFile = fopen(DebateQueueFile, "rb");

   if (DQFile != NULL) {
      count = fread(debateQueue, 1, sizeof(debateQueue), DQFile);
      fclose(DQFile);
   }
   else {
      TRACE("Unable to open DebateQueue file!");
      return;
   }

   if (count != sizeof(debateQueue)) {
      TRACE("Unable to read DebateQueue file!");
      return;
   }

   for (int i = 0; i < MAX_SEATS; i++) {

      if (locks[i] == 0) {
         if (m_JA_MemberSeatingList[i].vote == 2) {
            m_MemberSeat[i].SetBackColor(RGB(0, 255, 0));
         }
         else if (m_JA_MemberSeatingList[i].vote == 1) {
            m_MemberSeat[i].SetBackColor(RGB(255, 0, 0));
         }
         else if (i == 68) {
            m_MemberSeat[i].SetBackColor(RGB(255, 255, 0));
         }
         else if (debateQueue[i]) {
            m_MemberSeat[i].SetBackColor(RGB(0, 0, 255));
         }
         else {
            m_MemberSeat[i].SetBackColor(RGB(200, 200, 200));
         }
      }
      else {
         m_MemberSeat[i].SetBackColor(RGB(255, 255, 0));
      }
   }

   UpdateDisplay();
}
