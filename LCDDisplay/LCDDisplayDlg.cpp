
// LCDDisplayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pch.h"
#include "framework.h"
#include "LCDDisplay.h"
#include "LCDDisplayDlg.h"
#include "DisplayThread.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLCDDisplayDlg dialog

CLCDDisplayDlg::CLCDDisplayDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LCDDISPLAY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLCDDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_YEA_COUNT, mYEA);
	DDX_Control(pDX, IDC_STATIC_NAY_COUNT, mNAY);
	DDX_Control(pDX, IDC_STATIC_N_V_COUNT, mN_V);
}

BEGIN_MESSAGE_MAP(CLCDDisplayDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

extern BOOL m_TestMode;

extern unsigned char votes[];

// CLCDDisplayDlg message handlers

BOOL CLCDDisplayDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	if (m_TestMode) {
		m_ServerIPAddress = "192.168.10.140";
	}
	else {
		m_ServerIPAddress = "192.168.10.2";
	}

	ConnectToVM();

	DisplayInit();

	JaReadCurrentVote();

	int yeas = votes[136];
	int nays = votes[137];
	int abs = 134 - (yeas + nays);

	if ((yeas + nays) <= 134) {
		numeric_display(yeas, nays, abs, true);
	}
	char yeaString[5], nayString[5], absString[5];
	_itoa_s(yeas, (char*)yeaString, 5, 10);
	_itoa_s(nays, (char*)nayString, 5, 10);
	_itoa_s(abs, (char*)absString, 5, 10);

	mYEA.SetWindowTextA(yeaString);
	mNAY.SetWindowTextA(nayString);
	mN_V.SetWindowTextA(absString);

	//ShowWindow(SW_MINIMIZE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLCDDisplayDlg::OnPaint()
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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLCDDisplayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLCDDisplayDlg::ConnectToVM()
{
	if (AfxSocketInit() == FALSE) {
		AfxMessageBox("Failed to Initialize Sockets");
		return;
	}

	if (m_Client.Create() == FALSE) {
		AfxMessageBox("Failed to Create Socket");
		return;
	}

	if (m_Client.Connect(m_ServerIPAddress, 1004) == FALSE) {
		int error = GetLastError();
		AfxMessageBox("Failed to Connect");
		return;
	}

	m_Client.pDlg = this;
}


void CLCDDisplayDlg::UpdateCurrentVote(SockPacket votePacket)
{
	int yeas = votePacket.dataByte[136];
	int nays = votePacket.dataByte[137];
	int abs = 134 - (yeas + nays);

	if ((yeas + nays) <= 134) {
		numeric_display(yeas, nays, abs, true);
	}
	char yeaString[5], nayString[5], absString[5];
	_itoa_s(yeas, (char *)yeaString, 5, 10);
	_itoa_s(nays, (char *)nayString, 5, 10); 
	_itoa_s(abs, (char *)absString, 5, 10);
  
	mYEA.SetWindowTextA(yeaString);
	mNAY.SetWindowTextA(nayString);
	mN_V.SetWindowTextA(absString);
}