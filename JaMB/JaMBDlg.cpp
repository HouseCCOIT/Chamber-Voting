// JaMBDlg.cpp : implementation file
//

#include "stdafx.h"
#include "afxmt.h"
#include "JaMB.h"
#include "JaMBDlg.h"
#include "JaDisplay.h"
#include "MBThread.h"
#include "house.h"
#include "trace.h"

extern BOOL m_NoWeb;
extern BOOL m_TestMode;
extern BOOL m_NoBounds;

extern LEX m_JA_MemberSeatingList[MAX_SEATS];
extern unsigned char m_JA_StationLocks[MAX_VOTE_ARRAY];
extern unsigned char m_JA_RemoteVotes[MAX_VOTE_ARRAY];
extern unsigned char m_JA_CallInVotes[MAX_VOTE_ARRAY];

extern unsigned int callInCount;
extern unsigned int remoteCount;
extern unsigned int lockCount;

extern int  fileNameSel;
extern int  fileNumber;

extern CString m_JA_PrinterPrimary;
extern CString m_JA_PrinterSecondary;
extern CString m_JA_PrinterName;

CMutex LockMutex;
CMutex CallInMutex;
CMutex RemoteMutex;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog {
public:
   CAboutDlg();

   // Dialog Data
      //{{AFX_DATA(CAboutDlg)
   enum {
      IDD = IDD_ABOUTBOX
   };
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
public:

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
// CJaMBDlg dialog


BOOL m_JA_MessageBoardActive;

CJaMBDlg::CJaMBDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CJaMBDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CJaMBDlg)
   m_UpdateConsentCalendar = FALSE;
   m_UpdateCalendar = FALSE;
   m_FilesResolutions = _T("");
   m_RadioSummary = 1;
   m_FontSize = -1;
   //}}AFX_DATA_INIT
   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_radioSelected = 0;
}

void CJaMBDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CJaMBDlg)
   DDX_Control(pDX, IDC_COMBO_MB_WATERMARK, m_ComboWatermark);
   DDX_Control(pDX, IDC_COMBO_MB_FONTSIZE, m_ComboFontSize);
   DDX_Control(pDX, IDC_EDIT_MB_EDIT8, m_Edit8);
   DDX_Control(pDX, IDC_EDIT_MB_EDIT7, m_Edit7);
   DDX_Control(pDX, IDC_EDIT_MB_EDIT6, m_Edit6);
   DDX_Control(pDX, IDC_EDIT_MB_EDIT5, m_Edit5);
   DDX_Control(pDX, IDC_EDIT_MB_EDIT4, m_Edit4);
   DDX_Control(pDX, IDC_EDIT_MB_EDIT3, m_Edit3);
   DDX_Control(pDX, IDC_EDIT_MB_EDIT2, m_Edit2);
   DDX_Control(pDX, IDC_EDIT_MB_EDIT1, m_Edit1);
   DDX_Control(pDX, IDC_COMBO_MB_FONT, m_ComboFont);
   DDX_Control(pDX, IDC_COMBO_MB_ORDER_OF_BUSINESS, m_ComboOrders);
   DDX_Control(pDX, IDC_COMBO_MB_ACTIONS, m_ComboActions);
   DDX_Control(pDX, IDC_COMBO_MB_FILES_RESOLUTIONS, m_ComboFiles);
   DDX_Control(pDX, IDC_EDIT_MB_NUMBER, m_EditNumber);
   DDX_Check(pDX, IDC_CHECK_CONSENT_CALENDAR, m_UpdateConsentCalendar);
   DDX_Check(pDX, IDC_CHECK_CALENDAR, m_UpdateCalendar);
   DDX_CBString(pDX, IDC_COMBO_MB_FILES_RESOLUTIONS, m_FilesResolutions);
   DDV_MaxChars(pDX, m_FilesResolutions, 10);
   DDX_Radio(pDX, IDC_RADIO_MB_DISPLAY1, m_radioSelected);
   DDX_Radio(pDX, IDC_RADIO_SUMMARY, m_RadioSummary);
   DDX_CBIndex(pDX, IDC_COMBO_MB_FONTSIZE, m_FontSize);
   //}}AFX_DATA_MAP
   DDX_Control(pDX, IDC_CHECK_NO_WEB, m_CheckNoWeb);
   DDX_Control(pDX, IDC_LOCK_LIST, m_LockList);
   DDX_Control(pDX, IDC_REMOTE_LIST, m_RemoteList);
   DDX_Control(pDX, IDC_CALLIN_LIST, m_CallInList);
   DDX_Control(pDX, IDC_CHECK_LOCKS_ALL, m_LocksAll);
   DDX_Control(pDX, IDC_CHECK_REMOTES_ALL, m_RemotesAll);
   DDX_Control(pDX, IDC_CHECK_CALLINS_ALL, m_CallInsAll);
   DDX_Control(pDX, IDC_RADIO_REMOTE_FULL, m_RadioRemoteFull);
   DDX_Control(pDX, IDC_RADIO_CALLIN_FULL, m_RadioCallinFull);
   DDX_Control(pDX, IDC_RADIO_LOCK_FULL, m_RadioLocksFull);
}

BEGIN_MESSAGE_MAP(CJaMBDlg, CDialog)
   //{{AFX_MSG_MAP(CJaMBDlg)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_CBN_SELCHANGE(IDC_COMBO_MB_ORDER_OF_BUSINESS, OnSelchangeComboMbOrderOfBusiness)
   ON_CBN_SELCHANGE(IDC_COMBO_MB_FILES_RESOLUTIONS, OnSelchangeComboMbFilesResolutions)
   ON_CBN_SELCHANGE(IDC_COMBO_MB_ACTIONS, OnSelchangeComboMbActions)
   ON_EN_CHANGE(IDC_EDIT_MB_NUMBER, OnChangeEditMbNumber)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MB_NUMBER, OnDeltaposSpinMbNumber)
   ON_BN_CLICKED(IDC_RADIO_MB_DISPLAY1, OnRadioMbDisplay1)
   ON_BN_CLICKED(IDC_RADIO_MB_DISPLAY2, OnRadioMbDisplay2)
   ON_EN_KILLFOCUS(IDC_EDIT_MB_EDIT1, OnKillfocusEditMbEdit1)
   ON_EN_KILLFOCUS(IDC_EDIT_MB_EDIT2, OnKillfocusEditMbEdit2)
   ON_EN_KILLFOCUS(IDC_EDIT_MB_EDIT3, OnKillfocusEditMbEdit3)
   ON_EN_KILLFOCUS(IDC_EDIT_MB_EDIT4, OnKillfocusEditMbEdit4)
   ON_EN_KILLFOCUS(IDC_EDIT_MB_EDIT5, OnKillfocusEditMbEdit5)
   ON_EN_KILLFOCUS(IDC_EDIT_MB_EDIT6, OnKillfocusEditMbEdit6)
   ON_EN_KILLFOCUS(IDC_EDIT_MB_EDIT7, OnKillfocusEditMbEdit7)
   ON_EN_KILLFOCUS(IDC_EDIT_MB_EDIT8, OnKillfocusEditMbEdit8)
   ON_BN_CLICKED(IDC_CHECK_CONSENT_CALENDAR, OnCheckConsentCalendar)
   ON_BN_CLICKED(IDC_CHECK_CALENDAR, OnCheckCalendar)
   ON_CBN_SELCHANGE(IDC_COMBO_MB_CONSENT_CALENDAR, OnSelchangeComboMbConsentCalendar)
   ON_CBN_SELCHANGE(IDC_COMBO_MB_CALENDAR, OnSelchangeComboMbCalendar)
   ON_BN_CLICKED(IDC_BUTTON_TRANSMIT_DISPLAY, OnButtonTransmitDisplay)
   ON_BN_CLICKED(IDC_BUTTON_ADD_CONSENT_CALENDAR, OnButtonAddConsentCalendar)
   ON_BN_CLICKED(IDC_BUTTON_ADD_CALENDAR, OnButtonAddCalendar)
   ON_BN_CLICKED(IDC_BUTTON_CLEAR_CONSENT_CALENDAR, OnButtonClearConsentCalendar)
   ON_BN_CLICKED(IDC_BUTTON_CLEAR_CALENDAR, OnButtonClearCalendar)
   ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
   ON_CBN_SELCHANGE(IDC_LOCK_LIST, OnSelchangeLockList)
   ON_CLBN_CHKCHANGE(IDC_LOCK_LIST, OnCheckBoxChangeLockList)
   ON_BN_CLICKED(IDC_RADIO_SUMMARY, OnRadioLockSummary)
   ON_BN_CLICKED(IDC_RADIO_FULL_LIST, OnRadioLockFull)
   ON_BN_CLICKED(IDC_BUTTON_CLEAR_REMOTES, OnButtonClearRemotes)
   ON_WM_CLOSE()
   ON_CBN_DROPDOWN(IDC_COMBO_MB_FILES_RESOLUTIONS, OnDropdownComboMbFilesResolutions)
   ON_CBN_SELCHANGE(IDC_COMBO_MB_FONT, OnSelchangeComboMbFont)
   ON_CBN_SELCHANGE(IDC_COMBO_MB_FONTSIZE, OnSelchangeComboMbFontsize)
   ON_CBN_SELCHANGE(IDC_COMBO_MB_WATERMARK, OnSelchangeComboMbWatermark)
   ON_WM_MOUSEMOVE()
   //}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_BUTTON_PRINT_LOCKS, &CJaMBDlg::OnButtonPrintLocks)
   ON_BN_CLICKED(IDC_CHECK_NO_WEB, &CJaMBDlg::OnBnClickedCheckNoWeb)
   ON_BN_CLICKED(IDC_RADIO_REMOTE_FULL, &CJaMBDlg::OnRadioRemoteFull)
   ON_BN_CLICKED(IDC_RADIO_REMOTE_SUMMARY, &CJaMBDlg::OnRadioRemoteSummary)
   ON_BN_CLICKED(IDC_RADIO_CALLIN_FULL, &CJaMBDlg::OnRadioCallinFull)
   ON_BN_CLICKED(IDC_RADIO_CALLIN_SUMMARY, &CJaMBDlg::OnRadioCallinSummary)
   ON_BN_CLICKED(IDC_BUTTON_PRINT_REMOTES, &CJaMBDlg::OnButtonPrintRemotes)
   ON_BN_CLICKED(IDC_BUTTON_PRINT_CALLINS, &CJaMBDlg::OnButtonPrintCallins)
   ON_LBN_SELCHANGE(IDC_REMOTE_LIST, &CJaMBDlg::OnSelchangeRemoteList)
   ON_LBN_SELCHANGE(IDC_CALLIN_LIST, &CJaMBDlg::OnSelchangeCallinList)
   ON_BN_CLICKED(IDC_CHECK_LOCKS_ALL, &CJaMBDlg::OnBnClickedCheckLocksAll)
   ON_BN_CLICKED(IDC_CHECK_REMOTES_ALL, &CJaMBDlg::OnBnClickedCheckRemotesAll)
   ON_BN_CLICKED(IDC_CHECK_CALLINS_ALL, &CJaMBDlg::OnBnClickedCheckCallinsAll)
   ON_WM_TIMER()
   ON_CBN_DBLCLK(IDC_CALLIN_LIST, &CJaMBDlg::OnDblclkCallinList)
   ON_CBN_DBLCLK(IDC_REMOTE_LIST, &CJaMBDlg::OnDblclkRemoteList)
   ON_CLBN_CHKCHANGE(IDC_CALLIN_LIST, OnCheckBoxChangeCallinList)
   ON_CLBN_CHKCHANGE(IDC_REMOTE_LIST, OnCheckBoxChangeRemoteList)
   ON_CBN_DBLCLK(IDC_LOCK_LIST, &CJaMBDlg::OnDblclkLockList)
END_MESSAGE_MAP()

// TBD - TEST_ONLY
CString JaMBOptIni = "V:\\JaMBOpt.Ini";
//CString JaMBOptIni = "C:\\JaVee\\JaMBOpt.Ini";

CString StationLockFile = "V:\\JaLock.dat";
CString RemoteVotesFile = "V:\\JaRemoteVotes.dat";
CString CallInVotesFile = "V:\\JaCallInVotes.dat";


static FILETIME callinLastWrite;
static FILETIME remoteLastWrite;
static FILETIME lockLastWrite;

extern JaResults* pDisplayDlg;
extern CJaMBDlg* m_JaMBDialog;

/////////////////////////////////////////////////////////////////////////////
// CJaMBDlg message handlers

BOOL CJaMBDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Add "About..." menu item to system menu.

   TRACE("Entry");

   // IDM_ABOUTBOX must be in the system command range.
   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);

   CMenu* pSysMenu = GetSystemMenu(FALSE);
   if (pSysMenu != NULL) {
      CString strAboutMenu;
      strAboutMenu.LoadString(IDS_ABOUTBOX);
      if (!strAboutMenu.IsEmpty()) {
         pSysMenu->AppendMenu(MF_SEPARATOR);
         pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
   }

   if (m_NoWeb) {
      m_CheckNoWeb.SetCheck(m_NoWeb);
      SetWindowText("NO WEB");
   }

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);         // Set big icon
   SetIcon(m_hIcon, FALSE);        // Set small icon

   if (m_TestMode) {
      m_ServerIPAddress = "127.0.0.1";
   }
   else {
      m_ServerIPAddress = "192.168.10.2";
   }

   CFileFind finder;
   BOOL bWorking;

   bWorking = finder.FindFile("V:\\bitmaps\\*.bmp");
   while (bWorking) {
      bWorking = finder.FindNextFile();
      m_ComboWatermark.AddString(finder.GetFileName());
   }

   // TBD - TEST_ONLY
   //MoveWindow(0, 0, 1024, 768);

   // Initialize Drop Down List Boxes
   LoadMBOptions();

   m_MyFont.CreatePointFont(m_FontSizePoints, m_FontName);

   m_Edit1.SetFont(&m_MyFont);
   m_Edit2.SetFont(&m_MyFont);
   m_Edit3.SetFont(&m_MyFont);
   m_Edit4.SetFont(&m_MyFont);
   m_Edit5.SetFont(&m_MyFont);
   m_Edit6.SetFont(&m_MyFont);
   m_Edit7.SetFont(&m_MyFont);
   //m_Edit8.SetFont(&m_MyFont);

   m_Edit1.SetLimitText(m_DisplayLength);
   m_Edit2.SetLimitText(m_DisplayLength);
   m_Edit3.SetLimitText(m_DisplayLength);
   m_Edit4.SetLimitText(m_DisplayLength);
   m_Edit5.SetLimitText(m_DisplayLength);
   m_Edit6.SetLimitText(m_DisplayLength);
   m_Edit7.SetLimitText(m_DisplayLength);
   //m_Edit8.SetLimitText(m_DisplayLength);

   m_Edit8.EnableWindow(false);

   m_UpdateConsentCalendar = FALSE;
   m_FileNumber = 0;
   m_OrdersOfBusiness = "";
   m_FilesResolutions = "";
   m_Actions = "";
   m_ActiveDisplay = 0;

   m_OB[0] = "";
   m_OB[1] = "";
   m_FR[0] = "";
   m_FR[1] = "";
   m_FN[0] = 0;
   m_FN[1] = 0;
   m_AC[0] = "";
   m_AC[1] = "";

   JaLoadParametersAndSeating();

   for (int i = 0; i < sizeof(m_JA_RemoteVotes); i++) {
      m_JA_RemoteVotes[i] = 0;
   }
   for (int i = 0; i < sizeof(m_JA_CallInVotes); i++) {
      m_JA_CallInVotes[i] = 0;
   }

   JaReadLocks();
   JaReadRemotes();
   JaReadCallIns();

   m_LockList.SetCheckStyle(BS_AUTOCHECKBOX);

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
         m_LockList.AddString(m_JA_MemberSeatingList[i].name);
         int tmpItem = m_LockList.FindString(-1, m_JA_MemberSeatingList[i].name);
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         m_LockList.SetItemData(tmpItem, tmpSeat);
         // TBD: Remove when representative wakes up
         if (strncmp(m_JA_MemberSeatingList[i].name, "ZZZ", 3) == 0) {
            m_JA_StationLocks[tmpSeat - 1] = BST_CHECKED;
         }
         m_LockList.SetCheck(tmpItem, m_JA_StationLocks[tmpSeat - 1]);
      }
   }
   m_RadioLocksFull.SetCheck(1);

   m_RemoteList.SetCheckStyle(BS_AUTOCHECKBOX);

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
         m_RemoteList.AddString(m_JA_MemberSeatingList[i].name);
         int tmpItem = m_RemoteList.FindString(-1, m_JA_MemberSeatingList[i].name);
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         m_RemoteList.SetItemData(tmpItem, tmpSeat);
         // TBD: Remove when representative wakes up
         if (strncmp(m_JA_MemberSeatingList[i].name, "ZZZ", 3) == 0) {
            m_JA_RemoteVotes[tmpSeat - 1] = BST_CHECKED;
         }
         m_RemoteList.SetCheck(tmpItem, m_JA_RemoteVotes[tmpSeat - 1]);
      }
   }
   m_RadioRemoteFull.SetCheck(1);

   m_CallInList.SetCheckStyle(BS_AUTOCHECKBOX);

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
         m_CallInList.AddString(m_JA_MemberSeatingList[i].name);
         int tmpItem = m_CallInList.FindString(-1, m_JA_MemberSeatingList[i].name);
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         m_CallInList.SetItemData(tmpItem, tmpSeat);
         // TBD: Remove when representative wakes up
         if (strncmp(m_JA_MemberSeatingList[i].name, "ZZZ", 3) == 0) {
            m_JA_CallInVotes[tmpSeat - 1] = BST_CHECKED;
         }
         m_CallInList.SetCheck(tmpItem, m_JA_CallInVotes[tmpSeat - 1]);
      }
   }
   m_RadioCallinFull.SetCheck(1);

   // Update lists mutual enable/disable status
   EnableDisableLists();

   CenterWindow(); // MoveWindow(10, 10);

   //setsystemmo

   //DoModal();

   m_SwitchingDisplays = FALSE;

   //CWinThread* MessageBoardThread;

   pDisplayDlg = new JaResults();
   pDisplayDlg->Create(IDD_JADISPLAY_DIALOG);

   m_JaMBDialog = this;

   // MessageBoardThread = AfxBeginThread(JaMBThreadFunction, pDisplayDlg);

   ConnectToVM();   // Open socket to Voting Machine app

   // Specify the timer interval for checking CallIn and Remote file.
   UINT    nElapse = 2000;
   SetTimer(1, nElapse, NULL);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CJaMBDlg::ConnectToVM()
{
   if (AfxSocketInit() == FALSE) {
      AfxMessageBox("Failed to Initialize Sockets");
      return;
   }

   if (m_Client.Create() == FALSE) {
      MessageBox("Failed to Create Socket");
      return;
   }

   if (m_Client.Connect(m_ServerIPAddress, 1003) == FALSE) {
      int error = GetLastError();
      MessageBox("Failed to Connect");
      return;
   }

   m_Client.pDlg = this;
}

void CJaMBDlg::SendVMMessage(SockPacket *packet)
{
   m_Client.SendVMMessage(packet);
}

void CJaMBDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
   }
   else {
      CDialog::OnSysCommand(nID, lParam);
   }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJaMBDlg::OnPaint()
{
   if (IsIconic()) {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

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
   else {
      CDialog::OnPaint();
   }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CJaMBDlg::OnQueryDragIcon()
{
   return(HCURSOR)m_hIcon;
}

void CJaMBDlg::OnSelchangeComboMbOrderOfBusiness()
{
   m_ComboOrders.GetWindowText(m_OrdersOfBusiness);

   m_FilesResolutions = "   ";
   m_Actions = "   ";
   m_FileNumber = 0;

   m_OB[m_ActiveDisplay] = m_OrdersOfBusiness;
   m_FR[m_ActiveDisplay] = "   ";
   m_FI[m_ActiveDisplay] = "   ";
   m_AC[m_ActiveDisplay] = "   ";
   m_FN[m_ActiveDisplay] = 0;

   m_ComboFiles.SetCurSel(m_ComboFiles.FindString(-1, m_FilesResolutions));

   m_EditNumber.SetWindowText("");

   m_ComboActions.SetCurSel(m_ComboActions.FindString(-1, m_Actions));

   if (!m_UpdateConsentCalendar && !m_UpdateCalendar && !m_SwitchingDisplays) {
      UpdateDisplay();
   }
}

void CJaMBDlg::OnSelchangeComboMbFilesResolutions()
{
   m_ComboFiles.GetWindowText(m_FilesResolutions);

   fileNameSel = m_ComboFiles.GetCurSel();

   m_FR[m_ActiveDisplay] = m_FilesResolutions;
   m_FN[m_ActiveDisplay] = 0;

   //    if (!m_UpdateConsentCalendar && !m_UpdateCalendar && !m_SwitchingDisplays) {
   //        UpdateDisplay();
   //    }

   m_EditNumber.SetWindowText("");
   //    m_EditNumber.SetFocus();
}

void CJaMBDlg::OnSelchangeComboMbActions()
{
   m_ComboActions.GetWindowText(m_Actions);

   m_AC[m_ActiveDisplay] = m_Actions;

   if (!m_UpdateConsentCalendar && !m_UpdateCalendar && !m_SwitchingDisplays) {
      UpdateDisplay();
   }
}

void CJaMBDlg::OnChangeEditMbNumber()
{
   char tmpBuff[64];
   unsigned int tmpNumber;

   if (m_EditNumber.GetWindowTextLength() == 0) {
      return;
   }

   tmpNumber = GetDlgItemInt(IDC_EDIT_MB_NUMBER);

   if ((tmpNumber > 0) && (tmpNumber <= 10000)) {
      m_FileNumber = tmpNumber;
   }
   else {
      m_FileNumber = 0;
      m_EditNumber.SetWindowText("");
   }

   fileNumber = m_FileNumber;

   m_FN[m_ActiveDisplay] = m_FileNumber;

   if (m_FN[m_ActiveDisplay] == 0) {
      wsprintf(tmpBuff, "%s", m_FR[m_ActiveDisplay]);
   }
   else {
      wsprintf(tmpBuff, "%s %d", m_FR[m_ActiveDisplay], m_FN[m_ActiveDisplay]);
   }

   m_FI[m_ActiveDisplay] = tmpBuff;

   if (!m_UpdateConsentCalendar && !m_UpdateCalendar && !m_SwitchingDisplays) {
      UpdateDisplay();
   }
}

void CJaMBDlg::OnDeltaposSpinMbNumber(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

   CEdit* sEdit = (CEdit*)GetDlgItem(IDC_SPIN_MB_NUMBER);
   sEdit->SetFocus();
   m_EditNumber.SetFocus();

   if ((pNMUpDown->iDelta < 0) && (m_FileNumber >= 0) && (m_FileNumber < 10000)) {
      m_FileNumber++;
   }
   else if ((pNMUpDown->iDelta > 0) && (m_FileNumber > 0) && (m_FileNumber <= 10000)) {
      m_FileNumber--;
   }

   fileNumber = m_FileNumber;

   char buf[6];
   wsprintf(buf, "%.d", m_FileNumber);
   m_EditNumber.SetWindowText(buf);

   *pResult = 0;
}

void CJaMBDlg::OnRadioMbDisplay1()
{
   CStatic* pStatic = (CStatic*)GetDlgItem(IDC_STATIC_DISPLAY);
   pStatic->SetWindowText("Display One");

   setActiveDisplay(0);
   m_ActiveDisplay = 0;

   if (!m_UpdateConsentCalendar && !m_UpdateCalendar) {
      SwitchDisplay();
   }
}

void CJaMBDlg::OnRadioMbDisplay2()
{
   CStatic* pStatic = (CStatic*)GetDlgItem(IDC_STATIC_DISPLAY);
   pStatic->SetWindowText("Display Two");

   setActiveDisplay(1);
   m_ActiveDisplay = 1;

   if (!m_UpdateConsentCalendar && !m_UpdateCalendar) {
      SwitchDisplay();
   }
}

void CJaMBDlg::OnKillfocusEditMbEdit1()
{
   m_Edit1.GetWindowText(m_DisplayLine[m_ActiveDisplay][0]);
   process_line((LPCTSTR)m_DisplayLine[m_ActiveDisplay][0], 0, TRUE);
}

void CJaMBDlg::OnKillfocusEditMbEdit2()
{
   m_Edit2.GetWindowText(m_DisplayLine[m_ActiveDisplay][1]);
   process_line((LPCTSTR)m_DisplayLine[m_ActiveDisplay][1], 1, TRUE);
}

void CJaMBDlg::OnKillfocusEditMbEdit3()
{
   m_Edit3.GetWindowText(m_DisplayLine[m_ActiveDisplay][2]);
   process_line((LPCTSTR)m_DisplayLine[m_ActiveDisplay][2], 2, TRUE);
}

void CJaMBDlg::OnKillfocusEditMbEdit4()
{
   m_Edit4.GetWindowText(m_DisplayLine[m_ActiveDisplay][3]);
   process_line((LPCTSTR)m_DisplayLine[m_ActiveDisplay][3], 3, TRUE);
}

void CJaMBDlg::OnKillfocusEditMbEdit5()
{
   m_Edit5.GetWindowText(m_DisplayLine[m_ActiveDisplay][4]);
   process_line((LPCTSTR)m_DisplayLine[m_ActiveDisplay][4], 4, TRUE);
}

void CJaMBDlg::OnKillfocusEditMbEdit6()
{
   m_Edit6.GetWindowText(m_DisplayLine[m_ActiveDisplay][5]);
   process_line((LPCTSTR)m_DisplayLine[m_ActiveDisplay][5], 5, TRUE);
}

void CJaMBDlg::OnKillfocusEditMbEdit7()
{
   m_Edit7.GetWindowText(m_DisplayLine[m_ActiveDisplay][6]);
   process_line((LPCTSTR)m_DisplayLine[m_ActiveDisplay][6], 6, TRUE);
}

void CJaMBDlg::OnKillfocusEditMbEdit8()
{
   //m_Edit8.GetWindowText(m_DisplayLine[m_ActiveDisplay][7]);
   //process_line((LPCTSTR)m_DisplayLine[m_ActiveDisplay][7], 7, TRUE);
}

void CJaMBDlg::OnCheckConsentCalendar()
{
   m_ComboOrders.EnableWindow(m_UpdateConsentCalendar);
   m_ComboActions.EnableWindow(m_UpdateConsentCalendar);
   m_Edit1.EnableWindow(m_UpdateConsentCalendar);
   m_Edit2.EnableWindow(m_UpdateConsentCalendar);
   m_Edit3.EnableWindow(m_UpdateConsentCalendar);
   m_Edit4.EnableWindow(m_UpdateConsentCalendar);
   m_Edit5.EnableWindow(m_UpdateConsentCalendar);
   m_Edit6.EnableWindow(m_UpdateConsentCalendar);
   m_Edit7.EnableWindow(m_UpdateConsentCalendar);
   //m_Edit8.EnableWindow(m_UpdateConsentCalendar);
   GetDlgItem(IDC_RADIO_MB_DISPLAY1)->EnableWindow(m_UpdateConsentCalendar);
   GetDlgItem(IDC_RADIO_MB_DISPLAY2)->EnableWindow(m_UpdateConsentCalendar);
   GetDlgItem(IDC_CHECK_CALENDAR)->EnableWindow(m_UpdateConsentCalendar);
   GetDlgItem(IDC_COMBO_MB_CALENDAR)->EnableWindow(m_UpdateConsentCalendar);
   GetDlgItem(IDC_BUTTON_ADD_CONSENT_CALENDAR)->EnableWindow(!m_UpdateConsentCalendar);
   GetDlgItem(IDC_BUTTON_CLEAR_CALENDAR)->EnableWindow(m_UpdateConsentCalendar);
   GetDlgItem(IDC_BUTTON_TRANSMIT_DISPLAY)->EnableWindow(m_UpdateConsentCalendar);
   GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(m_UpdateConsentCalendar);
   //
   m_OrdersOfBusiness = "CONSENT CALENDAR";
   m_FilesResolutions = "   ";
   m_Actions = "   ";
   m_FileNumber = 0;

   m_OB[m_ActiveDisplay] = m_OrdersOfBusiness;
   m_FR[m_ActiveDisplay] = "   ";
   m_FI[m_ActiveDisplay] = "   ";
   m_AC[m_ActiveDisplay] = "   ";
   m_FN[m_ActiveDisplay] = 0;

   m_ComboOrders.SetCurSel(m_ComboOrders.FindString(-1, m_OrdersOfBusiness));
   m_ComboFiles.SetCurSel(m_ComboFiles.FindString(-1, m_FilesResolutions));
   m_EditNumber.SetWindowText("");
   m_ComboActions.SetCurSel(m_ComboActions.FindString(-1, m_Actions));

   if (!m_UpdateConsentCalendar && !m_UpdateCalendar && !m_SwitchingDisplays) {
      UpdateDisplay();
   }
   //
   if (m_UpdateConsentCalendar) {
      m_UpdateConsentCalendar = FALSE;
      OnButtonClear();
   }
   else {
      m_UpdateConsentCalendar = TRUE;
   }
}

void CJaMBDlg::OnCheckCalendar()
{
   m_ComboOrders.EnableWindow(m_UpdateCalendar);
   m_ComboActions.EnableWindow(m_UpdateCalendar);
   m_Edit1.EnableWindow(m_UpdateCalendar);
   m_Edit2.EnableWindow(m_UpdateCalendar);
   m_Edit3.EnableWindow(m_UpdateCalendar);
   m_Edit4.EnableWindow(m_UpdateCalendar);
   m_Edit5.EnableWindow(m_UpdateCalendar);
   m_Edit6.EnableWindow(m_UpdateCalendar);
   m_Edit7.EnableWindow(m_UpdateCalendar);
   //m_Edit8.EnableWindow(m_UpdateCalendar);
   GetDlgItem(IDC_RADIO_MB_DISPLAY1)->EnableWindow(m_UpdateCalendar);
   GetDlgItem(IDC_RADIO_MB_DISPLAY2)->EnableWindow(m_UpdateCalendar);
   GetDlgItem(IDC_CHECK_CONSENT_CALENDAR)->EnableWindow(m_UpdateCalendar);
   GetDlgItem(IDC_COMBO_MB_CONSENT_CALENDAR)->EnableWindow(m_UpdateCalendar);
   GetDlgItem(IDC_BUTTON_ADD_CALENDAR)->EnableWindow(!m_UpdateCalendar);
   GetDlgItem(IDC_BUTTON_CLEAR_CONSENT_CALENDAR)->EnableWindow(m_UpdateCalendar);
   GetDlgItem(IDC_BUTTON_TRANSMIT_DISPLAY)->EnableWindow(m_UpdateCalendar);
   GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(m_UpdateCalendar);
   //
   m_OrdersOfBusiness = "CALENDAR FOR THE DAY";
   m_FilesResolutions = "   ";
   m_Actions = "   ";
   m_FileNumber = 0;

   m_OB[m_ActiveDisplay] = m_OrdersOfBusiness;
   m_FR[m_ActiveDisplay] = "   ";
   m_FI[m_ActiveDisplay] = "   ";
   m_AC[m_ActiveDisplay] = "   ";
   m_FN[m_ActiveDisplay] = 0;

   m_ComboOrders.SetCurSel(m_ComboOrders.FindString(-1, m_OrdersOfBusiness));
   m_ComboFiles.SetCurSel(m_ComboFiles.FindString(-1, m_FilesResolutions));
   m_EditNumber.SetWindowText("");
   m_ComboActions.SetCurSel(m_ComboActions.FindString(-1, m_Actions));

   if (!m_UpdateConsentCalendar && !m_UpdateCalendar && !m_SwitchingDisplays) {
      UpdateDisplay();
   }
   //
   if (m_UpdateCalendar) {
      m_UpdateCalendar = FALSE;
      OnButtonClear();
   }
   else {
      m_UpdateCalendar = TRUE;
   }
}


void CJaMBDlg::OnSelchangeComboMbConsentCalendar()
{
   CString tmpStr;

   m_ComboOrders.SetCurSel(m_ComboOrders.FindString(-1, "CONSENT CALENDAR"));
   m_ComboOrders.GetWindowText(m_OrdersOfBusiness);
   m_OB[m_ActiveDisplay] = m_OrdersOfBusiness;

   GetDlgItem(IDC_COMBO_MB_CONSENT_CALENDAR)->GetWindowText(tmpStr);

   int lLen = tmpStr.ReverseFind('.') + 1;
   int rLen = tmpStr.GetLength() - lLen;

   m_ComboFiles.SelectString(-1, tmpStr.Left(lLen));
   m_FR[m_ActiveDisplay] = tmpStr.Left(lLen);         // EAK shouldn't have to do this

   m_EditNumber.SetWindowText(tmpStr.Right(rLen));

   if (!m_UpdateConsentCalendar && !m_UpdateCalendar && !m_SwitchingDisplays) {
      UpdateDisplay();
   }
}

void CJaMBDlg::OnSelchangeComboMbCalendar()
{
   CString tmpStr;

   m_ComboOrders.SetCurSel(m_ComboOrders.FindString(-1, "CALENDAR FOR THE DAY"));

   m_ComboOrders.GetWindowText(m_OrdersOfBusiness);
   m_OB[m_ActiveDisplay] = m_OrdersOfBusiness;

   GetDlgItem(IDC_COMBO_MB_CALENDAR)->GetWindowText(tmpStr);

   int lLen = tmpStr.ReverseFind('.') + 1;
   int rLen = tmpStr.GetLength() - lLen;

   m_ComboFiles.SelectString(-1, tmpStr.Left(lLen));
   m_FR[m_ActiveDisplay] = tmpStr.Left(lLen);         // EAK shouldn't have to do this

   m_EditNumber.SetWindowText(tmpStr.Right(rLen));

   if (!m_UpdateConsentCalendar && !m_UpdateCalendar && !m_SwitchingDisplays) {
      UpdateDisplay();
   }
}


void CJaMBDlg::UpdateDisplay()
{
   CString  StrTmp;
   int      StrLength, index;

   m_MyFont.DeleteObject();
   m_MyFont.CreatePointFont(m_FontSizePoints, m_FontName);

   m_Edit1.SetFont(&m_MyFont);
   m_Edit2.SetFont(&m_MyFont);
   m_Edit3.SetFont(&m_MyFont);
   m_Edit4.SetFont(&m_MyFont);
   m_Edit5.SetFont(&m_MyFont);
   m_Edit6.SetFont(&m_MyFont);
   m_Edit7.SetFont(&m_MyFont);
   //m_Edit8.SetFont(&m_MyFont);

   m_Edit1.SetLimitText(m_DisplayLength);
   m_Edit2.SetLimitText(m_DisplayLength);
   m_Edit3.SetLimitText(m_DisplayLength);
   m_Edit4.SetLimitText(m_DisplayLength);
   m_Edit5.SetLimitText(m_DisplayLength);
   m_Edit6.SetLimitText(m_DisplayLength);
   m_Edit7.SetLimitText(m_DisplayLength);
   //m_Edit8.SetLimitText(m_DisplayLength);

   for (int i = 0; i < 8; i++) {
      m_DisplayLine[m_ActiveDisplay][i] = "";
      process_line(m_DisplayLine[m_ActiveDisplay][i], i, 1);
   }

   // Update display of Orders of Business

   StrLength = m_OB[m_ActiveDisplay].GetLength();

   if (StrLength <= m_DisplayLength) {
      m_DisplayLine[m_ActiveDisplay][0] = m_OB[m_ActiveDisplay];
   }
   else {
      StrTmp = m_OB[m_ActiveDisplay];

      do {
         index = StrTmp.ReverseFind(' ');
         StrTmp = StrTmp.Left(index);
      } while (index > m_DisplayLength);

      m_DisplayLine[m_ActiveDisplay][0] = m_OB[m_ActiveDisplay].Left(index);
      m_DisplayLine[m_ActiveDisplay][1] = m_OB[m_ActiveDisplay].Right(StrLength - index);
      m_DisplayLine[m_ActiveDisplay][1].TrimLeft(' ');
   }

   // Update display of Files and Resolutions

   StrLength = m_FI[m_ActiveDisplay].GetLength();

   if (StrLength <= m_DisplayLength) {
      m_DisplayLine[m_ActiveDisplay][2] = m_FI[m_ActiveDisplay];
   }
   else {
      if (m_FI[m_ActiveDisplay].Find("Report") != -1) {
         index = m_FI[m_ActiveDisplay].Find(' ', m_FI[m_ActiveDisplay].Find("Report"));
      }
      else if (m_FI[m_ActiveDisplay].Find("Concurrence") != -1) {
         index = m_FI[m_ActiveDisplay].Find(' ', m_FI[m_ActiveDisplay].Find("Concurrence"));
      }
      else if (m_FI[m_ActiveDisplay].Find("Rpt.") != -1) {
         index = m_FI[m_ActiveDisplay].Find(' ', m_FI[m_ActiveDisplay].Find("Rpt."));
      }
      else {
         StrTmp = m_FI[m_ActiveDisplay];

         do {
            index = StrTmp.ReverseFind(' ');
            StrTmp = StrTmp.Left(index);
         } while (index > m_DisplayLength);
      }

      m_DisplayLine[m_ActiveDisplay][2] = m_FI[m_ActiveDisplay].Left(index);
      m_DisplayLine[m_ActiveDisplay][3] = m_FI[m_ActiveDisplay].Right(StrLength - index);
      m_DisplayLine[m_ActiveDisplay][3].TrimLeft(' ');
   }

   // Update display of Actions

   StrLength = m_AC[m_ActiveDisplay].GetLength();

   if (StrLength <= m_DisplayLength) {
      m_DisplayLine[m_ActiveDisplay][4] = m_AC[m_ActiveDisplay];
   }
   else {
      StrTmp = m_AC[m_ActiveDisplay];

      do {
         index = StrTmp.ReverseFind(' ');
         StrTmp = StrTmp.Left(index);
      } while (index > m_DisplayLength);

      m_DisplayLine[m_ActiveDisplay][4] = m_AC[m_ActiveDisplay].Left(index);
      m_DisplayLine[m_ActiveDisplay][5] = m_AC[m_ActiveDisplay].Right(StrLength - index);
      m_DisplayLine[m_ActiveDisplay][5].TrimLeft(' ');
   }

   m_Edit1.SetWindowText(m_DisplayLine[m_ActiveDisplay][0]);
   process_line(m_DisplayLine[m_ActiveDisplay][0], 0, 1);
   m_Edit2.SetWindowText(m_DisplayLine[m_ActiveDisplay][1]);
   process_line(m_DisplayLine[m_ActiveDisplay][1], 1, 1);
   m_Edit3.SetWindowText(m_DisplayLine[m_ActiveDisplay][2]);
   process_line(m_DisplayLine[m_ActiveDisplay][2], 2, 1);
   m_Edit4.SetWindowText(m_DisplayLine[m_ActiveDisplay][3]);
   process_line(m_DisplayLine[m_ActiveDisplay][3], 3, 1);
   m_Edit5.SetWindowText(m_DisplayLine[m_ActiveDisplay][4]);
   process_line(m_DisplayLine[m_ActiveDisplay][4], 4, 1);
   m_Edit6.SetWindowText(m_DisplayLine[m_ActiveDisplay][5]);
   process_line(m_DisplayLine[m_ActiveDisplay][5], 5, 1);
   m_Edit7.SetWindowText(m_DisplayLine[m_ActiveDisplay][6]);
   process_line(m_DisplayLine[m_ActiveDisplay][6], 6, 1);
   //m_Edit8.SetWindowText(m_DisplayLine[m_ActiveDisplay][7]);
   //process_line(m_DisplayLine[m_ActiveDisplay][7], 7, 1);
}

void CJaMBDlg::SwitchDisplay()
{
   m_SwitchingDisplays = TRUE;

   m_ComboOrders.SetCurSel(m_ComboOrders.FindString(-1, m_OB[m_ActiveDisplay]));

   m_ComboFiles.SetCurSel(m_ComboFiles.FindString(-1, m_FR[m_ActiveDisplay]));

   m_ComboActions.SetCurSel(m_ComboActions.FindString(-1, m_AC[m_ActiveDisplay]));

   char buf[5];
   wsprintf(buf, "%.d", m_FN[m_ActiveDisplay]);
   m_EditNumber.SetWindowText(buf);

   m_Edit1.SetWindowText(m_DisplayLine[m_ActiveDisplay][0]);
   process_line(m_DisplayLine[m_ActiveDisplay][0], 0, 1);
   m_Edit2.SetWindowText(m_DisplayLine[m_ActiveDisplay][1]);
   process_line(m_DisplayLine[m_ActiveDisplay][1], 1, 1);
   m_Edit3.SetWindowText(m_DisplayLine[m_ActiveDisplay][2]);
   process_line(m_DisplayLine[m_ActiveDisplay][2], 2, 1);
   m_Edit4.SetWindowText(m_DisplayLine[m_ActiveDisplay][3]);
   process_line(m_DisplayLine[m_ActiveDisplay][3], 3, 1);
   m_Edit5.SetWindowText(m_DisplayLine[m_ActiveDisplay][4]);
   process_line(m_DisplayLine[m_ActiveDisplay][4], 4, 1);
   m_Edit6.SetWindowText(m_DisplayLine[m_ActiveDisplay][5]);
   process_line(m_DisplayLine[m_ActiveDisplay][5], 5, 1);
   m_Edit7.SetWindowText(m_DisplayLine[m_ActiveDisplay][6]);
   process_line(m_DisplayLine[m_ActiveDisplay][6], 6, 1);
   //m_Edit8.SetWindowText(m_DisplayLine[m_ActiveDisplay][7]);
   //process_line(m_DisplayLine[m_ActiveDisplay][7], 7, 1);

   m_SwitchingDisplays = FALSE;
}


void CJaMBDlg::OnButtonTransmitDisplay()
{
   pDisplayDlg->SetFont(m_FontName);

   pDisplayDlg->SetFontSize(m_FontSize);

   //pDisplayDlg->SetWaterMark(m_Watermark);

   pDisplayDlg->ClearSix();

   TransmitDisplay();

   if (!m_TestMode) {
      //      int nResponse = pDisplayDlg->ShowWindow(SW_SHOW);
   }

   StoreMBOptions();
}

void CJaMBDlg::OnButtonAddConsentCalendar()
{
   CComboBox* pList = (CComboBox*)GetDlgItem(IDC_COMBO_MB_CONSENT_CALENDAR);

   if (m_FN[m_ActiveDisplay]) {
      if (pList->FindString(0, m_FI[m_ActiveDisplay]) == CB_ERR) {
         pList->AddString(m_FI[m_ActiveDisplay]);
         pList->SetCurSel(pList->FindString(-1, m_FI[m_ActiveDisplay]));
      }
   }
}

void CJaMBDlg::OnButtonAddCalendar()
{
   CComboBox* pList = (CComboBox*)GetDlgItem(IDC_COMBO_MB_CALENDAR);

   if (m_FN[m_ActiveDisplay]) {
      if (pList->FindString(0, m_FI[m_ActiveDisplay]) == CB_ERR) {
         pList->AddString(m_FI[m_ActiveDisplay]);
         pList->SetCurSel(pList->FindString(-1, m_FI[m_ActiveDisplay]));
      }
   }
}


void CJaMBDlg::OnButtonClearConsentCalendar()
{
   CComboBox* pList = (CComboBox*)GetDlgItem(IDC_COMBO_MB_CONSENT_CALENDAR);

   if (AfxMessageBox("Clear Consent Calendar?", MB_YESNO) == IDYES) {
      pList->ResetContent();
   }
}

void CJaMBDlg::OnButtonClearCalendar()
{
   CComboBox* pList = (CComboBox*)GetDlgItem(IDC_COMBO_MB_CALENDAR);

   if (AfxMessageBox("Clear Calendar?", MB_YESNO) == IDYES) {
      pList->ResetContent();
   }
}


void CJaMBDlg::OnButtonClear()
{
   m_OrdersOfBusiness = "   ";
   m_FilesResolutions = "   ";
   m_Actions = "   ";
   m_FileNumber = 0;

   m_OB[m_ActiveDisplay] = m_OrdersOfBusiness;
   m_FR[m_ActiveDisplay] = "   ";
   m_FI[m_ActiveDisplay] = "   ";
   m_AC[m_ActiveDisplay] = "   ";
   m_FN[m_ActiveDisplay] = 0;

   m_ComboOrders.SetCurSel(m_ComboOrders.FindString(-1, m_OrdersOfBusiness));

   m_ComboFiles.SetCurSel(m_ComboFiles.FindString(-1, m_FilesResolutions));

   m_EditNumber.SetWindowText("");

   m_ComboActions.SetCurSel(m_ComboActions.FindString(-1, m_Actions));

   if (!m_UpdateConsentCalendar && !m_UpdateCalendar) {
      UpdateDisplay();
   }
}

void CJaMBDlg::OnCancel()
{
   if (AfxMessageBox("Are you sure you want to exit?", MB_YESNO) == IDYES) {
      delete pDisplayDlg;
      CDialog::OnCancel();
   }

  // TBD 02092021 MD_ClearAll();
}


void CJaMBDlg::OnButtonClearRemotes()
{
   ClearDisplays();
}

void CJaMBDlg::OnClose()
{
   CDialog::OnClose();
}

void CJaMBDlg::OnOK()
{
   // TODO: Add extra validation here

   CDialog::OnOK();
}

void CJaMBDlg::OnDropdownComboMbFilesResolutions()
{
   m_ComboFiles.SetDroppedWidth(200);
}

void CJaMBDlg::OnSelchangeComboMbFont()
{
   CString fontName;

   m_ComboFont.GetWindowText(fontName);

   m_FontName = fontName;

   UpdateDisplay();
}

void CJaMBDlg::OnSelchangeComboMbFontsize()
{
   m_FontSize = m_ComboFontSize.GetCurSel() + 1;

   switch (m_FontSize) {
   case FontSizeLarge:
      m_FontSizePoints = 165;
      m_DisplayLength = 20;
      break;

   case FontSizeMedium:
      m_FontSizePoints = 155;
      m_DisplayLength = 24;
      break;

   case FontSizeSmall:
   default:
      m_FontSizePoints = 140;
      m_DisplayLength = 30;
      break;
   }

   UpdateDisplay();
}

typedef struct {
   int    count;
   CHAR  strings[40][64];
} myData;


bool CJaMBDlg::LoadMBOptions()
{
   FILE* tmpFile;

   char section[32];
   char key[32];
   char tmpStr[256];

   int i;

   myData OOBData;

   // Does JaMBOpt.Ini file exist?
   if ((tmpFile = fopen(JaMBOptIni, "r+")) == NULL) {
      return FALSE;
   }
   else {
      fclose(tmpFile);
   }

   m_ComboOrders.ResetContent();
   m_ComboActions.ResetContent();
   m_ComboFiles.ResetContent();
   m_ComboFont.ResetContent();

   wsprintf(section, "MBOptions");

   wsprintf(key, "OrderOfBusiness");
   GetPrivateProfileStruct(section, key, &OOBData, sizeof(myData), JaMBOptIni);
   m_ComboOrders.AddString("");
   for (i = 0; i < OOBData.count; i++) {
      m_ComboOrders.AddString(OOBData.strings[i]);
   }

   wsprintf(key, "Actions");
   GetPrivateProfileStruct(section, key, &OOBData, sizeof(myData), JaMBOptIni);
   m_ComboActions.AddString("");
   for (i = 0; i < OOBData.count; i++) {
      m_ComboActions.AddString(OOBData.strings[i]);
   }

   wsprintf(key, "Files/Resolutions");
   GetPrivateProfileStruct(section, key, &OOBData, sizeof(myData), JaMBOptIni);
   m_ComboFiles.AddString("");
   for (i = 0; i < OOBData.count; i++) {
      m_ComboFiles.AddString(OOBData.strings[i]);
   }

   wsprintf(key, "Fonts");
   GetPrivateProfileStruct(section, key, &OOBData, sizeof(myData), JaMBOptIni);
   for (i = 0; i < OOBData.count; i++) {
      m_ComboFont.AddString(OOBData.strings[i]);
   }

   wsprintf(key, "CurrentFont");
   GetPrivateProfileStruct(section, key, &tmpStr, 256, JaMBOptIni);
   m_FontName = tmpStr;
   m_ComboFont.SelectString(0, tmpStr);

   wsprintf(key, "CurrentSize");
   GetPrivateProfileStruct(section, key, &tmpStr, 256, JaMBOptIni);
   m_FontSize = m_ComboFontSize.SelectString(0, tmpStr) + 1;

   switch (m_FontSize) {
   case FontSizeLarge:
      m_FontSizePoints = 165;
      m_DisplayLength = 20;
      break;

   case FontSizeMedium:
      m_FontSizePoints = 155;
      m_DisplayLength = 24;
      break;

   case FontSizeSmall:
   default:
      m_FontSizePoints = 140;
      m_DisplayLength = 30;
      break;
   }

   wsprintf(key, "CurrentWatermark");
   GetPrivateProfileStruct(section, key, &tmpStr, 256, JaMBOptIni);
   m_Watermark = tmpStr;
   m_ComboWatermark.SelectString(0, m_Watermark);

   return TRUE;
}

void CJaMBDlg::OnSelchangeComboMbWatermark()
{
   CString watermark;

   m_ComboWatermark.GetWindowText(watermark);

   m_Watermark = watermark;
}

void CJaMBDlg::StoreMBOptions()
{
   char section[32];
   char key[32];
   char tmpStr[256];

   CString tmpCStr;

   wsprintf(section, "MBOptions");

   wsprintf(key, "CurrentFont");
   strcpy_s(tmpStr, m_FontName);
   WritePrivateProfileStruct(section, key, &tmpStr, 256, JaMBOptIni);

   wsprintf(key, "CurrentSize");
   m_ComboFontSize.GetWindowText(tmpCStr);
   strcpy_s(tmpStr, tmpCStr);
   WritePrivateProfileStruct(section, key, &tmpStr, 256, JaMBOptIni);

   wsprintf(key, "CurrentWatermark");
   strcpy_s(tmpStr, m_Watermark);
   WritePrivateProfileStruct(section, key, &tmpStr, 256, JaMBOptIni);
}

void CJaMBDlg::OnMouseMove(UINT nFlags, CPoint point)
{
   if (!m_TestMode && !m_NoBounds) {
      RECT rcClip;

      GetWindowRect(&rcClip);

      // TBD = Uncomment for release
 /*     if (point.x > rcClip.right)  point.x = rcClip.right;
      if (point.x < rcClip.left)   point.x = rcClip.left;
      if (point.y < rcClip.top)    point.y = rcClip.top;
      if (point.y > rcClip.bottom) point.y = rcClip.bottom;*/

      ::ClipCursor(&rcClip);
   }

   CDialog::OnMouseMove(nFlags, point);
}


void CJaMBDlg::OnBnClickedCheckNoWeb()
{
   m_NoWeb = !m_NoWeb;
}

void CJaMBDlg::EnableDisableLists()
{
   for (unsigned int i = 0; i < MAX_SEATS; i++) {
      if (m_LockList.GetCheck(i)) {
         m_RemoteList.Enable(i, false);
         m_CallInList.Enable(i, false);
         continue;
      }
      else {
         m_RemoteList.Enable(i, true);
         m_CallInList.Enable(i, true);
      }

      if (m_RemoteList.GetCheck(i)) {
         m_LockList.Enable(i, false);
         m_CallInList.Enable(i, false);
         continue;
      }
      else {
         m_LockList.Enable(i, true);
         m_CallInList.Enable(i, true);
      }

      if (m_CallInList.GetCheck(i)) {
         m_RemoteList.Enable(i, false);
         m_LockList.Enable(i, false);
         continue;
      }
      else {
         m_RemoteList.Enable(i, true);
         m_LockList.Enable(i, true);
      }
   }
}

void CJaMBDlg::OnDblclkLockList()
{
   int tmpItem = m_LockList.GetCurSel();
   int seat = m_LockList.GetItemData(tmpItem);
   unsigned char check = m_LockList.GetCheck(tmpItem);

   CSingleLock LockSem(&LockMutex);

   LockSem.Lock(1000);  // Lock for up to 1 second

   m_JA_StationLocks[seat - 1] = check; // LockCheck;

   if (check) {
      m_RemoteList.Enable(tmpItem, false);
      m_CallInList.Enable(tmpItem, false);
   }
   else {
      m_RemoteList.Enable(tmpItem, true);
      m_CallInList.Enable(tmpItem, true);
   }

   if (m_LockList.GetCount() > 0) {
      for (int i = 0; i < m_LockList.GetCount(); i++) {
         int tmpSeat = m_LockList.GetItemData(i);
         m_LockList.SetCheck(i, m_JA_StationLocks[tmpSeat - 1]);
      }
   }

   JaWriteAllFiles();

   LockSem.Unlock();
}

void CJaMBDlg::OnCheckBoxChangeLockList()
{
   int tmpItem = m_LockList.GetCurSel();
   int seat = m_LockList.GetItemData(tmpItem);
   unsigned char check = m_LockList.GetCheck(tmpItem);

   CSingleLock LockSem(&LockMutex);

   LockSem.Lock(1000);  // Lock for up to 1 second

   m_JA_StationLocks[seat - 1] = check;

   if (check) {
      m_RemoteList.Enable(tmpItem, false);
      m_CallInList.Enable(tmpItem, false);
   }
   else {
      m_RemoteList.Enable(tmpItem, true);
      m_CallInList.Enable(tmpItem, true);
   }

   if (m_LockList.GetCount() > 0) {
      for (int i = 0; i < m_LockList.GetCount(); i++) {
         int tmpSeat = m_LockList.GetItemData(i);
         m_LockList.SetCheck(i, m_JA_StationLocks[tmpSeat - 1]);
      }
   }

   JaWriteAllFiles();

   LockSem.Unlock();
}

void CJaMBDlg::OnSelchangeLockList()
{
   //int tmpItem = m_LockList.GetCurSel();
   //int seat = m_LockList.GetItemData(tmpItem);
   //unsigned char check = m_LockList.GetCheck(tmpItem);

   //CSingleLock LockSem(&LockMutex);

   //LockSem.Lock(1000);  // Lock for up to 1 second

   //m_JA_StationLocks[seat - 1] = check;

   //if (check) {
   //   m_RemoteList.Enable(tmpItem, false);
   //   m_CallInList.Enable(tmpItem, false);
   //}
   //else {
   //   m_RemoteList.Enable(tmpItem, true);
   //   m_CallInList.Enable(tmpItem, true);
   //}

   //if (m_LockList.GetCount() > 0) {
   //   for (int i = 0; i < m_LockList.GetCount(); i++) {
   //      int tmpSeat = m_LockList.GetItemData(i);
   //      m_LockList.SetCheck(i, m_JA_StationLocks[tmpSeat - 1]);
   //   }
   //}

   //JaWriteAllFiles();

   //LockSem.Unlock();
}


void CJaMBDlg::OnRadioLockFull()
{
   m_LockList.ResetContent();

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
         m_LockList.AddString(m_JA_MemberSeatingList[i].name);
         int tmpItem = m_LockList.FindString(-1, m_JA_MemberSeatingList[i].name);
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         m_LockList.SetItemData(tmpItem, tmpSeat);
         m_LockList.SetCheck(tmpItem, m_JA_StationLocks[tmpSeat - 1]);

         if (m_JA_RemoteVotes[tmpSeat - 1] || m_JA_CallInVotes[tmpSeat - 1]) {
            m_LockList.Enable(tmpItem, false);
         }
      }
   }
}


void CJaMBDlg::OnRadioLockSummary()
{
   m_LockList.ResetContent();

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         if (m_JA_StationLocks[tmpSeat - 1]) {
            m_LockList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_LockList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_LockList.SetItemData(tmpItem, tmpSeat);
            m_LockList.SetCheck(tmpItem, m_JA_StationLocks[tmpSeat - 1]);
         }
      }
   }
}


void CJaMBDlg::OnButtonPrintLocks()
{
   // Print the Locks

   lockCount = 0;
   for (int i = 0; i < sizeof(m_JA_StationLocks); i++) {
      if (m_JA_StationLocks[i]) lockCount++;
   }

   m_JA_PrinterName = m_JA_PrinterSecondary;

   PrintLocks(1);

   return;
}


void CJaMBDlg::OnDblclkRemoteList()
{
   int tmpItem = m_RemoteList.GetCurSel();
   int seat = m_RemoteList.GetItemData(tmpItem);
   unsigned char check = m_RemoteList.GetCheck(tmpItem);
   CSingleLock RemoteSem(&RemoteMutex);

   RemoteSem.Lock(1000);  // Lock for up to 1 second

   m_JA_RemoteVotes[seat - 1] = check;

   if (check) {
      m_LockList.Enable(tmpItem, false);
      m_CallInList.Enable(tmpItem, false);
   }
   else {
      m_LockList.Enable(tmpItem, true);
      m_CallInList.Enable(tmpItem, true);
   }

   if (m_RemoteList.GetCount() > 0) {
      for (int i = 0; i < m_RemoteList.GetCount(); i++) {
         int tmpSeat = m_RemoteList.GetItemData(i);
         m_RemoteList.SetCheck(i, m_JA_RemoteVotes[tmpSeat - 1]);
      }
   }

   JaWriteAllFiles();

   RemoteSem.Unlock();
}

void CJaMBDlg::OnCheckBoxChangeRemoteList()
{
   int tmpItem = m_RemoteList.GetCurSel();
   int seat = m_RemoteList.GetItemData(tmpItem);
   unsigned char check = m_RemoteList.GetCheck(tmpItem);
   CSingleLock RemoteSem(&RemoteMutex);

   RemoteSem.Lock(1000);  // Lock for up to 1 second

   m_JA_RemoteVotes[seat - 1] = check;

   if (check) {
      m_LockList.Enable(tmpItem, false);
      m_CallInList.Enable(tmpItem, false);
   }
   else {
      m_LockList.Enable(tmpItem, true);
      m_CallInList.Enable(tmpItem, true);
   }

   if (m_RemoteList.GetCount() > 0) {
      for (int i = 0; i < m_RemoteList.GetCount(); i++) {
         int tmpSeat = m_RemoteList.GetItemData(i);
         m_RemoteList.SetCheck(i, m_JA_RemoteVotes[tmpSeat - 1]);
      }
   }

   JaWriteAllFiles();;

   RemoteSem.Unlock();
}

void CJaMBDlg::OnSelchangeRemoteList()
{
   //int tmpItem = m_RemoteList.GetCurSel();
   //int seat = m_RemoteList.GetItemData(tmpItem);
   //unsigned char check = m_RemoteList.GetCheck(tmpItem);
   //CSingleLock RemoteSem(&RemoteMutex);

   //RemoteSem.Lock(1000);  // Lock for up to 1 second

   //m_JA_RemoteVotes[seat - 1] = check;

   //if (check) {
   //   m_LockList.Enable(tmpItem, false);
   //   m_CallInList.Enable(tmpItem, false);
   //}
   //else {
   //   m_LockList.Enable(tmpItem, true);
   //   m_CallInList.Enable(tmpItem, true);
   //}

   //if (m_RemoteList.GetCount() > 0) {
   //   for (int i = 0; i < m_RemoteList.GetCount(); i++) {
   //      int tmpSeat = m_RemoteList.GetItemData(i);
   //      m_RemoteList.SetCheck(i, m_JA_RemoteVotes[tmpSeat - 1]);
   //   }
   //}

   //JaWriteAllFiles();;

   //RemoteSem.Unlock();
}


void CJaMBDlg::OnRadioRemoteFull()
{
   m_RemoteList.ResetContent();

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
         m_RemoteList.AddString(m_JA_MemberSeatingList[i].name);
         int tmpItem = m_RemoteList.FindString(-1, m_JA_MemberSeatingList[i].name);
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         m_RemoteList.SetItemData(tmpItem, tmpSeat);
         m_RemoteList.SetCheck(tmpItem, m_JA_RemoteVotes[tmpSeat - 1]);

         if (m_JA_StationLocks[tmpSeat - 1] || m_JA_CallInVotes[tmpSeat - 1]) {
            m_RemoteList.Enable(tmpItem, false);
         }
      }
   }
}


void CJaMBDlg::OnRadioRemoteSummary()
{
   m_RemoteList.ResetContent();

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         if (m_JA_RemoteVotes[tmpSeat - 1]) {
            m_RemoteList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_RemoteList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_RemoteList.SetItemData(tmpItem, tmpSeat);
            m_RemoteList.SetCheck(tmpItem, m_JA_RemoteVotes[tmpSeat - 1]);
         }
      }
   }
}


void CJaMBDlg::OnButtonPrintRemotes()
{
   // Print the Remotes

   remoteCount = 0;
   for (int i = 0; i < sizeof(m_JA_RemoteVotes); i++) {
      if (m_JA_RemoteVotes[i]) remoteCount++;
   }

   m_JA_PrinterName = m_JA_PrinterSecondary;

   PrintRemotes(1);

   return;
}

void CJaMBDlg::OnDblclkCallinList()
{
   int tmpItem = m_CallInList.GetCurSel();
   int seat = m_CallInList.GetItemData(tmpItem);
   unsigned char check = m_CallInList.GetCheck(tmpItem);

   CSingleLock CallInSem(&CallInMutex);

   CallInSem.Lock(1000);  // Lock for up to 1 second

   m_JA_CallInVotes[seat - 1] = check;

   if (check) {
      m_RemoteList.Enable(tmpItem, false);
      m_LockList.Enable(tmpItem, false);
   }
   else {
      m_RemoteList.Enable(tmpItem, true);
      m_LockList.Enable(tmpItem, true);
   }

   if (m_CallInList.GetCount() > 0) {
      for (int i = 0; i < m_CallInList.GetCount(); i++) {
         int tmpSeat = m_CallInList.GetItemData(i);
         m_CallInList.SetCheck(i, m_JA_CallInVotes[tmpSeat - 1]);
      }
   }

   JaWriteAllFiles();

   CallInSem.Unlock();
}

void CJaMBDlg::OnCheckBoxChangeCallinList()
{
   int tmpItem = m_CallInList.GetCurSel();
   int seat = m_CallInList.GetItemData(tmpItem);
   unsigned char check = m_CallInList.GetCheck(tmpItem);

   CSingleLock CallInSem(&CallInMutex);

   CallInSem.Lock(1000);  // Lock for up to 1 second

   m_JA_CallInVotes[seat - 1] = check;

   if (check) {
      m_RemoteList.Enable(tmpItem, false);
      m_LockList.Enable(tmpItem, false);
   }
   else {
      m_RemoteList.Enable(tmpItem, true);
      m_LockList.Enable(tmpItem, true);
   }

   if (m_CallInList.GetCount() > 0) {
      for (int i = 0; i < m_CallInList.GetCount(); i++) {
         int tmpSeat = m_CallInList.GetItemData(i);
         m_CallInList.SetCheck(i, m_JA_CallInVotes[tmpSeat - 1]);
      }
   }

   JaWriteAllFiles();

   CallInSem.Unlock();
}

void CJaMBDlg::OnSelchangeCallinList()
{
   //int tmpItem = m_CallInList.GetCurSel();
   //int seat = m_CallInList.GetItemData(tmpItem);
   //unsigned char check = m_CallInList.GetCheck(tmpItem);

   //CSingleLock CallInSem(&CallInMutex);

   //CallInSem.Lock(1000);  // Lock for up to 1 second

   //m_JA_CallInVotes[seat - 1] = check;

   //if (check) {
   //   m_RemoteList.Enable(tmpItem, false);
   //   m_LockList.Enable(tmpItem, false);
   //}
   //else {
   //   m_RemoteList.Enable(tmpItem, true);
   //   m_LockList.Enable(tmpItem, true);
   //}

   //if (m_CallInList.GetCount() > 0) {
   //   for (int i = 0; i < m_CallInList.GetCount(); i++) {
   //      int tmpSeat = m_CallInList.GetItemData(i);
   //      m_CallInList.SetCheck(i, m_JA_CallInVotes[tmpSeat - 1]);
   //   }
   //}

   //JaWriteAllFiles();

   //CallInSem.Unlock();
}


void CJaMBDlg::OnRadioCallinFull()
{
   m_CallInList.ResetContent();

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
         m_CallInList.AddString(m_JA_MemberSeatingList[i].name);
         int tmpItem = m_CallInList.FindString(-1, m_JA_MemberSeatingList[i].name);
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         m_CallInList.SetItemData(tmpItem, tmpSeat);
         m_CallInList.SetCheck(tmpItem, m_JA_CallInVotes[tmpSeat - 1]);

         if (m_JA_StationLocks[tmpSeat - 1] || m_JA_RemoteVotes[tmpSeat - 1]) {
            m_CallInList.Enable(tmpItem, false);
         }
      }
   }
}


void CJaMBDlg::OnRadioCallinSummary()
{
   m_CallInList.ResetContent();

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         if (m_JA_CallInVotes[tmpSeat - 1]) {
            m_CallInList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_CallInList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_CallInList.SetItemData(tmpItem, tmpSeat);
            m_CallInList.SetCheck(tmpItem, m_JA_CallInVotes[tmpSeat - 1]);
         }
      }
   }
}


void CJaMBDlg::OnButtonPrintCallins()
{
   // Print the CallIns

   callInCount = 0;
   for (int i = 0; i < sizeof(m_JA_CallInVotes); i++) {
      if (i != 68 && i != 90) {
         if (m_JA_CallInVotes[i]) callInCount++;
      }
   }

   m_JA_PrinterName = m_JA_PrinterPrimary;

   PrintCallIns(1);

   m_JA_PrinterName = m_JA_PrinterSecondary;

   //PrintCallIns(1);

   return;
}

static BOOL FilesChanged = false;

void CJaMBDlg::JaWriteAllFiles()
{
   JaWriteLocks(StationLockFile);
   JaWriteRemotes(RemoteVotesFile);
   JaWriteCallIns(CallInVotesFile);

   //Beep(60, 5000);

   //FilesChanged = true;
}

void CJaMBDlg::JaWriteLocks(CString lockFilename)
{
   CSingleLock LockSem(&LockMutex);

   FILE* lockFile;
   int count;

   if (!(lockFile = fopen(lockFilename, "wb"))) {
      return;
   }

   LockSem.Lock(1000);  // Lock for up to 1 second

   lockCount = 0;
   for (int i = 0; i < sizeof(m_JA_StationLocks); i++) {
      if (m_JA_StationLocks[i]) lockCount++;
   }

   count = fwrite(m_JA_StationLocks, 1, sizeof(m_JA_StationLocks), lockFile);

   fclose(lockFile);

   lockLastWrite = JaGetFileTime(StationLockFile);

   LockSem.Unlock();
}

void CJaMBDlg::JaWriteRemotes(CString remoteFilename)
{
   CSingleLock RemoteSem(&RemoteMutex);

   FILE* remoteFile;
   int count;

   if (!(remoteFile = fopen(remoteFilename, "wb"))) {
      return;
   }

   RemoteSem.Lock(1000);  // Lock for up to 1 second

   remoteCount = 0;
   for (int i = 0; i < sizeof(m_JA_RemoteVotes); i++) {
      if (m_JA_RemoteVotes[i]) remoteCount++;
   }

   count = fwrite(m_JA_RemoteVotes, 1, sizeof(m_JA_RemoteVotes), remoteFile);

   fclose(remoteFile);

   remoteLastWrite = JaGetFileTime(RemoteVotesFile);

   RemoteSem.Unlock();
}

void CJaMBDlg::JaWriteCallIns(CString callinFilename)
{
   CSingleLock CallInSem(&CallInMutex);

   FILE* callinFile;
   int count;

   if (!(callinFile = fopen(callinFilename, "wb"))) {
      return;
   }

   CallInSem.Lock(1000);  // Lock for up to 1 second

   callInCount = 0;
   for (int i = 0; i < sizeof(m_JA_CallInVotes); i++) {
      if (m_JA_CallInVotes[i]) callInCount++;
   }

   count = fwrite(m_JA_CallInVotes, 1, sizeof(m_JA_CallInVotes), callinFile);

   fclose(callinFile);

   callinLastWrite = JaGetFileTime(CallInVotesFile);

   CallInSem.Unlock();
}

void CJaMBDlg::OnBnClickedCheckLocksAll()
{
   bool checked;

   m_LockList.ResetContent();

   checked = m_LocksAll.GetCheck();

   if (checked)
   {
      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
            m_LockList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_LockList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_LockList.SetItemData(tmpItem, tmpSeat);

            if (!m_JA_CallInVotes[tmpSeat - 1] && !m_JA_RemoteVotes[tmpSeat - 1]) {
               m_LockList.SetCheck(tmpItem, true);
               m_JA_StationLocks[tmpSeat - 1] = true;
               m_LockList.Enable(tmpItem, true);
               m_RemoteList.Enable(tmpItem, false);
               m_CallInList.Enable(tmpItem, false);
            }

            if (m_JA_CallInVotes[tmpSeat - 1] || m_JA_RemoteVotes[tmpSeat - 1]) {
               m_LockList.Enable(tmpItem, false);
            }
         }
      }
   }
   else
   {
      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
            m_LockList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_LockList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_LockList.SetItemData(tmpItem, tmpSeat);

            m_LockList.SetCheck(tmpItem, false);
            m_RemoteList.Enable(tmpItem, true);
            m_CallInList.Enable(tmpItem, true);
            m_JA_StationLocks[tmpSeat - 1] = false;

            if (m_JA_CallInVotes[tmpSeat - 1] || m_JA_RemoteVotes[tmpSeat - 1]) {
               m_LockList.Enable(tmpItem, false);
            }
         }
      }
   }

   JaWriteAllFiles();
}


void CJaMBDlg::OnBnClickedCheckRemotesAll()
{
   bool checked;

   m_RemoteList.ResetContent();

   checked = m_RemotesAll.GetCheck();

   if (checked)
   {
      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
            m_RemoteList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_RemoteList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_RemoteList.SetItemData(tmpItem, tmpSeat);

            if (!m_JA_StationLocks[tmpSeat - 1] && !m_JA_CallInVotes[tmpSeat - 1]) {
               m_RemoteList.SetCheck(tmpItem, true);
               m_JA_RemoteVotes[tmpSeat - 1] = true;
               m_RemoteList.Enable(tmpItem, true);
               m_LockList.Enable(tmpItem, false);
               m_CallInList.Enable(tmpItem, false);
            }

            if (m_JA_StationLocks[tmpSeat - 1] || m_JA_CallInVotes[tmpSeat - 1]) {
               m_RemoteList.Enable(tmpItem, false);
            }
         }
      }
   }
   else
   {
      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
            m_RemoteList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_RemoteList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_RemoteList.SetItemData(tmpItem, tmpSeat);

            m_RemoteList.SetCheck(tmpItem, false);
            m_LockList.Enable(tmpItem, true);
            m_CallInList.Enable(tmpItem, true);
            m_JA_RemoteVotes[tmpSeat - 1] = false;

            if (m_JA_StationLocks[tmpSeat - 1] || m_JA_CallInVotes[tmpSeat - 1]) {
               m_RemoteList.Enable(tmpItem, false);
            }
         }
      }
   }

   JaWriteAllFiles();
}


void CJaMBDlg::OnBnClickedCheckCallinsAll()
{
   bool checked;

   m_CallInList.ResetContent();

   checked = m_CallInsAll.GetCheck();

   if (checked)
   {
      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
            m_CallInList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_CallInList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_CallInList.SetItemData(tmpItem, tmpSeat);

            if (!m_JA_StationLocks[tmpSeat - 1] && !m_JA_RemoteVotes[tmpSeat - 1]) {
               m_CallInList.SetCheck(tmpItem, true);
               m_JA_CallInVotes[tmpSeat - 1] = true;
               m_CallInList.Enable(tmpItem, true);
               m_RemoteList.Enable(tmpItem, false);
               m_LockList.Enable(tmpItem, false);
            }

            if (m_JA_StationLocks[tmpSeat - 1] || m_JA_RemoteVotes[tmpSeat - 1]) {
               m_CallInList.Enable(tmpItem, false);
            }
         }
      }
   }
   else
   {
      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
            m_CallInList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_CallInList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_CallInList.SetItemData(tmpItem, tmpSeat);

            m_CallInList.SetCheck(tmpItem, false);
            m_RemoteList.Enable(tmpItem, true);
            m_LockList.Enable(tmpItem, true);
            m_JA_CallInVotes[tmpSeat - 1] = false;

            if (m_JA_StationLocks[tmpSeat - 1] || m_JA_RemoteVotes[tmpSeat - 1]) {
               m_CallInList.Enable(tmpItem, false);
            }
         }
      }
   }

   JaWriteAllFiles();
}

FILETIME CJaMBDlg::JaGetFileTime(CString fName)
{
   HANDLE hFile = NULL;
   FILETIME ftCreate, ftAccess, ftWrite;

   ftWrite.dwHighDateTime = 0;
   ftWrite.dwLowDateTime = 0;

   hFile = CreateFile(fName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

   if (hFile != INVALID_HANDLE_VALUE) {
      // Retrieve the file times for the file.
      if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
         DWORD LastError;
         LastError = GetLastError();
         LastError = 0;

         ftWrite.dwHighDateTime = 0;
         ftWrite.dwLowDateTime = 0;
      }

      CloseHandle(hFile);
   }

   return ftWrite;
}

// JaSetFileToCurrentTime - sets last write time to current system time
// Return value - TRUE if successful, FALSE otherwise
// fName  - must be a valid file name

BOOL CJaMBDlg::JaSetFileToCurrentTime(CString fName)
{
   HANDLE hFile = NULL;
   FILETIME ft;
   SYSTEMTIME st;
   BOOL f;

   hFile = CreateFile(fName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
  
   GetSystemTime(&st);              // Get the current system time
   SystemTimeToFileTime(&st, &ft);  // Converts the current system time to file time format

   // Sets last-write time of the file 
   // to the converted current system time
   f = SetFileTime(hFile, (LPFILETIME)NULL, (LPFILETIME)NULL, &ft);
  
   return f;
}

BOOL CJaMBDlg::JaCallInFileChanged()
{
   BOOL retVal = false;

   //static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(CallInVotesFile);

   if (ftWrite.dwLowDateTime != callinLastWrite.dwLowDateTime) {
      callinLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

void CJaMBDlg::JaReadCallIns(void)
{
   FILE* callInsFile;
   int count;

   callInsFile = fopen(CallInVotesFile, "rb");

   if (callInsFile != NULL) {
      count = fread(m_JA_CallInVotes, 1, sizeof(m_JA_CallInVotes), callInsFile);
      fclose(callInsFile);
   }
   else {
      TRACE("Unable to open CallIns file!");
      return;
   }

   if (count != sizeof(m_JA_CallInVotes)) {
      TRACE("Unable to read CallIns file!");
      return;
   }
}

BOOL CJaMBDlg::JaRemoteFileChanged()
{
   BOOL retVal = false;

   //static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(RemoteVotesFile);

   if (ftWrite.dwLowDateTime != remoteLastWrite.dwLowDateTime) {
      remoteLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

void CJaMBDlg::JaReadRemotes(void)
{
   FILE* remotesFile;
   int count;

   remotesFile = fopen(RemoteVotesFile, "rb");

   if (remotesFile != NULL) {
      count = fread(m_JA_RemoteVotes, 1, sizeof(m_JA_RemoteVotes), remotesFile);
      fclose(remotesFile);
   }
   else {
      TRACE("Unable to open Remotes file!");
      return;
   }

   if (count != sizeof(m_JA_RemoteVotes)) {
      TRACE("Unable to read Remotes file!");
      return;
   }
}

BOOL CJaMBDlg::JaLockFileChanged()
{
   BOOL retVal = false;

   //static FILETIME ftLastWrite;
   FILETIME ftWrite;

   ftWrite = JaGetFileTime(StationLockFile);

   if (ftWrite.dwLowDateTime != lockLastWrite.dwLowDateTime) {
      lockLastWrite = ftWrite;
      retVal = true;
   }
   else {
      retVal = false;
   }

   return retVal;
}

void CJaMBDlg::JaReadLocks()
{
   FILE* lockFile;
   int count;
   unsigned char tmp_lock[MAX_VOTE_ARRAY];                 // != 0: bypass this vote station in polling sequence

   lockFile = fopen(StationLockFile, "rb");

   if (lockFile != NULL) {
      count = fread(tmp_lock, 1, sizeof(tmp_lock), lockFile);
      fclose(lockFile);
   }
   else {
      TRACE("Unable to open Locks file!");
      return;
   }

   if (count != sizeof(tmp_lock)) {
      TRACE("Unable to read Locks file!");
      return;
   }

   for (int i = 0; i < sizeof(tmp_lock); i++) {
      m_JA_StationLocks[i] = tmp_lock[i];
   }
}

void CJaMBDlg::OnTimer(UINT_PTR nIDEvent)
{
   CSingleLock CallInSem(&CallInMutex);
   CSingleLock RemoteSem(&RemoteMutex);
   CSingleLock LockSem(&LockMutex);

   static bool bail = false;

   if (bail) goto BailOut;

   bail = true;

   if (JaCallInFileChanged()) {

      CallInSem.Lock(1000);  // Lock for up to 1 second

      JaReadCallIns();

      m_CallInList.ResetContent();

      m_CallInList.SetCheckStyle(BS_AUTOCHECKBOX);

      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
            m_CallInList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_CallInList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_CallInList.SetItemData(tmpItem, tmpSeat);
            // TBD: Remove when representative wakes up
            if (strncmp(m_JA_MemberSeatingList[i].name, "ZZZ", 3) == 0) {
               m_JA_CallInVotes[tmpSeat - 1] = BST_CHECKED;
            }
            m_CallInList.SetCheck(tmpItem, m_JA_CallInVotes[tmpSeat - 1]);
         }
      }
      // Update lists mutual enable/disable status
      EnableDisableLists();

      CallInSem.Unlock();
   }

   if (JaRemoteFileChanged()) {

      RemoteSem.Lock(1000);  // Lock for up to 1 second

      JaReadRemotes();

      m_RemoteList.ResetContent();

      m_RemoteList.SetCheckStyle(BS_AUTOCHECKBOX);

      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
            m_RemoteList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_RemoteList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_RemoteList.SetItemData(tmpItem, tmpSeat);
            // TBD: Remove when representative wakes up
            if (strncmp(m_JA_MemberSeatingList[i].name, "ZZZ", 3) == 0) {
               m_JA_RemoteVotes[tmpSeat - 1] = BST_CHECKED;
            }
            m_RemoteList.SetCheck(tmpItem, m_JA_RemoteVotes[tmpSeat - 1]);
         }
      }
      // Update lists mutual enable/disable status
      EnableDisableLists();

      RemoteSem.Unlock();
   }

   if (JaLockFileChanged()) {
      LockSem.Lock(1000);  // Lock for up to 1 second
      
      JaReadLocks();

      m_LockList.ResetContent();

      m_LockList.SetCheckStyle(BS_AUTOCHECKBOX);

      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
            m_LockList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_LockList.FindString(-1, m_JA_MemberSeatingList[i].name);
            int tmpSeat = m_JA_MemberSeatingList[i].seat;
            m_LockList.SetItemData(tmpItem, tmpSeat);
             // TBD: Remove when representative wakes up
            if (strncmp(m_JA_MemberSeatingList[i].name, "ZZZ", 3) == 0) {
               m_JA_StationLocks[tmpSeat - 1] = BST_CHECKED;
            }
            m_LockList.SetCheck(tmpItem, m_JA_StationLocks[tmpSeat - 1]);
         }
      }
      // Update lists mutual enable/disable status
      EnableDisableLists();
      
      LockSem.Unlock();
   }

   bail = false;

   if (FilesChanged) {
      JaSetFileToCurrentTime(StationLockFile);
      FilesChanged = false;
   }

BailOut:
   CDialog::OnTimer(nIDEvent);
}


