// MBThread.cpp : implementation file
//

#include "stdafx.h"
#include "afxmt.h"
#include "JaMB.h"
#include "JaMBDlg.h"
#include "tblResults.h"
#include "JaDisplay.h"
#include "MBThread.h"
#include "client32.h"
#include "VM_Sock.h"
#include "alphasort.h"

#ifdef _DEBUG
   #define new DEBUG_NEW
   #undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "house.h"

static char mister_speaker[]   = "Spk. ";
static char yeas_nays[]        = "YEAS      NAYS";

static char seating_date[7];
short session_number;

char virtual_display[2][8][32];     // 2 displays of 8 lines by 16 characters & terminating zero
unsigned short active_display;      // to determine which of the 2 displays is active/

char virtual_result[2][32];         // driven by vote machine communications
char printout_header[4][64];        // for xfer to vote system

int mssg_sema = 0;                  // permission to use the message boards

int lock_page, lock_total;          // for station lock routines


CString JaVoteIni = "V:\\JaVote.Ini";
CString m_JA_VotingMachineName = "vote";
CString    m_JA_PrinterPrimary = "\\\\vote\\Printer1";
CString    m_JA_PrinterSecondary = "\\\\vote\\Printer2";
//CString    m_JA_PrinterPrimary = "\\\\EAKDEV2022\\Brother HL-L3280CDW series";
//CString    m_JA_PrinterSecondary = "\\\\EAKDEV2022\\Brother HL-L3280CDW series";
CString    m_JA_PrinterName = "LPT1:";
static char null_column[] = "       ";          // 7
static char null_name[] = "                 ";  // 17


CString m_JA_VPTitle1 = "                                                                        ";
CString m_JA_VPTitle2 = "                                                                        ";
CString m_JA_VPTitle3 = "                                                                        ";
UINT    m_JA_SessionNumber;
INT     m_JA_SpecialSession;
UINT    m_JA_SequenceNumber;
CString m_JA_SeatingDate;

unsigned char m_JA_StationLocks[MAX_VOTE_ARRAY];
unsigned char m_JA_RemoteVotes[MAX_VOTE_ARRAY];
unsigned char m_JA_CallInVotes[MAX_VOTE_ARRAY];

unsigned int callInCount;
unsigned int remoteCount;
unsigned int lockCount;

LEX           m_JA_MemberSeatingList[MAX_SEATS];

JaResults *pDisplayDlg;
CJaMBDlg* m_JaMBDialog;

HANDLE  MBPipe;          // File or Pipe handle.

CMutex MD_Mutex;

//CSingleLock MD_Sem(&MD_Mutex);

extern BOOL m_NoWeb;
extern BOOL m_TestMode;

int  fileNumber;
int  fileNameSel;
char fileNameStr[16][32] = { "",
                             "hf",
                             "sf",
                             "hr",
                             "hc",
                             "sc",
                             "sf",
                             "hf",
                             "minorityreport",
                             "minorityreport",
                             "houserules",
                             "jointrules" };

struct tm *m_JA_HouseTime;

void setActiveDisplay(int display)
{
   active_display = display;
}


int process_line(LPCTSTR ss, int line, int update)
{
   // Now move the string into the appropriate virtual buffer
   // if the caller requested it
   if (update == YES) {
      if (ss) {
         strcpy_s(virtual_display[active_display][line], ss);
      }
      else {
         *virtual_display[active_display][line] = EOS;
      }
   }

   return(OK);
}

bool UpdateDB(void) 
{
   CDatabase    db;
   ////    This connect string will pop up the ODBC connect dialog
   //CString        cConnect = CString("ODBC;");
   //db.Open(NULL,                //    DSN
   //   FALSE,                //    Exclusive
   //   FALSE,                //    ReadOnly
   //   cConnect,            //    ODBC Connect string
   //   TRUE                //    Use cursor lib
   //   );

   CtblResults    rs(&db);

   if (!rs.Open()) {
      return false;
   }

   if (rs.CanAppend()) {
      //rs.Update();
      rs.Edit();
      rs.m_id = 1;
      rs.m_strLine1_ = virtual_display[active_display][0];
      rs.m_strLine2_ = virtual_display[active_display][1];
      rs.m_strLine3_ = virtual_display[active_display][2];
      rs.m_strLine4_ = virtual_display[active_display][3];
      rs.m_strLine5_ = virtual_display[active_display][4];
      rs.m_strLine6_ = virtual_display[active_display][5];
      rs.m_strLine7_ = virtual_display[active_display][6];
      rs.m_dtUpdate = CTime::GetCurrentTime();
      if (!rs.Update()) {
         AfxMessageBox(CString("Record not added; no field values were set."));
         return false;
      }
   }

   return true;
}

int TransmitDisplay(void)
{
   int i;
   char *p;

   CSingleLock MD_Sem(&MD_Mutex);


   // if message semaphore is set to complete lock, forget it
   if (mssg_sema == 2) {
      return 0;
   }

   // printout buffering
   for (i = 0; i < 3; i++) {
      if (virtual_display[active_display][i * 2]) {
         sprintf(printout_header[i], "%s %s", virtual_display[active_display][i * 2],
                 virtual_display[active_display][(i * 2) + 1]);
      }
      else {
         strcpy_s(printout_header[i], " ");
      }
   }
   if (virtual_display[active_display][6]) {
      sprintf(printout_header[3], "%s", virtual_display[active_display][6]);
   }
   else {
      strcpy_s(printout_header[3], " ");
   }

// M E S S A G E   D I S P L A Y

   // send clear character
   if (mssg_sema == 1) {
      MD_ClearSix();
   }
   else {
      MD_ClearAll();
   }

   MD_Sem.Lock(10000);

//   MD_ClearSix();

   // Push the contents of the buffer out
   for (i = 0; i < 7 + (!mssg_sema); i++) {
      p = virtual_display[active_display][i];
      if (*p) {
         pDisplayDlg->SetLine(i, "");
         pDisplayDlg->SetLine(i, p);
      }
   }
   pDisplayDlg->UpdateDisplay();


   MD_Sem.Unlock();

// W E B   P A G E

   if (m_NoWeb) {
      return 0;
   }

   // Try to update the database three times.
   if (!UpdateDB()) {
      Sleep(1000);
      if (!UpdateDB()) {
         Sleep(1000);
         UpdateDB();
      }
   }

   //FILE *msgHandle;
   //char  msgFilename[64] = "E:\\message.htm";
   //char  tmpStr[255];

   //if (!(msgHandle = fopen(msgFilename, "w"))) {
   //   AfxMessageBox("Unable to open web file message.htm", MB_ICONEXCLAMATION);
   //   return 0;
   //}

   //for (i = 0; i < 6 + (2 * (!mssg_sema)); i++) {
   //   strcpy_s(tmpStr, "<center>");
   //   strcat_s(tmpStr, virtual_display[active_display][i]);
   //   strcat_s(tmpStr, "</center>\n");
   //   fputs(tmpStr, msgHandle);
   //};

   //fclose(msgHandle);

   return 0;
}


int ClearDisplays(void)
{
   // if message semaphore is set to complete lock, forget it
   if (mssg_sema == 2) {
      return 0;
   }

   // else, if we are limited to first six lines...
   if (mssg_sema == 1) {
      MD_ClearSix();
   }
   else {
      MD_ClearAll();
   }
   return 0;
}

int MD_ClearSix(void)
{
   CSingleLock MD_Sem(&MD_Mutex);

   MD_Sem.Lock(10000);

   pDisplayDlg->ClearSix();

   MD_Sem.Unlock();

   return 0;
}


int MD_ClearAll(void)
{
   CSingleLock MD_Sem(&MD_Mutex);

   MD_Sem.Lock(10000);

   pDisplayDlg->ClearAll();

   MD_Sem.Unlock();

   return 0;
}


int MD_Result(char *text1, char *text2, char *text3, char *text4)
{
// M E S S A G E   D I S P L A Y

   CSingleLock MD_Sem(&MD_Mutex);

   MD_Sem.Lock(10000);

   pDisplayDlg->SetResultLine(text1, text2, text3, text4);

   pDisplayDlg->UpdateDisplay();

   MD_Sem.Unlock();

   return 0;
}

int process_result(char *s, int line)
{
   // Now move the string into the appropriate virtual buffer
   if (s) {
      strcpy_s(virtual_result[line], s);
   }
   else {
      *virtual_result[line] = EOS;
   }

   return(OK);
}

void ClearResults(void)
{
   pDisplayDlg->ClearResults();
}

void PrintCallinsOnOpen()
{
   // Print the CallIns

   m_JA_PrinterName = m_JA_PrinterPrimary;

   PrintCallIns(1);

   m_JA_PrinterName = m_JA_PrinterSecondary;

   PrintCallIns(1);
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

   unsigned int i, j, seat;

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
   for (i = 0, j = (72 - strlen(inBuff)) / 2; i < strlen(inBuff); i++) {
      m_JA_VPTitle1.SetAt(j++, inBuff[i]);
   }
   GetPrivateProfileString (section, " VPTitle2",    "Title2",  inBuff, 80, JaVoteIni);
   for (i = 0, j = (72 - strlen(inBuff)) / 2; i < strlen(inBuff); i++) {
      m_JA_VPTitle2.SetAt(j++, inBuff[i]);
   }
   GetPrivateProfileString (section, " VPTitle3",    "Title3",  inBuff, 80, JaVoteIni);
   for (i = 0, j = (72 - strlen(inBuff)) / 2; i < strlen(inBuff); i++) {
      m_JA_VPTitle3.SetAt(j++, inBuff[i]);
   }

   GetPrivateProfileString (section, " SessionNumber", "9999",  inBuff, 80, JaVoteIni);
   m_JA_SessionNumber = atoi(inBuff);

   GetPrivateProfileString (section, " SpecialSession", "0",  inBuff, 80, JaVoteIni);
   m_JA_SpecialSession = atoi(inBuff);

   int k = 0;
   for (i = 0; i < MAX_SEATS; i++) {
      seat = i + 1;
      wsprintf(section, "MemberSeat_%03.3d", seat);

      wsprintf(key, " Name");
      GetPrivateProfileString (section, key, "Hornsworthy", inBuff, 80, JaVoteIni);
      for (j = strlen(inBuff); j < 32; j++) {
         strcat_s(inBuff, " ");
      }

      if ((seat != 69) && (seat != 91)) {
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

   //boardMap[135] = 133;                                  // shame on you, Mr. Speaker
   //boardMap[134] = 132;                                  // shame on you, Mr. Speaker

}

void JaMBVoteSendHeader()
{
   SockPacket packet;

   packet.packetType = VM_MB;
   packet.MBAction = VM_MB_SET_HEADER;

   strcpy_s(packet.header[0], printout_header[0]);
   strcpy_s(packet.header[1], printout_header[1]);
   strcpy_s(packet.header[2], printout_header[2]);
   strcpy_s(packet.header[3], printout_header[3]);

   // Send Header to Voting Machine

   m_JaMBDialog->SendVMMessage(&packet);
}

void JaMBVoteOpen() 
{
   ClearResults();
   mssg_sema = 0;
}

void JaMBVoteActive() 
{
   mssg_sema = 1;
}

void JaMBVoteResults(SockPacket *mbPacket) 
{
   //MBPacket mbPacket;

   char buffer1[BUFFER_LENGTH];
   char buffer2[BUFFER_LENGTH];

   // first, close any other access to the message display board
   mssg_sema = 2;

   // the numeric results follow
   int yes_total = mbPacket->dataByte[0];
   int no_total  = mbPacket->dataByte[1];

   // Send header information back to the voting subsystem
   JaMBVoteSendHeader();

   // Now, the fun part...
   // First, delay the output ~9 seconds
   Sleep(9000);

   // EAK - New. Clear the results line before displaying the results
   // ClearResults();
   // Display the results on the bottom line of the Message Display Board
   MD_Result("Yeas", itoa(yes_total, buffer1, 10), "Nays", itoa(no_total, buffer2, 10));

   // Restore semaphore to allow access to first six lines
   mssg_sema = 1;
}

void JaPrintLine(FILE *stream, char *s)
{
   fprintf(stream, "    %s", s);
}

void PrintLocks(unsigned int numPages)
{
   char  LockName[3][20];
   unsigned int i, j, k;
   char buffer[BUFFER_LENGTH];

   CString LockTitle1 = "                                                                               ";
   CString LockTitle2 = "                                                                               ";
   CString LockTitle3 = "                                                                               ";
   CString LockTitle4 = "                                                                               ";
   CString LockTitle5 = "                                                                               ";

   FILE *lStream;
   unsigned int pageNum;

   // Time
   char       HouseDate[20];
   struct tm *HouseTime;
   time_t     tmpTime = time(&tmpTime);
   HouseTime = localtime(&tmpTime);

   strftime(HouseDate, 80, "%m/%d/%Y - %H:%M", HouseTime);

   //m_JA_PrinterName = m_JA_PrinterSecondary;

   sprintf(buffer, "MINNESOTA HOUSE OF REPRESENTATIVES\n");
   for (i = 0, j = (70 - strlen(buffer)) / 2; i < strlen(buffer); i++) {
      LockTitle1.SetAt(j++, buffer[i]);
   }
   sprintf(buffer, "Chief Clerk's Office\n\n");
   for (i = 0, j = (70 - strlen(buffer)) / 2; i < strlen(buffer); i++) {
      LockTitle2.SetAt(j++, buffer[i]);
   }
   sprintf(buffer, "Date: %s\n\n", HouseDate);
   for (i = 0, j = (70 - strlen(buffer)) / 2; i < strlen(buffer); i++) {
      LockTitle3.SetAt(j++, buffer[i]);
   }
   sprintf(buffer, "SPEAKER: \n\n");
   for (i = 0; i < strlen(buffer); i++) {
      LockTitle4.SetAt(i, buffer[i]);
   }
   sprintf(buffer, "The Following %d members have requested that you excuse them for today:\n\n", lockCount);
   for (i = 0, j = 3; i < strlen(buffer); i++) {
      LockTitle5.SetAt(j++, buffer[i]);
   }

   if ((lStream = fopen(m_JA_PrinterName, "a+")) != NULL) {

      for (pageNum = 1; pageNum <= numPages; pageNum++) {

         sprintf(buffer, "\n\n\n\n");
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", LockTitle1.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", LockTitle2.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", LockTitle3.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", LockTitle4.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", LockTitle5.GetString());
         JaPrintLine(lStream, buffer);

         // print spacing line with vertical separators
         sprintf(buffer, "%s%s: %s%s: %s%s\n", null_column, null_name, null_column, null_name, null_column, null_name);
         JaPrintLine(lStream, buffer);

         // print the member's results
         for (i = 0; i < PRINTOUT_ROWS; i++) {
            for (j = 0; j < PRINTOUT_COLS; j++) {
               k = (j * PRINTOUT_ROWS) + i;
               if (m_JA_MemberSeatingList[k].seat && m_JA_StationLocks[m_JA_MemberSeatingList[k].seat - 1]) {
                  strncpy_s(LockName[j], m_JA_MemberSeatingList[k].name, NAME_PRINTABLE);
                  LockName[j][NAME_PRINTABLE] = '\0';
               }
               else {
                  strcpy_s(LockName[j], null_name);
               }
            }

            sprintf(buffer, "%s%s: %s%s: %s%s\n", null_column, LockName[0], null_column, LockName[1], null_column, LockName[2]);
            JaPrintLine(lStream, buffer);
         }

         fflush(lStream);
      }

      fclose(lStream);
   }
   else {
      //wsprintf(outBuff, "ERROR opening printer device %s!", m_JA_PrinterName);
      //JaUpdateStatusWindow(outBuff);
   }

   return;
}

void PrintRemotes(unsigned int numPages)
{
   char  RemoteName[3][20];
   unsigned int i, j, k;
   char buffer[BUFFER_LENGTH];

   CString RemoteTitle1 = "                                                                               ";
   CString RemoteTitle2 = "                                                                               ";
   CString RemoteTitle3 = "                                                                               ";
   CString RemoteTitle4 = "                                                                               ";
   CString RemoteTitle5 = "                                                                               ";

   FILE* lStream;
   unsigned int pageNum;

   // Time
   char       HouseDate[20];
   struct tm* HouseTime;
   time_t     tmpTime = time(&tmpTime);
   HouseTime = localtime(&tmpTime);

   strftime(HouseDate, 80, "%m/%d/%Y - %H:%M", HouseTime);

   // m_JA_PrinterName = m_JA_PrinterSecondary;

   sprintf(buffer, "MINNESOTA HOUSE OF REPRESENTATIVES\n");
   for (i = 0, j = (70 - strlen(buffer)) / 2; i < strlen(buffer); i++) {
      RemoteTitle1.SetAt(j++, buffer[i]);
   }
   sprintf(buffer, "Chief Clerk's Office\n\n");
   for (i = 0, j = (70 - strlen(buffer)) / 2; i < strlen(buffer); i++) {
      RemoteTitle2.SetAt(j++, buffer[i]);
   }
   sprintf(buffer, "Date: %s\n\n", HouseDate);
   for (i = 0, j = (70 - strlen(buffer)) / 2; i < strlen(buffer); i++) {
      RemoteTitle3.SetAt(j++, buffer[i]);
   }
   sprintf(buffer, "SPEAKER: \n\n");
   for (i = 0; i < strlen(buffer); i++) {
      RemoteTitle4.SetAt(i, buffer[i]);
   }
   sprintf(buffer, "The Following %d members have requested to vote remotely for today:\n\n", remoteCount);
   for (i = 0, j = 3; i < strlen(buffer); i++) {
      RemoteTitle5.SetAt(j++, buffer[i]);
   }

   if ((lStream = fopen(m_JA_PrinterName, "a+")) != NULL) {

      for (pageNum = 1; pageNum <= numPages; pageNum++) {

         sprintf(buffer, "\n\n\n\n");
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", RemoteTitle1.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", RemoteTitle2.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", RemoteTitle3.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", RemoteTitle4.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", RemoteTitle5.GetString());
         JaPrintLine(lStream, buffer);

         // print spacing line with vertical separators
         sprintf(buffer, "%s%s: %s%s: %s%s\n", null_column, null_name, null_column, null_name, null_column, null_name);
         JaPrintLine(lStream, buffer);

         // print the member's results
         for (i = 0; i < PRINTOUT_ROWS; i++) {
            for (j = 0; j < PRINTOUT_COLS; j++) {
               k = (j * PRINTOUT_ROWS) + i;
               if (m_JA_MemberSeatingList[k].seat && m_JA_RemoteVotes[m_JA_MemberSeatingList[k].seat - 1]) {
                  strncpy_s(RemoteName[j], m_JA_MemberSeatingList[k].name, NAME_PRINTABLE);
                  RemoteName[j][NAME_PRINTABLE] = '\0';
               }
               else {
                  strcpy_s(RemoteName[j], null_name);
               }
            }

            sprintf(buffer, "%s%s: %s%s: %s%s\n", null_column, RemoteName[0], null_column, RemoteName[1], null_column, RemoteName[2]);
            JaPrintLine(lStream, buffer);
         }

         fflush(lStream);
      }

      fclose(lStream);
   }
   else {
      //wsprintf(outBuff, "ERROR opening printer device %s!", m_JA_PrinterName);
      //JaUpdateStatusWindow(outBuff);
   }

   return;
}

void PrintCallIns(unsigned int numPages)
{
   char  CallInName[3][20];
   unsigned int i, j, k;
   char buffer[BUFFER_LENGTH];

   CString CallInTitle1 = "                                                                               ";
   CString CallInTitle2 = "                                                                               ";
   CString CallInTitle3 = "                                                                               ";
   CString CallInTitle4 = "                                                                               ";
   CString CallInTitle5 = "                                                                               ";

   FILE* lStream;
   unsigned int pageNum;

   // Time
   char       HouseDate[20];
   struct tm* HouseTime;
   time_t     tmpTime = time(&tmpTime);
   HouseTime = localtime(&tmpTime);

   strftime(HouseDate, 80, "%m/%d/%Y - %H:%M", HouseTime);

   // m_JA_PrinterName = m_JA_PrinterSecondary;

   sprintf(buffer, "MINNESOTA HOUSE OF REPRESENTATIVES\n");
   for (i = 0, j = (70 - strlen(buffer)) / 2; i < strlen(buffer); i++) {
      CallInTitle1.SetAt(j++, buffer[i]);
   }
   sprintf(buffer, "Chief Clerk's Office\n\n");
   for (i = 0, j = (70 - strlen(buffer)) / 2; i < strlen(buffer); i++) {
      CallInTitle2.SetAt(j++, buffer[i]);
   }
   sprintf(buffer, "Date: %s\n\n", HouseDate);
   for (i = 0, j = (70 - strlen(buffer)) / 2; i < strlen(buffer); i++) {
      CallInTitle3.SetAt(j++, buffer[i]);
   }
   sprintf(buffer, "SPEAKER: \n\n");
   for (i = 0; i < strlen(buffer); i++) {
      CallInTitle4.SetAt(i, buffer[i]);
   }
   sprintf(buffer, "The Following %d members have requested to vote by calling in for today:\n\n", callInCount);
   for (i = 0, j = 3; i < strlen(buffer); i++) {
      CallInTitle5.SetAt(j++, buffer[i]);
   }

   if ((lStream = fopen(m_JA_PrinterName, "a+")) != NULL) {

      for (pageNum = 1; pageNum <= numPages; pageNum++) {

         sprintf(buffer, "\n\n\n\n");
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", CallInTitle1.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", CallInTitle2.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", CallInTitle3.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", CallInTitle4.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", CallInTitle5.GetString());
         JaPrintLine(lStream, buffer);

         // print spacing line with vertical separators
         sprintf(buffer, "%s%s: %s%s: %s%s\n", null_column, null_name, null_column, null_name, null_column, null_name);
         JaPrintLine(lStream, buffer);

         // print the member's results
         for (i = 0; i < PRINTOUT_ROWS; i++) {
            for (j = 0; j < PRINTOUT_COLS; j++) {
               k = (j * PRINTOUT_ROWS) + i;
               if (m_JA_MemberSeatingList[k].seat && m_JA_CallInVotes[m_JA_MemberSeatingList[k].seat - 1]) {
                  strncpy_s(CallInName[j], m_JA_MemberSeatingList[k].name, NAME_PRINTABLE);
                  CallInName[j][NAME_PRINTABLE] = '\0';
               }
               else {
                  strcpy_s(CallInName[j], null_name);
               }
            }

            sprintf(buffer, "%s%s: %s%s: %s%s\n", null_column, CallInName[0], null_column, CallInName[1], null_column, CallInName[2]);
            JaPrintLine(lStream, buffer);
         }

         fflush(lStream);
      }

      fclose(lStream);
   }
   else {
      //wsprintf(outBuff, "ERROR opening printer device %s!", m_JA_PrinterName);
      //JaUpdateStatusWindow(outBuff);
   }

   return;
}