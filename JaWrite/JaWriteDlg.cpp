
// JaWriteDlg.cpp : implementation file
//

//#include <io.h>
//#include <fcntl.h>
//#include <time.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <strsafe.h>

#include "stdafx.h"
#include "JaWrite.h"
#include "JaWriteDlg.h"
#include "afxdialogex.h"
#include "trace.h"

#include "JaOpto.h"
#include "VM_Sock.h"

using namespace System;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT JaWriteThreadFunction(LPVOID DlgObj);
UINT JaReadThreadFunction(LPVOID DlgObj);

#define WriteFileName "V:\\JaWrite.dat"


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


// CJaWriteDlg dialog



CJaWriteDlg::CJaWriteDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_JAWRITE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJaWriteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WRITE_OUT, WriteOut);
	DDX_Control(pDX, IDC_READ_OUT, ReadOut);
}

BEGIN_MESSAGE_MAP(CJaWriteDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SPAWNWRITE, &CJaWriteDlg::OnBnClickedSpawnWrite)
	ON_BN_CLICKED(IDC_SPAWNREAD, &CJaWriteDlg::OnBnClickedSpawnread)
END_MESSAGE_MAP()


// CJaWriteDlg message handlers

BOOL CJaWriteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	//CWinThread* WriteThread;

	//CJaWriteDlg *WriteObj = this;

	//WriteThread = AfxBeginThread(JaWriteThreadFunction, WriteObj);

	//GetDlgItem(IDC_EDIT_STATUS_WINDOW)->SetWindowText(LPCTSTR("Main Write Thread Spawned."));

	JaOpto ^opto; // = gcnew JaOpto;

	try {
		opto = gcnew JaOpto;
	}
	catch (int card) {
		// CString except = e->ToString();
		CString except = "Error opening Opto card " + card.ToString();

		AfxMessageBox(except, MB_ICONINFORMATION | MB_OK | MB_APPLMODAL);
		//exit(0);
	}

	opto->collect();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CJaWriteDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJaWriteDlg::OnPaint()
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
HCURSOR CJaWriteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CJaWriteDlg::OnBnClickedSpawnWrite()
{
	CWinThread* WriteThread;

	CJaWriteDlg *WriteObj = this;

	WriteThread = AfxBeginThread(JaWriteThreadFunction, WriteObj);
}

void CJaWriteDlg::OnBnClickedSpawnread()
{
	CWinThread* ReadThread;

	CJaWriteDlg *WriteObj = this;

	ReadThread = AfxBeginThread(JaReadThreadFunction, WriteObj);
}

void JaWriteCurrentVote(CString voteFilename)
{
	FILE *voteFile;
	int count;
	unsigned char votes[MAX_VOTE_ARRAY];
	static unsigned char votesLast[MAX_VOTE_ARRAY];
	static int WriteFailCount = 0;
	bool votesChanged = false;

	SYSTEMTIME time1, time2;
	GetSystemTime(&time1);

	TRACE("Before open");

	//voteFile = _fsopen(voteFilename, "wb", _SH_DENYWR);
	voteFile = fopen(voteFilename, "wb");
	if (voteFile == NULL) {
		TRACE("could not open votefile - %s", strerror(errno));
		WriteFailCount++;
		return;
	}
	GetSystemTime(&time2);

	int timeDiff = time2.wMilliseconds - time1.wMilliseconds;

	if (timeDiff > 2) TRACE("time difference = %d <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<", timeDiff);

	for (int i = 0; i < sizeof(votes) - 2; i++) {
		// Sergeant at arms does not vote (i = seat - 1 )
		//if ((i != 68) && (i != 90)) {  // 69 && 91 
		//	votes[i] = 1;  // m_JA_MemberSeatingList[boardMap[i]].vote;
		//	if (votes[i] != votesLast[i]) {
		//		votesChanged = true;
		//	}
		//}
		//else {
		//	votes[i] = 0;
		//}

		votes[i] = 1;  // m_JA_MemberSeatingList[boardMap[i]].vote;
		if (votes[i] != votesLast[i]) {
			votesChanged = true;
		}

		votesLast[i] = votes[i];
	}
	votes[136] = 100; // yes_total;
	votes[137] = 34; // no_total;

	TRACE("before write");
	count = fwrite(votes, 1, sizeof(votes), voteFile);
	//TRACE("after write");
	//TRACE("before flush");
	fflush(voteFile);
	//TRACE("after flush");

	fclose(voteFile);
	//TRACE("after close");
}

void JaReadCurrentVote(CString voteFilename)
{
	FILE *voteFile;
	int count;
	unsigned char votes[MAX_VOTE_ARRAY];

	TRACE("Before open");

	voteFile = _fsopen(voteFilename, "rb+", _SH_DENYWR);
	if (voteFile == NULL) {
		TRACE(strerror(errno));
		return;
	}

    TRACE("Before read");

	count = fread(votes, 1, sizeof(votes), voteFile);

	//TRACE("After read");

	//Sleep(2000);

	fclose(voteFile);

	TRACE("After close");
}

UINT JaWriteThreadFunction(LPVOID DlgObj)
{
	int system_ready = 0;
	bool killMainThread = false;

	CJaWriteDlg *m_JA_WriteDialog = (CJaWriteDlg *)DlgObj;

	TRACE("JaWriteThreadFunction");

	while (!killMainThread) {

		m_JA_WriteDialog->WriteOut.SetWindowTextA("Delay Writing JaWrite.dat");

		JaWriteCurrentVote(WriteFileName);

		m_JA_WriteDialog->WriteOut.SetWindowTextA("");

		Sleep(50);
	}

	return 1;
}


UINT JaReadThreadFunction(LPVOID DlgObj)
{
	int system_ready = 0;
	bool killMainThread = false;

	CJaWriteDlg *m_JA_WriteDialog = (CJaWriteDlg *)DlgObj;

	TRACE("JaReadThreadFunction");

	while (!killMainThread) {

		m_JA_WriteDialog->ReadOut.SetWindowTextA("Delay Reading JaWrite.dat");

		JaReadCurrentVote(WriteFileName);

		m_JA_WriteDialog->ReadOut.SetWindowTextA("");

		Sleep(50);
	}

	return 1;
}