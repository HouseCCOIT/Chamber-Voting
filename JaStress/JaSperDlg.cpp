
// CJaSperDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "JaSper.h"
#include "JaSperDlg.h"
#include "afxdialogex.h"
#include "alphasort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CJaSperDlg dialog



CJaSperDlg::CJaSperDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_JASTRESS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJaSperDlg::DoDataExchange(CDataExchange* pDX)
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
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_ButtonConnect);
	DDX_Control(pDX, IDC_BUTTON_DISCONNECT, m_ButtonDisconnect);
	DDX_Control(pDX, IDCANCEL2, m_ButtonExit);
	DDX_Control(pDX, IDC_STATIC_FULL_NAME, m_FullName);
	DDX_Control(pDX, IDC_LIST_STATIONS, m_StationList);
	DDX_Control(pDX, IDC_STATIC_LOCK_FRAME_NAMES, m_CallInNames);
}

BEGIN_MESSAGE_MAP(CJaSperDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_YEA, &CJaSperDlg::OnBnClickedButtonYea)
	ON_BN_CLICKED(IDC_BUTTON_NAY, &CJaSperDlg::OnBnClickedButtonNay)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CJaSperDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CJaSperDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDCANCEL2, &CJaSperDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CJaSperDlg::OnBnClickedButtonDisconnect)
	ON_WM_CTLCOLOR()
	ON_LBN_SELCHANGE(IDC_LIST_STATIONS, &CJaSperDlg::OnLbnSelchangeListStations)
	ON_WM_TIMER()
END_MESSAGE_MAP()

LEX2 m_JA_MemberSeatingList[MAX_SEATS];

unsigned char RemoteVotes[MAX_VOTE_ARRAY];
unsigned char currentVote[MAX_VOTE_ARRAY];

unsigned char boardMap[MAX_SEATS];           // vectors absolute vote station numbers to display board locations

//CString JaRemoteIni = ".\\JaRemote.Ini";
//
//CString RemoteVotesFile = ".\\JaRemoteVotes.dat";

CString JaRemoteIni = "V:\\JaRemote.Ini";

CString RemoteVotesFile = "V:\\JaRemoteVotes.dat";

// CJaStressDlg message handlers

BOOL CJaSperDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

   m_Brush.CreateSolidBrush(RGB(0xdb, 0x88, 0x20));

   // Creates a 18-point-Courier-font
   m_Elephant.CreatePointFont(180, _T("Elephant"));

   m_LucidaSans.CreatePointFont(130, _T("Lucida Sans"));

   m_Calibri.CreatePointFont(100, _T("Calibri"));

   m_FullName.SetFont(&m_Elephant);
   m_FullName.SetBackColor(RGB(0x00, 0x00, 0x00));
   m_FullName.SetTextColor(RGB(0xdb, 0x88, 0x20));
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

   m_ButtonConnect.SetFaceColor(RGB(0x00, 0x00, 0x00));
   m_ButtonConnect.SetTextColor(RGB(0xff, 0xff, 0xff));
   m_ButtonConnect.SetFont(&m_Calibri, true);

   m_ButtonDisconnect.SetFaceColor(RGB(0x00, 0x00, 0x00));
   m_ButtonDisconnect.SetTextColor(RGB(0xff, 0xff, 0xff));
   m_ButtonDisconnect.SetFont(&m_Calibri, true);

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

   m_CallInNames.SetWindowTextA("Remote Names");

   JaLoadParametersAndSeating();
   JaReadRemotes();

   m_StationList.SetFont(&m_LucidaSans);
   m_StationList.SetColumnWidth(160);

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if ((strlen(m_JA_MemberSeatingList[i].name) > 0) && RemoteVotes[m_JA_MemberSeatingList[i].seat - 1]) { // boardMap[i]]) {
         m_StationList.AddString(m_JA_MemberSeatingList[i].name);
         int tmpItem = m_StationList.FindString(-1, m_JA_MemberSeatingList[i].name);
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         m_StationList.SetItemData(tmpItem, tmpSeat);
      }
   }
   m_StationList.SetFocus();
   m_StationList.SetCurSel(0);
   OnLbnSelchangeListStations(); // Selection changed

   // DEV MODE
   //m_ServerIPAddress = "192.168.10.125";
   //m_ServerIPAddress = "10.60.5.250";
   m_ServerIPAddress = "127.0.0.1";
   //m_ServerIPAddress = "192.168.10.2";

   for (int i = 0; i < MAX_SEATS; i++) {
      m_Client[i].m_InUse = false;
   }

   if (AfxSocketInit() == FALSE)
   {
      AfxMessageBox("Failed to Initialize Sockets");
   }

   // Specify the timer interval for checking CallIn file.
   UINT    nElapse = 1000;
   SetTimer(1, nElapse, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CJaSperDlg::GetCurrentVote()
{
   UpdateData(TRUE);

   SockRemotePacket remotePacket;

   remotePacket.packetType = VM_REMOTE;
   remotePacket.remoteAction = VM_VOTE_GET;
   //remotePacket.index = m_Seat;

   for (int i = 0; i < MAX_SEATS; i++) {
      if (m_Client[i].m_InUse) {
         remotePacket.index = i + 1;
         m_Client[i].Send((char*)&remotePacket, sizeof(SockRemotePacket));
      }
   }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJaSperDlg::OnPaint()
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
HCURSOR CJaSperDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CJaSperDlg::OnBnClickedButtonConnect()
{
   if (AfxSocketInit() == FALSE)
   {
      AfxMessageBox("Failed to Initialize Sockets");
      return;
   }

   //if (m_Client[0].Create() == FALSE)
   //{
   //   MessageBox("Failed to Create Socket");
   //   return;

   //}


   ////if (m_Client[0].Connect("192.168.10.140", 1001) == FALSE)
   //if (m_Client[0].Connect(m_ServerIPAddress, 1001) == FALSE)
   //   //if (m_Client[0].Connect("192.168.1.45", 1001) == FALSE)
   //{
   //   MessageBox("Failed to Connect");
   //   return;

   //}

   GetDlgItem(IDCANCEL2)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(false);
   GetDlgItem(IDC_BUTTON_YEA)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_NAY)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(true);

   m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
   m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
   m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));

  // m_Client[0].pDlg = this;
}


void CJaSperDlg::OnBnClickedButtonDisconnect()
{
   m_Client[0].ShutDown(2);  //Shutdown both send and receive
   m_Client[0].Close();

   GetDlgItem(IDCANCEL2)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_YEA)->EnableWindow(false);
   GetDlgItem(IDC_BUTTON_NAY)->EnableWindow(false);
   GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
   GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(false);

   m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
   m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
   m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
}


void CJaSperDlg::OnBnClickedCancel()
{
   m_Client[0].ShutDown(2);  //Shutdown both send and receive
   m_Client[0].Close();

   CDialog::OnCancel();
}


void CJaSperDlg::OnBnClickedButtonYea()
{
   UpdateData(TRUE);

   SockRemotePacket remotePacket;

   remotePacket.packetType = VM_REMOTE;
   remotePacket.remoteAction = VM_VOTE_YEA;
   //remotePacket.index = m_Seat;

   for (int i = 0; i < MAX_SEATS; i++) {
      if (m_Client[i].m_InUse) {
         remotePacket.index = i+1;
         m_Client[i].Send((char*)&remotePacket, sizeof(SockRemotePacket));
      }
   }
}


void CJaSperDlg::OnBnClickedButtonNay()
{
   UpdateData(TRUE);

   SockRemotePacket remotePacket;

   remotePacket.packetType = VM_REMOTE;
   remotePacket.remoteAction = VM_VOTE_NAY;
  //remotePacket.index = m_Seat;

   for (int i = 0; i < MAX_SEATS; i++) {
      if (m_Client[i].m_InUse) {
         remotePacket.index = i+1;
         m_Client[i].Send((char*)&remotePacket, sizeof(SockRemotePacket));
      }
   }
}


void CJaSperDlg::OnBnClickedButtonReset()
{
   UpdateData(TRUE);

   SockRemotePacket remotePacket;

   remotePacket.packetType = VM_REMOTE;
   remotePacket.remoteAction = VM_VOTE_RESET;
   //remotePacket.index = m_Seat;

   for (int i = 0; i < MAX_SEATS; i++) {
      if (m_Client[i].m_InUse) {
         remotePacket.index = i+1;
         m_Client[i].Send((char*)&remotePacket, sizeof(SockRemotePacket));
      }
   }
}

bool voteOpen = false;

void CJaSperDlg::ButtonReset()
{
   for (unsigned int i = 0; i < MAX_SEATS; i++) {
      currentVote[i] = VM_VOTE_RESET;
   }

   GetDlgItem(IDCANCEL2)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(false);
   GetDlgItem(IDC_BUTTON_YEA)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_NAY)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
   GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(false);

   m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
   m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
   m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));

   voteOpen = false;
}


void CJaSperDlg::UpdateButtons(SockRemotePacket* SockData)
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
      case VM_VOTE_RESET_REMOTE:
         ButtonReset();
         break;
      case VM_VOTE_DISCONNECT_REMOTE:
         OnBnClickedButtonDisconnect();
         break;
      case VM_VOTE_SETINDEX:
         
         break;
      default:
         break;
      }
   }
  // TBD m_StationList.SetFocus();
}

HBRUSH CJaSperDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


void CJaSperDlg::OnLbnSelchangeListStations()
{
   //int tmpItem = m_StationList.GetCurSel();
   m_Selection = m_StationList.GetCurSel();
   int seat = m_StationList.GetItemData(m_Selection);
   CString name;
   //m_StationList.GetText(tmpItem, name);   //TBD fix full name
   name = m_JA_MemberSeatingList[boardMap[seat - 1]].fullName;

   m_FullName.SetWindowTextA(name);
   m_Seat = seat;

   for (int i = 0; i < MAX_SEATS; i++) {
      if (m_Client[i].m_InUse) {
         m_Client[i].ShutDown(2);  //Shutdown both send and receive
         m_Client[i].Close();
         m_Client[i].m_InUse = false;
      }
   }

   for (int i = 0; i < m_StationList.GetCount(); i++) {
      if (m_StationList.GetSel(i)) {
         int seat = m_StationList.GetItemData(i);
         if (m_Client[seat - 1].Create() == FALSE) {
            MessageBox("Failed to Create Socket");
         }

         if (m_Client[seat - 1].Connect(m_ServerIPAddress, 1001) == FALSE) {
            MessageBox("Failed to Connect");
         }
         m_Client[seat - 1].m_Index = seat;
         m_Client[seat - 1].pDlg = this;
         m_Client[seat - 1].m_InUse = true;
      }
   }

   GetCurrentVote();

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

void CJaSperDlg::JaLoadParametersAndSeating(void)
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


FILETIME CJaSperDlg::JaGetFileTime(CString fName)
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

BOOL CJaSperDlg::JaRemoteFileChanged()
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

void CJaSperDlg::JaReadRemotes(void)
{
   FILE* callInsFile;
   int count;

   callInsFile = fopen(RemoteVotesFile, "rb");

   if (callInsFile != NULL) {
      count = fread(RemoteVotes, 1, sizeof(RemoteVotes), callInsFile);
      fclose(callInsFile);
   }
}

void CJaSperDlg::OnTimer(UINT_PTR nIDEvent)
{
   static bool bail = false;

   if (bail) goto BailOut;

   bail = true;

   if (JaRemoteFileChanged()) {
      JaReadRemotes();

      m_StationList.ResetContent();

      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if ((strlen(m_JA_MemberSeatingList[i].name) > 0) && RemoteVotes[m_JA_MemberSeatingList[i].seat - 1]) { // boardMap[i]]) {
            m_StationList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_StationList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_StationList.SetItemData(tmpItem, tmpSeat);
         }
      }
      m_StationList.SetFocus();
      m_StationList.SetCurSel(0);
      OnLbnSelchangeListStations(); // Selection changed
   }

   bail = false;

BailOut:
   CDialog::OnTimer(nIDEvent);
}


void CJaSperDlg::OnMouseMove(UINT nFlags, CPoint point)
{
   //if (!m_TestMode) {
   RECT rcClip;

   GetWindowRect(&rcClip);

   // TBD = Uncomment for release
   if (point.x > rcClip.right)  point.x = rcClip.right;
   if (point.x < rcClip.left)   point.x = rcClip.left;
   if (point.y < rcClip.top)    point.y = rcClip.top;
   if (point.y > rcClip.bottom) point.y = rcClip.bottom;

   ::ClipCursor(&rcClip);
   //}

   CDialog::OnMouseMove(nFlags, point);
}
