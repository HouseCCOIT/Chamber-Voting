// JaVEDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JaVE.h"
#include "JaVEDlg.h"
#include "VEThread.h"

#include "house.h"
#include "VM_Sock.h"

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

extern LEX m_JA_MemberSeatingList[MAX_SEATS];

extern int     m_Sequence;
extern CString m_Date;
extern CString m_File;
extern CString m_Order;
extern CString m_Action;

CString m_CurrentSequence;

CString m_OriginalVoteDir = "V:\\VOTES\\";
CString m_EditedVoteDir   = "S:\\";	  
CString m_LegHouseVoteDir = "L:\\";	  

/////////////////////////////////////////////////////////////////////////////
// CJaVEDlg dialog

CJaVEDlg::CJaVEDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CJaVEDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CJaVEDlg)
    m_JA_Vote = 0;
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJaVEDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CJaVEDlg)
    DDX_Control(pDX, IDC_EDIT_EDITED, m_EditEdited);
    DDX_Control(pDX, IDC_RADIO_ABSENT, m_RadioAbsent);
    DDX_Control(pDX, IDC_RADIO_NO, m_RadioNo);
    DDX_Control(pDX, IDC_RADIO_YES, m_RadioYes);
    DDX_Control(pDX, IDC_COMBO_NAME, m_ComboName);
    DDX_Control(pDX, IDC_EDIT_ORDER, m_EditOrderOfBusiness);
    DDX_Control(pDX, IDC_EDIT_FILE, m_EditFilesAndResolutions);
    DDX_Control(pDX, IDC_EDIT_DATE, m_EditDate);
    DDX_Control(pDX, IDC_EDIT_ACTION, m_EditAction);
    DDX_Control(pDX, IDC_COMBO_SEQUENCE, m_ComboSequence);
    DDX_Radio(pDX, IDC_RADIO_YES, m_JA_Vote);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CJaVEDlg, CDialog)
    //{{AFX_MSG_MAP(CJaVEDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_RADIO_YES, OnRadioYes)
    ON_BN_CLICKED(IDC_RADIO_NO, OnRadioNo)
    ON_BN_CLICKED(IDC_RADIO_ABSENT, OnRadioNovote)
    ON_CBN_SELCHANGE(IDC_COMBO_NAME, OnSelchangeComboName)
    ON_CBN_SELCHANGE(IDC_COMBO_SEQUENCE, OnSelchangeComboSequence)
    ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
    ON_WM_TIMER()
    ON_EN_SETFOCUS(IDC_EDIT_EDITED, OnSetfocusEditEdited)
    ON_BN_CLICKED(IDC_BUTTON_RESET, OnButtonReset)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJaVEDlg message handlers

BOOL CJaVEDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

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

   CenterWindow(); // Center the window
   SetCursorPos(::GetSystemMetrics(SM_CXSCREEN) / 2, ::GetSystemMetrics(SM_CYSCREEN) / 2);

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon
    
    JaLoadParametersAndSeating();

    SetTimer(1, 10000, NULL);        // Check for new .vot files every 10 seconds.

    m_ComboName.ResetContent();

    for (int i = 0; i < MAX_SEATS; i++) {
        if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
           m_ComboName.AddString(m_JA_MemberSeatingList[i].name);
           m_ComboName.SetItemData(m_ComboName.FindString(-1, m_JA_MemberSeatingList[i].name), i);
        }
    }

    m_ComboName.SetCurSel(0);

    CFileFind finder;
    BOOL bWorking;

    m_ComboSequence.ResetContent();

    CString searchStr = m_OriginalVoteDir + "H*.VOT";

    bWorking = finder.FindFile(searchStr);
    while (bWorking) {
       bWorking = finder.FindNextFile();
       m_ComboSequence.AddString((LPCTSTR)finder.GetFileName());   
    }

    m_ComboSequence.SetCurSel(m_ComboSequence.GetCount() - 1);

    OnSelchangeComboSequence();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CJaVEDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CJaVEDlg::OnPaint() 
{
    if (IsIconic()) {
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
    else {
        CDialog::OnPaint();
    }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CJaVEDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CJaVEDlg::OnRadioYes() 
{
    m_JA_MemberSeatingList[m_JA_Index].vote = VOTE_YES;
    
    m_ComboName.SetFocus();
}

void CJaVEDlg::OnRadioNo() 
{
    m_JA_MemberSeatingList[m_JA_Index].vote = VOTE_NO;
    
    m_ComboName.SetFocus();
}

void CJaVEDlg::OnRadioNovote() 
{
    m_JA_MemberSeatingList[m_JA_Index].vote = VOTE_ABSENT;
    
    m_ComboName.SetFocus();
}

void CJaVEDlg::OnSelchangeComboName() 
{
    m_JA_Index = m_ComboName.GetItemData(m_ComboName.GetCurSel());

    if (m_JA_Index == -1) {
        return;
    }

    m_JA_Vote = m_JA_MemberSeatingList[m_JA_Index].vote;

    if (m_JA_Vote == VOTE_ABSENT) {
       m_RadioYes.SetCheck(0);
       m_RadioNo.SetCheck(0);
       m_RadioAbsent.SetCheck(1);
    }
    else if (m_JA_Vote == VOTE_NO) {
       m_RadioYes.SetCheck(0);
       m_RadioNo.SetCheck(1);
       m_RadioAbsent.SetCheck(0);
    }
    else if (m_JA_Vote == VOTE_YES) {
       m_RadioYes.SetCheck(1);
       m_RadioNo.SetCheck(0);
       m_RadioAbsent.SetCheck(0);
    }
}


void CJaVEDlg::OnSelchangeComboSequence() 
{
    CString orgStr;
    CString edtStr;
    CString edtSeq;

    CFileFind finder;
    BOOL edtFound;

    m_ComboSequence.GetWindowText(m_CurrentSequence);
    edtSeq = m_CurrentSequence;
    edtSeq.Replace(".VOT", ".EDT");

    orgStr = m_OriginalVoteDir + m_CurrentSequence;

    // Now try to find an edited file of same vintage
    edtStr = m_EditedVoteDir + edtSeq;

    edtFound = finder.FindFile(edtStr);
    if (edtFound) {
        JaReadSequenceFile(edtStr);
        m_EditEdited.SetWindowText(edtSeq);
    }
    else {
        JaReadSequenceFile(orgStr);
        m_EditEdited.SetWindowText("");
    }

    m_EditDate.SetWindowText(m_Date);
    m_EditOrderOfBusiness.SetWindowText(m_Order);
    m_EditFilesAndResolutions.SetWindowText(m_File);
    m_EditAction.SetWindowText(m_Action);

    OnSelchangeComboName();
}

void CJaVEDlg::OnButtonSend() 
{
    CString edtStr;
    CString txtStr;
    CString dbStr;

    edtStr = m_EditedVoteDir + m_CurrentSequence;
    edtStr.Replace(".VOT", ".EDT");

    JaWriteEditedSequenceFile(edtStr);

    txtStr = m_LegHouseVoteDir + m_CurrentSequence;
//    txtStr.Replace(".VOT", ".TXT");
    JaSend(txtStr);

    dbStr = m_LegHouseVoteDir + m_CurrentSequence;
    dbStr.Replace(".VOT", ".TXT");
    JaSendTabbed(dbStr);
}

void CJaVEDlg::OnTimer(UINT nIDEvent) 
{
   CFileFind finder;
   BOOL bWorking;

   m_ComboSequence.ResetContent();

   CString searchStr = m_OriginalVoteDir + "H*.VOT";
   bWorking = finder.FindFile(searchStr);
   while (bWorking) {
      bWorking = finder.FindNextFile();
      m_ComboSequence.AddString((LPCTSTR)finder.GetFileName());   
   }

   m_ComboSequence.SetCurSel(m_ComboSequence.FindString(-1, m_CurrentSequence));

   CDialog::OnTimer(nIDEvent);
}

void CJaVEDlg::OnSetfocusEditEdited() 
{
    CString edtStr;

    m_EditEdited.GetWindowText(edtStr);

    if (edtStr.GetLength() == 0) {
        return;
    }

    CString tmpStr;

    tmpStr = m_EditedVoteDir + edtStr;

    JaReadSequenceFile(tmpStr);

    m_EditDate.SetWindowText(m_Date);
    m_EditOrderOfBusiness.SetWindowText(m_Order);
    m_EditFilesAndResolutions.SetWindowText(m_File);
    m_EditAction.SetWindowText(m_Action);

    OnSelchangeComboName();
}


void CJaVEDlg::OnCancel() 
{
    if (AfxMessageBox("Are you sure you want to exit?", MB_YESNO) == IDYES) {
        CDialog::OnCancel();
    }
}

void CJaVEDlg::OnButtonReset() 
{
    CString orgStr;

    m_ComboSequence.GetWindowText(m_CurrentSequence);

    orgStr = m_OriginalVoteDir + m_CurrentSequence;

    JaReadSequenceFile(orgStr);

    m_EditDate.SetWindowText(m_Date);
    m_EditOrderOfBusiness.SetWindowText(m_Order);
    m_EditFilesAndResolutions.SetWindowText(m_File);
    m_EditAction.SetWindowText(m_Action);

    OnSelchangeComboName();
}

void CJaVEDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CJaVEDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
   RECT rcClip;

   //GetWindowRect(&rcClip);

   //if (point.x > rcClip.right)  point.x = rcClip.right;
   //if (point.x < rcClip.left)   point.x = rcClip.left;
   //if (point.y < rcClip.top)    point.y = rcClip.top;
   //if (point.y > rcClip.bottom) point.y = rcClip.bottom;

   //::ClipCursor(&rcClip); 
	
	CDialog::OnMouseMove(nFlags, point);
}
