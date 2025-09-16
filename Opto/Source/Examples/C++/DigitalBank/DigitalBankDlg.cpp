// Opto 22 OptoMMP System Developer Kit (SDK) PAC-DEV-OPTOMMP-CPLUS
// DigitalBankDlg.cpp : implementation file
//

#include "Digital Bank.h"
#include "DigitalBankDlg.h"
#include "O22SIOMMXUtils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDigitalBankDlg dialog

CDigitalBankDlg::CDigitalBankDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDigitalBankDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDigitalBankDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDigitalBankDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDigitalBankDlg)
	DDX_Control(pDX, IDC_STATUS_EDIT, m_StatusEdit);
	DDX_Control(pDX, IDC_WRITE_STATES_BTN, m_WriteStatesBtn);
	DDX_Control(pDX, IDC_WRITE_ON_BTN, m_WriteOnBtn);
	DDX_Control(pDX, IDC_WRITE_OFF_BTN, m_WriteOffBtn);
	DDX_Control(pDX, IDC_WRITE_STATES_EDIT, m_WriteStatesEdit);
	DDX_Control(pDX, IDC_WRITE_ON_EDIT, m_WriteOnEdit);
	DDX_Control(pDX, IDC_WRITE_OFF_EDIT, m_WriteOffEdit);
	DDX_Control(pDX, IDC_CLEAR_OFF_LATCH_BTN, m_ClearOffLatchBtn);
	DDX_Control(pDX, IDC_CLEAR_ON_LATCH_BTN, m_ClearOnLatchBtn);
	DDX_Control(pDX, IDC_OFFLATCH_STATES_EDIT, m_OffLatchEdit);
	DDX_Control(pDX, IDC_ONLATCH_STATES_EDIT, m_OnLatchEdit);
	DDX_Control(pDX, IDC_READ_STATES_EDIT, m_PointsStatesEdit);
	DDX_Control(pDX, IDC_PORT_EDIT, m_PortEdit);
	DDX_Control(pDX, IDC_IP_ADDRESS_EDIT, m_IpAddressEdit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDigitalBankDlg, CDialog)
	//{{AFX_MSG_MAP(CDigitalBankDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT_BTN, OnConnectBtn)
	ON_BN_CLICKED(IDC_DISCONNECT_BTN, OnDisconnectBtn)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CLEAR_ON_LATCH_BTN, OnClearOnLatchBtn)
	ON_BN_CLICKED(IDC_CLEAR_OFF_LATCH_BTN, OnClearOffLatchBtn)
	ON_BN_CLICKED(IDC_WRITE_ON_BTN, OnWriteOnBtn)
	ON_BN_CLICKED(IDC_WRITE_OFF_BTN, OnWriteOffBtn)
	ON_BN_CLICKED(IDC_WRITE_STATES_BTN, OnWriteStatesBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDigitalBankDlg message handlers

BOOL CDigitalBankDlg::OnInitDialog()
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// Initialize edit boxes
  m_IpAddressEdit.SetWindowText("10.192.54.0");
  m_PortEdit.SetWindowText("2001");
	m_WriteStatesEdit.SetWindowText("0x0000000000000000");
	m_WriteOnEdit.SetWindowText("0x0000000000000000");
	m_WriteOffEdit.SetWindowText("0x0000000000000000");

  // Get a fixed width font
  m_CourierFont.CreateStockObject(ANSI_FIXED_FONT);

  // Set the following edit boxes to use the fixed font
  m_PointsStatesEdit.SetFont(&m_CourierFont);
  m_OnLatchEdit.SetFont(&m_CourierFont);
  m_OffLatchEdit.SetFont(&m_CourierFont);
	m_WriteStatesEdit.SetFont(&m_CourierFont);
	m_WriteOnEdit.SetFont(&m_CourierFont);
	m_WriteOffEdit.SetFont(&m_CourierFont);

  StopScanning();

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CDigitalBankDlg::OnPaint() 
//---------------------------------------------------------------------------------------
// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.
//---------------------------------------------------------------------------------------
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


HCURSOR CDigitalBankDlg::OnQueryDragIcon()
//---------------------------------------------------------------------------------------
// The system calls this to obtain the cursor to display while the user drags
// the minimized window.
//---------------------------------------------------------------------------------------
{
	return (HCURSOR) m_hIcon;
}

void CDigitalBankDlg::OnConnectBtn() 
//---------------------------------------------------------------------------------------
// Handle the connect button by starting the connection process.
//---------------------------------------------------------------------------------------
{
  int    nResult;
  CString strIpAddress;
  CString strPort;

  // Get the IP Address from the edit box
  m_IpAddressEdit.GetWindowText(strIpAddress);

  // Get the port from the edit box
  m_PortEdit.GetWindowText(strPort);

  // Start the open process
  nResult = m_Brain.OpenEnet(strIpAddress.LockBuffer(), atol(strPort), 10000, 1);
  strIpAddress.UnlockBuffer();

  CWaitCursor wait; // show the hourglass

  if (SIOMM_OK == nResult)
  {
    // Pause a moment before we check the connection.
    Sleep(250);

    // Check if the open is complete
    nResult = m_Brain.IsOpenDone();

    // If we're not connected yet, create a Cancel dialog and use a timer
    // to keep checking if the open is successful.  This allows the user to 
    // cancel the connectiong operation.
    if (SIOMM_ERROR_NOT_CONNECTED_YET == nResult)
    {
      // Create the cancel dialog
      m_dlgCancel.Create(IDD_CANCEL_OPEN, this);
      m_dlgCancel.m_bCancel = FALSE;

      // Create a timer
      m_nOpenTimerId = SetTimer(ID_OPEN_TIMER, 250, NULL);
    }
    else if (SIOMM_OK == nResult)
    {
      // Open is good, so start scanning.
      StartScanning();
    }

  } 
}


void CDigitalBankDlg::OnDisconnectBtn() 
//---------------------------------------------------------------------------------------
// Handle the disconnect button by stopping the scanning and close the connetion.
//---------------------------------------------------------------------------------------
{
  int nResult;

  // Stop the scan timer
  StopScanning();

  // Close the SNAP I/O Unit
	nResult = m_Brain.Close();

  // Handle the result
  HandleResult(nResult);

}



void CDigitalBankDlg::OnTimer(UINT nIDEvent) 
//---------------------------------------------------------------------------------------
// Handle timer message
//---------------------------------------------------------------------------------------
{
  int nResult;

  if (ID_OPEN_TIMER == nIDEvent) // Check the status of the open
  {
    // Check the status of the open
    nResult = m_Brain.IsOpenDone();
    
    if ((nResult != SIOMM_ERROR_NOT_CONNECTED_YET) || (m_dlgCancel.m_bCancel))
    {
      // Destroy the cancel dialog
      m_dlgCancel.DestroyWindow();

      // If here because cancel button was pressed, close the connection.
      if (m_dlgCancel.m_bCancel)
      {
        m_Brain.Close();
      }
      else
      {
        // If everything is okay, start the scan timer
        if (SIOMM_OK == nResult)
        {
          StartScanning();
        }
      }

      // Kill this timer
      KillTimer(nIDEvent);
    }

    // Handle the result
    HandleResult(nResult);

  }
  else if (ID_SCAN_TIMER == nIDEvent) // scan
  {
    SIOMM_DigBankReadArea DigBankData;
    CString strTemp;

    // Get digital bank point read area
    nResult = m_Brain.GetDigBankReadAreaEx(&DigBankData);

    // Check the result
    if (SIOMM_OK == nResult)
    {
      // Format the string for digital bank point states
      strTemp.Format("0x%08X%08X", DigBankData.nStatePts63to32, 
                                   DigBankData.nStatePts31to0);

      // Update the edit box for digital bank point states
      m_PointsStatesEdit.SetWindowText(strTemp);

      // Format the string for digital bank on-latch states
      strTemp.Format("0x%08X%08X", DigBankData.nOnLatchStatePts63to32, 
                                   DigBankData.nOnLatchStatePts31to0);

      // Update the edit box for digital bank on-latch states
      m_OnLatchEdit.SetWindowText(strTemp);

      // Format the string for digital bank off-latch states
      strTemp.Format("0x%08X%08X", DigBankData.nOffLatchStatePts63to32, 
                                   DigBankData.nOffLatchStatePts31to0);

      // Update the edit box for digital bank off-latch states
      m_OffLatchEdit.SetWindowText(strTemp);
    }

    // Handle the result
    HandleResult(nResult);
  }


	CDialog::OnTimer(nIDEvent);
}


void CDigitalBankDlg::OnClearOnLatchBtn() 
//---------------------------------------------------------------------------------------
// Clear On-Latches by reading and clearing each point
//---------------------------------------------------------------------------------------
{
  int nPoint;
  int nResult;
  int nState;
  
  // Clear the on-latches by reading and clearing them all.
  for (nPoint = 0; nPoint < 64; nPoint++)
  {
    nResult = m_Brain.ReadClearDigPtOnLatch(nPoint, &nState);

    // Check the result
    if (SIOMM_OK != nResult)
    {
      // Break out of the loop on error
      break;
    }
  }

  // Handle the result
  HandleResult(nResult);
}


void CDigitalBankDlg::OnClearOffLatchBtn() 
//---------------------------------------------------------------------------------------
// Clear Off-Latches by reading and clearing each point
//---------------------------------------------------------------------------------------
{
  int nPoint;
  int nResult;
  int nState;
  
  // Clear the off-latches by reading and clearing them all.
  for (nPoint = 0; nPoint < 64; nPoint++)
  {
    nResult = m_Brain.ReadClearDigPtOffLatch(nPoint, &nState);

    // Check the result
    if (SIOMM_OK != nResult)
    {
      // Break out of the loop on error
      break;
    }
  }

  // Handle the result
  HandleResult(nResult);
}


void CDigitalBankDlg::OnWriteOnBtn() 
//---------------------------------------------------------------------------------------
// Handle the Turn On button
//---------------------------------------------------------------------------------------
{
  int    nPts63to32;
  int    nPts31to0;
  CString strBitmask;
  int    nResult;

  // Get text from the edit box
  m_WriteOnEdit.GetWindowText(strBitmask);

  // Convert the string into a bitmask
  if (GetBitmask64FromString(strBitmask, &nPts63to32, &nPts31to0))
  {
    // Set the on mask
    nResult = m_Brain.SetDigBankOnMask(nPts63to32, nPts31to0);
    
    // Handle the result
    HandleResult(nResult);
  }
}

void CDigitalBankDlg::OnWriteOffBtn() 
//---------------------------------------------------------------------------------------
// Handle the Turn Off button
//---------------------------------------------------------------------------------------
{
  int    nPts63to32;
  int    nPts31to0;
  CString strBitmask;
  int    nResult;
  
  // Get text from the edit box
  m_WriteOffEdit.GetWindowText(strBitmask);

  // Convert the string into a bitmask
  if (GetBitmask64FromString(strBitmask, &nPts63to32, &nPts31to0))
  {
    // Set the on mask
    nResult = m_Brain.SetDigBankOffMask(nPts63to32, nPts31to0);
    
    // Handle the result
    HandleResult(nResult);
  }
}

void CDigitalBankDlg::OnWriteStatesBtn() 
//---------------------------------------------------------------------------------------
// Handle the Set States button
//---------------------------------------------------------------------------------------
{
  int    nPts63to32;
  int    nPts31to0;
  CString strBitmask;
  int    nResult;
  
  // Get text from the edit box
  m_WriteStatesEdit.GetWindowText(strBitmask);

  // Convert the string into a bitmask
  if (GetBitmask64FromString(strBitmask, &nPts63to32, &nPts31to0))
  {
    // Set the on mask
    nResult = m_Brain.SetDigBankPointStates(nPts63to32, nPts31to0, 0xFFFFFFFF, 0xFFFFFFFF);

    // Handle the result
    HandleResult(nResult);
  }
}


void CDigitalBankDlg::HandleResult(int nResult)
//---------------------------------------------------------------------------------------
// Handles results by updating the status text and closing the connection on error.
//---------------------------------------------------------------------------------------
{
  // Check for bad error
  if ((SIOMM_OK != nResult) && (SIOMM_ERROR_NOT_CONNECTED_YET != nResult))
  {
    // Close connect on error
    m_Brain.Close();

    // Stop scanning on error.
    StopScanning();
  }

  // Update the status text
  m_StatusEdit.SetWindowText(GetResultAsString(nResult));  
}


void CDigitalBankDlg::StartScanning()
//---------------------------------------------------------------------------------------
// Start scanning
//---------------------------------------------------------------------------------------
{
  // Enable controls
	m_WriteStatesBtn.EnableWindow(TRUE);
	m_WriteOnBtn.EnableWindow(TRUE);
	m_WriteOffBtn.EnableWindow(TRUE);
	m_ClearOffLatchBtn.EnableWindow(TRUE);
	m_ClearOnLatchBtn.EnableWindow(TRUE);

  // Start the timer
  m_nOpenTimerId = SetTimer(ID_SCAN_TIMER, 100, NULL);
}


void CDigitalBankDlg::StopScanning()
//---------------------------------------------------------------------------------------
// Stop scanning
//---------------------------------------------------------------------------------------
{
  // Stop the timer
  KillTimer(ID_SCAN_TIMER);

	// Disable controls
  m_WriteStatesBtn.EnableWindow(FALSE);
	m_WriteOnBtn.EnableWindow(FALSE);
	m_WriteOffBtn.EnableWindow(FALSE);
	m_ClearOffLatchBtn.EnableWindow(FALSE);
	m_ClearOnLatchBtn.EnableWindow(FALSE);

  // Clear text
  m_OffLatchEdit.SetWindowText("");
	m_OnLatchEdit.SetWindowText("");
	m_PointsStatesEdit.SetWindowText("");
}
