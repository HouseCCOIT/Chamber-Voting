#if !defined(AFX_JASTATIC_H__C4999578_3523_4D1B_AEA7_C02C88EB171A__INCLUDED_)
#define AFX_JASTATIC_H__C4999578_3523_4D1B_AEA7_C02C88EB171A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JaStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJaStatic window

#define TRANS_BACK 555

class CJaStatic : public CStatic
{
   // Construction
public:
   CJaStatic();

   // Attributes
public:

private:
   COLORREF    m_TextColor;
   COLORREF    m_BackColor;
   CBrush      m_Brush;
   CWnd        *m_Parent;
   // Operations
public:
   void SetTextColor(COLORREF col) {
      m_TextColor = col;
      //UpdateCtrl();             
   }
   void SetBackColor(COLORREF col) {
      m_BackColor = col;
      //UpdateCtrl();            
   }

   void UpdateCtrl();

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CJaStatic)
   //}}AFX_VIRTUAL

   // Implementation
public:
   virtual ~CJaStatic();

   // Generated message map functions
protected:
   //{{AFX_MSG(CJaStatic)
   afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
   afx_msg void OnUpdate();
   //    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   //    afx_msg void OnKillfocus();
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JASTATIC_H__C4999578_3523_4D1B_AEA7_C02C88EB171A__INCLUDED_)
