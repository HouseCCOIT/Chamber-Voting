
// JaLEDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JaLED.h"
#include "JaLEDDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CJaLEDDlg dialog



CJaLEDDlg::CJaLEDDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_JALED_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJaLEDDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_EDIT1, FontID);
}

BEGIN_MESSAGE_MAP(CJaLEDDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDC_BUTTON1, &CJaLEDDlg::OnBnClickedButton1)
   ON_BN_CLICKED(IDCANCEL, &CJaLEDDlg::OnBnClickedCancel)
   ON_BN_CLICKED(IDOK, &CJaLEDDlg::OnBnClickedOk)
   ON_BN_CLICKED(IDC_BUTTON2, &CJaLEDDlg::OnBnClickedButton2)
   ON_BN_CLICKED(IDC_BUTTON3, &CJaLEDDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CJaLEDDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CJaLEDDlg message handlers

BOOL CJaLEDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

   COMPort5 = "\\\\.\\COM4";
   COMPort6 = "\\\\.\\COM6";

   comHandle6 = OpenCOMPort(COMPort6);
   if (comHandle6 == NULL) {
      AfxMessageBox(CString("Unable to open COM6:"), MB_OK);
   }
   comHandle5 = OpenCOMPort(COMPort5);
   if (comHandle5 == NULL) {
      AfxMessageBox(CString("Unable to open COM5:"), MB_OK);
   }

   Initialize();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJaLEDDlg::OnPaint()
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
HCURSOR CJaLEDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HANDLE CJaLEDDlg::OpenCOMPort(CString comPort)
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
      ComDCB.DCBlength = sizeof(DCB);
      ComDCB.BaudRate = 115200;
      ComDCB.Parity = 0;
      ComDCB.StopBits = 0;
      ComDCB.ByteSize = 8;

      //if (!BuildCommDCB(LPCTSTR("baud=9600 parity=N data=8 stop=1"), &ComDCB)) {
      //   Error = GetLastError();
      //   CloseHandle(comHandle);
      //   comHandle = NULL;
      //}
      //else 
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

BOOLEAN CJaLEDDlg::DisableScreenSaver()
{
   int Error;

   // Disable Screen Saver
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, 0x0C);
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, 0x00);

   return true;
}

BOOLEAN CJaLEDDlg::ClearScreen()
{
   int Error;

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0xD7);

   return true;
}

BOOLEAN CJaLEDDlg::SetTextColor(short color)
{
   int Error;

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0x7F);
   Error = TransmitCommChar(comHandle5, color >> 8);
   Error = TransmitCommChar(comHandle5, color &0xFF);

   return true;
}

BOOLEAN CJaLEDDlg::SetTextSize(unsigned char size)
{
   int Error;

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0x7C); // width
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, size & 0xFF);

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0x7B); // height
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, size & 0xFF);

   return true;
}

BOOLEAN CJaLEDDlg::SetTextFontId(short id)
{
   int Error;

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0x7D);
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, id);

   return true;
}

BOOLEAN CJaLEDDlg::MoveTo(unsigned char x, unsigned char y)
{
   int Error;

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0xD6);
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, x);
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, y);

   return true;
}


BOOLEAN CJaLEDDlg::SendDisplayString(char *dispStr)
{
   int Error;

   // If COM5 exists send string to port

   if (comHandle5 != NULL) {
      // Send first half of display string to COM5

      Error = TransmitCommChar(comHandle5, 0x00);
      Error = TransmitCommChar(comHandle5, 0x06);

      for (int i = 0; i < 16; i++) {
         Error = TransmitCommChar(comHandle5, dispStr[i]);
      }
      Error = TransmitCommChar(comHandle5, 0x00);
   }

   // Repeat for COM6

   if (comHandle6 != NULL) {
      // Send first half of display string to COM6

      Error = TransmitCommChar(comHandle6, 0x00);
      Error = TransmitCommChar(comHandle6, 0x06);

      for (int i = 0; i < 16; i++) {
         Error = TransmitCommChar(comHandle6, dispStr[i]);
      }
      Error = TransmitCommChar(comHandle6, 0x00);
   }

   return true;
}

BOOLEAN CJaLEDDlg::DisableScreenSaver2()
{
   int Error;

   // Disable Screen Saver
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, 0x0C);
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, 0x00);

   Error = TransmitCommChar(comHandle6, 0x00);
   Error = TransmitCommChar(comHandle6, 0x0C);
   Error = TransmitCommChar(comHandle6, 0x00);
   Error = TransmitCommChar(comHandle6, 0x00);

   return true;
}

BOOLEAN CJaLEDDlg::ClearScreen2()
{
   int Error;

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0x82);

   Error = TransmitCommChar(comHandle6, 0xFF);
   Error = TransmitCommChar(comHandle6, 0x82);

   return true;
}

BOOLEAN CJaLEDDlg::SetTextColor2(short color)
{
   int Error;

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0xEE);
   Error = TransmitCommChar(comHandle5, color >> 8);
   Error = TransmitCommChar(comHandle5, color & 0xFF);

   Error = TransmitCommChar(comHandle6, 0xFF);
   Error = TransmitCommChar(comHandle6, 0xEE);
   Error = TransmitCommChar(comHandle6, color >> 8);
   Error = TransmitCommChar(comHandle6, color & 0xFF);

   return true;
}

BOOLEAN CJaLEDDlg::SetTextSize2(unsigned char size)
{
   int Error;

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0xEB); // width
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, size & 0xFF);

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0xEA); // height
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, size & 0xFF);

   Error = TransmitCommChar(comHandle6, 0xFF);
   Error = TransmitCommChar(comHandle6, 0xEB); // width
   Error = TransmitCommChar(comHandle6, 0x00);
   Error = TransmitCommChar(comHandle6, size & 0xFF);

   Error = TransmitCommChar(comHandle6, 0xFF);
   Error = TransmitCommChar(comHandle6, 0xEA); // height
   Error = TransmitCommChar(comHandle6, 0x00);
   Error = TransmitCommChar(comHandle6, size & 0xFF);

   return true;
}

BOOLEAN CJaLEDDlg::SetTextFontId2(short id)
{
   int Error;

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0xEC);
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, id);

   Error = TransmitCommChar(comHandle6, 0xFF);
   Error = TransmitCommChar(comHandle6, 0xEC);
   Error = TransmitCommChar(comHandle6, 0x00);
   Error = TransmitCommChar(comHandle6, id);

   return true;
}

BOOLEAN CJaLEDDlg::MoveTo2(unsigned char x, unsigned char y)
{
   int Error;

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0x81);
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, x);
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, y);

   Error = TransmitCommChar(comHandle6, 0xFF);
   Error = TransmitCommChar(comHandle6, 0x81);
   Error = TransmitCommChar(comHandle6, 0x00);
   Error = TransmitCommChar(comHandle6, x);
   Error = TransmitCommChar(comHandle6, 0x00);
   Error = TransmitCommChar(comHandle6, y);

   return true;
}

BOOLEAN CJaLEDDlg::ScreenMode2(unsigned char mode)
{
   int Error;

   Error = TransmitCommChar(comHandle5, 0xFF);
   Error = TransmitCommChar(comHandle5, 0x42);
   Error = TransmitCommChar(comHandle5, 0x00);
   Error = TransmitCommChar(comHandle5, mode);

   Error = TransmitCommChar(comHandle6, 0xFF);
   Error = TransmitCommChar(comHandle6, 0x42);
   Error = TransmitCommChar(comHandle6, 0x00);
   Error = TransmitCommChar(comHandle6, mode);

   return true;
}

BOOLEAN CJaLEDDlg::SendDisplayString2(char *dispStr)
{
   int Error;

   // If COM5 exists send string to port

   if (comHandle5 != NULL) {
      // Send first half of display string to COM5

      Error = TransmitCommChar(comHandle5, 0x00);
      Error = TransmitCommChar(comHandle5, 0x18);

      for (int i = 0; i < 14; i++) {
         Error = TransmitCommChar(comHandle5, dispStr[i]);
         //Sleep(1);
         if (dispStr[i] == 0x00) break;
      }
      //Error = TransmitCommChar(comHandle5, 0x00);
   }

   // Repeat for COM6

   if (comHandle6 != NULL) {
      // Send first half of display string to COM6

      Error = TransmitCommChar(comHandle6, 0x00);
      Error = TransmitCommChar(comHandle6, 0x18);

      for (int i = 0; i < 14; i++) {
         Error = TransmitCommChar(comHandle6, dispStr[i]);
         //Sleep(1);
         if (dispStr[i] == 0x00) break;
      }
      //Error = TransmitCommChar(comHandle6, 0x00);
   }

   return true;
}
void CJaLEDDlg::OnBnClickedCancel()
{
   // TODO: Add your control notification handler code here
   if (comHandle5 != NULL) {
      CloseHandle(comHandle5);
      comHandle5 = NULL;
   }
   if (comHandle6 != NULL) {
      CloseHandle(comHandle6);
      comHandle6 = NULL;
   }

   CDialogEx::OnCancel();
}


void CJaLEDDlg::OnBnClickedOk()
{
   // TODO: Add your control notification handler code here
   if (comHandle5 != NULL) {
      CloseHandle(comHandle5);
      comHandle5 = NULL;
   }
   if (comHandle6 != NULL) {
      CloseHandle(comHandle6);
      comHandle6 = NULL;
   }

   CDialogEx::OnOK();
}

void CJaLEDDlg::OnBnClickedButton1()
{
   char dispStr[20] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
   int Error;

   DisableScreenSaver();

   //SetTextColor(0xF7DF);
   //MoveTo(0, 0);
   //strcpy_s(dispStr, "  HELLO   DAVE  ");

   //Sleep(10000);

   ClearScreen();

   Sleep(100);

   SetTextColor(0xFFFF);
   SetTextSize(2);
   // SetTextFontId(FontId++);

   MoveTo(10, 10);
   sprintf(dispStr, "YEA  %.3d", 134);
   SendDisplayString(dispStr);

   Sleep(100);

   MoveTo(10, 55);
   sprintf(dispStr, "NAY  %.3d", 134);
   SendDisplayString(dispStr);

   Sleep(100);

   MoveTo(10, 100);
   sprintf(dispStr, "ABS  %.3d", 134);
   SendDisplayString(dispStr);
}

void CJaLEDDlg::OnBnClickedButton2()
{
   char dispStr[20];
   int Error;

   DisableScreenSaver();

   //SetTextColor(0xF7DF);
   //MoveTo(0, 0);
   //strcpy_s(dispStr, "  HELLO   DAVE  ");

   //Sleep(10000);

   ClearScreen();

   Sleep(100);

   SetTextColor(0xFFFF);
   SetTextSize(2);
   // SetTextFontId(FontId++);

   MoveTo(10, 15);
   sprintf(dispStr, "YEA    %.3d", 134);
   SendDisplayString(dispStr);

   Sleep(100);

   MoveTo(10, 60);
   sprintf(dispStr, "NAY    %.3d", 134);
   SendDisplayString(dispStr);

   Sleep(100);

   MoveTo(10, 105);
   sprintf(dispStr, "ABSENT %.3d", 134);
   SendDisplayString(dispStr);
}

unsigned char FontId = 3;
unsigned char count = 134;

void CJaLEDDlg::OnBnClickedButton3()
{
   char dispStr[24];
   int Error;

   ScreenMode2(0);
   Sleep(1);

   Sleep(1);

   SetTextColor2(0xFFFF);
   Sleep(1);
   
   CString fontid;
   fontid.Format(_T("%d"), FontId);
   FontID.SetWindowTextW(LPCTSTR(fontid));
 
   SetTextFontId2(FontId);

   SetTextSize2(6);

   MoveTo2(50, 30);
   Sleep(1);
   sprintf(dispStr, "YEA  %*d", 3, count);
   SendDisplayString2(dispStr);
   Sleep(1);

   MoveTo2(50, 130);
   Sleep(1);
   sprintf(dispStr, "NAY  %*d", 3, (134 - count));
   SendDisplayString2(dispStr);
   Sleep(1);

   MoveTo2(50, 230);
   Sleep(1);
   sprintf(dispStr, "N/V  %*d", 3, count % 12);
   SendDisplayString2(dispStr);
   Sleep(1);

   count--;
}

bool CJaLEDDlg::Initialize(void)
{
   char dispStr[24];

   ScreenMode2(0);
   Sleep(1);

   ClearScreen2();
   Sleep(1);

   SetTextColor2(0xFFFF);
   Sleep(1);

   CString fontid;
   fontid.Format(_T("%d"), FontId);
   FontID.SetWindowTextW(LPCTSTR(fontid));

   SetTextFontId2(FontId);

   SetTextSize2(6);

   MoveTo2(50, 30);
   Sleep(1);
   sprintf(dispStr, "YEA  %.3d", 134);
   SendDisplayString2(dispStr);
   Sleep(1);

   MoveTo2(50, 130);
   Sleep(1);
   sprintf(dispStr, "NAY  %.3d", 133);
   SendDisplayString2(dispStr);
   Sleep(1);

   MoveTo2(50, 230);
   Sleep(1);
   sprintf(dispStr, "N/V  %.3d", 132);
   SendDisplayString2(dispStr);
   Sleep(1);

   return true;
}


void CJaLEDDlg::OnBnClickedButton4()
{
	//JaWriteCurrentVote();
}
