// VEThread.cpp : implementation file
//

#include "stdafx.h"
#include "JaVE.h"
#include "VEThread.h"
#include "VM_Sock.h"
#include "alphasort.h"

#include "house.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VEThread

#define ROLL_RECORD 215

LEX m_JA_MemberSeatingList[MAX_SEATS];
CString m_JA_VPTitle1 = "                                                                        ";
CString m_JA_VPTitle2 = "                                                                        ";
CString m_JA_VPTitle3 = "                                                                        ";
UINT    m_JA_SessionNumber;
UINT    m_JA_SequenceNumber;
CString m_JA_SeatingDate;

CString JaVoteIni = "V:\\JaVote.Ini";
static char mister_speaker[] = "Spk. ";

int     m_Sequence;
CString m_Date;
CString m_File;
CString m_Order;
CString m_Action;

char rollData[2][ROLL_RECORD];

int yeaTotal;
int nayTotal;
char *yeaToken[2] = { "yeas", "yea" };
char *nayToken[2] = { "nays", "nay" };

char *rtrim(char *s)
{
    char *p;

    for (p = s + strlen(s) - 1; *p == ' ' && p >= s; p--) ;

    *(p + 1) = '\0';
    return (s);
}


int JaReadSequenceFile(CString seqFileName)
{
   char hi_date[64];
   char hi_file[64];
   char hi_order[64];
   char hi_action[64];

   char buffer[255];

   int i;

   FILE *seqFile;

   if (seqFileName && !(seqFile = fopen(seqFileName, "r"))) {
      wsprintf(buffer, "I cannot open Roll Call Sequence File %s.", seqFileName);
      AfxMessageBox(buffer, MB_ICONEXCLAMATION);
      return 1;
   }
   else if (!seqFileName) {
      return 1;
   }

   // read in the two records (control and vote)
   for(i = 0; i < 2; i++) {
      if (!fgets(rollData[i], ROLL_RECORD, seqFile)) {
         wsprintf(buffer, "Could not read data from Roll Call Sequence File %s.", seqFileName);
         AfxMessageBox(buffer, MB_ICONEXCLAMATION);
         return 1;
      }
   }

   fclose(seqFile); 

   // load strings with data from control record
   strncpy_s(hi_date, rollData[0] + 13, 8);
   hi_date[8] = '\0';
   strncpy_s(hi_order, rollData[0] + 21, 53);
   hi_order[53] = '\0';
   strncpy_s(hi_file, rollData[0] + 74, 53);
   hi_file[53] = '\0';
   strncpy_s(hi_action, rollData[0] + 127, 53);
   hi_action[53] = '\0';

   rtrim(hi_date);
   rtrim(hi_order);
   rtrim(hi_file);
   rtrim(hi_action);

   // put this stuff on the screen
   wsprintf(buffer, "%d-%d-%d", (10   * (hi_date[4] - '0')) + (hi_date[5] - '0'),
                                (10   * (hi_date[6] - '0')) + (hi_date[7] - '0'),
                                (1000 * (hi_date[0] - '0')) + 
                                (100  * (hi_date[1] - '0')) + 
                                (10   * (hi_date[2] - '0')) + (hi_date[3] - '0'));
   m_Date   = buffer;
   m_Order  = hi_order;
   m_File   = hi_file;
   m_Action = hi_action;

   // Get each members vote
   for (i = 0; i < (MAX_SEATS - 2); i++) {
       m_JA_MemberSeatingList[i].vote = rollData[1][m_JA_MemberSeatingList[i].seat] - '0';
   }

   return 0;
}


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

   unsigned int i, j, k, seat;

   // Does JaVote.Ini file exist?
   if ((tmpFile = fopen( JaVoteIni, "r+" )) == NULL ) {
      return;
   }
   else {
      fclose(tmpFile);
   }

   wsprintf(section, "Parameters");

   GetPrivateProfileString (section, " SeatingDate", "970103", inBuff, 80,  JaVoteIni); 
   m_JA_SeatingDate = inBuff;

   GetPrivateProfileString (section, " VPTitle1",    "Title1",  inBuff, 80, JaVoteIni); 
   for(i = 0, j = (72 - strlen(inBuff)) / 2; i < strlen(inBuff); i++) {
      m_JA_VPTitle1.SetAt(j++, inBuff[i]);
   }
   GetPrivateProfileString (section, " VPTitle2",    "Title2",  inBuff, 80, JaVoteIni); 
   for(i = 0, j = (72 - strlen(inBuff)) / 2; i < strlen(inBuff); i++) {
      m_JA_VPTitle2.SetAt(j++, inBuff[i]);
   }
   GetPrivateProfileString (section, " VPTitle3",    "Title3",  inBuff, 80, JaVoteIni); 
   for(i = 0, j = (72 - strlen(inBuff)) / 2; i < strlen(inBuff); i++) {
      m_JA_VPTitle3.SetAt(j++, inBuff[i]);
   }

   GetPrivateProfileString (section, " SessionNumber", "9999",  inBuff, 80, JaVoteIni); 
   m_JA_SessionNumber = atoi(inBuff);

   k = 0;
   for (i = 0; i < MAX_SEATS; i++) {
      seat = i + 1;
      wsprintf(section, "MemberSeat_%03.3d", seat);

      wsprintf(key, " Name");
      GetPrivateProfileString (section, key, "Hornsworthy", inBuff, 80, JaVoteIni); 
      for (j = strlen(inBuff); j < 30; j++) {
          strcat_s(inBuff, " ");
      }
      if (seat != 69 && seat != 91) {
         if (seat == 136) {
            strcpy_s(m_JA_MemberSeatingList[k].name, mister_speaker);
            strncat(m_JA_MemberSeatingList[k].name, inBuff, 25);
         }
         else {
            strncpy_s(m_JA_MemberSeatingList[k].name, inBuff, 30);
         }
         m_JA_MemberSeatingList[k].seat = seat;
         k++;
      }
   }

   setlocale(LC_COLLATE, "en-029"); // set locale to english-carribean

   qsort((char *) m_JA_MemberSeatingList, (MAX_SEATS - 3), sizeof(LEX), CompareMembers);
}


int JaWriteEditedSequenceFile(CString seqFileName)
{
   char buffer[255];

   int i;

   FILE *seqFile;

   if (seqFileName && !(seqFile = fopen(seqFileName, "w"))) {
      wsprintf(buffer, "I cannot open Roll Call Sequence File %s.", seqFileName);
      AfxMessageBox(buffer, MB_ICONEXCLAMATION);
      return 1;
   }
   else if (!seqFileName) {
      return 1;
   }

   yeaTotal = nayTotal = 0;

   // if we got here, time to remap member votes to second file record
   for (i = 0; i < (MAX_SEATS - 2); i++) {
       rollData[1][m_JA_MemberSeatingList[i].seat] = m_JA_MemberSeatingList[i].vote + '0';
       if (m_JA_MemberSeatingList[i].vote == VOTE_NO) {
           nayTotal++;
       }
       else if (m_JA_MemberSeatingList[i].vote == VOTE_YES) {
           yeaTotal++;
       }
   }

   // write out the two records (control and vote)
   for (i = 0; i < 2; i++) {
      if (fputs(rollData[i], seqFile) == EOF) {
         wsprintf(buffer, "Could not write record %d of edited Roll Call Sequence File %s", i, seqFileName);
         AfxMessageBox(buffer, MB_ICONEXCLAMATION);
      }
   }

   fclose(seqFile);

   return 0;
}


int JaSend(CString editedResultName)
{
   char buffer[255];

   int i;

   FILE *editedResultFile;

   if (editedResultName && !(editedResultFile = fopen(editedResultName, "w"))) {
      wsprintf(buffer, "I cannot open Edited Result File %s.", editedResultName);
      AfxMessageBox(buffer, MB_ICONEXCLAMATION);
      return 1;
   }
   else if (!editedResultName) {
      return 1;
   }

   // Construct the vote summary record 
   wsprintf(buffer, "There were %d %s and %d %s as follows:\n\n",
                    yeaTotal, yeaToken[yeaTotal == 1], nayTotal, nayToken[nayTotal == 1]);
   fputs(buffer, editedResultFile);

   // Then, the lead-in for the affirmatives, followed by the rollcall itself
   if (yeaTotal) {

      wsprintf(buffer, "Those who voted in the affirmative were:\n");
      fputs(buffer, editedResultFile);

      wsprintf(buffer, "[COL_ON]\n");
      fputs(buffer, editedResultFile);

      for (i = 0; i < (MAX_SEATS - 2); i++) {
         if (m_JA_MemberSeatingList[i].vote == VOTE_YES) {
            wsprintf(buffer, "%s\n", m_JA_MemberSeatingList[i].name);
            fputs(buffer, editedResultFile);
         }
      }
      wsprintf(buffer, "[COL_OFF]\n");
      fputs(buffer, editedResultFile);
   }

   // Then, the lead-in for the negatives...
   if (nayTotal) {

      wsprintf(buffer, "Those who voted in the negative were:\n");
      fputs(buffer, editedResultFile);

      wsprintf(buffer, "[COL_ON]\n");
      fputs(buffer, editedResultFile);

      for (i = 0; i < (MAX_SEATS - 2); i++) {
         if (m_JA_MemberSeatingList[i].vote == VOTE_NO) {
            if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
               wsprintf(buffer, "%s\n", m_JA_MemberSeatingList[i].name);
               fputs(buffer, editedResultFile);
            }
         }
      }
      wsprintf(buffer, "[COL_OFF]\n");
      fputs(buffer, editedResultFile);
   }

   fclose(editedResultFile);

   return 0;
}


int JaSendTabbed(CString editedResultName)
{
   char buffer[255];

   int i;

   FILE *editedResultFile;

   if (editedResultName && !(editedResultFile = fopen(editedResultName, "w"))) {
      wsprintf(buffer, "I cannot open Edited Result File %s.", editedResultName);
      AfxMessageBox(buffer, MB_ICONEXCLAMATION);
      return 1;
   }
   else if (!editedResultName) {
      return 1;
   }

   wsprintf(buffer, "%s\n", m_Date);
   fputs(buffer, editedResultFile);
   wsprintf(buffer, "%s\n", m_File);
   fputs(buffer, editedResultFile);
   wsprintf(buffer, "%s\n", m_Order);
   fputs(buffer, editedResultFile);
   wsprintf(buffer, "%s\n", m_Action);
   fputs(buffer, editedResultFile);

   // Construct the vote summary record 

   for (i = 0; i < (MAX_SEATS - 2); i++) {
      wsprintf(buffer, "%s\t", m_JA_MemberSeatingList[i].name);
      if (m_JA_MemberSeatingList[i].vote == VOTE_YES) {
         strcat_s(buffer, "Y\n");
      }
      else if (m_JA_MemberSeatingList[i].vote == VOTE_NO) {
         strcat_s(buffer, "N\n");
      }
      else {
         strcat_s(buffer, "O\n");
      }
         fputs(buffer, editedResultFile);
   }

   fclose(editedResultFile);

   return 0;
}

