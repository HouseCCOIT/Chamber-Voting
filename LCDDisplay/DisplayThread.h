#if !defined(AFX_DISPLAYTHREAD_H)
#define AFX_DISPLAYTHREAD_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VMThread.h : header file
//

#include "house.h"

// Function Declarations

BOOLEAN SetBaudRate(unsigned char baudIndex);
BOOLEAN DisableScreenSaver();
BOOLEAN ClearScreen();
BOOLEAN SetTextColor(short color);
BOOLEAN SetTextSize(unsigned char size);
BOOLEAN SetTextFontId(unsigned char id);
BOOLEAN MoveTo(unsigned char x, unsigned char y);
BOOLEAN ScreenMode(unsigned char mode);
BOOLEAN SendDisplayString(char* dispStr);
int DisplayInit();
void JaReadCurrentVote();

//UINT DisplayThreadFunction(LPVOID DlgObj);
void numeric_display(short y, short n, short a, bool force);

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYTHREAD_H)
