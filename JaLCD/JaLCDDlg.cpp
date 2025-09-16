
// JaLCDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JaLCD.h"
#include "JaLCDDlg.h"
#include "JaOpto.h"
#include "afxdialogex.h"
#include "trace.h"
#include "VM_Sock.h"

UINT JaLCDThreadFunction(LPVOID pParam);
bool    m_LightsYea = false;
bool    m_LightsNay = false;
bool    m_LightsReset = false;
bool    m_DisplayOn = false;
bool    m_DisplayOff = false;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CJaLCDDlg dialog

CJaLCDDlg::CJaLCDDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_JALCD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJaLCDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CJaLCDDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDC_BUTTON_START_LEDTEST, &CJaLCDDlg::OnBnClickedButtonStartLedtest)
   ON_BN_CLICKED(IDC_BUTTON_YEA, &CJaLCDDlg::OnBnClickedButtonYea)
   ON_BN_CLICKED(IDC_BUTTON_NAY, &CJaLCDDlg::OnBnClickedButtonNay)
   ON_BN_CLICKED(IDC_BUTTON_RESET, &CJaLCDDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_ON, &CJaLCDDlg::OnBnClickedButtonOn)
	ON_BN_CLICKED(IDC_BUTTON_OFF, &CJaLCDDlg::OnBnClickedButtonOff)
	ON_BN_CLICKED(IDC_BUTTON_ON2, &CJaLCDDlg::OnBnClickedButtonOn2)
END_MESSAGE_MAP()


// CJaLCDDlg message handlers

BOOL CJaLCDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

   COMPort5 = "COM5";
   COMPort6 = "COM6";

   //comHandle5 = OpenCOMPort(COMPort5);
   //if (comHandle5 == NULL) {
   //   AfxMessageBox(CString("Unable to open COM5:"), MB_OK);
   //}

   //comHandle6 = OpenCOMPort(COMPort6);
   //if (comHandle6 == NULL) {
   //   AfxMessageBox(CString("Unable to open COM6:"), MB_OK);
   //}

   CWinThread* LCDThread;

   CJaLCDDlg *LCDObj = this;

   LCDThread = AfxBeginThread(JaLCDThreadFunction, LCDObj);

 //  GetDlgItem(IDC_EDIT_STATUS_WINDOW)->SetWindowText(LPCTSTR("Main Voting Machine Thread Spawned."));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJaLCDDlg::OnPaint()
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
HCURSOR CJaLCDDlg::OnQueryDragIcon()
{
   return static_cast<HCURSOR>(m_hIcon);
}

HANDLE CJaLCDDlg::OpenCOMPort(CString comPort)
{
   DCB    ComDCB;
   int    Error;
   HANDLE  comHandle;

   comHandle = CreateFile(comPort,     // Communications Port
      GENERIC_WRITE | GENERIC_READ,
      0,                               // Exclusive access
      NULL,                            // No security.
      OPEN_EXISTING,                   // Fail if not existing.
      0,                               // No overlap.
      NULL);                           // No template.

   if (comHandle != INVALID_HANDLE_VALUE) {

      GetCommState(comHandle, &ComDCB);

      if (!BuildCommDCB(LPCTSTR("baud=9600 parity=N data=8 stop=1"), &ComDCB)) {
         Error = GetLastError();
         CloseHandle(comHandle);
         comHandle = NULL;
      }

      if (!SetCommState(comHandle, &ComDCB)) {
         Error = GetLastError();
         CloseHandle(comHandle);
         comHandle = NULL;
      }
   }
   else {
      Error = GetLastError();
      comHandle = NULL;
   }

   return comHandle;
}

BOOLEAN CJaLCDDlg::SendDisplayString(char *dispStr)
{
   int Error;

   // If COM5 exists send string to port

   if (comHandle5 != NULL) {
      // Send first half of display string to COM5

      Error = TransmitCommChar(comHandle5, 0);

      for (int i = 0; i < 8; i++) {
         Error = TransmitCommChar(comHandle5, dispStr[i]);
      }

      Error = TransmitCommChar(comHandle5, 9);

      // Send second half of display string

      Error = TransmitCommChar(comHandle5, 1);

      for (int i = 8; i < 16; i++) {
         Error = TransmitCommChar(comHandle5, dispStr[i]);
      }

      Error = TransmitCommChar(comHandle5, 9);
   }

   // Repeat for COM6

   if (comHandle6 != NULL) {
      // Send first half of display string

      Error = TransmitCommChar(comHandle6, 0);

      for (int i = 0; i < 8; i++) {
         Error = TransmitCommChar(comHandle6, dispStr[i]);
      }

      Error = TransmitCommChar(comHandle6, 9);

      // Send second half of display string

      Error = TransmitCommChar(comHandle6, 1);

      for (int i = 8; i < 16; i++) {
         Error = TransmitCommChar(comHandle6, dispStr[i]);
      }

      Error = TransmitCommChar(comHandle6, 9);
   }

   return true;
}

UINT JaLCDThreadFunction(LPVOID DlgObj)
{
   JaOpto ^opto; // = gcnew JaOpto;

   try {
      opto = gcnew JaOpto;
   }
   catch (int card) {
      // CString except = e->ToString();
      CString except = "Error opening Opto card " + card.ToString();

      AfxMessageBox(except, MB_ICONINFORMATION | MB_OK | MB_APPLMODAL);
  //    exit(0);
      return 1;
   }
   //catch (...) {
   //   CString except = "Error opening Opto card"; // opto->getException(); // e->ToString();
   //   AfxMessageBox(except, MB_ICONINFORMATION | MB_OK | MB_APPLMODAL);
   //   exit(0);
   //}

   // Clear and initialize system components
   opto->Initialize();

   while (!opto->voteOn()) {

      if (m_LightsYea == true) {
         m_LightsYea = false;

         for (int i = 0; i < MAX_SEATS; i++) { // 136 desks
            // Turn on/off the Desk Lights
            opto->voteDeskLights(i, 2);

            // Handle transmission to display board
            opto->voteBoardLights(i, 2);
         }
      }

      if (m_LightsNay == true) {
         m_LightsNay = false;

         for (int i = 0; i < MAX_SEATS; i++) { // 136 desks
            // Turn on/off the Desk Lights
            opto->voteDeskLights(i, 1);

            // Handle transmission to display board
            opto->voteBoardLights(i, 1);
          }
      }

      if (m_LightsReset == true) {
         m_LightsReset = false;

         for (int i = 0; i < MAX_SEATS; i++) { // 136 desks
            // Turn on/off the Desk Lights
            opto->voteDeskLights(i, 0);

            // Handle transmission to display board
            opto->voteBoardLights(i, 0);
         }
      }

	  if (m_DisplayOn == true) {
		  m_DisplayOn = false;

     	  // Turn on the Displays
		  opto->displayOn();
	  }

	  if (m_DisplayOff == true) {
		  m_DisplayOff = false;

		  // Turn off the Displays
		  opto->displayOff();
	  }

      Sleep(1000);
   }

   return 0;
}

void CJaLCDDlg::OnBnClickedButtonStartLedtest()
{
   char dispStr[20];

   comHandle5 = OpenCOMPort(COMPort5);
   if (comHandle5 == NULL) {
      AfxMessageBox(CString("Unable to open COM5:"), MB_OK);
   }

   comHandle6 = OpenCOMPort(COMPort6);
   if (comHandle6 == NULL) {
      AfxMessageBox(CString("Unable to open COM6:"), MB_OK);
   }

   strcpy_s(dispStr, "  HELLO   DAVE  ");
   SendDisplayString(dispStr);

   Sleep(2000);

   sprintf(dispStr, " %.3d  %.3d  %.3d  ", 134, 134, 134);
   SendDisplayString(dispStr);

   if (comHandle5 != NULL) {
      CloseHandle(comHandle5);
      comHandle5 = NULL;
   }
   if (comHandle6 != NULL) {
      CloseHandle(comHandle6);
      comHandle6 = NULL;
   }
}


void CJaLCDDlg::OnBnClickedButtonYea()
{
   m_LightsYea = true;
}


void CJaLCDDlg::OnBnClickedButtonReset()
{
   m_LightsReset = true;
}


void CJaLCDDlg::OnBnClickedButtonNay()
{
   m_LightsNay = true;
}




void CJaLCDDlg::OnBnClickedButtonOn()
{
	m_DisplayOn = true;
}


void CJaLCDDlg::OnBnClickedButtonOff()
{
	m_DisplayOff = true;
}


void JaWriteCurrentVote(CString voteFilename)
{
	FILE *voteFile;
	int count;
	unsigned char votes[MAX_VOTE_ARRAY];
	static unsigned char votesLast[MAX_VOTE_ARRAY];
	bool votesChanged = false;

	for (int i = 0; i < MAX_SEATS; i++) {
		// Sergeant at arms does not vote (i = seat - 1 )
		if ((i != 68) && (i != 90)) {  // 69 && 91 
			votes[i] = rand() % 2; // m_JA_MemberSeatingList[boardMap[i]].vote;
			//if (votes[i] != votesLast[i]) {
				votesChanged = true;
			//}
		}
		else {
			votes[i] = 0;
		}
		votesLast[i] = votes[i];
	}
	votes[136] = 68;
	votes[137] = 69;

	if (votesChanged) {
		TRACE("before open");
		voteFile = fopen(voteFilename, "wb");
		if (voteFile == NULL) {
			TRACE("could not open votefile");
			return;
		}
		TRACE("before write");
		count = fwrite(votes, 1, sizeof(votes), voteFile);
		TRACE("after write");
		fclose(voteFile);
		TRACE("after close");
	}
}

void CJaLCDDlg::OnBnClickedButtonOn2()
{
	for (int i = 0; i < 2000; i++)
	{
		JaWriteCurrentVote("V:\\JaVote.dat");
	}
	
}
