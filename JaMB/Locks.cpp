// Locks.cpp : implementation file
//

#include "stdafx.h"
#include "JaMB.h"
#include "Locks.h"
#include "house.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Locks dialog


extern LEX m_JA_MemberSeatingList[136];
extern unsigned char m_JA_StationLocks[136];

extern BOOL m_LockSummary;

Locks::Locks(CWnd* pParent /*=NULL*/)
	: CDialog(Locks::IDD, pParent)
{
	//{{AFX_DATA_INIT(Locks)
    if (m_LockSummary) {	
    	m_Summary = 0;
    }
	else {
        m_Summary = 1;
	}
	//}}AFX_DATA_INIT
}


void Locks::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Locks)
	DDX_Control(pDX, IDC_LIST_LOCKS, m_LockList);
	DDX_Radio(pDX, IDC_RADIO_SUMMARY, m_Summary);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Locks, CDialog)
	//{{AFX_MSG_MAP(Locks)
	ON_LBN_SELCHANGE(IDC_LIST_LOCKS, OnSelchangeListLocks)
	ON_BN_CLICKED(IDC_RADIO_SUMMARY, OnRadioSummary)
	ON_BN_CLICKED(IDC_RADIO_FULL_LIST, OnRadioFullList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Locks message handlers

BOOL Locks::OnInitDialog() 
{
	CDialog::OnInitDialog();

    if (m_LockSummary) {	
       for (int i = 0; i < 134; i++) {
          if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
             int tmpSeat = m_JA_MemberSeatingList[i].seat;
             if (m_JA_StationLocks[tmpSeat - 1]) {
                m_LockList.AddString(m_JA_MemberSeatingList[i].name);
                int tmpItem = m_LockList.FindString(-1, m_JA_MemberSeatingList[i].name);
                m_LockList.SetItemData(tmpItem, tmpSeat);
                m_LockList.SetCheck(tmpItem, m_JA_StationLocks[tmpSeat - 1]);
 			 }
          }
       }
	}
	else {
       for (int i = 0; i < 134; i++) {
          if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
             m_LockList.AddString(m_JA_MemberSeatingList[i].name);
             int tmpItem = m_LockList.FindString(-1, m_JA_MemberSeatingList[i].name);
             int tmpSeat = m_JA_MemberSeatingList[i].seat;
             m_LockList.SetItemData(tmpItem, tmpSeat);
             m_LockList.SetCheck(tmpItem, m_JA_StationLocks[tmpSeat - 1]);
		  }
	   }
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void Locks::OnSelchangeListLocks() 
{
    int tmpItem         = m_LockList.GetCurSel();
    int seat            = m_LockList.GetItemData(tmpItem);
    unsigned char check = m_LockList.GetCheck(tmpItem);

    m_JA_StationLocks[seat - 1] = check;
}


void Locks::OnRadioSummary() 
{
	m_LockSummary = TRUE;

    m_LockList.ResetContent();

    for (int i = 0; i < 134; i++) {
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

void Locks::OnRadioFullList() 
{
	m_LockSummary = FALSE;

    m_LockList.ResetContent();

    for (int i = 0; i < 134; i++) {
       if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
          m_LockList.AddString(m_JA_MemberSeatingList[i].name);
          int tmpItem = m_LockList.FindString(-1, m_JA_MemberSeatingList[i].name);
          int tmpSeat = m_JA_MemberSeatingList[i].seat;
          m_LockList.SetItemData(tmpItem, tmpSeat);
          m_LockList.SetCheck(tmpItem, m_JA_StationLocks[tmpSeat - 1]);
       }
    }
}
