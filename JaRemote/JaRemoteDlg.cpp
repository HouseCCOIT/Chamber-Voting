
// JaRemoteDlg.cpp : implementation file
//

#include <strsafe.h>

#include "pch.h"
#include "framework.h"
#include "JaRemote.h"
#include "JaRemoteDlg.h"
#include "afxdialogex.h"
#include "BusyForm.h"
#include "trace.h"
#include "NError.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString RemoteDataFile = ".\\JaRemote.dat";

// CJaRemoteDlg dialog

#define WM_BUSY_FORM_FINISH (WM_USER + 1001)

CJaRemoteDlg::CJaRemoteDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_JAREMOTE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJaRemoteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_YEA, m_ButtonYea);
	DDX_Control(pDX, IDC_BUTTON_NAY, m_ButtonNay);
	DDX_Control(pDX, IDC_BUTTON_RESET, m_ButtonReset);
	DDX_Control(pDX, IDC_BUTTON_PAGE, m_ButtonPage);
	DDX_Control(pDX, IDC_BUTTON_RTS, m_ButtonRTS);
	DDX_Control(pDX, IDC_BUTTON_LED_YEA, m_ButtonLEDYea);
	DDX_Control(pDX, IDC_BUTTON_LED_NAY, m_ButtonLEDNay);
	DDX_Control(pDX, IDC_BUTTON_LED_RESET, m_ButtonLEDReset);
	DDX_Control(pDX, IDC_BUTTON_LED_PAGE, m_ButtonLEDPage);
	DDX_Control(pDX, IDC_BUTTON_LED_RTS, m_ButtonLEDRTS);
	DDX_Control(pDX, IDC_STATIC_FULL_NAME, m_FullName);
	DDX_Control(pDX, IDC_BUTTON_VERIFY, m_ButtonVerify);
	DDX_Control(pDX, IDC_BUTTON_VERIFY2, m_ButtonVerify2);
	DDX_Control(pDX, IDCANCEL, m_ButtonExit);
	DDX_Control(pDX, IDC_VERIFY_MESSAGE, m_VerifyMessage);
	DDX_Control(pDX, IDC_CONNECTION_LED, m_ConnectionLED);
	DDX_Control(pDX, IDC_BUTTON_CONNECTION, m_ButtonConnection);
}

BEGIN_MESSAGE_MAP(CJaRemoteDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_YEA, &CJaRemoteDlg::OnBnClickedButtonYea)
	ON_BN_CLICKED(IDC_BUTTON_NAY, &CJaRemoteDlg::OnBnClickedButtonNay)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CJaRemoteDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_PAGE, &CJaRemoteDlg::OnBnClickedButtonPage)
	ON_BN_CLICKED(IDC_BUTTON_VERIFY, &CJaRemoteDlg::OnBnClickedButtonVerify)
	ON_BN_CLICKED(IDC_BUTTON_VERIFY2, &CJaRemoteDlg::OnBnClickedButtonVerify2)
	ON_BN_CLICKED(IDCANCEL, &CJaRemoteDlg::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_BUSY_FORM_FINISH, &CJaRemoteDlg::OnBusyFormFinish)
	ON_COMMAND(IDC_SCAN_CANCEL, &CJaRemoteDlg::OnScanCancel)
	ON_BN_CLICKED(IDC_BUTTON_RTS, &CJaRemoteDlg::OnBnClickedButtonRts)
	ON_BN_CLICKED(IDC_BUTTON_CONNECTION, &CJaRemoteDlg::OnBnClickedButtonConnection)
END_MESSAGE_MAP()

extern DWORD decryptError;
extern BOOL m_TestMode;

BOOL m_Connected = false;

// CJaRemoteDlg message handlers

BOOL CJaRemoteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	NResult result;

	char currDir[100];
	//GetCurrentDirectory(100, (LPSTR)&currDir);
	//TRACE(currDir);

	if (m_TestMode) {
		m_ServerIPAddress = "192.168.10.125";
		RemoteDataFile = ".\\JaRemote.dat";
		//SetCurrentDirectory("\\Program Files (x86)\\MHR\\RemoteVote"); // TBD Remove for local debugging
	}
	else {
		m_ServerIPAddress = "10.60.5.250";
		// TBD for local debugging only m_ServerIPAddress = "192.168.10.140";
		RemoteDataFile = ".\\JaRemote.dat";
		SetCurrentDirectory("\\Program Files (x86)\\MHR\\RemoteVote");
	}

	GetCurrentDirectory(100, (LPSTR)&currDir);
	TRACE(currDir);

	m_fpDB = "mhrRemote.dat";
	m_pwd  = "MrSpeaker";
	m_fp   = "DigitalPersonaUareU";
	m_uDB  = "remoteDB.txt";

	NAChar* szModules = NULL;

	AfxEnableControlContainer();

	result = NffvGetAvailableScannerModulesA(&szModules);
	if (NFailed(result)) {
		return FALSE;
	}

	NffvFreeMemory(szModules);

	result = NffvInitializeA((NAChar*)m_fpDB.GetString(), (NAChar*)m_pwd.GetString(), (NAChar*)m_fp.GetString());

	if (NFailed(result))
	{
		AfxMessageBox(_T("Failed to initialize Nffv or create/load database.\r\n")
			_T("Please check if:\r\n")
			_T(" - Provided password is correct;\r\n")
			_T(" - Database filename is correct;\r\n")
			_T(" - Scanners are used properly.\r\n"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Extra initialization done here

   //	Set the password.
	m_strPassword = "Mr. Speaker";
	m_crypto.DeriveKey(m_strPassword);

	LoadConfig();

	if (m_strData.GetLength() == 0) {
		AfxMessageBox(_T("Unable to load user data.\r\n"));
		return FALSE;
	}

	int Pos = 0;
	m_Name = m_strData.Tokenize("\t", Pos);
	CString seat = m_strData.Tokenize("\t", Pos);

	m_Name.TrimRight(" ");
	m_FullName.SetWindowTextA(m_Name);

	m_Seat = atoi(seat);

   //m_Brush.CreateSolidBrush(RGB(0xdb, 0x88, 0x20));
	m_Brush.CreateSolidBrush(RGB(0xda, 0xb2, 0x75));
	
	// Creates a 18-point-Courier-font
	m_Elephant.CreatePointFont(180, _T("Elephant"));

	m_LucidaSans.CreatePointFont(130, _T("Lucida Sans"));

	m_LucidaSansSmall.CreatePointFont(100, _T("Lucida Sans"));

	m_Calibri.CreatePointFont(100, _T("Calibri"));

	m_FullName.SetFont(&m_Elephant);
	m_FullName.SetBackColor(RGB(0x00, 0x00, 0x00));
	m_FullName.SetTextColor(RGB(0xda, 0xb2, 0x75));

	m_VerifyMessage.SetFont(&m_Calibri);
	m_VerifyMessage.SetBackColor(RGB(0x00, 0x00, 0x00));
	//m_VerifyMessage.SetTextColor(RGB(0xee, 0xcf, 0x7d));
	m_VerifyMessage.SetTextColor(RGB(0xFF, 0xFF, 0xFF));

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

	m_ButtonRTS.SetFaceColor(RGB(0x00, 0x00, 0x00));
	m_ButtonRTS.SetTextColor(RGB(0xff, 0xff, 0xff));
	m_ButtonRTS.SetWindowTextA("\n\nRequest \nto Speak");
	m_ButtonRTS.SetFont(&m_LucidaSansSmall, true);

	m_ButtonConnection.SetFaceColor(RGB(0x00, 0x00, 0x00));
	m_ButtonConnection.SetTextColor(RGB(0xff, 0xff, 0xff));
	m_ButtonConnection.SetWindowTextA("Connection");
	m_ButtonConnection.SetFont(&m_LucidaSansSmall, true);
	m_ButtonConnection.m_nAlignStyle = CMFCButton::ALIGN_LEFT;

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

	m_ButtonLEDRTS.EnableWindowsTheming(FALSE);
	m_ButtonLEDRTS.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	m_ButtonLEDRTS.m_bTransparent = false;
	m_ButtonLEDRTS.SetFaceColor(RGB(0x35, 0x35, 0x70), true);
	m_ButtonLEDRTS.SetTextColor(RGB(0, 0, 0));

	m_ConnectionLED.EnableWindowsTheming(FALSE);
	m_ConnectionLED.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	m_ConnectionLED.m_bTransparent = false;
	m_ConnectionLED.SetFaceColor(RGB(0x00, 0x55, 0x00), true);
	m_ConnectionLED.SetTextColor(RGB(0, 0, 0));

	m_ButtonVerify.SetFaceColor(RGB(0x00, 0x00, 0x00));
	m_ButtonVerify.SetTextColor(RGB(0xff, 0xff, 0xff));
	m_ButtonVerify.SetFont(&m_Calibri, true);

	m_ButtonVerify2.SetFaceColor(RGB(0x00, 0x00, 0x00));
	m_ButtonVerify2.SetTextColor(RGB(0xff, 0xff, 0xff));
	m_ButtonVerify2.SetFont(&m_Calibri, true);

	m_ButtonExit.SetFaceColor(RGB(0x00, 0x00, 0x00));
	m_ButtonExit.SetTextColor(RGB(0xff, 0xff, 0xff));
	m_ButtonExit.SetFont(&m_Calibri, true);

	OnBnClickedButtonConnect();

	GetCurrentVote(); // Ask the VM for the current vote for this seat

	GetDlgItem(IDC_BUTTON_CONNECTION)->EnableWindow(true);

	ButtonReset(); //

	Verify();       // Comment for debugging
	//ButtonOpen();   // Uncomment for debugging

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJaRemoteDlg::OnPaint()
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
HCURSOR CJaRemoteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CJaRemoteDlg::Verify(void)
{
	m_VerifyMessage.SetWindowTextA("Please scan your primary fingerprint.");

	OnBnClickedButtonVerify();

	return true;
}

void CJaRemoteDlg::UpdateButtons(SockRemotePacket* SockData)
{
	CString sDisplay;

	sDisplay.Format("%x : %x", SockData->packetType, SockData->response);

	if (SockData->packetType == VM_RESPONSE) {
		switch (SockData->response) {
		case VM_VOTE_YEA_ACK:
			m_ButtonLEDYea.SetFaceColor(RGB(0, 255, 0), true);
			m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
			m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
			m_ButtonLEDYea.SetFocus();
			m_Vote = VM_VOTE_YEA;
			break;
		case VM_VOTE_NAY_ACK:
			m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
			m_ButtonLEDNay.SetFaceColor(RGB(255, 0, 0));
			m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
			m_ButtonLEDNay.SetFocus();
			m_Vote = VM_VOTE_NAY;
			break;
		case VM_VOTE_RESET_ACK:
			m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
			m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
			m_ButtonLEDReset.SetFaceColor(RGB(255, 255, 0));
			m_ButtonLEDReset.SetFocus();
			m_Vote = VM_VOTE_RESET;
			break;
		case VM_VOTE_PAGE_ACK:
			m_ButtonLEDPage.SetFaceColor(RGB(0xF0, 0xF0, 0x00), true);
			m_ButtonLEDPage.SetFocus();
			break;
		case VM_VOTE_RTS_ACK:
			m_ButtonLEDRTS.SetFaceColor(RGB(0x00, 0x00, 0xFF), true);
			m_ButtonLEDRTS.SetFocus();
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
		case VM_VOTE_PAGE_NAK:
			m_ButtonLEDPage.SetFaceColor(RGB(0x83, 0x50, 0x0e));
			m_ButtonLEDPage.SetFocus();
			break;
		case VM_VOTE_RTS_NAK:
			m_ButtonLEDRTS.SetFaceColor(RGB(0x35, 0x35, 0x70));
			m_ButtonLEDRTS.SetFocus();
			break;
		default:
			break;
		}
	}
	if (SockData->packetType == VM_VM) {
		switch (SockData->remoteAction) {
		case VM_VOTE_OPEN_REMOTE:
			// Small risk of RTS button getting left disabled if vote is open for less than 30 seconds
			// and fingerprint verification times out right before Reset button is pressed.
			Verify();   // Comment for debugging
			//ButtonOpen(); // Uncomment for debugging
			GetDlgItem(IDC_BUTTON_RTS)->EnableWindow(false);
			m_ButtonLEDRTS.SetFaceColor(RGB(0x35, 0x35, 0x70));
			m_ButtonLEDRTS.SetFocus();
			m_VoteOpen = true;
		   break;
		case VM_VOTE_RESET_REMOTE:
			ButtonReset();
			GetDlgItem(IDC_BUTTON_RTS)->EnableWindow(true);
			m_ButtonLEDRTS.SetFaceColor(RGB(0x35, 0x35, 0x70));
			m_ButtonLEDRTS.SetFocus();
			m_Vote = 0;
			m_VoteOpen = false;
			break;
		case VM_VOTE_DISCONNECT_REMOTE:
			OnBnClickedButtonDisconnect();
			break;
		case VM_VOTE_SETINDEX:
			m_VoteOpen = SockData->index;
			break;
		default:
			break;
		}
	}
}


void CJaRemoteDlg::OnBnClickedButtonConnect()
{
	// If already connected, just return
	if (m_Connected) {
		m_ConnectionLED.SetFaceColor(RGB(0x00, 0xFF, 0x00), true);
		m_ConnectionLED.SetFocus();
		return;
	}

	if (AfxSocketInit() == FALSE)	{
		AfxMessageBox("Failed to Initialize Sockets");
		m_ConnectionLED.SetFaceColor(RGB(0xFF, 0x00, 0x00), true);
		m_ConnectionLED.SetFocus();
		return;
	}

	if (m_Client.Create() == FALSE) {
		MessageBox("Failed to Create Socket");
		m_ConnectionLED.SetFaceColor(RGB(0xFF, 0x00, 0x00), true);
		m_ConnectionLED.SetFocus();
		m_Client.ShutDown(2);  //Shutdown both send and receive
		m_Client.Close();
		return;
	}

	if (m_Client.Connect(m_ServerIPAddress, 1001) == FALSE) {
		int error = GetLastError();
		MessageBox("Failed to Connect");
		m_ConnectionLED.SetFaceColor(RGB(0xFF, 0x00, 0x00), true);
		m_ConnectionLED.SetFocus();
		m_Client.ShutDown(2);  //Shutdown both send and receive
		m_Client.Close();
		return;
	}
	m_ConnectionLED.SetFaceColor(RGB(0x00, 0xFF, 0x00), true);
	m_ConnectionLED.SetFocus();

	m_Connected = true;

	m_Client.pDlg = this;
}


void CJaRemoteDlg::OnBnClickedButtonDisconnect()
{
	m_Client.ShutDown(2);  //Shutdown both send and receive
	m_Client.Close();

	GetDlgItem(IDCANCEL)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_YEA)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_NAY)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_PAGE)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_RTS)->EnableWindow(false);

	m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
	m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
	m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
	m_ButtonLEDPage.SetFaceColor(RGB(0x83, 0x50, 0x0e));
	m_ButtonLEDRTS.SetFaceColor(RGB(0x35, 0x35, 0x70));

	m_ConnectionLED.SetFaceColor(RGB(0xFF, 0x00, 0x00));
	m_VerifyMessage.SetWindowTextA("Network Error. Please reconnect and re-verify fingerprint.");

	m_Connected = false;
}


void CJaRemoteDlg::GetCurrentVote()
{
	UpdateData(TRUE);

	SockRemotePacket remotePacket;

	remotePacket.packetType = VM_REMOTE;
	remotePacket.remoteAction = VM_VOTE_GET;
	remotePacket.index = m_Seat;

	m_Client.Send((char*)&remotePacket, sizeof(SockRemotePacket));
}

void CJaRemoteDlg::OnBnClickedButtonYea()
{
	UpdateData(TRUE);

	SockRemotePacket remotePacket;

	remotePacket.packetType = VM_REMOTE;
	remotePacket.remoteAction = VM_VOTE_YEA;
	remotePacket.index = m_Seat;

	m_Client.Send((char *)&remotePacket, sizeof(SockRemotePacket));
}

void CJaRemoteDlg::OnBnClickedButtonNay()
{
	UpdateData(TRUE);

	SockRemotePacket remotePacket;

	remotePacket.packetType = VM_REMOTE;
	remotePacket.remoteAction = VM_VOTE_NAY;
	remotePacket.index = m_Seat;

	m_Client.Send((char*)&remotePacket, sizeof(SockRemotePacket));
}


void CJaRemoteDlg::OnBnClickedButtonReset()
{
	UpdateData(TRUE);

	SockRemotePacket remotePacket;

	remotePacket.packetType = VM_REMOTE;
	remotePacket.remoteAction = VM_VOTE_RESET;
	remotePacket.index = m_Seat;

	m_Client.Send((char*)&remotePacket, sizeof(SockRemotePacket));
}


void CJaRemoteDlg::OnBnClickedButtonPage()
{
	UpdateData(TRUE);

	SockRemotePacket remotePacket;

	remotePacket.packetType = VM_REMOTE;
	remotePacket.remoteAction = VM_VOTE_PAGE;
	remotePacket.index = m_Seat;

	m_Client.Send((char*)&remotePacket, sizeof(SockRemotePacket));
}

void CJaRemoteDlg::OnBnClickedButtonRts()
{
	UpdateData(TRUE);

	SockRemotePacket remotePacket;

	remotePacket.packetType = VM_REMOTE;
	remotePacket.remoteAction = VM_VOTE_RTS;
	remotePacket.index = m_Seat;

	m_Client.Send((char*)&remotePacket, sizeof(SockRemotePacket));
}

void CJaRemoteDlg::ButtonOpen()
{
	GetDlgItem(IDCANCEL)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_YEA)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_NAY)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_PAGE)->EnableWindow(true);
	if (!m_VoteOpen) {
		GetDlgItem(IDC_BUTTON_RTS)->EnableWindow(true);
	}

	switch (m_Vote) {
	case VM_VOTE_YEA:
		m_ButtonLEDYea.SetFaceColor(RGB(0, 255, 0), true);
		m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
		m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
		m_ButtonLEDYea.SetFocus();
		break;
	case VM_VOTE_NAY:
		m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
		m_ButtonLEDNay.SetFaceColor(RGB(255, 0, 0), true);
		m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
		m_ButtonLEDNay.SetFocus();
		break;
	case VM_VOTE_RESET:
		m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
		m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
		m_ButtonLEDReset.SetFaceColor(RGB(255, 255, 0), true);
		m_ButtonLEDReset.SetFocus();
		break;
	}

	m_ButtonLEDRTS.SetFaceColor(RGB(0x35, 0x35, 0x70));
	m_ButtonLEDRTS.SetFocus();
	m_ButtonLEDPage.SetFaceColor(RGB(0x83, 0x50, 0x0e));
	m_ButtonLEDPage.SetFocus();
	//m_ConnectionLED.SetFaceColor(RGB(0x00, 0xFF, 0x00), true);
	//m_ConnectionLED.SetFocus();
}

void CJaRemoteDlg::ButtonReset()
{
	GetDlgItem(IDCANCEL)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_YEA)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_NAY)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
	//GetDlgItem(IDC_BUTTON_PAGE)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_RTS)->EnableWindow(false);

	m_ButtonLEDYea.SetFaceColor(RGB(0x52, 0x45, 0x0E));
	m_ButtonLEDNay.SetFaceColor(RGB(0x63, 0x26, 0x09));
	m_ButtonLEDReset.SetFaceColor(RGB(0x83, 0x50, 0x0e));
	m_ButtonLEDPage.SetFaceColor(RGB(0x83, 0x50, 0x0e));
	m_ButtonLEDRTS.SetFaceColor(RGB(0x35, 0x35, 0x70));
	m_ButtonLEDRTS.SetFocus();
}


void CJaRemoteDlg::OnBnClickedCancel()
{
	m_Client.ShutDown(2);  //Shutdown both send and receive
	m_Client.Close();

	CDialog::OnCancel();
}


HBRUSH CJaRemoteDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr; // = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	switch (nCtlColor) {
		case CTLCOLOR_LISTBOX:
		{
			hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
			//pDC->SetBkColor(nCtlColor);
		}
		break;
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

void CJaRemoteDlg::LoadConfig()
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;

	//	Create a byte array to store the file data.
	CByteArray arData;

	//	Load the file into the byte array.
	CFile file;

   if (file.Open(RemoteDataFile, CFile::modeRead) == FALSE)
	{
		AfxMessageBox(_T("Couldn't open file."));
		return;
	}

	//	Size the array to accomodate the file bytes.
	arData.SetSize(static_cast<INT_PTR>(file.GetLength()));

	//	Copy the data and close the file.
	file.Read(arData.GetData(), static_cast<UINT>(file.GetLength()));
	file.Close();

	//	Try and deserialize the data.
	if (m_crypto.Decrypt(arData, m_strData) == false) {
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL, decryptError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
		lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + 40) * sizeof(TCHAR));
		StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("Couldn't decrypt data. Error %d: %s"), decryptError, lpMsgBuf);

		MessageBox((LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

		LocalFree(lpMsgBuf);
		LocalFree(lpDisplayBuf);
	}
	else {
		UpdateData(FALSE);
	}
}

struct VerifyParam
{
	HNffvUser hUser;
	NInt* pScore;
	CJaRemoteDlg* pMainForm;
	CBusyForm* pBusyForm;
	NffvStatus engineStatus;
};

UINT CJaRemoteDlg::VerifyUserThread(LPVOID pParam)
{
	VerifyParam* pVerifyParam = (VerifyParam*)pParam;
	CJaRemoteDlg* form = ((CJaRemoteDlg*)pVerifyParam->pMainForm);

	NffvSetQualityThreshold(25);

	NResult result = NffvVerify(pVerifyParam->hUser, 30000, &pVerifyParam->engineStatus, pVerifyParam->pScore);
	if (NFailed(result)) {} // throw result;

	form->PostMessage(WM_BUSY_FORM_FINISH, 0, (LPARAM)pVerifyParam->pBusyForm);

	return 0;   // thread completed successfully
}


void CJaRemoteDlg::OnBnClickedButtonVerify()
{
	NResult result;
	HNffvUser hUser;
	NInt score;
	CBusyForm busy("Verifying Primary Fingerprint");

	result = NffvGetUser(0, &hUser);
	if (NFailed(result)) {} // throw result;

	VerifyParam p;
	p.pMainForm = this;
	p.pBusyForm = &busy;
	p.hUser = hUser;
	p.pScore = &score;

	AfxBeginThread(VerifyUserThread, &p);
	busy.DoModal();

	if (p.engineStatus == nfesTemplateCreated)
	{
		if (score > 0)
		{
			m_VerifyMessage.SetWindowTextA("Fingerprint verified");

			ButtonOpen();
		}
		else
		{
			m_VerifyMessage.SetWindowTextA("Fingerprint did not match. Please re-verify.");

			ButtonReset();
		}
	}
	else
	{
		CString description = GetNffvStatusDescription(p.engineStatus);
		CString errorMsg;
		errorMsg.Format(_T("Verification failed. Reason: %s"), description);
		m_VerifyMessage.SetWindowTextA(errorMsg);

		ButtonReset();
	}
}


void CJaRemoteDlg::OnBnClickedButtonVerify2()
{
	NResult result;
	HNffvUser hUser;
	NInt score;
	CBusyForm busy("Verifying Secondary Fingerprint");

	result = NffvGetUser(1, &hUser);
	if (NFailed(result)) {} // throw result;

	VerifyParam p;
	p.pMainForm = this;
	p.pBusyForm = &busy;
	p.hUser = hUser;
	p.pScore = &score;

	AfxBeginThread(VerifyUserThread, &p);
	busy.DoModal();

	if (p.engineStatus == nfesTemplateCreated)
	{
		if (score > 0)
		{
			m_VerifyMessage.SetWindowTextA("Fingerprint verified");

			ButtonOpen();
		}
		else
		{
			m_VerifyMessage.SetWindowTextA("Fingerprint did not match. Please re-verify.");

			ButtonReset();
		}
	}
	else
	{
		CString description = GetNffvStatusDescription(p.engineStatus);
		CString errorMsg;
		errorMsg.Format(_T("Verification failed. Reason: %s"), description);
		m_VerifyMessage.SetWindowTextA(errorMsg);

		ButtonReset();
	}
}


afx_msg LRESULT CJaRemoteDlg::OnBusyFormFinish(WPARAM wParam, LPARAM lParam)
{
	CBusyForm* busyForm = (CBusyForm*)lParam;
	busyForm->Stop();

	return 0;
}


void CJaRemoteDlg::OnScanCancel()
{
	NffvCancel();
}

const TCHAR* CJaRemoteDlg::GetNffvStatusDescription(NffvStatus status)
{
	switch (status)
	{
	case nfesNone: return _T("None");
	case nfesTemplateCreated: return _T("Template created");
	case nfesNoScanner: return _T("No scanner");
	case nfesScannerTimeout: return _T("Scanner timeout");
	case nfesUserCanceled: return _T("User cancelled.");
	case nfesQualityCheckFailed: return _T("Quality check failed");
	default: return _T("");
	}
}


void CJaRemoteDlg::OnBnClickedButtonConnection()
{
	OnBnClickedButtonConnect();
}
