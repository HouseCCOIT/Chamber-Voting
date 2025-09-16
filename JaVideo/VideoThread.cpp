// VMThread.cpp : implementation file
//

#include "stdafx.h"
#include "JaVideoDlg.h"
#include "VideoThread.h"
#include "client32.h"
#include "alphasort.h"

#include <winioctl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char mister_speaker[] = "Spk. ";
unsigned short boardMap[MAX_SEATS];            // vectors absolute vote station numbers to display board locations
short station_lock[MAX_VOTE_ARRAY];                 // != 0: bypass this vote station in polling sequence

extern CString JaVoteIni; // = CString("V:\\JaVote.Ini");
CString    m_JA_VPTitle1 = "                                                                        ";
CString    m_JA_VPTitle2 = "                                                                        ";
CString    m_JA_VPTitle3 = "                                                                        ";
UINT       m_JA_SessionNumber;
UINT       m_JA_SequenceNumber;
CString    m_JA_SeatingDate;

LEX        m_JA_MemberSeatingList[MAX_SEATS];

CJaVideoDlg *m_JA_VideoDialog;


int CompareMembers(const void* member1, const void* member2)
{
   const char apostrophe = (const char)'\'';

   CString name1 = ((LEX*)member1)->name;
   CString name2 = ((LEX*)member2)->name;

   // Remove apostrophe's prior to comparison
   name1.Remove(apostrophe);
   name2.Remove(apostrophe);

   // Comparing name in lower case guarantees telephone sort
   name1.MakeLower();
   name2.MakeLower();

   return(strcoll(name1, name2));
}

void JaLoadParametersAndSeating(void)
{
   FILE *tmpFile;

   char section[32];
   char key[32];
   char inBuff[81];

   unsigned int i, j, seat;

   // Does JaVote.Ini file exist?
   tmpFile = fopen(JaVoteIni, "r+");
   if (tmpFile == NULL) {
      DWORD error = GetLastError();
      return;
   }
   else {
      fclose(tmpFile);
   }

   wsprintf(section, "Parameters");

   GetPrivateProfileString(section, " SeatingDate", "19981225", inBuff, 80, JaVoteIni);
   m_JA_SeatingDate = inBuff;

   GetPrivateProfileString(section, " VPTitle1", "Title1", inBuff, 80, JaVoteIni);
   for (i = 0, j = (70 - strlen(inBuff)) / 2; i < strlen(inBuff); i++) {
      m_JA_VPTitle1.SetAt(j++, inBuff[i]);
   }
   GetPrivateProfileString(section, " VPTitle2", "Title2", inBuff, 80, JaVoteIni);
   for (i = 0, j = (70 - strlen(inBuff)) / 2; i < strlen(inBuff); i++) {
      m_JA_VPTitle2.SetAt(j++, inBuff[i]);
   }
   GetPrivateProfileString(section, " VPTitle3", "Title3", inBuff, 80, JaVoteIni);
   for (i = 0, j = (70 - strlen(inBuff)) / 2; i < strlen(inBuff); i++) {
      m_JA_VPTitle3.SetAt(j++, inBuff[i]);
   }

   GetPrivateProfileString(section, " SessionNumber", "9999", inBuff, 80, JaVoteIni);
   m_JA_SessionNumber = atoi(inBuff);

   int k = 0;
   for (i = 0; i < MAX_SEATS; i++) {
      seat = i + 1;
      wsprintf(section, "MemberSeat_%03.3d", seat);

      wsprintf(key, " Name");
      GetPrivateProfileString(section, key, "Hornsworthy", inBuff, 80, JaVoteIni);
      for (j = strlen(inBuff); j < 30; j++) {
         strcat_s(inBuff, " ");
      }

      if ((seat != 69) && (seat != 91)) {
         if (seat == 136) {
            strcpy_s(m_JA_MemberSeatingList[k].name, mister_speaker);
            strncat(m_JA_MemberSeatingList[k].name, inBuff, 25);
         }
         else {
            strncpy(m_JA_MemberSeatingList[k].name, inBuff, 30);
         }
         m_JA_MemberSeatingList[k].seat = seat;
         m_JA_MemberSeatingList[k].seatID = seat + 2000; // Window Id
         k++;
      }
   }
   m_JA_MemberSeatingList[k].seat = 0;

   setlocale(LC_COLLATE, "en-029"); // set locale to english-carribean

   qsort((char *)m_JA_MemberSeatingList, (MAX_SEATS - 3), sizeof(LEX), CompareMembers);

   // map members to board location
   for (i = 0; m_JA_MemberSeatingList[i].seat; i++) {
      boardMap[m_JA_MemberSeatingList[i].seat - 1] = i;
      m_JA_MemberSeatingList[i].name[NAME_PRINTABLE] = '\0';  // truncate the padded name for columnation on printout
   }

   boardMap[135] = 133;                                  // shame on you, Mr. Speaker
   //boardMap[134] = 132;                                  // shame on you, Mr. Speaker

   m_JA_MemberSeatingList[i].name[NAME_PRINTABLE] = '\0';

   for (i = MAX_SEATS; i < MAX_VOTE_ARRAY; i++) {
      station_lock[i] = 1;
   }
}
