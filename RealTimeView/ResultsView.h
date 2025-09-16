#if !defined(AFX_JADISPLAY_H__3493B41C_8624_4D0D_B273_2A86EAD2B2AD__INCLUDED_)
#define AFX_JADISPLAY_H__3493B41C_8624_4D0D_B273_2A86EAD2B2AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JaDisplay.h : header file
//

#include "JaStatic.h"
#include "dib256.h"	// Added by ClassView
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// JaDisplay dialog

class CRVDlg : public CDialog
{
   // Construction
public:
   void DisplayResults(BOOL DisplayOn);
   void ShuttingDown();
   void SetWaterMark(CString watermark);
   void UpdateDisplay();
   void ClearResults();
   void SetResultLine(char *text1, char *text2, char *text3, char *text4);
   void SetLine(int i, char *text);
   void ClearAll();
   void ClearSix();
   void SetFontSize(int fontSize);
   void SetFont(CString fontName);
   BOOL LoadDispOptions();
   CRVDlg(CWnd* pParent = NULL);   // standard constructor

                                      // Dialog Data
                                      //{{AFX_DATA(JaDisplay)
   enum { IDD = IDD_RESULTSVIEW_DIALOG };
   CJaStatic	m_JD_Static12;
   CJaStatic	m_JD_Static11;
   CJaStatic	m_JD_Static10;
   CJaStatic	m_JD_Static9;
   CJaStatic	m_JD_Static8;
   CJaStatic	m_JD_Static7;
   CJaStatic	m_JD_Static6;
   CJaStatic	m_JD_Static5;
   CJaStatic	m_JD_Static3;
   CJaStatic	m_JD_Static4;
   CJaStatic	m_JD_Static2;
   CJaStatic	m_JD_Static1;
   //}}AFX_DATA


   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(JaDisplay)
protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
                                                       //}}AFX_VIRTUAL

                                                       // Implementation
protected:

   // Generated message map functions
   //{{AFX_MSG(JaDisplay)
   virtual BOOL OnInitDialog();
   afx_msg void OnCancel();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   //}}AFX_MSG
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   DECLARE_MESSAGE_MAP()

private:
   BOOL m_ShowResults;
   FILETIME JaGetFileTime(CString fName);
   BOOL DisplayFileChanged();
   BOOLEAN m_ShuttingDown;
   int m_TextColor;
   CString m_Watermark;
   CDIBitmap m_bmpBackground;
   int m_BackColor;
   CString m_ResultLine[5];
   CString m_DisplayLine[9];
   int m_ActiveDisplay;

   enum {
      FontSizeUnknown,
      FontSizeSmall,
      FontSizeMedium,
      FontSizeLarge
   } FONTSIZE;

   int m_FontSize;
   int m_FontSizePoints;
   CFont m_MyFont;
   CString m_FontName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JADISPLAY_H__3493B41C_8624_4D0D_B273_2A86EAD2B2AD__INCLUDED_)
