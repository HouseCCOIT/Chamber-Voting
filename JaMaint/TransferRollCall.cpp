// TransferRollCall.cpp : implementation file
//

#include "stdafx.h"
#include "JaMaint.h"
#include "JaFile.h"
#include "TransferRollCall.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransferRollCall dialog


CTransferRollCall::CTransferRollCall(CWnd* pParent /*=NULL*/)
    : CDialog(CTransferRollCall::IDD, pParent)
{
    //{{AFX_DATA_INIT(CTransferRollCall)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
	m_radioSelected = 0;
}


void CTransferRollCall::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTransferRollCall)
	DDX_Radio(pDX, IDC_RADIO_TO_DISKETTE, m_radioSelected);
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransferRollCall, CDialog)
    //{{AFX_MSG_MAP(CTransferRollCall)
    ON_BN_CLICKED(IDC_RADIO_TO_DISKETTE, OnRadioToDiskette)
    ON_BN_CLICKED(IDC_RADIO_FROM_DISKETTE, OnRadioFromDiskette)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_JA_SEQUENCENUMBER_END, OnDeltaposSpinJaSequencenumberEnd)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_JA_SEQUENCENUMBER_START, OnDeltaposSpinJaSequencenumberStart)
    ON_EN_KILLFOCUS(IDC_EDIT_JA_SEQUENCENUMBER_END, OnKillfocusEditJaSequencenumberEnd)
    ON_EN_KILLFOCUS(IDC_EDIT_JA_SEQUENCENUMBER_START, OnKillfocusEditJaSequencenumberStart)
    ON_BN_CLICKED(IDC_CHECK_DELETE_FROM_SOURCE, OnCheckDeleteFromSource)
	ON_BN_CLICKED(IDC_BUTTON_TRANSFER, OnButtonTransfer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransferRollCall message handlers

BOOL CTransferRollCall::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEQUENCENUMBER_START);
    pEdit->SetWindowText("1");

    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEQUENCENUMBER_END);
    pEdit->SetWindowText("1");

    CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_DELETE_FROM_SOURCE);
    pButton->SetWindowText("Delete from hard drive");

	m_JA_ToDiskette = TRUE;

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CTransferRollCall::OnRadioToDiskette() 
{
    CButton *pButton;

    m_JA_ToDiskette = TRUE;

    pButton = (CButton *)GetDlgItem(IDC_CHECK_DELETE_FROM_SOURCE);
    pButton->SetWindowText("Delete from hard drive");

    pButton = (CButton *)GetDlgItem(IDC_BUTTON_TRANSFER);
    pButton->SetWindowText("Transfer to Diskette");
}

void CTransferRollCall::OnRadioFromDiskette() 
{
    CButton *pButton;

    m_JA_ToDiskette = FALSE;

    pButton = (CButton *)GetDlgItem(IDC_CHECK_DELETE_FROM_SOURCE);
    pButton->SetWindowText("Delete from diskette");

    pButton = (CButton *)GetDlgItem(IDC_BUTTON_TRANSFER);
    pButton->SetWindowText("Transfer to Hard Drive");
}

void CTransferRollCall::OnDeltaposSpinJaSequencenumberStart(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    
    CEdit *sEdit = (CEdit *)GetDlgItem(IDC_SPIN_JA_SEQUENCENUMBER_START);
    sEdit->SetFocus();
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEQUENCENUMBER_START);
    pEdit->SetFocus();

    if ((pNMUpDown->iDelta < 0) && (m_JA_SequenceNumberStart >= 1) && (m_JA_SequenceNumberStart < 999999)) {
       m_JA_SequenceNumberStart++;
    }
    else if ((pNMUpDown->iDelta > 0) && (m_JA_SequenceNumberStart <= 999999) && (m_JA_SequenceNumberStart > 1)) {
       m_JA_SequenceNumberStart--;
    }
    
    char buf[5];
    wsprintf(buf, "%.d", m_JA_SequenceNumberStart);
    pEdit->SetWindowText(buf);
    
    *pResult = 0;
}

void CTransferRollCall::OnDeltaposSpinJaSequencenumberEnd(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    
    CEdit *sEdit = (CEdit *)GetDlgItem(IDC_SPIN_JA_SEQUENCENUMBER_END);
    sEdit->SetFocus();
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEQUENCENUMBER_END);
    pEdit->SetFocus();

    if ((pNMUpDown->iDelta < 0) && (m_JA_SequenceNumberEnd >= 1) && (m_JA_SequenceNumberEnd < 999999)) {
       m_JA_SequenceNumberEnd++;
    }
    else if ((pNMUpDown->iDelta > 0) && (m_JA_SequenceNumberEnd <= 999999) && (m_JA_SequenceNumberEnd > 1)) {
       m_JA_SequenceNumberEnd--;
    }
    
    char buf[5];
    wsprintf(buf, "%.d", m_JA_SequenceNumberEnd);
    pEdit->SetWindowText(buf);
    
    *pResult = 0;
}

void CTransferRollCall::OnKillfocusEditJaSequencenumberStart() 
{
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEQUENCENUMBER_START);

    CString editBuf;
    pEdit->GetWindowText(editBuf);

    unsigned int tmpSeq = atoi(editBuf);

    if ((tmpSeq >= 1) && (tmpSeq <= 999999)) {
       m_JA_SequenceNumberStart = tmpSeq;
    }

    char buf[5];
    wsprintf(buf, "%.d", m_JA_SequenceNumberStart);
    pEdit->SetWindowText(buf);
}

void CTransferRollCall::OnKillfocusEditJaSequencenumberEnd() 
{
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEQUENCENUMBER_END);

    CString editBuf;
    pEdit->GetWindowText(editBuf);

    unsigned int tmpSeq = atoi(editBuf);

    if ((tmpSeq >= 1) && (tmpSeq <= 999999)) {
       m_JA_SequenceNumberEnd = tmpSeq;
    }

    char buf[5];
    wsprintf(buf, "%.d", m_JA_SequenceNumberEnd);
    pEdit->SetWindowText(buf);
}

void CTransferRollCall::OnCheckDeleteFromSource() 
{
    CButton *pButton = (CButton *)GetDlgItem(IDC_CHECK_DELETE_FROM_SOURCE);
    char buf[32];

	if (m_JA_DeleteFromSource == TRUE){
	    m_JA_DeleteFromSource = FALSE;
	}
	else {
	    m_JA_DeleteFromSource = TRUE;
	}

    if (m_JA_ToDiskette == TRUE) {
        wsprintf(buf, "Delete from hard drive");
        pButton->SetWindowText(buf);
	}
	else {
        wsprintf(buf, "Delete from diskette");
        pButton->SetWindowText(buf);
	}
}

void CTransferRollCall::OnButtonTransfer() 
{
   JaTransferRollCall(m_JA_ToDiskette, m_JA_DeleteFromSource);	
}
