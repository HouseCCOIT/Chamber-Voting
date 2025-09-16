
// DebateQueueDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "DebateQueue.h"
#include "DebateQueueDlg.h"
#include "afxdialogex.h"
#include "VM_Sock.h"
#include "alphasort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDebateQueueDlg dialog



CDebateQueueDlg::CDebateQueueDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DEBATEQUEUE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDebateQueueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STATIONS, m_StationList);
	DDX_Control(pDX, IDC_LIST_RTS, m_RTSList);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_ButtonAdd);
	DDX_Control(pDX, IDC_BUTTON_REMOVE, m_ButtonRemove);
	DDX_Control(pDX, IDCANCEL, m_ButtonExit);
	DDX_Control(pDX, IDC_STATIC_LOCK_FRAME_NAMES, m_CallInNames);
	DDX_Control(pDX, IDC_STATIC_LOCK_FRAME_RTS, m_RTS);
}

BEGIN_MESSAGE_MAP(CDebateQueueDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
   ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_LIST_STATIONS, &CDebateQueueDlg::OnLbnSelchangeListStations)
	ON_LBN_SELCHANGE(IDC_LIST_RTS, &CDebateQueueDlg::OnLbnSelchangeListRTS)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDebateQueueDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, &CDebateQueueDlg::OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDCANCEL, &CDebateQueueDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

LEX2 m_JA_MemberSeatingList[MAX_SEATS];

unsigned char callInVotes[MAX_VOTE_ARRAY];
unsigned char debateQueue[MAX_VOTE_ARRAY];

unsigned char boardMap[MAX_SEATS];           // vectors absolute vote station numbers to display board locations

CString JaRemoteIni = "V:\\JaRemote.Ini";

CString CallInVotesFile = "V:\\JaCallInVotes.dat";
CString DebateQueueFile = "V:\\JaDQ.dat";

extern BOOL m_TestMode;

BOOL m_VoteOpen;

// CDebateQueueDlg message handlers

BOOL CDebateQueueDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_Brush.CreateSolidBrush(RGB(0xda, 0xb2, 0x75));

	// Creates a 18-point-Courier-font
	m_Elephant.CreatePointFont(180, _T("Elephant"));

	m_LucidaSans.CreatePointFont(130, _T("Lucida Sans"));

	m_Calibri.CreatePointFont(100, _T("Calibri"));

	m_ButtonAdd.EnableWindowsTheming(false);
	m_ButtonAdd.SetFaceColor(RGB(0x00, 0x00, 0x00));
	m_ButtonAdd.SetTextColor(RGB(0xff, 0xff, 0xff));
	m_ButtonAdd.SetWindowTextA("ADD");
	m_ButtonAdd.SetFont(&m_LucidaSans, true);

	m_ButtonRemove.EnableWindowsTheming(false);
	m_ButtonRemove.SetFaceColor(RGB(0x00, 0x00, 0x00));
	m_ButtonRemove.SetTextColor(RGB(0xff, 0xff, 0xff));
	m_ButtonRemove.SetWindowTextA("REMOVE");
	m_ButtonRemove.SetFont(&m_LucidaSans, true);

	m_ButtonExit.EnableWindowsTheming(false);
	m_ButtonExit.SetFaceColor(RGB(0x00, 0x00, 0x00));
	m_ButtonExit.SetTextColor(RGB(0xff, 0xff, 0xff));
	m_ButtonExit.SetWindowTextA("EXIT");
	m_ButtonExit.SetFont(&m_LucidaSans, true);

   if (m_TestMode) {
      m_ServerIPAddress = "192.168.10.125";
   }
   else {
      m_ServerIPAddress = "192.168.10.2";
   }

	JaLoadParametersAndSeating();
	//JaReadCallIns();

   m_StationList.SetFont(&m_Calibri);
   m_StationList.SetColumnWidth(117);

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      // if ((strlen(m_JA_MemberSeatingList[i].name) > 0) && callInVotes[m_JA_MemberSeatingList[i].seat - 1]) { // boardMap[i]]) {
      if ((strlen(m_JA_MemberSeatingList[i].name) > 0)) {
         m_StationList.AddString(m_JA_MemberSeatingList[i].name);
         int tmpItem = m_StationList.FindString(-1, m_JA_MemberSeatingList[i].name);
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         m_StationList.SetItemData(tmpItem, tmpSeat);
      }
   }
   m_StationList.SetFocus();
   m_StationList.SetCurSel(0);
   OnLbnSelchangeListStations(); // Selection changed

   JaReadDQFile();

   m_RTSList.SetFont(&m_Calibri);
   m_RTSList.SetColumnWidth(120);

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if ((strlen(m_JA_MemberSeatingList[i].name) > 0) && debateQueue[m_JA_MemberSeatingList[i].seat - 1]) { // boardMap[i]]) {
         m_RTSList.AddString(m_JA_MemberSeatingList[i].name);
         int tmpItem = m_RTSList.FindString(-1, m_JA_MemberSeatingList[i].name);
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         m_RTSList.SetItemData(tmpItem, tmpSeat);
      }
   }
   m_RTSList.SetFocus();
   m_RTSList.SetCurSel(0);
   OnLbnSelchangeListRTS(); // Selection changed

   // Specify the timer interval for checking CallIn file.
   UINT    nElapse = 1000;
   SetTimer(1, nElapse, NULL);

	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(true);

   Connect();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDebateQueueDlg::OnPaint()
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
HCURSOR CDebateQueueDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HBRUSH CDebateQueueDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr; // = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// Change any attributes of the DC here
	hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	switch (nCtlColor) {
	case CTLCOLOR_LISTBOX:
	{
		hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		//pDC->SetBkColor(RGB(0xff, 0xff, 0x00));
	}
	break;
	//case 3:
		//case 4:
	case CTLCOLOR_STATIC:
	{
		hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		//pDC->SetBkColor(nCtlColor);
	}
	break;
	default:
	{
		// m_Brush created in OnInitDialog
		hbr = m_Brush;
	}
	}

	return hbr;
}

void CDebateQueueDlg::Connect()
{
   if (AfxSocketInit() == FALSE) {
      AfxMessageBox("Failed to Initialize Sockets");
      return;
   }

   if (m_Client.Create() == FALSE) {
      MessageBox("Failed to Create Socket");
      return;
   }

   if (m_Client.Connect(m_ServerIPAddress, 1001) == FALSE) {
      MessageBox("Failed to Connect");
      return;
   }

   GetDlgItem(IDCANCEL)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(true);

   m_Client.pDlg = this;
}

void CDebateQueueDlg::OnLbnSelchangeListStations()
{
   m_Selection = m_StationList.GetCurSel();
   if (m_Selection < 0) {
      m_Selection = 0;
      return;
   }

   int seat = m_StationList.GetItemData(m_Selection);
   m_StationList.GetText(m_Selection, m_Name);

   m_Seat = seat;

   m_StationList.SetFocus();
}


void CDebateQueueDlg::OnLbnSelchangeListRTS()
{
   m_SelectionRTS = m_RTSList.GetCurSel();
   if (m_SelectionRTS < 0) {
      m_SelectionRTS = 0;
      return;
   }

   int seat = m_RTSList.GetItemData(m_SelectionRTS);
   m_RTSList.GetText(m_SelectionRTS, m_NameRTS);

   m_SeatRTS = seat;

   m_RTSList.SetFocus();
}


void CDebateQueueDlg::OnBnClickedButtonAdd()
{
   UpdateData(TRUE);

   if (m_VoteOpen) return;

   SockRemotePacket remotePacket;

   remotePacket.packetType = VM_REMOTE;
   remotePacket.remoteAction = VM_VOTE_RTS;
   remotePacket.index = m_Seat + 1000;

   m_Client.Send((char*)&remotePacket, sizeof(SockRemotePacket));

   UpdateData(TRUE);

   //OnLbnSelchangeListRTS();
}


void CDebateQueueDlg::OnBnClickedButtonRemove()
{
   UpdateData(TRUE);

   if (m_RTSList.GetCount() > 0) {
      SockRemotePacket remotePacket;

      remotePacket.packetType = VM_REMOTE;
      remotePacket.remoteAction = VM_VOTE_RTS;
      remotePacket.index = m_SeatRTS + 1000;

      m_Client.Send((char*)&remotePacket, sizeof(SockRemotePacket));
   }
}


void CDebateQueueDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}


int CompareMembers(const void* member1, const void* member2)
{
   const char apostrophe = (const char)'\'';

   CString name1 = ((LEX2*)member1)->name;
   CString name2 = ((LEX2*)member2)->name;

   // Remove apostrophe's prior to comparison
   name1.Remove(apostrophe);
   name2.Remove(apostrophe);

   // Comparing name in lower case guarantees telephone sort
   name1.MakeLower();
   name2.MakeLower();

   return(strcoll(name1, name2));
}

void CDebateQueueDlg::JaLoadParametersAndSeating(void)
{
   FILE* tmpFile;
   int fileError;

   char section[32];
   char key[32];
   char inBuff[81];
   char inBuff2[81];

   unsigned int i, seat;

   // Does JaRemote.Ini file exist?
   fileError = fopen_s(&tmpFile, JaRemoteIni, "r+");

   if (fileError != 0) {
      return;
   }
   else {
      fclose(tmpFile);
   }

   int k = 0;
   for (i = 0; i < MAX_SEATS; i++) {
      seat = i + 1;
      wsprintf(section, "MemberSeat_%03.3d", seat);

      wsprintf(key, "FullName");
      GetPrivateProfileString(section, key, "Hornsworthy", inBuff, 80, JaRemoteIni);

      wsprintf(key, "Name");
      GetPrivateProfileString(section, key, "Hornsworthy", inBuff2, 80, JaRemoteIni);

      if ((seat != 69) && (seat != 91)) {
         if (seat == 136) {
            strcpy_s(m_JA_MemberSeatingList[k].fullName, "Spkr. ");
            strncat_s(m_JA_MemberSeatingList[k].fullName, inBuff, 35);
            strcpy_s(m_JA_MemberSeatingList[k].name, "Spkr. ");
            strncat_s(m_JA_MemberSeatingList[k].name, inBuff2, 24);
         }
         else {
            strncpy_s(m_JA_MemberSeatingList[k].fullName, inBuff, 40);
            strncpy_s(m_JA_MemberSeatingList[k].name, inBuff2, 30);
         }
         m_JA_MemberSeatingList[k].seat = seat;
         k++;
      }
   }

   setlocale(LC_COLLATE, "en-029"); // set locale to english-carribean

   qsort((char*)m_JA_MemberSeatingList, (MAX_SEATS - 3), sizeof(LEX2), CompareMembers);

   // map members to board location
   for (i = 0; m_JA_MemberSeatingList[i].seat; i++) {
      boardMap[m_JA_MemberSeatingList[i].seat - 1] = i;
   }

   boardMap[135] = 133;                                   // shame on you, Mr. Speaker
   //boardMap[134] = 132;                                  // shame on you, Mr. Speaker
}


FILETIME CDebateQueueDlg::JaGetFileTime(CString fName)
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


BOOL CDebateQueueDlg::JaCallInFileChanged()
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

void CDebateQueueDlg::JaReadCallIns(void)
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
      TRACE("Unable to read CallIns file!");
      return;
   }
}

BOOL CDebateQueueDlg::JaDQFileChanged()
{
   BOOL retVal = false;

   static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(DebateQueueFile);

   if (ftWrite.dwLowDateTime != ftLastWrite.dwLowDateTime) {
      ftLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

void CDebateQueueDlg::JaReadDQFile(void)
{
   FILE* DQFile;
   int count;

   DQFile = fopen(DebateQueueFile, "rb");

   if (DQFile != NULL) {
      count = fread(debateQueue, 1, sizeof(debateQueue), DQFile);
      fclose(DQFile);
   }
   else {
      TRACE("Unable to open Debate Queue file!");
      return;
   }

   if (count != sizeof(debateQueue)) {
      TRACE("Unable to read Debate Queue file!");
      return;
   }
}

void CDebateQueueDlg::OnTimer(UINT_PTR nIDEvent)
{
   static bool bail = false;

   if (bail) goto BailOut;

   bail = true;

   //if (JaCallInFileChanged()) {
   //   JaReadCallIns();

   //   m_StationList.ResetContent();

   //   for (int i = 0; i < (MAX_SEATS - 2); i++) {
   //      // if ((strlen(m_JA_MemberSeatingList[i].name) > 0) && callInVotes[m_JA_MemberSeatingList[i].seat - 1]) { // boardMap[i]]) {
   //      if ((strlen(m_JA_MemberSeatingList[i].name) > 0)) {
   //         m_StationList.AddString(m_JA_MemberSeatingList[i].name);
   //         int tmpItem = m_StationList.FindString(-1, m_JA_MemberSeatingList[i].name);
   //         int tmpSeat = m_JA_MemberSeatingList[i].seat;
   //         m_StationList.SetItemData(tmpItem, tmpSeat);
   //         if (m_JA_MemberSeatingList[i].name == m_Name) {
   //            m_Selection = tmpItem;
   //         }
   //      }
   //   }
   //   m_StationList.SetFocus();

   //   if (m_Selection < m_StationList.GetCount()) {
   //      m_StationList.SetCurSel(m_Selection);
   //   }
   //   else {
   //      m_StationList.SetCurSel(0);
   //   }
   //}

   if (JaDQFileChanged()) {
      JaReadDQFile();

      m_RTSList.ResetContent();

      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if ((strlen(m_JA_MemberSeatingList[i].name) > 0) && debateQueue[m_JA_MemberSeatingList[i].seat - 1]) { // boardMap[i]]) {
            m_RTSList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_RTSList.FindString(-1, m_JA_MemberSeatingList[i].name);
            if (tmpItem != LB_ERR) {
               int tmpSeat = m_JA_MemberSeatingList[i].seat;
               m_RTSList.SetItemData(tmpItem, tmpSeat);
               if (m_JA_MemberSeatingList[i].name == m_NameRTS) {
                  m_SelectionRTS = tmpItem;
               }
            }
         }
      }
      m_RTSList.SetFocus();

      int count = m_RTSList.GetCount();

      if ((count > 0) && (m_SelectionRTS >= count)) {
         m_SelectionRTS = count - 1;
      }

      m_RTSList.SetCurSel(m_SelectionRTS);
  
      OnLbnSelchangeListRTS();
   }

   bail = false;

BailOut:
   CDialog::OnTimer(nIDEvent);
}