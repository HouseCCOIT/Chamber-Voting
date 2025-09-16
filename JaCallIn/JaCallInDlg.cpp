
// JaCallInDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "JaCallIn.h"
#include "JaCallInDlg.h"
#include "afxdialogex.h"
#include "alphasort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CJaCallInDlg dialog

CJaCallInDlg::CJaCallInDlg(CWnd* pParent /*=nullptr*/)
   : CDialog(IDD_JACALLIN_DIALOG, pParent)
{
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJaCallInDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_BUTTON_YEA, m_ButtonYea);
   DDX_Control(pDX, IDC_BUTTON_NAY, m_ButtonNay);
   DDX_Control(pDX, IDC_BUTTON_RESET, m_ButtonReset);
   DDX_Control(pDX, IDC_BUTTON_PAGE, m_ButtonPage);
   DDX_Control(pDX, IDC_BUTTON_LED_YEA, m_ButtonLEDYea);
   DDX_Control(pDX, IDC_BUTTON_LED_NAY, m_ButtonLEDNay);
   DDX_Control(pDX, IDC_BUTTON_LED_RESET, m_ButtonLEDReset);
   DDX_Control(pDX, IDC_BUTTON_LED_PAGE, m_ButtonLEDPage);
   DDX_Control(pDX, IDCANCEL, m_ButtonExit);
   DDX_Control(pDX, IDC_STATIC_FULL_NAME, m_FullName);
   DDX_Control(pDX, IDC_LIST_STATIONS, m_StationList);
   DDX_Control(pDX, IDC_STATIC_LOCK_FRAME_NAMES, m_CallInNames);
   DDX_Control(pDX, IDC_BUTTON_PRINT, m_ButtonPrint);
   DDX_Control(pDX, IDC_BUTTON_UPDATE, m_ButtonUpdate);
}

BEGIN_MESSAGE_MAP(CJaCallInDlg, CDialog)
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDC_BUTTON_YEA, &CJaCallInDlg::OnBnClickedButtonYea)
   ON_BN_CLICKED(IDC_BUTTON_NAY, &CJaCallInDlg::OnBnClickedButtonNay)
   ON_BN_CLICKED(IDC_BUTTON_RESET, &CJaCallInDlg::OnBnClickedButtonReset)
   ON_BN_CLICKED(IDCANCEL, &CJaCallInDlg::OnBnClickedCancel)
   ON_BN_CLICKED(IDC_BUTTON_PRINT, &CJaCallInDlg::OnBnClickedButtonPrint)
   ON_LBN_SELCHANGE(IDC_LIST_STATIONS, &CJaCallInDlg::OnLbnSelchangeListStations)
   ON_WM_CTLCOLOR()
   ON_WM_TIMER()
   ON_WM_MOUSEMOVE()

   ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CJaCallInDlg::OnBnClickedButtonUpdate)
END_MESSAGE_MAP()

LEX2 m_JA_MemberSeatingList[MAX_SEATS];

// TBD - Unify array sizes
unsigned char stationLock[MAX_VOTE_ARRAY];                 // != 0: bypass this vote station in polling sequence
unsigned char callInVotes[MAX_VOTE_ARRAY];
unsigned char remoteVotes[MAX_VOTE_ARRAY];
unsigned char currentVote[MAX_VOTE_ARRAY];

unsigned char boardMap[MAX_SEATS];           // vectors absolute vote station numbers to display board locations

CString JaRemoteIni = "V:\\JaRemote.Ini";

CString JaVoteDat = "V:\\JaVote.dat";
CString StationLockFile = "V:\\JaLock.dat";
CString CallInVotesFile = "V:\\JaCallInVotes.dat";
CString RemoteVotesFile = "V:\\JaRemoteVotes.dat";
CString m_JA_MemberNames = "V:\\MemberNames.txt";

bool voteOpen = true;

extern BOOL m_TestMode;

CString    m_JA_PrinterPrimary = "\\\\vote\\Printer1";
CString    m_JA_PrinterSecondary = "\\\\vote\\Printer2";
//CString    m_JA_PrinterPrimary = "\\\\EAKDEV2022\\Brother HL-L3280CDW series";
//CString    m_JA_PrinterSecondary = "\\\\EAKDEV2022\\Brother HL-L3280CDW series";
CString    m_JA_PrinterName = "LPT1:";
static char null_column[] = "       ";          // 7
static char null_name[] = "                 ";  // 17

unsigned int callInCount;

CMutex CallInMutex;

// CJaCallInDlg message handlers

BOOL CJaCallInDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);			// Set big icon
   SetIcon(m_hIcon, FALSE);		// Set small icon

   // TODO: Add extra initialization here

   m_Brush.CreateSolidBrush(RGB(0xda, 0xb2, 0x75));

   // Creates a 18-point-Courier-font
   m_Elephant.CreatePointFont(180, _T("Elephant"));

   m_LucidaSans.CreatePointFont(130, _T("Lucida Sans"));

   m_Calibri.CreatePointFont(100, _T("Calibri"));

   m_FullName.SetFont(&m_Elephant);
   m_FullName.SetBackColor(RGB(0x00, 0x00, 0x00));
   m_FullName.SetTextColor(RGB(0xda, 0xb2, 0x75));
   //m_FullName.

   m_ButtonYea.SetFaceColor(RGB(0x00, 0x00, 0x00));
   m_ButtonYea.SetTextColor(RGB(0xff, 0xff, 0xff));
   m_ButtonYea.SetWindowTextA("\n\nYEA");
   m_ButtonYea.SetFont(&m_LucidaSans, true);

   m_ButtonNay.SetFaceColor(RGB(0x00, 0x00, 0x00));
   m_ButtonNay.SetTextColor(RGB(0xff, 0xff, 0xff));
   m_ButtonNay.SetWindowTextA("\n\nNAY");
   m_ButtonNay.SetFont(&m_LucidaSans, true);

   m_ButtonReset.SetFaceColor(RGB(0x00, 0x00, 0x00));
   m_ButtonReset.SetTextColor(RGB(0xff, 0xff, 0xff));
   m_ButtonReset.SetWindowTextA("\n\nRESET");
   m_ButtonReset.SetFont(&m_LucidaSans, true);

   m_ButtonPage.SetFaceColor(RGB(0x00, 0x00, 0x00));
   m_ButtonPage.SetTextColor(RGB(0xff, 0xff, 0xff));
   m_ButtonPage.SetWindowTextA("\n\nPAGE");
   m_ButtonPage.SetFont(&m_LucidaSans, true);

   m_ButtonUpdate.SetFaceColor(RGB(0x00, 0x00, 0x00));
   m_ButtonUpdate.SetTextColor(RGB(0xff, 0xff, 0xff));
   m_ButtonUpdate.SetFont(&m_Calibri, true);

   m_ButtonPrint.SetFaceColor(RGB(0x00, 0x00, 0x00));
   m_ButtonPrint.SetTextColor(RGB(0xff, 0xff, 0xff));
   m_ButtonPrint.SetFont(&m_Calibri, true);

   m_ButtonExit.SetFaceColor(RGB(0x00, 0x00, 0x00));
   m_ButtonExit.SetTextColor(RGB(0xff, 0xff, 0xff));
   m_ButtonExit.SetFont(&m_Calibri, true);

   m_ButtonLEDYea.EnableWindowsTheming(FALSE);
   m_ButtonLEDYea.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
   m_ButtonLEDYea.m_bTransparent = false;
   m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E), true);
   m_ButtonLEDYea.SetTextColor(RGB(0, 0, 0));

   m_ButtonLEDNay.EnableWindowsTheming(FALSE);
   m_ButtonLEDNay.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
   m_ButtonLEDNay.m_bTransparent = false;
   m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09), true);
   m_ButtonLEDNay.SetTextColor(RGB(0, 0, 0));

   m_ButtonLEDReset.EnableWindowsTheming(FALSE);
   m_ButtonLEDReset.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
   m_ButtonLEDReset.m_bTransparent = false;
   m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e), true);
   m_ButtonLEDReset.SetTextColor(RGB(0, 0, 0));

   m_ButtonLEDPage.EnableWindowsTheming(FALSE);
   m_ButtonLEDPage.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
   m_ButtonLEDPage.m_bTransparent = false;
   m_ButtonLEDPage.SetFaceColor(RGB(0x83, 0x50, 0x0e), true);
   m_ButtonLEDPage.SetTextColor(RGB(0, 0, 0));

   m_CallInNames.SetWindowTextA("Call In Names");

   for (int i = 0; i < sizeof(remoteVotes); i++) {
      remoteVotes[i] = 0;
   }
   for (int i = 0; i < sizeof(callInVotes); i++) {
      callInVotes[i] = 0;
   }

   JaLoadParametersAndSeating();
   JaReadCallIns();
   JaReadCurrentVote();

   m_StationList.SetFont(&m_LucidaSans);
   m_StationList.SetColumnWidth(160);

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if ((strlen(m_JA_MemberSeatingList[i].name) > 0) && callInVotes[m_JA_MemberSeatingList[i].seat - 1]) { // boardMap[i]]) {
         m_StationList.AddString(m_JA_MemberSeatingList[i].name);
         int tmpItem = m_StationList.FindString(-1, m_JA_MemberSeatingList[i].name);
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         m_StationList.SetItemData(tmpItem, tmpSeat);
      }
   }

   if (m_TestMode) {
      //m_ServerIPAddress = "192.168.10.125";
      m_ServerIPAddress = "127.0.0.1";
   }
   else {
      m_ServerIPAddress = "192.168.10.2";
   }

   // Specify the timer interval for checking CallIn file.
   UINT    nElapse = 2000;
   SetTimer(1, nElapse, NULL);

   JaReadCurrentVote();

   Connect();
   ConnectToVM();

   if (m_StationList.GetCount() > 0) {
      m_StationList.SetFocus();
      m_StationList.SetCurSel(0);
      //m_Seat = m_StationList.GetItemData(0);  // Set to first name in callin list on load //TBD
      OnLbnSelchangeListStations(); // Selection changed
   }

   return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJaCallInDlg::OnPaint()
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
HCURSOR CJaCallInDlg::OnQueryDragIcon()
{
   return static_cast<HCURSOR>(m_hIcon);
}


void CJaCallInDlg::Connect()
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
   GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(false);
   GetDlgItem(IDC_BUTTON_PRINT)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_YEA)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_NAY)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);

   m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
   m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
   m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));

   m_Client.pDlg = this;
}

void CJaCallInDlg::ConnectToVM()
{
   if (AfxSocketInit() == FALSE) {
      AfxMessageBox("Failed to Initialize Sockets");
      return;
   }

   if (m_VMClient.Create() == FALSE) {
      MessageBox("Failed to Create VM Socket");
      return;
   }

   if (m_VMClient.Connect(m_ServerIPAddress, 1004) == FALSE) {
      int error = GetLastError();
      MessageBox("Failed to Connect to VM");
      return;
   }

   m_VMClient.pDlg = this;
}

void CJaCallInDlg::Disconnect()
{
   m_Client.ShutDown(2);  //Shutdown both send and receive
   m_Client.Close();

   m_VMClient.ShutDown(2);  //Shutdown both send and receive
   m_VMClient.Close();

   GetDlgItem(IDCANCEL)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(false);
   GetDlgItem(IDC_BUTTON_PRINT)->EnableWindow(false);
   GetDlgItem(IDC_BUTTON_YEA)->EnableWindow(false);
   GetDlgItem(IDC_BUTTON_NAY)->EnableWindow(false);
   GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);

   m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
   m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
   m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
}


void CJaCallInDlg::OnBnClickedCancel()
{
   m_Client.ShutDown(2);  //Shutdown both send and receive
   m_Client.Close();

   m_VMClient.ShutDown(2);  //Shutdown both send and receive
   m_VMClient.Close();

   CDialog::OnCancel();
}


void CJaCallInDlg::OnBnClickedButtonYea()
{
   UpdateData(TRUE);

   SockRemotePacket remotePacket;

   remotePacket.packetType = VM_REMOTE;
   remotePacket.remoteAction = VM_VOTE_YEA;
   remotePacket.index = m_Seat + 1000;

   m_Client.Send((char*)&remotePacket, sizeof(SockRemotePacket));
}


void CJaCallInDlg::OnBnClickedButtonNay()
{
   UpdateData(TRUE);

   SockRemotePacket remotePacket;

   remotePacket.packetType = VM_REMOTE;
   remotePacket.remoteAction = VM_VOTE_NAY;
   remotePacket.index = m_Seat + 1000;

   m_Client.Send((char*)&remotePacket, sizeof(SockRemotePacket));
}


void CJaCallInDlg::OnBnClickedButtonReset()
{
   UpdateData(TRUE);

   SockRemotePacket remotePacket;

   remotePacket.packetType = VM_REMOTE;
   remotePacket.remoteAction = VM_VOTE_RESET;
   remotePacket.index = m_Seat + 1000;

   m_Client.Send((char*)&remotePacket, sizeof(SockRemotePacket));
}


void CJaCallInDlg::ButtonReset()
{
   for (unsigned int i = 0; i < MAX_SEATS; i++) {
      currentVote[i] = VM_VOTE_RESET;
   }

   GetDlgItem(IDCANCEL)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(false);
   GetDlgItem(IDC_BUTTON_PRINT)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_YEA)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_NAY)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);

   m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
   m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
   m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));

   voteOpen = false;
}


void CJaCallInDlg::UpdateButtons(SockRemotePacket* SockData)
{
   CString sDisplay;
   CString sTmp;

   sDisplay.Format("%x : %x", SockData->packetType, SockData->response);

   if (SockData->packetType == VM_RESPONSE) {
      switch (SockData->response) {
      case VM_VOTE_YEA_ACK:
         m_ButtonLEDYea.SetFaceColor(RGB(0, 255, 0), true);
         m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
         m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
         m_ButtonLEDYea.SetFocus();
         //m_StationList.GetWindowTextA(sTmp);
         //sTmp = "Y " + sTmp;
         //m_StationList.SetWindowTextA(sTmp);
         currentVote[m_Seat - 1] = VM_VOTE_YEA;
         voteOpen = true;
         break;
      case VM_VOTE_NAY_ACK:
         m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
         m_ButtonLEDNay.SetFaceColor(RGB(255, 0, 0));
         m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
         m_ButtonLEDNay.SetFocus();
         //m_StationList.GetWindowTextA(sTmp); //TBD - How to set color
         //sTmp = "N " + sTmp;
         //m_StationList.SetWindowTextA(sTmp);
         currentVote[m_Seat - 1] = VM_VOTE_NAY;
         voteOpen = true;
         break;
      case VM_VOTE_RESET_ACK:
         m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
         m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
         m_ButtonLEDReset.SetFaceColor(RGB(255, 255, 0));
         m_ButtonLEDReset.SetFocus();
         //m_StationList.GetWindowTextA(sTmp);
         //sTmp = "R " + sTmp;
         //m_StationList.SetWindowTextA(sTmp);
         currentVote[m_Seat - 1] = VM_VOTE_RESET;
         voteOpen = true;
         break;
      case VM_VOTE_YEA_NAK:
         m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
         m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
         m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
         m_ButtonLEDYea.SetFocus();
         break;
      case VM_VOTE_NAY_NAK:
         m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
         m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
         m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
         m_ButtonLEDNay.SetFocus();
         break;
      case VM_VOTE_RESET_NAK:
         m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
         m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
         m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
         m_ButtonLEDReset.SetFocus();
         break;
      default:
         break;
      }
   }
   if (SockData->packetType == VM_VM) {
      switch (SockData->remoteAction) {
      case VM_VOTE_OPEN_REMOTE:
         GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(true);
         m_VoteOpen = true;
         break;
      case VM_VOTE_RESET_REMOTE:
         ButtonReset();
         m_VoteOpen = false;
         break;
      case VM_VOTE_DISCONNECT_REMOTE:
         Disconnect();
         break;
      case VM_VOTE_SETINDEX:
         m_VoteOpen = SockData->index;
         if (m_VoteOpen) {
            GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(true);
         }
         break;
      default:
         break;
      }
   }
   m_StationList.SetFocus();
}

void CJaCallInDlg::UpdateCurrentVote(SockPacket votePacket)
{
   for (int i = 0; i < 138 - 2; i++) {
      if ((i != 68) && (i != 90)) { // 69 & 91 excluded from voting
         currentVote[i] = votePacket.dataByte[i];
      }
   }
}

HBRUSH CJaCallInDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


void CJaCallInDlg::OnLbnSelchangeListStations()
{
   //int tmpItem = m_StationList.GetCurSel();
   m_Selection = m_StationList.GetCurSel();

   if (m_Selection < 0) {
      m_Selection = 0;
      return;
   }

   int seat = m_StationList.GetItemData(m_Selection);
   CString fullName;
   m_StationList.GetText(m_Selection, m_Name);   //TBD fix full name
   fullName = m_JA_MemberSeatingList[boardMap[seat-1]].fullName;

   m_FullName.SetWindowTextA(fullName);
   m_Seat = seat;

  // m_StationList.SetTextColor(RGB(0x80, 0x00, 0x00)); //    GetItemData(m_Selection).S;

   if (voteOpen) {
      // Reset LED's
      switch (currentVote[m_Seat - 1]) {
      case VM_VOTE_YEA:
         m_ButtonLEDYea.SetFaceColor(RGB(0, 255, 0), true);
         m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
         m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
         m_ButtonLEDYea.SetFocus();
         break;
      case VM_VOTE_NAY:
         m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
         m_ButtonLEDNay.SetFaceColor(RGB(255, 0, 0));
         m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
         m_ButtonLEDNay.SetFocus();
         break;
      case VM_VOTE_RESET:
         m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
         m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
         m_ButtonLEDReset.SetFaceColor(RGB(255, 255, 0));
         m_ButtonLEDReset.SetFocus();
         break;
      default:
         break;
      }
      m_StationList.SetFocus();
   }
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

// Print member names after qsort to ensure correct order
// (for development and debugging only)
void PrintMemberNames()
{
   FILE* lStream;

   if ((lStream = fopen(m_JA_MemberNames, "a+")) != NULL) {
      // Print member names in order
      for (int i = 0; m_JA_MemberSeatingList[i].seat; i++) {
         boardMap[m_JA_MemberSeatingList[i].seat - 1] = i;
         fprintf(lStream, "%s\n", m_JA_MemberSeatingList[i].name);
      }

      fflush(lStream);
      fclose(lStream);
   }

   return;
}

void CJaCallInDlg::JaLoadParametersAndSeating(void)
{
   FILE* tmpFile;
   int fileError;

   char section[32];
   char key[32];
   char inBuff[81];
   char inBuff2[81];

   unsigned int i, j, seat;

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
      for (j = strlen(inBuff2); j < 30; j++) {
         strcat_s(inBuff2, " ");
      }

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

   //PrintMemberNames(); 

   // map members to board location
   for (i = 0; m_JA_MemberSeatingList[i].seat; i++) {
      boardMap[m_JA_MemberSeatingList[i].seat - 1] = i;
   }

   boardMap[135] = 133;                                   // shame on you, Mr. Speaker
   //boardMap[134] = 132;                                  // shame on you, Mr. Speaker
}


FILETIME CJaCallInDlg::JaGetFileTime(CString fName)
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

BOOL CJaCallInDlg::JaVoteFileChanged()
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

void CJaCallInDlg::JaReadCurrentVote()
{
   FILE* voteFile;
   int count;
   unsigned char tmpVote[MAX_VOTE_ARRAY];

   voteFile = fopen(JaVoteDat, "rb");

   if (voteFile != NULL) {
      count = fread(tmpVote, 1, sizeof(tmpVote), voteFile);
      fclose(voteFile);
   }
   else {
      TRACE("Unable to open Votes file!");
      return;
   }

   if (count != sizeof(tmpVote)) {
      TRACE("Unable to read Votes file! Count Off");
      return;
   }

   // successful read of current vote
   for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
      currentVote[i] = tmpVote[i];
   }
}

BOOL CJaCallInDlg::JaCallInFileChanged()
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

void CJaCallInDlg::JaReadCallIns(void)
{
   FILE* callInsFile;
   int count;
   unsigned char tmpCallIns[MAX_VOTE_ARRAY];

   callInsFile = fopen(CallInVotesFile, "rb");

   if (callInsFile != NULL) {
      count = fread(tmpCallIns, 1, sizeof(tmpCallIns), callInsFile);
      fclose(callInsFile);
   }
   else {
      TRACE("Unable to open CallIns file!");
      return;
   }

   if (count != sizeof(tmpCallIns)) {
      TRACE("Unable to read CallIns file!");
      return;
   }

   for (int i = 0; i < sizeof(tmpCallIns); i++) {
      callInVotes[i] = tmpCallIns[i];
   }
}

BOOL CJaCallInDlg::JaRemoteFileChanged()
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

void CJaCallInDlg::JaReadRemotes(void)
{
   FILE* remotesFile;
   int count;
   unsigned char tmpRemotes[MAX_VOTE_ARRAY];

   remotesFile = fopen(RemoteVotesFile, "rb");

   if (remotesFile != NULL) {
      count = fread(tmpRemotes, 1, sizeof(tmpRemotes), remotesFile);
      fclose(remotesFile);
   }
   else {
      TRACE("Unable to open Remotes file!");
      return;
   }

   if (count != sizeof(tmpRemotes)) {
      TRACE("Unable to read Remotes file!");
      return;
   }

   for (int i = 0; i < sizeof(tmpRemotes); i++) {
      remoteVotes[i] = tmpRemotes[i];
   }
}

BOOL CJaCallInDlg::JaLockFileChanged()
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

void CJaCallInDlg::JaReadLocks()
{
   FILE* lockFile;
   int count;
   unsigned char tmpLock[MAX_VOTE_ARRAY];                 // != 0: bypass this vote station in polling sequence

   lockFile = fopen(StationLockFile, "rb");

   if (lockFile != NULL) {
      count = fread(tmpLock, 1, sizeof(tmpLock), lockFile);
      fclose(lockFile);
   }
   else {
      TRACE("Unable to open Lock file!");
      return;
   }

   if (count != sizeof(tmpLock)) {
      TRACE("Unable to read Lock file!");
      return;
   }

   for (int i = 0; i < sizeof(tmpLock); i++) {
      stationLock[i] = tmpLock[i];
   }
}

void CJaCallInDlg::OnTimer(UINT_PTR nIDEvent)
{
   CSingleLock CallInSem(&CallInMutex);

   static bool bail = false;

   if (bail) goto BailOut;

   CallInSem.Lock(1000);  // Lock for up to 1 second

   bail = true;

   if (JaLockFileChanged()) {
      JaReadLocks();
   }

   if (JaCallInFileChanged()) {
      JaReadCallIns();

      m_StationList.ResetContent();

      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if ((strlen(m_JA_MemberSeatingList[i].name) > 0) && callInVotes[m_JA_MemberSeatingList[i].seat - 1]) { // boardMap[i]]) {
            m_StationList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_StationList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_StationList.SetItemData(tmpItem, tmpSeat);
            if (m_JA_MemberSeatingList[i].name == m_Name) {
               m_Selection = tmpItem;
            }
         }
      }
      m_StationList.SetFocus();

      if (m_Selection < m_StationList.GetCount()) {
         m_StationList.SetCurSel(m_Selection);
      }
      else {
         m_StationList.SetCurSel(0);
      }
   }

   if (JaRemoteFileChanged()) {
      JaReadRemotes();
   }

   bail = false;

   CallInSem.Unlock();

BailOut:
   CDialog::OnTimer(nIDEvent);
}


void CJaCallInDlg::OnMouseMove(UINT nFlags, CPoint point)
{
   //if (!m_TestMode) {
      //RECT rcClip;

      //GetWindowRect(&rcClip);

      //// TBD = Uncomment for release
      //if (point.x > rcClip.right)  point.x = rcClip.right;
      //if (point.x < rcClip.left)   point.x = rcClip.left;
      //if (point.y < rcClip.top)    point.y = rcClip.top;
      //if (point.y > rcClip.bottom) point.y = rcClip.bottom;

      //::ClipCursor(&rcClip);
   //}

   CDialog::OnMouseMove(nFlags, point);
}

_locale_t locale;

void PrintLine(FILE* stream, char* s)
{
   //const size_t cSize = strlen(s) + 1;
   wchar_t wc[80];
   mbstowcs(wc, s, 80);

   fwprintf(stream, L"    %s", wc);
}

void PrintLineW(FILE* stream, CString s)
{
   //const size_t cSize = strlen(s) + 1;
   //wchar_t wc[80];
   //mbstowcs(wc, s, 80);

   fwprintf(stream, L"    %s", s);
}

void PrintCallIns(unsigned int numPages)
{
   char  CallInName[3][17];
   unsigned int i, j, k;
   char buffer[BUFFER_LENGTH];
   CString bufferw;


   // Create a locale object representing the German (Switzerland) locale
   //locale = _create_locale(LC_ALL, "en-029");

   //SetConsoleOutputCP(852);
   //SetConsoleCP(852);
   setlocale(LC_ALL, "en-029"); // set locale to english-carribean

   CString CallInTitle1 = "                                                                               ";
   CString CallInTitle2 = "                                                                               ";
   CString CallInTitle3 = "                                                                               ";
   CString CallInTitle4 = "                                                                               ";
   CString CallInTitle5 = "                                                                               ";

   FILE* lStream;
   unsigned int pageNum;

   // Time
   char       HouseDate[20];
   struct tm* HouseTime;
   time_t     tmpTime = time(&tmpTime);
   HouseTime = localtime(&tmpTime);

   strftime(HouseDate, 80, "%m/%d/%Y - %H:%M", HouseTime);

   // m_JA_PrinterName = m_JA_PrinterSecondary;

   bufferw = CString(L"MINNESOTA HOUSE OF REPRESENTATIVES - Pérez-Vega\n");

   if ((lStream = fopen(m_JA_PrinterName, "a+")) != NULL) {
      PrintLineW(lStream, bufferw);
   }

   return;

   for (i = 0, j = (70 - strlen(buffer)) / 2; i < strlen(buffer); i++) {
      CallInTitle1.SetAt(j++, buffer[i]);
   }
   sprintf(buffer, "Chief Clerk's Office\n\n");
   for (i = 0, j = (70 - strlen(buffer)) / 2; i < strlen(buffer); i++) {
      CallInTitle2.SetAt(j++, buffer[i]);
   }
   sprintf(buffer, "Date: %s\n\n", HouseDate);
   for (i = 0, j = (70 - strlen(buffer)) / 2; i < strlen(buffer); i++) {
      CallInTitle3.SetAt(j++, buffer[i]);
   }
   sprintf(buffer, "SPEAKER: \n\n");
   for (i = 0; i < strlen(buffer); i++) {
      CallInTitle4.SetAt(i, buffer[i]);
   }
   sprintf(buffer, "The Following %d members have requested to vote by calling in for today:\n\n", callInCount);
   for (i = 0, j = 3; i < strlen(buffer); i++) {
      CallInTitle5.SetAt(j++, buffer[i]);
   }

   if ((lStream = fopen(m_JA_PrinterName, "a+")) != NULL) {

      for (pageNum = 1; pageNum <= numPages; pageNum++) {

         sprintf(buffer, "\n\n\n\n");
         PrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", CallInTitle1.GetString());
         PrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", CallInTitle2.GetString());
         PrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", CallInTitle3.GetString());
         PrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", CallInTitle4.GetString());
         PrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", CallInTitle5.GetString());
         PrintLine(lStream, buffer);

         // print spacing line with vertical separators
         sprintf(buffer, "%s%s: %s%s: %s%s\n", null_column, null_name, null_column, null_name, null_column, null_name);
         PrintLine(lStream, buffer);

         // print the member's results
         for (i = 0; i < PRINTOUT_ROWS; i++) {
            for (j = 0; j < PRINTOUT_COLS; j++) {
               k = (j * PRINTOUT_ROWS) + i;
               if (m_JA_MemberSeatingList[k].seat && callInVotes[m_JA_MemberSeatingList[k].seat - 1]) {
                  strncpy_s(CallInName[j], m_JA_MemberSeatingList[k].name, 16);
                  CallInName[j][NAME_PRINTABLE] = '\0';
               }
               else if (m_JA_MemberSeatingList[k].seat && ((currentVote[m_JA_MemberSeatingList[k].seat - 1] == 0) && (stationLock[m_JA_MemberSeatingList[k].seat - 1] == 0))) {
                  strncpy_s(CallInName[j], m_JA_MemberSeatingList[k].name, 16);
                  CallInName[j][NAME_PRINTABLE] = '\0';
               }
               else {
                  strcpy_s(CallInName[j], null_name);
               }
            }

            sprintf(buffer, "%s%s: %s%s: %s%s\n", null_column, CallInName[0], null_column, CallInName[1], null_column, CallInName[2]);
            PrintLine(lStream, buffer);
         }

         fflush(lStream);
      }

      fclose(lStream);
   }
   else {
      //wsprintf(outBuff, "ERROR opening printer device %s!", m_JA_PrinterName);
      //JaUpdateStatusWindow(outBuff);
   }

   return;
}

void CJaCallInDlg::OnBnClickedButtonPrint()
{
   callInCount = 0;
   for (int i = 0; i < MAX_SEATS; i++) {
      if (i != 68 && i != 90) {
         if (callInVotes[i]) callInCount++;
      }
   }

   m_JA_PrinterName = m_JA_PrinterPrimary;

   PrintCallIns(1);

   m_JA_PrinterName = m_JA_PrinterSecondary;

  // PrintCallIns(2);
}

void CJaCallInDlg::OnBnClickedButtonUpdate()
{
   CSingleLock CallInSem(&CallInMutex);
   CallInSem.Lock(1000);  // Lock for up to 1 second

   // Read last vote written, just to be sure.
   JaReadCurrentVote();

   for (int i = 0; i < sizeof(callInVotes); i++) {
      if (callInVotes[i] == 0) {
         // if ((currentVote[i] == 0) && (remoteVotes[i] == 1) && (stationLock[i] == 0)) {
         if ((currentVote[i] == 0) && (stationLock[i] == 0)) {
            callInVotes[i] = 1;
            remoteVotes[i] = 0;
         }
      }
   }

   JaWriteCallIns(CallInVotesFile);
   JaWriteRemotes(RemoteVotesFile);

   CallInSem.Unlock();
}

void CJaCallInDlg::JaWriteRemotes(CString remoteFilename)
{
   FILE* remoteFile;
   int count;

   if (!(remoteFile = fopen(remoteFilename, "wb"))) {
      return;
   }

   count = fwrite(remoteVotes, 1, sizeof(remoteVotes), remoteFile);

   fclose(remoteFile);
}

void CJaCallInDlg::JaWriteCallIns(CString callinFilename)
{
   FILE* callinFile;
   int count;

   if (!(callinFile = fopen(callinFilename, "wb"))) {
      return;
   }

   callInCount = 0;
   for (int i = 0; i < MAX_SEATS; i++) {
      if (i != 68 && i != 90) {
         if (callInVotes[i]) callInCount++;
      }
   }

   count = fwrite(callInVotes, 1, sizeof(callInVotes), callinFile);

   fclose(callinFile);
}