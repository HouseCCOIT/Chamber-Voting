#if !defined(AFX_JART_H__7DA75E5D_F552_4881_9667_58E460871C75__INCLUDED_)
#define AFX_JART_H__7DA75E5D_F552_4881_9667_58E460871C75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JaRT.h : header file
//

#include "JaStatic.h"
#include "JaGB.h"
#include "resource.h"

typedef BOOL(WINAPI *lpfnSetLayeredWindowAttributes)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);


/////////////////////////////////////////////////////////////////////////////
// JaRT dialog

class JaRT : public CDialog
{
   // Construction
public:
   void FadeIn();
   void FadeOut();
   void SetMemberVote(int Member, int vote);
   JaRT(CWnd* pParent = NULL);   // standard constructor

                                 // Dialog Data
                                 //{{AFX_DATA(JaRT)
   enum { IDD = IDD_JART_DIALOG };
   //}}AFX_DATA
   CJaStatic m_BorderTop;
   CJaStatic m_BorderBottom;
   CStatic m_MemberYes[150];
   CStatic m_MemberNo[150];
   CJaStatic m_MemberName[150];
	CJaGB     m_MemberGroupBox[5];

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(JaRT)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
                                                       //}}AFX_VIRTUAL

                                                       // Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(JaRT)
   virtual BOOL OnInitDialog();
   afx_msg void OnCancel();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
private:
   BOOL m_FadeOut;
   int m_NoColor;
   int m_YesColor;
   CFont m_RTFont;
   CFont m_RTFontThin;
   int m_BackColor;
   int m_TextColor;
   int m_GBColor;
   int m_FadeLevel;
   lpfnSetLayeredWindowAttributes m_pSetLayeredWindowAttributes;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JART_H__7DA75E5D_F552_4881_9667_58E460871C75__INCLUDED_)
