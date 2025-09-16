// VMThread.cpp : implementation file
//

#include "stdafx.h"
#include "LCDDisplayDlg.h"
#include "DisplayThread.h"

#include <winioctl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLCDDisplayDlg *m_DisplayDialog;

CString JaVoteDat = CString("V:\\JaVote.dat"); // V:
//CString JaLockDat = CString("V:\\JaLock.dat");

// LCD Display variables
CString COMPortA = CString("\\\\.\\COM3");
CString COMPortB = CString("\\\\.\\COM4");
CString COMPortC = CString("\\\\.\\COM5");
CString COMPortD = CString("\\\\.\\COM6");
HANDLE  comHandleA = NULL;
HANDLE  comHandleB = NULL;
HANDLE OpenCOMPort(CString comPort);

unsigned char votes[MAX_VOTE_ARRAY];

void JaReadCurrentVote()
{
   FILE* voteFile;
   int count;

   TRACE("Open Vote file");

   voteFile = fopen(JaVoteDat, "rb");

   if (voteFile == NULL) {    // No vote file - just return;
      TRACE("No Vote file!");
      return;
   }
   else {   // Read current votes
      count = fread(votes, 1, sizeof(votes), voteFile);

      if (count != sizeof(votes)) {
         TRACE("Unable to read Vote file!");
      }

      fclose(voteFile);
   }
}

BOOLEAN SetBaudRate(unsigned char baudIndex)
{
   int Error;

   // Set Baud Rate
   Error = TransmitCommChar(comHandleA, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x26);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, baudIndex);
   Sleep(1);

   Error = TransmitCommChar(comHandleB, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x26);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, baudIndex);
   Sleep(1);

   return true;
}

BOOLEAN DisableScreenSaver()
{
   int Error;

   // Disable Screen Saver
   Error = TransmitCommChar(comHandleA, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x0C);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x00);
   Sleep(1);

   Error = TransmitCommChar(comHandleB, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x0C);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x00);
   Sleep(1);

   return true;
}

BOOLEAN ClearScreen()
{
   int Error;

   Error = TransmitCommChar(comHandleA, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x82);
   Sleep(1);

   Error = TransmitCommChar(comHandleB, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x82);
   Sleep(1);

   return true;
}

BOOLEAN SetTextColor(short color)
{
   int Error;

   Error = TransmitCommChar(comHandleA, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0xEE);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, color >> 8);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, color & 0xFF);
   Sleep(1);

   Error = TransmitCommChar(comHandleB, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0xEE);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, color >> 8);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, color & 0xFF);
   Sleep(1);

   return true;
}

BOOLEAN SetTextSize(unsigned char size)
{
   int Error;

   Error = TransmitCommChar(comHandleA, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0xEB); // width
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, size & 0xFF);
   Sleep(1);

   Error = TransmitCommChar(comHandleA, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0xEA); // height
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, size & 0xFF);
   Sleep(1);

   Error = TransmitCommChar(comHandleB, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0xEB); // width
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, size & 0xFF);
   Sleep(1);

   Error = TransmitCommChar(comHandleB, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0xEA); // height
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, size & 0xFF);
   Sleep(1);

   return true;
}

BOOLEAN SetTextFontId(unsigned char id)
{
   int Error;

   Error = TransmitCommChar(comHandleA, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0xEC);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, id);
   Sleep(1);

   Error = TransmitCommChar(comHandleB, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0xEC);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, id);
   Sleep(1);

   return true;
}

BOOLEAN MoveTo(unsigned char x, unsigned char y)
{
   int Error;

   Error = TransmitCommChar(comHandleA, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x81);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, x);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, y);
   Sleep(1);

   Error = TransmitCommChar(comHandleB, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x81);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, x);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, y);
   Sleep(1);

   return true;
}

BOOLEAN ScreenMode(unsigned char mode)
{
   int Error;

   // COM5
   Error = TransmitCommChar(comHandleA, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x42);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleA, mode);
   Sleep(1);

   // COM6
   Error = TransmitCommChar(comHandleB, 0xFF);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x42);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, 0x00);
   Sleep(1);
   Error = TransmitCommChar(comHandleB, mode);
   Sleep(1);

   return true;
}

BOOLEAN SendDisplayString(char* dispStr)
{
   int Error;

   // If COM5 exists send string to port

   if (comHandleA != NULL) {
      Error = TransmitCommChar(comHandleA, 0x00);
      Sleep(1);
      Error = TransmitCommChar(comHandleA, 0x18);
      Sleep(1);

      for (int i = 0; i < 16; i++) {
         Error = TransmitCommChar(comHandleA, dispStr[i]);
         Sleep(1);
         if (dispStr[i] == 0x00) break;
      }

      //Error = TransmitCommChar(comHandle5, 0x00);
   }

   // Repeat for COM6

   if (comHandleB != NULL) {
      Error = TransmitCommChar(comHandleB, 0x00);
      Sleep(1);
      Error = TransmitCommChar(comHandleB, 0x18);
      Sleep(1);

      for (int i = 0; i < 16; i++) {
         Error = TransmitCommChar(comHandleB, dispStr[i]);
         Sleep(1);
         if (dispStr[i] == 0x00) break;
      }

      //Error = TransmitCommChar(comHandle6, 0x00);
   }

   return true;
}

void numeric_display(short y, short n, short a, bool force)
{
   char dispStr[20];
   unsigned char FontId = 10;

   static short yL, nL, aL;

   if (((y != yL) || (n != nL) || (a != aL)) || force) {

      //ScreenMode(0);
      //ClearScreen();
      //Sleep(2);

      //SetTextColor(0xFFFF);
      //SetTextFontId(FontId);
      SetTextSize(6);
      //Sleep(2);

      MoveTo(50, 30);
      Sleep(2);
      sprintf_s(dispStr, "YEA  %*d", 3, y);
      SendDisplayString(dispStr);
      //Sleep(2);

      MoveTo(50, 130);
      //Sleep(2);
      sprintf_s(dispStr, "NAY  %*d", 3, n);
      SendDisplayString(dispStr);
      //Sleep(2);

      MoveTo(50, 230);
      //Sleep(2);
      sprintf_s(dispStr, "N/V  %*d", 3, a);
      SendDisplayString(dispStr);

      yL = y;
      nL = n;
      aL = a;
   }
}

void clear_display()
{
   char dispStr[20];

   ClearScreen();
   Sleep(1);

   SetTextSize(6);
   Sleep(1);

   MoveTo(50, 30);
   Sleep(1);
   sprintf_s(dispStr, "     ");
   SendDisplayString(dispStr);
   Sleep(1);

   MoveTo(50, 130);
   Sleep(1);
   sprintf_s(dispStr, "     ");
   SendDisplayString(dispStr);
   Sleep(1);

   MoveTo(5, 230);
   Sleep(1);
   sprintf_s(dispStr, "     ");
   SendDisplayString(dispStr);
}

HANDLE OpenCOMPort(CString comPort)
{
   DCB    ComDCB;
   int    Error;
   HANDLE  comHandle;

   comHandle = CreateFile(comPort,     // Communications Port
      GENERIC_WRITE | GENERIC_READ,
      0,                               // Exclusive access
      NULL,                            // No security.
      OPEN_EXISTING,                   // Fail if not existing.
      0,                               // No overlap.
      NULL);                           // No template.

   if (comHandle != INVALID_HANDLE_VALUE) {

      GetCommState(comHandle, &ComDCB);

      ComDCB.DCBlength = sizeof(DCB);
      ComDCB.BaudRate = 115200;
      ComDCB.Parity = 0;
      ComDCB.StopBits = 0;
      ComDCB.ByteSize = 8;

      //if (!BuildCommDCB(LPCTSTR("baud=9600 parity=N data=8 stop=1"), &ComDCB)) {
      //   Error = GetLastError();
      //   CloseHandle(comHandle);
      //   comHandle = NULL;
      //}

      if (!SetCommState(comHandle, &ComDCB)) {
         Error = GetLastError();
         CloseHandle(comHandle);
         comHandle = NULL;
      }
   }
   else {
      comHandle = NULL;
   }

   return comHandle;
}

//unsigned char votes[MAX_VOTE_ARRAY];

int DisplayInit()
{
   if (comHandleA != NULL) {
      CloseHandle(comHandleA);
      comHandleA = NULL;
   }
   if (comHandleB != NULL) {
      CloseHandle(comHandleB);
      comHandleB = NULL;
   }

   comHandleA = OpenCOMPort(COMPortA);
   if (comHandleA == NULL) {
      comHandleA = OpenCOMPort(COMPortC);
      if (comHandleA == NULL) {
         comHandleA = OpenCOMPort(COMPortD);
         if (comHandleA == NULL) {
            printf("Unable to open COM-A\n");
         }
      }
   }

   comHandleB = OpenCOMPort(COMPortB);
   if (comHandleB == NULL) {
      comHandleB = OpenCOMPort(COMPortD);
      if (comHandleB == NULL) {
         comHandleB = OpenCOMPort(COMPortC);
         if (comHandleB == NULL) {
            printf("Unable to open COM-B\n");
         }
      }
   }

   ScreenMode(0);
   ClearScreen();
   SetTextColor(0xFFFF);
   SetTextFontId(3);
   SetTextSize(6);

   return 0;
}

int DisplayThread()
{
   //std::cout << "JaLCDConsole - Hit <ESC> to quit.\n";

   if (comHandleA != NULL) {
      CloseHandle(comHandleA);
      comHandleA = NULL;
   }
   if (comHandleB != NULL) {
      CloseHandle(comHandleB);
      comHandleB = NULL;
   }

   comHandleA = OpenCOMPort(COMPortA);
   if (comHandleA == NULL) {
      comHandleA = OpenCOMPort(COMPortC);
      if (comHandleA == NULL) {
         comHandleA = OpenCOMPort(COMPortD);
         if (comHandleA == NULL) {
            printf("Unable to open COM-A\n");
         }
      }
   }

   comHandleB = OpenCOMPort(COMPortB);
   if (comHandleB == NULL) {
      comHandleB = OpenCOMPort(COMPortD);
      if (comHandleB == NULL) {
         comHandleB = OpenCOMPort(COMPortC);
         if (comHandleB == NULL) {
            printf("Unable to open COM-B\n");
         }
      }
   }

   ScreenMode(0);
   ClearScreen();
   SetTextColor(0xFFFF);
   SetTextFontId(3);
   SetTextSize(6);

   bool exit = false;

   while (exit == false)
   {
      if (GetAsyncKeyState(VK_ESCAPE))
      {
         exit = true;
      }

      Sleep(1000);



      //if (JaVoteFileChanged()) {
      //   JaReadCurrentVote();
      //   int yeas = votes[136];
      //   int nays = votes[137];
      //   if ((yeas + nays) <= 134) {
      //      numeric_display(yeas, nays, 134 - (yeas + nays), true);
      //   }

      //   system("CLS");

      //  // std::cout << "JaLCDConsole - Hit <ESC> to quit.\n";

      //  // printf("YEAS %.*d  NAYS %.*d  N/V %.*d", 3, votes[136], 3, votes[137], 3, 134 - (votes[136] + votes[137]));
      //}
      //else {
      //   numeric_display(0, 0, 134, true);

      //   system("CLS");

      //   std::cout << "JaLCDConsole - Hit <ESC> to quit.\n";

      //   printf("YEAS %.*d  NAYS %.*d  N/V %.*d", 3, 0, 3, 0, 3, 134);
      //}
   }

   if (comHandleA != NULL) {
      CloseHandle(comHandleA);
      comHandleA = NULL;
   }
   if (comHandleB != NULL) {
      CloseHandle(comHandleB);
      comHandleB = NULL;
   }

   return true;
}