// JaMaintDlg.cpp : implementation file
//

#include "stdafx.h"
#include "io.h"
#include "stdio.h"
#include "JaMaint.h"
#include "JaMaintDlg.h"
#include "JaFile.h"
#include "VPTitle.h"
#include "MemberSeating.h"
#include "TransferRollCall.h"
#include "MBOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString JaVoteIniFile;
extern CString JaVoteScratch;

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
// CJaMaintDlg dialog

CJaMaintDlg::CJaMaintDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CJaMaintDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CJaMaintDlg)
   //}}AFX_DATA_INIT
    m_JA_SequenceNumberStart = 1;
    m_JA_SequenceNumberEnd = 1;
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJaMaintDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CJaMaintDlg)
   DDX_Control(pDX, IDC_CHECK_SCRATCH, m_ScratchFile);
   DDX_Text(pDX, IDC_EDIT_JA_SEQUENCENUMBER, m_JA_SequenceNumber);
   DDV_MinMaxUInt(pDX, m_JA_SequenceNumber, 1, 9999);
   DDX_Text(pDX, IDC_EDIT_JA_SESSIONNUMBER, m_JA_SessionNumber);
   DDV_MinMaxUInt(pDX, m_JA_SessionNumber, 1, 99);
   DDX_Text(pDX, IDC_EDIT_JA_SPECIALSESSION, m_JA_SpecialSession);
   DDV_MinMaxInt(pDX, m_JA_SpecialSession, 0, 10);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CJaMaintDlg, CDialog)
   //{{AFX_MSG_MAP(CJaMaintDlg)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_JA_SEQUENCENUMBER, OnDeltaposSpinJaSequencenumber)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_JA_SESSIONNUMBER, OnDeltaposSpinJaSessionnumber)
   ON_EN_KILLFOCUS(IDC_EDIT_JA_SEQUENCENUMBER, OnKillfocusEditJaSequencenumber)
   ON_EN_KILLFOCUS(IDC_EDIT_JA_SESSIONNUMBER, OnKillfocusEditJaSessionnumber)
   ON_BN_CLICKED(IDC_BUTTON_UPDATE_MEMBER_SEATING, OnButtonChangeMemberSeating)
   ON_BN_CLICKED(IDC_BUTTON_CHANGE_PRINTOUT_TITLE, OnButtonChangePrintoutTitle)
   ON_BN_CLICKED(IDC_BUTTON_TRANSFER_ROLL_CALL, OnButtonTransferRollCall)
   ON_BN_CLICKED(IDC_CHECK_SCRATCH, OnCheckScratch)
   ON_BN_CLICKED(IDC_BUTTON_MB_OPTIONS, OnButtonMbOptions)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_JA_SPECIALSESSION, OnDeltaposSpinJaSpecialsession)
   ON_EN_KILLFOCUS(IDC_EDIT_JA_SPECIALSESSION, OnKillfocusEditJaSpecialsession)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJaMaintDlg message handlers

BOOL CJaMaintDlg::OnInitDialog()
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
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    if (m_JA_ScratchFile) {
        m_ScratchFile.SetCheck(TRUE);
   }
   else {
        m_ScratchFile.SetCheck(FALSE);
   }

   CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SPECIALSESSION);

   char buf[5];
   wsprintf(buf, "%.d", m_JA_SpecialSession);
   pEdit->SetWindowText(buf);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CJaMaintDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CJaMaintDlg::OnPaint()
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
HCURSOR CJaMaintDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CJaMaintDlg::OnDeltaposSpinJaSequencenumber(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

    CEdit *sEdit = (CEdit *)GetDlgItem(IDC_SPIN_JA_SEQUENCENUMBER);
    sEdit->SetFocus();
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEQUENCENUMBER);
    pEdit->SetFocus();


    if ((pNMUpDown->iDelta < 0) && (m_JA_SequenceNumber >= 1) && (m_JA_SequenceNumber < 9999)) {
       m_JA_SequenceNumber++;
    }
    else if ((pNMUpDown->iDelta > 0) && (m_JA_SequenceNumber <= 9999) && (m_JA_SequenceNumber > 1)) {
       m_JA_SequenceNumber--;
    }

    char buf[5];
    wsprintf(buf, "%.d", m_JA_SequenceNumber);
    pEdit->SetWindowText(buf);

    *pResult = 0;
}

void CJaMaintDlg::OnDeltaposSpinJaSessionnumber(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

    CEdit *sEdit = (CEdit *)GetDlgItem(IDC_SPIN_JA_SESSIONNUMBER);
    sEdit->SetFocus();
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SESSIONNUMBER);
    pEdit->SetFocus();


    if ((pNMUpDown->iDelta < 0) && (m_JA_SessionNumber >= 1) && (m_JA_SessionNumber < 99)) {
       m_JA_SessionNumber++;
    }
    else if ((pNMUpDown->iDelta > 0) && (m_JA_SessionNumber <= 99) && (m_JA_SessionNumber > 1)) {
       m_JA_SessionNumber--;
    }

    char buf[5];
    wsprintf(buf, "%.d", m_JA_SessionNumber);
    pEdit->SetWindowText(buf);

    *pResult = 0;
}

void CJaMaintDlg::OnKillfocusEditJaSequencenumber()
{
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEQUENCENUMBER);

    CString editBuf;
    pEdit->GetWindowText(editBuf);

    unsigned int tmpSeq = atoi(editBuf);

    if ((tmpSeq >= 1) && (tmpSeq <= 9999)) {
       m_JA_SequenceNumber = tmpSeq;
    }

    char buf[5];
    wsprintf(buf, "%.d", m_JA_SequenceNumber);
    pEdit->SetWindowText(buf);
}

void CJaMaintDlg::OnKillfocusEditJaSessionnumber()
{
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SESSIONNUMBER);

    CString editBuf;
    pEdit->GetWindowText(editBuf);

    unsigned int tmpSess = atoi(editBuf);

    if ((tmpSess >= 1) && (tmpSess <= 99)) {
       m_JA_SessionNumber = tmpSess;
    }

    char buf[5];
    wsprintf(buf, "%.d", m_JA_SessionNumber);
    pEdit->SetWindowText(buf);
}

void CJaMaintDlg::OnButtonChangePrintoutTitle()
{
    CVPTitle dlg;

    int nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
//       JaUpdateParameters();
    }
    else if (nResponse == IDCANCEL)
    {
    }
}

void CJaMaintDlg::OnButtonChangeMemberSeating()
{
    CMemberSeating dlg;

    int nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
       JaUpdateSeating();
    }
    else if (nResponse == IDCANCEL)
    {
    }
}

void CJaMaintDlg::OnButtonTransferRollCall()
{
    CTransferRollCall dlg;

    int nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    }
    else if (nResponse == IDCANCEL)
    {
    }
}

void CJaMaintDlg::OnCheckScratch()
{
   if (m_ScratchFile.GetCheck()) {
      m_JA_ScratchFile = TRUE;
   }
   else {
      m_JA_ScratchFile = FALSE;
       if ((_access(JaVoteScratch, 0)) != -1) {
          if (AfxMessageBox("Do you wish to delete the Scratch File?", MB_YESNO) == IDYES) {
             remove(JaVoteScratch);
        }
      }
   }
}

void CJaMaintDlg::OnButtonMbOptions()
{
    CMBOptions dlg;

    int nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    }
    else if (nResponse == IDCANCEL)
    {
    }
}

void CJaMaintDlg::OnDeltaposSpinJaSpecialsession(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

   CEdit *sEdit = (CEdit *)GetDlgItem(IDC_SPIN_JA_SPECIALSESSION);
   sEdit->SetFocus();
   CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SPECIALSESSION);
   pEdit->SetFocus();

   if ((pNMUpDown->iDelta < 0) && (m_JA_SpecialSession >= 0) && (m_JA_SpecialSession < 10)) {
      m_JA_SpecialSession++;
   }
   else if ((pNMUpDown->iDelta > 0) && (m_JA_SpecialSession <= 10) && (m_JA_SpecialSession > 0)) {
      m_JA_SpecialSession--;
   }

   char buf[5];
   wsprintf(buf, "%.d", m_JA_SpecialSession);
   pEdit->SetWindowText(buf);

   *pResult = 0;
}

void CJaMaintDlg::OnKillfocusEditJaSpecialsession()
{
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SPECIALSESSION);

    CString editBuf;
    pEdit->GetWindowText(editBuf);

    int tmpSess = atoi(editBuf);

    if ((tmpSess >= 0) && (tmpSess <= 10)) {
       m_JA_SpecialSession = tmpSess;
    }

    char buf[5];
    wsprintf(buf, "%.d", m_JA_SpecialSession);
    pEdit->SetWindowText(buf);
}
