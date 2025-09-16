// JaRCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JaRC.h"
#include "JaRCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJaRCDlg dialog

CJaRCDlg::CJaRCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJaRCDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJaRCDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJaRCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJaRCDlg)
	DDX_Control(pDX, IDC_SLIDER_BRIGHTNESS, m_SliderBrightness);
	DDX_Control(pDX, IDC_SLIDER_CONTRAST, m_SliderContrast);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CJaRCDlg, CDialog)
	//{{AFX_MSG_MAP(CJaRCDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_CONTRAST, OnReleasedcaptureSliderContrast)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_BRIGHTNESS, OnReleasedcaptureSliderBrightness)
	ON_BN_CLICKED(IDC_RADIO_POWER_OFF, OnRadioPowerOff)
	ON_BN_CLICKED(IDC_RADIO_INPUT_COMPONENT, OnRadioInputComponent)
	ON_BN_CLICKED(IDC_RADIO_POWER_ON, OnRadioPowerOn)
	ON_BN_CLICKED(IDC_RADIO_INPUT_RGB, OnRadioInputRgb)
	ON_BN_CLICKED(IDC_RADIO_INPUT_SVIDEO, OnRadioInputSvideo)
	ON_BN_CLICKED(IDC_RADIO_INPUT_VIDEO, OnRadioInputVideo)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CString JaRCIni  = "V:\\JaRC.Ini";

/////////////////////////////////////////////////////////////////////////////
// CJaRCDlg message handlers

BOOL CJaRCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	
	// Initialize Stuff

   LoadOptions();

   SetTimer(1, 10000, NULL);        // Ping the Display every 10 seconds

   m_SetID = 1;

   m_ComPort = "COM5:";
   InitComPort();

   m_ComPort2 = "COM2:";
   InitComPort2();

   SendCommand('m', m_SetID, 1);

	m_SliderContrast.SetRange(0, 0x64, TRUE);
	m_SliderBrightness.SetRange(0, 0x64, TRUE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CJaRCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJaRCDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CJaRCDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CJaRCDlg::OnReleasedcaptureSliderContrast(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
   m_ContrastLevel = m_SliderContrast.GetPos();

   SendCommand('g', m_SetID, m_ContrastLevel);

	*pResult = 0;
}

void CJaRCDlg::OnReleasedcaptureSliderBrightness(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

   m_BrightnessLevel = m_SliderBrightness.GetPos();

   SendCommand('h', m_SetID, m_BrightnessLevel);

	*pResult = 0;
}

BOOLEAN CJaRCDlg::InitComPort()
{
   DCB    ComDCB;
   int    Error;

   m_ComHandle = CreateFile (m_ComPort,             // Communications Port
                             GENERIC_WRITE          // Generic access, read/write.
                             | GENERIC_READ,
                             FILE_SHARE_READ        // Share both read and write.
                             | FILE_SHARE_WRITE ,
                             NULL,                  // No security.
                             OPEN_EXISTING,         // Fail if not existing.
                             FILE_FLAG_OVERLAPPED,  // Use overlap.
                             NULL);                 // No template.

   if (m_ComHandle != INVALID_HANDLE_VALUE) {

      GetCommState(m_ComHandle, &ComDCB);

      if (!BuildCommDCB(LPCTSTR("baud=115200 parity=N data=8 stop=1"), &ComDCB)) {
         Error = GetLastError();
         CloseHandle(m_ComHandle);
         m_ComHandle = NULL;
      }

      if (!SetCommState(m_ComHandle, &ComDCB)) {
         Error = GetLastError();
         CloseHandle(m_ComHandle);
         m_ComHandle = NULL;
      }
   }
   else {
      m_ComHandle = NULL;
   }
   
	return TRUE;
}


BOOLEAN CJaRCDlg::InitComPort2()
{
   DCB    ComDCB;
   int    Error;

   m_ComHandle2 = CreateFile (m_ComPort2,             // Communications Port
                             GENERIC_WRITE          // Generic access, read/write.
                             | GENERIC_READ,
                             FILE_SHARE_READ        // Share both read and write.
                             | FILE_SHARE_WRITE ,
                             NULL,                  // No security.
                             OPEN_EXISTING,         // Fail if not existing.
                             FILE_FLAG_OVERLAPPED,  // Use overlap.
                             NULL);                 // No template.

   if (m_ComHandle2 != INVALID_HANDLE_VALUE) {

      GetCommState(m_ComHandle2, &ComDCB);

      if (!BuildCommDCB(LPCTSTR("baud=115200 parity=N data=8 stop=1"), &ComDCB)) {
         Error = GetLastError();
         CloseHandle(m_ComHandle2);
         m_ComHandle2 = NULL;
      }

      if (!SetCommState(m_ComHandle2, &ComDCB)) {
         Error = GetLastError();
         CloseHandle(m_ComHandle2);
         m_ComHandle2 = NULL;
      }
   }
   else {
      m_ComHandle2 = NULL;
   }
   
	return TRUE;
}


BOOLEAN CJaRCDlg::SendCommand(char cmd2, char setID, char data)
{
  int Error;
  char setIdStr[10];
  char dataStr[10];

  sprintf(setIdStr, "%.2x", setID);
  sprintf(dataStr, "%.2x", data);

  Error = TransmitCommChar(m_ComHandle, 'k');
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle, cmd2);
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle, ' ');
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle, setIdStr[0]);
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle, setIdStr[1]);
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle, ' ');
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle, dataStr[0]);
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle, dataStr[1]);
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle, 0x0d);

  Error = TransmitCommChar(m_ComHandle2, 'k');
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle2, cmd2);
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle2, ' ');
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle2, setIdStr[0]);
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle2, setIdStr[1]);
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle2, ' ');
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle2, dataStr[0]);
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle2, dataStr[1]);
  Sleep(10);
  Error = TransmitCommChar(m_ComHandle2, 0x0d);

  return TRUE;
}

void CJaRCDlg::OnRadioPowerOn() 
{
   CheckRadioButton(IDC_RADIO_POWER_ON, IDC_RADIO_POWER_OFF, IDC_RADIO_POWER_ON);

   m_Power = 1;
   
   SendCommand('a', m_SetID, m_Power);

   GetResponse();
}
                 
void CJaRCDlg::OnRadioPowerOff() 
{
   CheckRadioButton(IDC_RADIO_POWER_ON, IDC_RADIO_POWER_OFF, IDC_RADIO_POWER_OFF);

   m_Power = 0;
   
   SendCommand('a', m_SetID, m_Power);
}

void CJaRCDlg::OnRadioInputRgb() 
{
   CheckRadioButton(IDC_RADIO_INPUT_RGB, IDC_RADIO_INPUT_SVIDEO, IDC_RADIO_INPUT_RGB);

	m_Input = 0;

   SendCommand('b', m_SetID, m_Input);
}

void CJaRCDlg::OnRadioInputComponent() 
{
   CheckRadioButton(IDC_RADIO_INPUT_RGB, IDC_RADIO_INPUT_SVIDEO, IDC_RADIO_INPUT_COMPONENT);

	m_Input = 1;

   SendCommand('b', m_SetID, m_Input);
}

void CJaRCDlg::OnRadioInputVideo() 
{
   CheckRadioButton(IDC_RADIO_INPUT_RGB, IDC_RADIO_INPUT_SVIDEO, IDC_RADIO_INPUT_VIDEO);

	m_Input = 2;

   SendCommand('b', m_SetID, m_Input);
}

void CJaRCDlg::OnRadioInputSvideo() 
{
   CheckRadioButton(IDC_RADIO_INPUT_RGB, IDC_RADIO_INPUT_SVIDEO, IDC_RADIO_INPUT_SVIDEO);

	m_Input = 3;

   SendCommand('b', m_SetID, m_Input);
}

void CJaRCDlg::OnCancel() 
{
   CloseHandle(m_ComHandle);
   CloseHandle(m_ComHandle2);

	CDialog::OnCancel();
}

void CJaRCDlg::OnOK() 
{
   CloseHandle(m_ComHandle);
   CloseHandle(m_ComHandle2);

   StoreOptions();

	CDialog::OnOK();
}

BOOLEAN CJaRCDlg::GetResponse()
{
   OVERLAPPED o;
   DWORD dwEvtMask = 0;

   // Create an event object for use in WaitCommEvent. 

   o.hEvent = CreateEvent(NULL,   // no security attributes 
                          FALSE,  // auto reset event 
                          FALSE,  // not signaled 
                          NULL    // no name 
                          );

//   assert(o.hEvent);

   if (WaitCommEvent(m_ComHandle, &dwEvtMask, &o)) {
      if (dwEvtMask & EV_DSR) {
         // To do.
      }

      if (dwEvtMask & EV_CTS) {
         // To do. 
      }

   }
   else {
      dwEvtMask = GetLastError();
   }

   return TRUE;
}

void CJaRCDlg::OnTimer(UINT nIDEvent) 
{
   SendCommand('a', m_SetID, 1);

   SendCommand('d', m_SetID, 0);

   CDialog::OnTimer(nIDEvent);
}


BOOLEAN CJaRCDlg::LoadOptions()
{
   FILE *tmpFile;

   char section[32];
   char inBuff[81];

   // Does JaVote.Ini file exist?
   if ((tmpFile = fopen( JaRCIni, "r+" )) == NULL ) {
      return FALSE;
   }
   else {
      fclose(tmpFile);
   }

   wsprintf(section, "Parameters");

   GetPrivateProfileString (section, " Brightness", "100",  inBuff, 80, JaRCIni); 
   m_BrightnessLevel = atoi(inBuff);

   GetPrivateProfileString (section, " Contrast", "100",  inBuff, 80, JaRCIni); 
   m_ContrastLevel = atoi(inBuff);

   GetPrivateProfileString (section, " Input", "1",  inBuff, 80, JaRCIni); 
   m_Input = atoi(inBuff);

   m_SliderBrightness.SetPos(m_BrightnessLevel);
   SendCommand('h', m_SetID, m_BrightnessLevel);

   m_SliderContrast.SetPos(m_ContrastLevel);
   SendCommand('g', m_SetID, m_ContrastLevel);

   switch (m_Input) {
      case 3:
         CheckRadioButton(IDC_RADIO_INPUT_RGB, IDC_RADIO_INPUT_SVIDEO, IDC_RADIO_INPUT_SVIDEO);
         break;
      case 2:
         CheckRadioButton(IDC_RADIO_INPUT_RGB, IDC_RADIO_INPUT_SVIDEO, IDC_RADIO_INPUT_VIDEO);
         break;
      case 1:
         CheckRadioButton(IDC_RADIO_INPUT_RGB, IDC_RADIO_INPUT_SVIDEO, IDC_RADIO_INPUT_COMPONENT);
         break;
      case 0:
      default:
         CheckRadioButton(IDC_RADIO_INPUT_RGB, IDC_RADIO_INPUT_SVIDEO, IDC_RADIO_INPUT_RGB);
         break;
   }

   SendCommand('b', m_SetID, m_Input);

    return TRUE;
}

BOOLEAN CJaRCDlg::StoreOptions()
{
    char section[32];
    char outBuff[81];

    wsprintf(section, "Parameters");

    wsprintf(outBuff, "%.d", m_BrightnessLevel);
    WritePrivateProfileString (section, " Brightness", outBuff, JaRCIni); 

    wsprintf(outBuff, "%.d", m_ContrastLevel);
    WritePrivateProfileString (section, " Contrast", outBuff, JaRCIni); 

    wsprintf(outBuff, "%.d", m_Input);
    WritePrivateProfileString (section, " Input", outBuff, JaRCIni); 

    return TRUE;
}
