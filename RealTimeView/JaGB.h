#pragma once
#include <afxwin.h>

#define TRANS_BACK 555

class CJaGB : public CStatic
{
private:
   COLORREF m_BorderColor;
   COLORREF m_BackColor;
   CBrush   m_Brush;

public:
   void SetBorderColor(COLORREF col) {
      m_BorderColor = col;
      //UpdateCtrl();
   }
   void SetBackColor(COLORREF col) {
      m_BackColor = col;
      //UpdateCtrl();
   }

private:
   void UpdateCtrl();

   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CJaGB)
   //}}AFX_VIRTUAL

protected:
   //{{AFX_MSG(CJaGB)
   afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
   afx_msg void OnPaint();
   afx_msg void OnUpdate();
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
