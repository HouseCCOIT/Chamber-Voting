#if !defined(AFX_MEMBERSEATING_H__1C3D7AD1_3F5F_11D2_9F53_00104B9A2A27__INCLUDED_)
#define AFX_MEMBERSEATING_H__1C3D7AD1_3F5F_11D2_9F53_00104B9A2A27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MemberSeating.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MemberSeating dialog

class CMemberSeating : public CDialog
{
// Construction
public:
    CMemberSeating(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CMemberSeating)
    enum { IDD = IDD_MEMBER_SEATING_DIALOG };
    BOOL    m_JA_Autoincrement;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMemberSeating)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CMemberSeating)
    afx_msg void OnDeltaposSpinJaSeatnumberNew(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDeltaposSpinJaSeatnumberOld(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnKillfocusEditJaSeatnumberNew();
    afx_msg void OnKillfocusEditJaSeatnumberOld();
    virtual BOOL OnInitDialog();
    afx_msg void OnButtonAddMember();
    afx_msg void OnButtonTransfer();
    afx_msg void OnSelchangeComboJaNameOld();
    afx_msg void OnSelchangeComboJaNameNew();
    afx_msg void OnKillfocusComboJaNameNew();
    afx_msg void OnSetfocusComboJaNameNew();
    afx_msg void OnCheckJaAutoincrement();
	afx_msg void OnSelchangeComboDistrict();
	afx_msg void OnSelchangeComboParty();
	//}}AFX_MSG

    void UpdateComboListNew(void);
    void UpdateComboListOld(void);

    UINT    m_JA_MemberSeatNew;
    UINT    m_JA_MemberSeatOld;

    CString m_JA_MemberNameNew;
    CString m_JA_MemberDistrictNew;
    CString m_JA_MemberPartyNew;
	CString m_JA_MemberCountyNew;

    CString m_JA_MemberNameOld;
    CString m_JA_MemberDistrictOld;
    CString m_JA_MemberPartyOld;
	CString m_JA_MemberCountyOld;

    DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboCounty();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEMBERSEATING_H__1C3D7AD1_3F5F_11D2_9F53_00104B9A2A27__INCLUDED_)
