#pragma once
#include <afxwin.h>
class CJaGB : public CStatic
{
private:
   COLORREF m_BorderColor;

public:
   void SetBorderColor(COLORREF col) {
      m_BorderColor = col;
      //UpdateCtrl();
   }

public:
   CJaGB();

protected:
   afx_msg void OnPaint();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

