// JaVMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "afxmt.h"
#include "JaVM.h"
#include "JaVMDlg.h"
#include "VMThread.h"
#include "RemoteSocketServer.h"
#include "VideoSocketServer.h"
#include "VMSocketServer.h"
#include "SockThread.h"
#include "trace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL m_VoteOpen;

extern char station_lock[MAX_VOTE_ARRAY];                 // != 0: bypass this vote station in polling sequence
extern char remoteVotes[MAX_VOTE_ARRAY];
extern char callInVotes[MAX_VOTE_ARRAY];
extern char debateQueue[MAX_VOTE_ARRAY];
extern BOOL gDebateQueueChanged;

CString StationLockFile = "V:\\JaLock.dat";
CString RemoteVotesFile = "V:\\JaRemoteVotes.dat";
CString CallInVotesFile = "V:\\JaCallInVotes.dat";


/////////////////////////////////////////////////////////////////////////////
// CJaVMDlg dialog

extern BOOL m_TestMode;
BOOL m_JA_VotingMachineActive;

extern unsigned char yes_total, no_total, absent_total;

CJaVMDlg::CJaVMDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CJaVMDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CJaVMDlg)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJaVMDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CJaVMDlg)
   DDX_Control(pDX, IDC_CHECK_OPEN, m_CheckOpen);
   //}}AFX_DATA_MAP

   for (int i = 0; i < 144; i++) {
      DDX_Control(pDX, (IDC_STATIC_001 + i), m_MemberSeat[i]);
   }
}

BEGIN_MESSAGE_MAP(CJaVMDlg, CDialog)
   //{{AFX_MSG_MAP(CJaVMDlg)
   ON_WM_PAINT()
   ON_BN_CLICKED(IDC_CHECK_OPEN, OnCheckOpen)
   ON_WM_MOUSEMOVE()
   ON_WM_MOVE()
   //}}AFX_MSG_MAP
   ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJaVMDlg message handlers

BOOL CJaVMDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   TRACE("Entry");

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);         // Set big icon
   SetIcon(m_hIcon, FALSE);        // Set small icon

   m_SFont.CreatePointFont(80, LPCTSTR("Arial"));

   CenterWindow();
   SetCursorPos(::GetSystemMetrics(SM_CXSCREEN) / 2, ::GetSystemMetrics(SM_CYSCREEN) / 2);

   for (int i = 0; i < 144; i++) {
      m_MemberSeat[i].SetBackColor(RGB(255, 255, 255));
      m_MemberSeat[i].SetFont(&m_SFont);
   }

   CWinThread* VotingMachineThread;
   CWinThread* SocketThread;

   CJaVMDlg* VMObj = this;

   if (m_TestMode) {  // Use SocketThread for Dev mode
      SocketThread = AfxBeginThread(JaSockThreadFunction, VMObj);
   }
   else { // Otherwise spawn VM thread
      VotingMachineThread = AfxBeginThread(JaVMThreadFunction, VMObj);
   }

   // Initialize Windows Sockets
   if (AfxSocketInit() == FALSE) {
      AfxMessageBox("Failed to Initialize Sockets");
      return 0;
   }

   // Instantiate Socket between VM and Remote Vote applications
   VMSockRemote = new CRemoteSocketServer(this);

   if (VMSockRemote->Create(1001) == FALSE) {
      TRACE("Failed to create VMSockRemote");
      return 0;
   }

   if (VMSockRemote->Listen() == FALSE) {
      TRACE("VMSockRemote Listen Failed");
      return 0;
   }

   //// Instantiate Socket between VM and Video apps
   VMSockVideo = new CVideoSocketServer(this);

   if (VMSockVideo->Create(1002) == FALSE) {
       TRACE("Failed to create VMSockVideo");
       return 0;
   }

   if (VMSockVideo->Listen() == FALSE) {
       TRACE("VMSockVideo Listen Failed");
       return 0;
   }

   //// Instantiate Socket between VM and Message Board app
   VMSockMB = new CMBSocketServer(this);

   if (VMSockMB->Create(1003) == FALSE) {
      TRACE("Failed to create VMSockMB");
      return 0;
   }

   if (VMSockMB->Listen() == FALSE) {
      TRACE("VMSockMB Listen Failed");
      return 0;
   }

   //// Instantiate Socket between VM and apps thats need the vote updates
   VMSockVote = new CVMSocketServer(this);

   if (VMSockVote->Create(1004) == FALSE) {
      TRACE("Failed to create VMSockMB");
      return 0;
   }

   if (VMSockVote->Listen() == FALSE) {
      TRACE("VMSockMB Listen Failed");
      return 0;
   }

   GetDlgItem(IDC_EDIT_STATUS_WINDOW)->SetWindowText(LPCTSTR("Main Voting Machine Thread Spawned."));

   JaLoadParametersAndSeating();

   if (m_TestMode) { // Show Open Checkbox in Dev mode
      m_CheckOpen.ShowWindow(TRUE);
   }
   else {
      m_CheckOpen.ShowWindow(FALSE);
   }

   // Specify the timer interval for checking Lock, Remote, and CallIn files.
   UINT    nElapse = 1000;
   SetTimer(1, nElapse, NULL);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJaVMDlg::OnPaint()
{
   if (IsIconic()) {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

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
   else {
      CDialog::OnPaint();
   }
}

void CJaVMDlg::OnOK()
{
}

void CJaVMDlg::OnCancel()
{
   if (AfxMessageBox(LPCTSTR("Are you sure you want to exit?"), MB_YESNO) == IDYES) {
      m_JA_VotingMachineActive = FALSE;

      KillSock();
      KillVM();

      Sleep(2000);

      exit(0); // Just get the heck out.

      CDialog::OnCancel();
   }
}

void CJaVMDlg::SetMemberVote(int member, int seat, int vote)
{
   seat--;

   if (vote == 1) {
      m_MemberSeat[seat].SetBackColor(RGB(255, 0, 0));
   }
   else if (vote == 2) {
      m_MemberSeat[seat].SetBackColor(RGB(0, 255, 0));
   }
   else if (vote == 3) {
      m_MemberSeat[seat].SetBackColor(RGB(0, 0, 255));
   }
   else {
      m_MemberSeat[seat].SetBackColor(RGB(255, 255, 0));
   }

   if (m_TestMode) { // Update seat color in Dev mode 
      m_MemberSeat[seat].Invalidate(true);
   }
   else {
      //
   }
}

CTime openTime;
void CJaVMDlg::OnCheckOpen()
{
   SockVMPacket remotePacket;
   SockPacket   mbPacket;
   SockPacket   videoPacket;

   if (m_CheckOpen.GetState() & 0x03) {
      m_VoteOpen = TRUE;

      openTime = CTime::GetCurrentTime();

      remotePacket.packetType = VM_VM;
      remotePacket.VMAction = VM_VOTE_OPEN_REMOTE;
      SendSockRemoteMsg(&remotePacket);

      // Send message to Video Clients
      videoPacket.packetType = VM_VIDEO;
      videoPacket.VideoAction = VM_VIDEO_SHOW_REALTIME;
      SendVideoMsg(&videoPacket);

      // Freeze the message display transmission mechanisms
      mbPacket.packetType = VM_MB;
      mbPacket.MBAction = VM_MB_VOTE_ACTIVE;
      SendMBMsg(&mbPacket);

      // Clear the debate list on Vote Open
      for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
         debateQueue[i] = 0;
      }
      gDebateQueueChanged = true;

      if (m_TestMode) {
         //         ShowRealTime();
      }
   }
   else {
      m_VoteOpen = FALSE;

      CTime  resetTime = CTime::GetCurrentTime();

      __time64_t elapsedTime = resetTime.GetTime() - openTime.GetTime();
      if (elapsedTime < 5) {
         Sleep((5 - elapsedTime) * 1000);
      }

      remotePacket.packetType = VM_VM;
      remotePacket.VMAction = VM_VOTE_RESET_REMOTE;
      SendSockRemoteMsg(&remotePacket);

      // Send the totals to communications central (MB) for message display
      mbPacket.packetType = VM_MB;
      mbPacket.MBAction = VM_MB_VOTE_RESULTS;
      mbPacket.dataByte[0] = yes_total;
      mbPacket.dataByte[1] = no_total;
      SendMBMsg(&mbPacket);
      
      Sleep(5000);

      // Send message to Video Clients
      videoPacket.packetType = VM_VIDEO;
      videoPacket.VideoAction = VM_VIDEO_SHOW_RESULTS;
      SendVideoMsg(&videoPacket);
 
      Sleep(5000);

      // Eliminate vote information from the message_display
      // Thaw the message display transmission mechanisms
      mbPacket.packetType = VM_MB;
      mbPacket.MBAction = VM_MB_VOTE_OPEN;
      SendMBMsg(&mbPacket);

      if (m_TestMode) {
         //         ShowResults();
      }
   }
}

void CJaVMDlg::OnMouseMove(UINT nFlags, CPoint point)
{
   if (!m_TestMode) {
      //RECT rcClip;
      //GetWindowRect(&rcClip);

      //// TBD = Uncomment for release
      //if (point.x > rcClip.right)  point.x = rcClip.right;
      //if (point.x < rcClip.left)   point.x = rcClip.left;
      //if (point.y < rcClip.top)    point.y = rcClip.top;
      //if (point.y > rcClip.bottom) point.y = rcClip.bottom;

      //::ClipCursor(&rcClip);
   }

   CDialog::OnMouseMove(nFlags, point);
}

void CJaVMDlg::OnMove(int x, int y)
{
   if (m_TestMode) {
      //RECT rcClip;
      //GetWindowRect(&rcClip);

      //// TBD = Uncomment for rwelease
      //if (x > rcClip.left) x = rcClip.left;
      //if (x < rcClip.left) x = rcClip.left;
      //if (y < rcClip.top)  y = rcClip.top;
      //if (y > rcClip.top)  y = rcClip.top;
   }

   CDialog::OnMove(x, y);
}

void CJaVMDlg::AddMsg(SockRemotePacket* SockData)
{
   CString sUserName;

   sUserName.Format("%X : %X", SockData->packetType, SockData->remoteAction);

   //m_Display.AddString(sUserName);
}

void CJaVMDlg::SendSockRemoteMsg(SockVMPacket* remotePacket)
{
   //TRACE("Entry");

   VMSockRemote->SendSockRemoteMessage(remotePacket);
}

void CJaVMDlg::SendVideoMsg(SockPacket* videoPacket)
{
   //TRACE("Entry");

   VMSockVideo->SendVideoClientMessage(videoPacket);
}

void CJaVMDlg::SendMBMsg(SockPacket* mbPacket)
{
   //TRACE("Entry");

   VMSockMB->SendMBClientMessage(mbPacket);
}

void CJaVMDlg::SendSockRemoteMsgToRTS(SockRemotePacket* SockPacket)
{
   //TRACE("Entry");

   VMSockRemote->SendSockRemoteMessageToRTS(SockPacket);
}

void CJaVMDlg::SendCurrentVoteMsg(SockPacket* vmPacket)
{
   //TRACE("Entry");

   VMSockVote->SendVMClientMessage(vmPacket);

   VMSockVideo->SendVideoClientMessage(vmPacket);
}

void CJaVMDlg::JaReadLocks()
{
   FILE* lockFile;
   int count;

   lockFile = fopen(StationLockFile, "rb");

   if (lockFile != NULL) {
      count = fread(station_lock, 1, sizeof(station_lock), lockFile);
      fclose(lockFile);
   }
   else {
      TRACE("Unable to open Locks file!");
      return;
   }

   if (count != sizeof(station_lock)) {
      TRACE("Unable to read Locks file! Count off");
      return;
   }
}

void CJaVMDlg::JaReadRemotes()
{
   FILE* remotesFile;
   int count;

   remotesFile = fopen(RemoteVotesFile, "rb");

   if (remotesFile != NULL) {
      count = fread(remoteVotes, 1, sizeof(remoteVotes), remotesFile);
      fclose(remotesFile);
   }
   else {
      TRACE("Unable to open Remotes file!");
      return;
   }

   if (count != sizeof(remoteVotes)) {
      TRACE("Unable to read Remotes file! Count off");
      return;
   }
}

void CJaVMDlg::JaReadCallIns()
{
   FILE* callInsFile;
   int count;

   callInsFile = fopen(CallInVotesFile, "rb");

   if (callInsFile != NULL) {
      count = fread(callInVotes, 1, sizeof(callInVotes), callInsFile);
      fclose(callInsFile);
   }
   else {
      TRACE("Unable to open CallIns file!");
      return;
   }

   if (count != sizeof(callInVotes)) {
      TRACE("Unable to read CallIns file! Count off");
      return;
   }
}

FILETIME CJaVMDlg::JaGetFileTime(CString fName)
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
   }

   CloseHandle(hFile);

   return ftWrite;
}

BOOL CJaVMDlg::JaLockFileChanged()
{
   BOOL retVal = false;

   static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(StationLockFile);

   if (ftWrite.dwLowDateTime != ftLastWrite.dwLowDateTime) {
      ftLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

BOOL CJaVMDlg::JaRemoteFileChanged()
{
   BOOL retVal = false;

   static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(RemoteVotesFile);

   if (ftWrite.dwLowDateTime != ftLastWrite.dwLowDateTime) {
      ftLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

BOOL CJaVMDlg::JaCallInFileChanged()
{
   BOOL retVal = false;

   static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(CallInVotesFile);

   if (ftWrite.dwLowDateTime != ftLastWrite.dwLowDateTime) {
      ftLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

void CJaVMDlg::OnTimer(UINT_PTR nIDEvent)
{
   static bool bail = false;

   if (bail) goto BailOut;

   bail = true;

  // if (JaLockFileChanged()) {
      JaReadLocks();
  // }

  // if (JaRemoteFileChanged()) {
      JaReadRemotes();
  // }


  // if (JaCallInFileChanged()) {
      JaReadCallIns();
  // }

   bail = false;

BailOut:

   CDialog::OnTimer(nIDEvent);
}
