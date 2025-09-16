// MemberSeating.cpp : implementation file
//

#include "stdafx.h"
#include "JaMaint.h"
#include "MemberSeating.h"
#include "VM_Sock.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// MemberSeating dialog

CMemberSeating::CMemberSeating(CWnd* pParent /*=NULL*/)
    : CDialog(CMemberSeating::IDD, pParent)
{
    //{{AFX_DATA_INIT(CMemberSeating)
    m_JA_MemberSeatNew = 1;
    m_JA_MemberSeatOld = 1;
    m_JA_MemberNameNew = _T("");
    m_JA_MemberDistrictNew = _T("");
    m_JA_MemberPartyNew = _T("");
    m_JA_MemberNameOld = _T("");
    m_JA_MemberDistrictOld = _T("");
    m_JA_MemberPartyOld = _T("");
    m_JA_Autoincrement = FALSE;
    //}}AFX_DATA_INIT
}


void CMemberSeating::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CMemberSeating)
    DDX_Check(pDX, IDC_CHECK_JA_AUTOINCREMENT, m_JA_Autoincrement);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMemberSeating, CDialog)
    //{{AFX_MSG_MAP(CMemberSeating)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_JA_SEATNUMBER1, OnDeltaposSpinJaSeatnumberNew)
    ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_JA_SEATNUMBER2, OnDeltaposSpinJaSeatnumberOld)
    ON_EN_KILLFOCUS(IDC_EDIT_JA_SEATNUMBER1, OnKillfocusEditJaSeatnumberNew)
    ON_EN_KILLFOCUS(IDC_EDIT_JA_SEATNUMBER2, OnKillfocusEditJaSeatnumberOld)
    ON_BN_CLICKED(IDC_BUTTON_ADD_MEMBER, OnButtonAddMember)
    ON_BN_CLICKED(IDC_BUTTON_TRANSFER, OnButtonTransfer)
    ON_CBN_SELCHANGE(IDC_COMBO_JA_NAME_OLD, OnSelchangeComboJaNameOld)
    ON_CBN_SELCHANGE(IDC_COMBO_JA_NAME, OnSelchangeComboJaNameNew)
    ON_CBN_KILLFOCUS(IDC_COMBO_JA_NAME, OnKillfocusComboJaNameNew)
    ON_CBN_SETFOCUS(IDC_COMBO_JA_NAME, OnSetfocusComboJaNameNew)
    ON_BN_CLICKED(IDC_CHECK_JA_AUTOINCREMENT, OnCheckJaAutoincrement)
    ON_CBN_SELCHANGE(IDC_COMBO_DISTRICT, OnSelchangeComboDistrict)
    ON_CBN_SELCHANGE(IDC_COMBO_PARTY, OnSelchangeComboParty)
	ON_CBN_SELCHANGE(IDC_COMBO_COUNTY, &CMemberSeating::OnCbnSelchangeComboCounty)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CMemberSeating::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    CComboBox *pName;
    CComboBox *pDistrict;
    CComboBox *pParty;
	CComboBox *pCounty;

    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEATNUMBER1);
    pEdit->SetWindowText("1");

    pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEATNUMBER2);
    pEdit->SetWindowText("1");

    pDistrict = (CComboBox *)GetDlgItem(IDC_COMBO_DISTRICT);
    for (int i = 1; i < 68; i++) {
       char buf[3];
       wsprintf(buf, "%.2dA", i);
       pDistrict->AddString(buf);
       wsprintf(buf, "%.2dB", i);
       pDistrict->AddString(buf);
    }
    
    // Update fields

    for (int i = 0; i < MAX_SEATS; i++) {
       strcpy_s(m_JA_MemberSeatingListNew[i].name, m_JA_MemberSeatingListOld[i].name);
       m_JA_MemberSeatingListNew[i].seat = m_JA_MemberSeatingListOld[i].seat;
       strcpy_s(m_JA_MemberSeatingListNew[i].district, m_JA_MemberSeatingListOld[i].district);
       strcpy_s(m_JA_MemberSeatingListNew[i].party, m_JA_MemberSeatingListOld[i].party);
       strcpy_s(m_JA_MemberSeatingListNew[i].county, m_JA_MemberSeatingListOld[i].county);
    }

    UpdateComboListNew();
    UpdateComboListOld();

    // New Stuff
    m_JA_MemberNameNew     = m_JA_MemberSeatingListOld[m_JA_MemberSeatNew].name;
    m_JA_MemberDistrictNew = m_JA_MemberSeatingListOld[m_JA_MemberSeatNew].district;
    m_JA_MemberPartyNew    = m_JA_MemberSeatingListOld[m_JA_MemberSeatNew].party;
	m_JA_MemberCountyNew   = m_JA_MemberSeatingListOld[m_JA_MemberSeatNew].county;

    pName     = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME);
    pDistrict = (CComboBox *)GetDlgItem(IDC_COMBO_DISTRICT);
    pParty    = (CComboBox *)GetDlgItem(IDC_COMBO_PARTY);
	pCounty   = (CComboBox *)GetDlgItem(IDC_COMBO_COUNTY);

    pName->SetCurSel(pName->FindString(-1, m_JA_MemberNameNew));
    pDistrict->SetCurSel(pDistrict->FindString(-1, m_JA_MemberDistrictNew));
    pParty->SetCurSel(pParty->FindString(-1, m_JA_MemberPartyNew));
	pCounty->SetCurSel(pCounty->FindString(-1, m_JA_MemberCountyNew));

    // Old Stuff
    m_JA_MemberNameOld     = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].name;
    m_JA_MemberDistrictOld = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].district;
    m_JA_MemberPartyOld    = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].party;
	m_JA_MemberCountyOld   = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].county;

    pName    = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME_OLD);

    pName->SetCurSel(pName->FindString(-1, m_JA_MemberNameOld));
    GetDlgItem(IDC_EDIT_JA_DISTRICT2)->SetWindowText(m_JA_MemberDistrictOld);
    GetDlgItem(IDC_EDIT_JA_PARTY2)->SetWindowText(m_JA_MemberPartyOld);
	GetDlgItem(IDC_EDIT_JA_COUNTY2)->SetWindowText(m_JA_MemberCountyOld);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// MemberSeating message handlers


void CMemberSeating::OnDeltaposSpinJaSeatnumberNew(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    
    GetDlgItem(IDC_SPIN_JA_SEATNUMBER1)->SetFocus();

    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEATNUMBER1);
    pEdit->SetFocus();

    // Number must be between 1-136, and 69 and 91 are for the Sergeants.

    if ((pNMUpDown->iDelta < 0) && (m_JA_MemberSeatNew >= 1) && (m_JA_MemberSeatNew < 136)) {
       m_JA_MemberSeatNew++;
       if ((m_JA_MemberSeatNew == 69) || (m_JA_MemberSeatNew == 91)) {
          m_JA_MemberSeatNew++;
       }
    }
    else if ((pNMUpDown->iDelta > 0) && (m_JA_MemberSeatNew <= 136) && (m_JA_MemberSeatNew > 1)) {
       m_JA_MemberSeatNew--;
       if ((m_JA_MemberSeatNew == 69) || (m_JA_MemberSeatNew == 91)) {
          m_JA_MemberSeatNew--;
       }
    }
    
    char buf[5];
    wsprintf(buf, "%.d", m_JA_MemberSeatNew);
    pEdit->SetWindowText(buf);
    
    // Update fields

    m_JA_MemberNameNew     = m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].name;
    m_JA_MemberDistrictNew = m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].district;
    m_JA_MemberPartyNew    = m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].party;
	m_JA_MemberCountyNew   = m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].county;

    CComboBox *pName     = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME);
    CComboBox *pDistrict = (CComboBox *)GetDlgItem(IDC_COMBO_DISTRICT);
    CComboBox *pParty    = (CComboBox *)GetDlgItem(IDC_COMBO_PARTY);
	CComboBox *pCounty   = (CComboBox *)GetDlgItem(IDC_COMBO_COUNTY);

    pName->SetCurSel(pName->FindString(-1, m_JA_MemberNameNew));
    pDistrict->SetCurSel(pDistrict->FindString(-1, m_JA_MemberDistrictNew));
    pParty->SetCurSel(pParty->FindString(-1, m_JA_MemberPartyNew));
	pCounty->SetCurSel(pCounty->FindString(-1, m_JA_MemberCountyNew));

    *pResult = 0;
}

void CMemberSeating::OnKillfocusEditJaSeatnumberNew() 
{
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEATNUMBER1);

    CString editBuf;
    pEdit->GetWindowText(editBuf);

    unsigned int tmpSeat = atoi(editBuf);

    if ((tmpSeat >= 1)  && (tmpSeat <=136) &&
        (tmpSeat != 69) && (tmpSeat != 91)) {
       m_JA_MemberSeatNew = tmpSeat;
    }
    else {
       AfxMessageBox("Seat Number must be between 1 and 136, and not 69 or 91 which are reserved for sergeants!", MB_ICONEXCLAMATION);
    }

    char buf[5];
    wsprintf(buf, "%.d", m_JA_MemberSeatNew);
    pEdit->SetWindowText(buf);

    // Update fields

    m_JA_MemberNameNew     = m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].name;
    m_JA_MemberDistrictNew = m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].district;
    m_JA_MemberPartyNew    = m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].party;
	m_JA_MemberCountyNew   = m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].county;

    CComboBox *pName     = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME);
    CComboBox *pDistrict = (CComboBox *)GetDlgItem(IDC_COMBO_DISTRICT);
    CComboBox *pParty    = (CComboBox *)GetDlgItem(IDC_COMBO_PARTY);
	CComboBox *pCounty   = (CComboBox *)GetDlgItem(IDC_COMBO_COUNTY);

    pName->SetCurSel(pName->FindString(-1, m_JA_MemberNameNew));
    pDistrict->SetCurSel(pDistrict->FindString(-1, m_JA_MemberDistrictNew));
    pParty->SetCurSel(pParty->FindString(-1, m_JA_MemberPartyNew));
	pCounty->SetCurSel(pCounty->FindString(-1, m_JA_MemberCountyNew));
}

void CMemberSeating::OnDeltaposSpinJaSeatnumberOld(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
    
    GetDlgItem(IDC_SPIN_JA_SEATNUMBER2)->SetFocus();

    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEATNUMBER2);
    pEdit->SetFocus();

    // Number must be between 1-136, and 69 and 91 are for the Sergeants.

    if ((pNMUpDown->iDelta < 0) && (m_JA_MemberSeatOld >= 1) && (m_JA_MemberSeatOld < 136)) {
       m_JA_MemberSeatOld++;
       if ((m_JA_MemberSeatOld == 69) || (m_JA_MemberSeatOld == 91)) {
          m_JA_MemberSeatOld++;
       }
    }
    else if ((pNMUpDown->iDelta > 0) && (m_JA_MemberSeatOld <= 136) && (m_JA_MemberSeatOld > 1)) {
       m_JA_MemberSeatOld--;
       if ((m_JA_MemberSeatOld == 69) || (m_JA_MemberSeatOld == 91)) {
          m_JA_MemberSeatOld--;
       }
    }
    
    char buf[5];
    wsprintf(buf, "%.d", m_JA_MemberSeatOld);
    pEdit->SetWindowText(buf);
    
    // Update fields

    m_JA_MemberNameOld     = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].name;
    m_JA_MemberDistrictOld = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].district;
    m_JA_MemberPartyOld    = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].party;
	m_JA_MemberCountyOld   = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].county;

    CComboBox *pName = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME_OLD);
    pName->SetCurSel(pName->FindString(-1, m_JA_MemberNameOld));

    GetDlgItem(IDC_EDIT_JA_DISTRICT2)->SetWindowText(m_JA_MemberDistrictOld);
    GetDlgItem(IDC_EDIT_JA_PARTY2)->SetWindowText(m_JA_MemberPartyOld);
	GetDlgItem(IDC_EDIT_JA_COUNTY2)->SetWindowText(m_JA_MemberCountyOld);

    *pResult = 0;
}

void CMemberSeating::OnKillfocusEditJaSeatnumberOld() 
{
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_JA_SEATNUMBER2);

    CString editBuf;
    pEdit->GetWindowText(editBuf);

    unsigned int tmpSeat = atoi(editBuf);

    if ((tmpSeat >= 1)  && (tmpSeat <=136) &&
        (tmpSeat != 69) && (tmpSeat != 91)) {
       m_JA_MemberSeatOld = tmpSeat;
    }
    else {
       AfxMessageBox("Seat Number must be between 1 and 136, and not 69 or 91 which are reserved for sergeants!", MB_ICONEXCLAMATION);
    }

    char buf[5];
    wsprintf(buf, "%.d", m_JA_MemberSeatOld);
    pEdit->SetWindowText(buf);

    // Update fields

    m_JA_MemberNameOld     = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].name;
    m_JA_MemberDistrictOld = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].district;
    m_JA_MemberPartyOld    = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].party;
	m_JA_MemberCountyOld   = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].county;

    CComboBox *pName = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME_OLD);
    pName->SetCurSel(pName->FindString(-1, m_JA_MemberNameOld));

    GetDlgItem(IDC_EDIT_JA_DISTRICT2)->SetWindowText(m_JA_MemberDistrictOld);
    GetDlgItem(IDC_EDIT_JA_PARTY2)->SetWindowText(m_JA_MemberPartyOld);
	GetDlgItem(IDC_EDIT_JA_COUNTY2)->SetWindowText(m_JA_MemberCountyOld);
}


void CMemberSeating::OnButtonAddMember() 
{
    strcpy_s(m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].name, m_JA_MemberNameNew);
    m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].seat = m_JA_MemberSeatNew;
    strcpy_s(m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].district, m_JA_MemberDistrictNew);
    strcpy_s(m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].party, m_JA_MemberPartyNew);
	strcpy_s(m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].county, m_JA_MemberCountyNew);

    UpdateComboListNew();

    CComboBox *pName = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME);
    pName->SetCurSel(pName->FindString(-1, m_JA_MemberNameNew));

    if (m_JA_Autoincrement) {
       m_JA_MemberSeatNew++;

       if (m_JA_MemberSeatNew == 69) {
           strcpy_s(m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].name, "Sarge1");
           m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].seat = m_JA_MemberSeatNew;
           strcpy_s(m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].district, "XXX");
           strcpy_s(m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].party, m_JA_PartyList[2]);
           m_JA_MemberSeatNew++;
       }
       else if (m_JA_MemberSeatNew == 91) {
           strcpy_s(m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].name, "Sarge2");
           m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].seat = m_JA_MemberSeatNew;
           strcpy_s(m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].district, "XXX");
           strcpy_s(m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].party, m_JA_PartyList[2]);
           m_JA_MemberSeatNew++;
       } 

        char buf[5];
        wsprintf(buf, "%.d", m_JA_MemberSeatNew);
        GetDlgItem(IDC_EDIT_JA_SEATNUMBER1)->SetWindowText(buf);
    }

    GetDlgItem(IDC_EDIT_JA_SEATNUMBER1)->SetFocus();
}

void CMemberSeating::OnButtonTransfer() 
{
    m_JA_MemberNameNew     = m_JA_MemberNameOld;
    m_JA_MemberDistrictNew = m_JA_MemberDistrictOld;
    m_JA_MemberPartyNew    = m_JA_MemberPartyOld;
	m_JA_MemberCountyNew   = m_JA_MemberCountyOld;

    CComboBox *pName     = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME);
    CComboBox *pDistrict = (CComboBox *)GetDlgItem(IDC_COMBO_DISTRICT);
    CComboBox *pParty    = (CComboBox *)GetDlgItem(IDC_COMBO_PARTY);
	CComboBox *pCounty   = (CComboBox *)GetDlgItem(IDC_COMBO_COUNTY);

    pName->SetCurSel(pName->FindString(-1, m_JA_MemberNameOld));
    pDistrict->SetCurSel(pDistrict->FindString(-1, m_JA_MemberDistrictNew));
    pParty->SetCurSel(pParty->FindString(-1, m_JA_MemberPartyNew));
	pCounty->SetCurSel(pCounty->FindString(-1, m_JA_MemberCountyNew));
}

void CMemberSeating::OnSelchangeComboJaNameNew() 
{
    CComboBox *pName      = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME);
    CComboBox *pDistrict  = (CComboBox *)GetDlgItem(IDC_COMBO_DISTRICT);
    CComboBox *pParty     = (CComboBox *)GetDlgItem(IDC_COMBO_PARTY);
	CComboBox *pCounty    = (CComboBox *)GetDlgItem(IDC_COMBO_COUNTY);

    int tmpSeat;

    tmpSeat = pName->GetItemData(pName->GetCurSel());

    if (tmpSeat == -1) {
        return;
    }

    pName->GetWindowText(m_JA_MemberNameNew);

    m_JA_MemberSeatNew = tmpSeat;
    m_JA_MemberDistrictNew = m_JA_MemberSeatingListOld[m_JA_MemberSeatNew].district;
    m_JA_MemberPartyNew    = m_JA_MemberSeatingListOld[m_JA_MemberSeatNew].party;
	m_JA_MemberCountyNew   = m_JA_MemberSeatingListOld[m_JA_MemberSeatNew].county;

    pDistrict->SetCurSel(pDistrict->FindString(-1, m_JA_MemberDistrictNew));
    pParty->SetCurSel(pParty->FindString(-1, m_JA_MemberPartyNew));
	pCounty->SetCurSel(pCounty->FindString(-1, m_JA_MemberCountyNew));

    char buf[5];
    wsprintf(buf, "%.d", m_JA_MemberSeatNew);
    GetDlgItem(IDC_EDIT_JA_SEATNUMBER1)->SetWindowText(buf);
}

void CMemberSeating::OnSelchangeComboJaNameOld() 
{
    CComboBox *pName = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME_OLD);

    pName->GetWindowText(m_JA_MemberNameOld);
    m_JA_MemberSeatOld = pName->GetItemData(pName->GetCurSel());

    m_JA_MemberDistrictOld = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].district;
    m_JA_MemberPartyOld    = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].party;
	m_JA_MemberCountyOld   = m_JA_MemberSeatingListOld[m_JA_MemberSeatOld].county;

    GetDlgItem(IDC_EDIT_JA_DISTRICT2)->SetWindowText(m_JA_MemberDistrictOld);
    GetDlgItem(IDC_EDIT_JA_PARTY2)->SetWindowText(m_JA_MemberPartyOld);
	GetDlgItem(IDC_EDIT_JA_COUNTY2)->SetWindowText(m_JA_MemberCountyOld);

    char buf[5];
    wsprintf(buf, "%.d", m_JA_MemberSeatOld);
    GetDlgItem(IDC_EDIT_JA_SEATNUMBER2)->SetWindowText(buf);
}


void CMemberSeating::OnKillfocusComboJaNameNew() 
{
    CComboBox *pName = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME);

    pName->GetWindowText(m_JA_MemberNameNew);
}


void CMemberSeating::UpdateComboListNew(void) 
{
   CComboBox *pName = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME);

   pName->ResetContent();

   for (int i = 0; i < MAX_SEATS; i++) {
      if ((i != 69) && (i != 91)) {
         if (strlen(m_JA_MemberSeatingListNew[i].name) > 0) {
            pName->AddString(m_JA_MemberSeatingListNew[i].name);
            pName->SetItemData(pName->FindString(-1, m_JA_MemberSeatingListNew[i].name), i);
         }
      }
   }
}


void CMemberSeating::UpdateComboListOld(void) 
{
   CComboBox *pName = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME_OLD);

   pName->ResetContent();

   for (int i = 0; i < MAX_SEATS; i++) {
      if ((i != 69) && (i != 91)) {
         if (strlen(m_JA_MemberSeatingListOld[i].name) > 0) {
            pName->AddString(m_JA_MemberSeatingListOld[i].name);
            pName->SetItemData(pName->FindString(-1, m_JA_MemberSeatingListOld[i].name), i);
         }
      }
   }
}

void CMemberSeating::OnSetfocusComboJaNameNew() 
{
    CComboBox *pName = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME);

    pName->SetCurSel(pName->FindString(-1, m_JA_MemberNameNew));
}

void CMemberSeating::OnCheckJaAutoincrement() 
{
    m_JA_Autoincrement = ((CButton *)GetDlgItem(IDC_CHECK_JA_AUTOINCREMENT))->GetCheck();

    if (m_JA_Autoincrement) {
        if (AfxMessageBox("Clear Current Member Seating List?", MB_YESNO) == IDYES) {
            CComboBox *pName = (CComboBox *)GetDlgItem(IDC_COMBO_JA_NAME);
            pName->ResetContent();
            for (int i = 0; i < MAX_SEATS; i++) {
               strcpy_s(m_JA_MemberSeatingListNew[i].name, "                              ");
               m_JA_MemberSeatingListNew[i].seat = i;
               strcpy_s(m_JA_MemberSeatingListNew[i].district, "    ");
               strcpy_s(m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].party, "        ");
			   strcpy_s(m_JA_MemberSeatingListNew[m_JA_MemberSeatNew].county, "        ");
            }
            CComboBox *pDistrict = (CComboBox *)GetDlgItem(IDC_COMBO_DISTRICT);
            CComboBox *pParty    = (CComboBox *)GetDlgItem(IDC_COMBO_PARTY);
			CComboBox *pCounty   = (CComboBox *)GetDlgItem(IDC_COMBO_COUNTY);

            pDistrict->SetTopIndex(0);
            pParty->SetTopIndex(0);
			pCounty->SetTopIndex(0);
        }
    }
}

void CMemberSeating::OnSelchangeComboDistrict() 
{
    CComboBox  *pDistrict = (CComboBox *)GetDlgItem(IDC_COMBO_DISTRICT);

    pDistrict->GetWindowText(m_JA_MemberDistrictNew);
}

void CMemberSeating::OnSelchangeComboParty() 
{
    CComboBox  *pParty = (CComboBox *)GetDlgItem(IDC_COMBO_PARTY);

    pParty->GetWindowText(m_JA_MemberPartyNew);
}


void CMemberSeating::OnCbnSelchangeComboCounty()
{
	CComboBox  *pCounty = (CComboBox *)GetDlgItem(IDC_COMBO_COUNTY);

	pCounty->GetWindowText(m_JA_MemberCountyNew);
}
