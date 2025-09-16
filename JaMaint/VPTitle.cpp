// VPTitle.cpp : implementation file
//

#include "stdafx.h"
#include "JaMaint.h"
#include "VPTitle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVPTitle dialog


CVPTitle::CVPTitle(CWnd* pParent /*=NULL*/)
	: CDialog(CVPTitle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVPTitle)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CVPTitle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVPTitle)
    DDX_Text(pDX, IDC_EDIT_JA_VP_TITLE1, m_JA_VPTitle1);
    DDV_MaxChars(pDX, m_JA_VPTitle1, 50);
    DDX_Text(pDX, IDC_EDIT_JA_VP_TITLE2, m_JA_VPTitle2);
    DDV_MaxChars(pDX, m_JA_VPTitle2, 50);
    DDX_Text(pDX, IDC_EDIT_JA_VP_TITLE3, m_JA_VPTitle3);
    DDV_MaxChars(pDX, m_JA_VPTitle3, 50);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVPTitle, CDialog)
	//{{AFX_MSG_MAP(CVPTitle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVPTitle message handlers

BOOL CVPTitle::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    CEdit * pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_VP_TITLE1);
    pEdit->SetWindowText(m_JA_VPTitle1);
    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_VP_TITLE2);
    pEdit->SetWindowText(m_JA_VPTitle2);
    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_VP_TITLE3);
    pEdit->SetWindowText(m_JA_VPTitle3);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
