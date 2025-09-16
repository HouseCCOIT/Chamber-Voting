// VMThread.cpp : implementation file
//

#include "stdafx.h"
#include "JaVM.h"
#include "JaVMDlg.h"
#include "JaOpto.h"
#include "VMThread.h"
#include "server32.h"
#include "trace.h"
#include "alphasort.h"
#include "RemoteSocketServer.h"
#include "RemoteSocketClient.h"
#include "VideoSocketServer.h"
#include "VideoSocketClient.h"

#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "gpioctl.h"        // This defines the IOCTL constants.
// #include "house.h"

#include <winioctl.h>

using namespace System;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char station_lock[MAX_VOTE_ARRAY];   // Who is not allowed to vote. X != 0: bypass this vote station in polling sequence
char remoteVotes[MAX_VOTE_ARRAY];    // Who's scheduled to vote remotely
char remoteActive[MAX_VOTE_ARRAY];   // Who's actively voting remotely
char callInVotes[MAX_VOTE_ARRAY];    // Who's scheduled to vote by call in
char pageButtons[MAX_VOTE_ARRAY];    // Who's pressing the Page button
char debateQueue[MAX_VOTE_ARRAY];    // Who's in the Debate Queue
BOOL gDebateQueueChanged = FALSE;

short system_test;                       // suppresses tx to editing machine and sequence incrementation
short count_sync_warn;                   // indicates that a vote counter synchronization problem has been reported

unsigned char absent_total = 134;
unsigned char no_total, yes_total;
unsigned char oldVote[MAX_VOTE_ARRAY];

unsigned char boardMap[MAX_SEATS];           // vectors absolute vote station numbers to display board locations

unsigned char totalAbsent, totalYes, totalNo = 0;

// STUFF TO HANDLE THE PRINTER FUNCTIONS

char ht_date[] = "DATE - ";
char ht_sequence[] = "SEQUENCE NO. - ";

char hi_date[40];
long hi_sequence;
char hi_file[64];
char hi_order[64];
char hi_action[64];
char hi_line7[64];

static char mister_speaker[] = "Spk. ";
static char* journal_report[2] = { "The question was taken on the _______________________________________\n",
"and the roll was called.\n\n" };
static char* passage_report[2] = { "The bill was read for the third time and placed upon its final passage.\n\n",
"The question was taken on the passage of the bill and the roll was\n    called.  " };
static char* repassage_report[2] = { "The bill was read for the third time and placed upon its repassage.\n\n",
"The question was taken on the repassage of the bill and the roll was\n    called.  " };
static char yea_nay_format[] = "There were %d %s and %d %s as follows:\n\n";
static char* yea_token[2] = { "yeas", "yea" };
static char* nay_token[2] = { "nays", "nay" };
static char yea_column[] = "Y      ";
static char nay_column[] = "  N    ";
static char absent_column[] = "    O  ";
/* static char *column_header[2] = { "    N  ", "Y N V  " }; */
static char null_column[] = "       ";          // 7
static char null_name[] = "                 ";  // 17
static char member_column[] = "MEMBER           ";


// The following parameter is used in the IOCTL call
HANDLE  g_hndFile = NULL;   // Handle to device, obtain from CreateFile

CString m_OriginalVoteDir = "V:\\VOTES\\";
CString m_StevesVoteDir = "S:\\VOTES\\";

//CString StationLockFile = "V:\\JaLock.dat";
//CString RemoteVotesFile = "V:\\JaRemoteVotes.dat";
//CString CallInVotesFile = "V:\\JaCallInVotes.dat";

CString JaVoteDat = "V:\\JaVote.dat";
CString JaLockDat = "V:\\JaLock.dat";
CString JaDispDat = "V:\\JaDisp.dat";
CString JaDQDat = "V:\\JaDQ.dat";
CString JaRADat = "V:\\JaRA.dat";
CString JaVoteIni = "V:\\JaVote.Ini";
CString JaSequence = "V:\\VOTES\\SEQUENCE.VOT";
CString JaSequenceFile = "V:\\VOTES\\SEQUENCE.VOT";
CString JaSequenceBack = "S:\\VOTES\\SEQUENCE.VOT";

// This is the printers Network Name
CString    m_JA_PrinterPrimary = "\\\\vote\\Printer1";
CString    m_JA_PrinterSecondary = "\\\\vote\\Printer2";
//CString    m_JA_PrinterPrimary = "\\\\EAKDEV2022\\Brother HL-L3280CDW series";
//CString    m_JA_PrinterSecondary = "\\\\EAKDEV2022\\Brother HL-L3280CDW series";
CString    m_JA_PrinterName = "LPT1:";
CString    m_JA_VPTitle1 = "                                                                        ";
CString    m_JA_VPTitle2 = "                                                                        ";
CString    m_JA_VPTitle3 = "                                                                        ";
UINT       m_JA_SessionNumber;
UINT       m_JA_SequenceNumber;
CString    m_JA_SeatingDate;

LEX        m_JA_MemberSeatingList[MAX_SEATS];
struct tm* m_JA_HouseTime;

char outBuff[255];

extern BOOL m_TestMode;
extern BOOL m_Delay;
extern BOOL m_JA_VotingMachineActive;

BOOL m_VoteOpen = false;
CTime m_VoteOpenTime;

short vote_in_progress = 1;              // flag that drives the polling loop TBD - should be bool

CJaVMDlg* m_JA_VMDialog;

CWinThread* WriteThread;

gcroot<JaOpto^> opto;

bool killWriteThread = false;
bool gWriteTheVote = false;

void JaUpdateStatusWindow(char* message)
{
   m_JA_VMDialog->GetDlgItem(IDC_EDIT_STATUS_WINDOW)->SetWindowText(message);

   Sleep(100);
}


void DisplayVote(unsigned char seat, unsigned char vote)
{
   if ((vote != oldVote[seat]) && (vote != 4)) {

      // Turn on/off the Desk Lights
      opto->voteDeskLights(seat, vote);

      // Handle transmission to display board
      opto->voteBoardLights(boardMap[seat], vote, seat);

      m_JA_MemberSeatingList[boardMap[seat]].vote = vote;

      // Memorize vote station state for next time around 
      oldVote[seat] = vote;
   }
}

void DisplayVoteRemote(unsigned char seat, unsigned char vote)
{
   if ((vote != oldVote[seat]) && (vote != 4)) {

      // Turn on/off the Desk Lights
      opto->voteDeskLightsRemote(seat, vote);

      // Handle transmission to display board
      opto->voteBoardLightsRemote(boardMap[seat], vote, seat);

      m_JA_MemberSeatingList[boardMap[seat]].vote = vote;

      // Memorize vote station state for next time around 
      oldVote[seat] = vote;
   }
}


int vote(JaOpto^ opto)
{
   long loop;

   unsigned char vote_in = 0;
   unsigned char yes_check, no_check, absent_check;
   unsigned char check_cycle = 0;
   unsigned char vote_lock = 0;
   unsigned char vote_open = 0;

   //short vote_in_progress = 1;              // flag that drives the polling loop TBD - should be bool

   unsigned char voteIn = 0;
   unsigned short redrawNow = 0;

   DWORD lastTickCount, currTickCount;
   bool motionOn = false;

    vote_in_progress = 1;              // flag that drives the polling loop TBD - should be bool

   /* reset counter synchronization error warning flag */
   count_sync_warn = 0;

   // Clear all latches before vote opens
   for (int i = 0; i < MAX_SEATS; i++) { // 136 desks
      if (i == 0 || i == 21 || i == 42 || i == 63 || i == 84 || i == 105 || i == 126) {
         opto->readSwitchLatches(i / 21);
      }
   }

   while (m_JA_VotingMachineActive) {
      while (vote_in_progress) {

         //TRACE("Before For")
         for (int i = 0; i < MAX_SEATS; i++) { // 136 desks

            // Reset output information 
            voteIn = oldVote[i];

            if (i == 0 || i == 21 || i == 42 || i == 63 || i == 84 || i == 105 || i == 126) {
               opto->readSwitchLatches(i / 21);
               //TRACE("After readSwitchLatches")
               //Test Sleep(5);
            }

            // If station locked, skip it, and continue to next station
            if (station_lock[i]) {
               continue;
            }

            if (remoteVotes[i] || callInVotes[i]) {
               // Get the vote for this module from Socket
               voteIn = m_JA_MemberSeatingList[boardMap[i]].vote;
               // TBD - 02132021 - Reconcile this with setting in ClientSocket.cpp
               //gWriteTheVote = TRUE;
            }
            else {
               // Get the vote for this module from Opto
               voteIn = opto->voteInput(i);
            }

            // Calculate total values and change flags for LED display
            if (voteIn == 0) {
               if (oldVote[i] == 2) {
                  yes_total--;
                  absent_total++;
                  gWriteTheVote = TRUE;
               }
               else if (oldVote[i] == 1) {
                  no_total--;
                  absent_total++;
                  gWriteTheVote = TRUE;
               }
            }
            else if (voteIn == 2) {
               if (oldVote[i] == 1) {
                  no_total--;
                  yes_total++;
                  gWriteTheVote = TRUE;
               }
               else if (oldVote[i] != 2) {
                  absent_total--;
                  yes_total++;
                  gWriteTheVote = TRUE;
               }
            }
            else if (voteIn == 1) {
               if (oldVote[i] == 2) {
                  yes_total--;
                  no_total++;
                  gWriteTheVote = TRUE;
               }
               else if (oldVote[i] != 1) {
                  absent_total--;
                  no_total++;
                  gWriteTheVote = TRUE;
               }
            }

            if (remoteVotes[i] || callInVotes[i]) {
               DisplayVoteRemote(i, voteIn);  // Turn on lights at Desk and Display Board
            }
            else {
               DisplayVote(i, voteIn); // Turn on lights at Desk and Display Board
            }
         }

         //TRACE(" After For");

         if (m_Delay) {
            Sleep(m_Delay);
         }

         /* after we have examined every strobe line, display new vote totals on led displays */

         // MODIFICATION: Portlas on 22 Feb 88
         // If a vote station card is sending faulty information, the program may increment
         // the total counters excessively.  the following code checks the member structure
         // array itself and compares the vote count to the totals accumulated during the
         // iterations.  Because of doubts surrounding real-time efficiency, the check will be
         // performed every 20 vote cycles

         //if (++check_cycle == 20) {
         yes_check = no_check = absent_check = 0;

         for (loop = 0; loop < (MAX_SEATS - 2); loop++) {
            if (m_JA_MemberSeatingList[loop].seat != 69 && m_JA_MemberSeatingList[loop].seat != 91) {
               if (m_JA_MemberSeatingList[loop].vote == 2) {
                  yes_check++;
               }
               else if (m_JA_MemberSeatingList[loop].vote == 1) {
                  no_check++;
               }
               else if (m_JA_MemberSeatingList[loop].vote == 0) {
                  absent_check++;
               }
            }
            // TBD - Remove?
            if (m_JA_VotingMachineActive) {
               m_JA_VMDialog->SetMemberVote(loop, m_JA_MemberSeatingList[loop].seat, m_JA_MemberSeatingList[loop].vote);
            }
         }

         //TRACE(" Before Write");

         //Sleep(2000);

         // Signal the write thread to write the vote
         //gWriteTheVote = TRUE;

         //TRACE(" After Write");

         if (yes_check != yes_total || no_check != no_total || absent_check != absent_total) {
            if (!count_sync_warn) {
               //wsprintf(outBuff, "WARNING: Vote counts did not match member tally, possible H/W error");
               TRACE(outBuff);
               //JaUpdateStatusWindow(outBuff);
               count_sync_warn = 1;
            }

            yes_total = yes_check;
            no_total = no_check;
            absent_total = absent_check;
         }

         check_cycle = 0;

         //TRACE("Before Lock Check");

         vote_lock = opto->voteLockCheck();

         //TRACE("After Lock Check");

         if (vote_lock) {
            vote_in_progress = 0;
         }

         //TRACE("Before Motion Switches");

         // Update motion switches and toggle lights
         currTickCount = GetTickCount();
         if (currTickCount - lastTickCount > 1000) {
            opto->getMotionSwitches();
            lastTickCount = currTickCount;
            motionOn = !motionOn;
            opto->setMotionLights(motionOn);
         }

        // TRACE("After Motion Switches");

         //if (redrawNow++ % 10) {
         //   m_JA_VMDialog->RedrawWindow();
         //}

      } // while (vote_in_progress)

      // Update motion switches and toggle lights
      currTickCount = GetTickCount();
      if (currTickCount - lastTickCount > 1000) {
         opto->getMotionSwitches();
         lastTickCount = currTickCount;
         motionOn = !motionOn;
         opto->setMotionLights(motionOn);
      }

      // Signal the write thread to write the vote
      gWriteTheVote = TRUE;

      //TBD - Remove?
      if (m_JA_VotingMachineActive) {
         for (int i = 0; i < (MAX_SEATS - 3); i++) {
            m_JA_VMDialog->SetMemberVote(i, m_JA_MemberSeatingList[i].seat, m_JA_MemberSeatingList[i].vote);
         }
      }
      // vote open check
      vote_in_progress = opto->voteOpenCheck(false);

      // vote end check
      if (vote_in = opto->voteCloseCheck()) {
         return(vote_in);
      }

   } // end while (m_JA_VotingMachineActive)

   return vote_in;
}

void initialize(void)
{
   unsigned short loop;

   if (m_TestMode) {
      return;
   }

   /* initialize old_vote: corresponds to all data lines high */
   for (loop = 0; loop < MAX_VOTE_ARRAY; loop++) {
      oldVote[loop] = 0;
   }

   // reset the votes for all of the members
   for (loop = 0; loop < MAX_SEATS; loop++) {
      m_JA_MemberSeatingList[loop].vote = 0;
      m_JA_VMDialog->SetMemberVote(loop, m_JA_MemberSeatingList[loop].seat, m_JA_MemberSeatingList[loop].vote);
   }

   // clear all the station locks
   for (loop = 0; loop < MAX_SEATS; loop++) {
      station_lock[loop] = 0;
   }

   // clear all the remote votes
   for (loop = 0; loop < MAX_SEATS; loop++) {
      remoteVotes[loop] = 0;
   }

   // clear all the call in votes
   for (loop = 0; loop < MAX_SEATS; loop++) {
      callInVotes[loop] = 0;
   }

   // finally, reset the led total accumulators and send them to led displays
   yes_total = 0;
   no_total = 0;
   absent_total = 134;
}

void JaPrintLine(FILE* stream, char* s)
{
   fprintf(stream, "    %s", s);
}

void JaPrintVoteResults(unsigned int numPages)
{
   char* vote_result[3], * vote_name[3];
   int i, j, k;
   char buffer[BUFFER_LENGTH];

   FILE* lStream;
   unsigned int pageNum;

   if ((lStream = fopen(m_JA_PrinterName, "a+")) != NULL) {

      for (pageNum = 1; pageNum <= numPages; pageNum++) {

         sprintf(buffer, "%s\n", m_JA_VPTitle1.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", m_JA_VPTitle2.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "%s\n", m_JA_VPTitle3.GetString());
         JaPrintLine(lStream, buffer);

         sprintf(buffer, "\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
         JaPrintLine(lStream, buffer);

         // first the header stuff
         sprintf(buffer, "%-51.51s%s%s\n", hi_file, ht_date, hi_date);
         JaPrintLine(lStream, buffer);
         sprintf(buffer, "%-51.51s%s%s%ld\n", hi_order, ht_sequence, system_test ? "T" : "", hi_sequence);
         JaPrintLine(lStream, buffer);
         sprintf(buffer, "%s\n", hi_action);
         JaPrintLine(lStream, buffer);
         sprintf(buffer, "%s\n\n\n", hi_line7);
         JaPrintLine(lStream, buffer);

         // kick out the lead-in material
         for (i = 0; i < 2; i++) {
            if (!_strnicmp(hi_action, "PASSAGE", 7)) {
               sprintf(buffer, "%s", passage_report[i]);
            }
            else if (!_strnicmp(hi_action, "REPASSAGE", 9)) {
               sprintf(buffer, "%s", repassage_report[i]);
            }
            else {
               sprintf(buffer, "%s", journal_report[i]);
            }
            JaPrintLine(lStream, buffer);
         }

         sprintf(buffer, yea_nay_format, yes_total, yea_token[yes_total == 1], no_total, nay_token[no_total == 1]);
         JaPrintLine(lStream, buffer);

         // print a horizontal double line (equal signs)
         for (i = 0; i < 71; i++) {
            buffer[i] = '=';
         }
         buffer[i] = '\0';

         sprintf(buffer, "%s\n", buffer);
         JaPrintLine(lStream, buffer);

         // print spacing line with vertical separators
         sprintf(buffer, "%s%s: %s%s: %s%s\n", null_column, null_name, null_column, null_name, null_column, null_name);
         JaPrintLine(lStream, buffer);

         // print the member's results
         for (i = 0; i < PRINTOUT_ROWS; i++) {
            for (j = 0; j < PRINTOUT_COLS; j++) {
               k = (j * PRINTOUT_ROWS) + i;
               if (m_JA_MemberSeatingList[k].seat) {
                  vote_name[j] = m_JA_MemberSeatingList[k].name;
                  vote_result[j] = (m_JA_MemberSeatingList[k].vote == VOTE_ABSENT) ? absent_column
                     : ((m_JA_MemberSeatingList[k].vote == VOTE_NO) ? nay_column : yea_column);
               }
               else {
                  vote_name[j] = null_name;
                  vote_result[j] = null_column;
               }
            }

            sprintf(buffer, "%s%s: %s%s: %s%s\n", vote_result[0], vote_name[0], vote_result[1], vote_name[1], vote_result[2], vote_name[2]);
            JaPrintLine(lStream, buffer);
         }

         // Print the line number of the page
         sprintf(buffer, "\n                                  %d\n", pageNum);
         JaPrintLine(lStream, buffer);
         buffer[0] = 0x0c;   // Form Feed
         buffer[1] = 0x00;   // Terminate String
         fprintf(lStream, "%s", buffer);

         fflush(lStream);
      }

      fclose(lStream);
   }
   else {
      wsprintf(outBuff, "ERROR opening printer device %s!", m_JA_PrinterName);
      JaUpdateStatusWindow(outBuff);
   }

   return;
}


void JaInitializeHeaderParms(void)
{
   char outBuff[255];

   char seqFilename[] = "V:\\VOTES\\SEQUENCE.VOT";
   FILE* seqHandle;

   if (!(seqHandle = fopen(seqFilename, "r"))) {
      DWORD error = GetLastError();
      wsprintf(outBuff, "JaLoadSequenceNumber - Unable to open %s! - %d", seqFilename, error);
      AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
      return;
   }

   fscanf(seqHandle, "%ld", &(hi_sequence));

   fclose(seqHandle);

   // Time

   time_t     tmpTime = time(&tmpTime);
   m_JA_HouseTime = localtime(&tmpTime);

   strftime(hi_date, 40, "%m-%d-%Y", m_JA_HouseTime);
}

void JaSetCurrentTime(void)
{
   time_t     tmpTime = time(&tmpTime);
   m_JA_HouseTime = localtime(&tmpTime);

   strftime(hi_date, 40, "%m-%d-%Y", m_JA_HouseTime);
}

void JaUpdateSequence()
{
   char outBuff[255];

   FILE* seqHandle;

   // Save sequence number

   seqHandle = fopen(JaSequenceFile, "w+");
   if (seqHandle == NULL) {
      wsprintf(outBuff, "JaUpdateSequenceNumber - Unable to open %s!", JaSequenceFile);
      AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
      return;
   }

   fscanf(seqHandle, "%ld", &(hi_sequence));

   ++hi_sequence;
   fprintf(seqHandle, "%ld", hi_sequence);

   fclose(seqHandle);

   // Make a Backup

   if (!(seqHandle = fopen(JaSequenceBack, "w"))) {
      wsprintf(outBuff, "JaUpdateSequenceNumber - Unable to open %s!", JaSequenceBack);
      AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
   }
   else {
      fprintf(seqHandle, "%ld", hi_sequence);
      fclose(seqHandle);
   }

   return;
}

void JaWriteRollCall(CString rollFilename, BOOL backup)
{
   char rollBuff[256];
   unsigned int i;
   FILE* rollFile;

   char tmpDate[80];

   time_t tmpTime = time(&tmpTime);
   m_JA_HouseTime = localtime(&tmpTime);

   strftime(tmpDate, 80, "%Y%m%d", m_JA_HouseTime);

   // Build control record
   sprintf(rollBuff, "%1.1d%04.4ld%8.8s%8.8s%-53s%-53s%-53s%-32s\n",
      0,                           // record type
      hi_sequence,                 // sequence #
      m_JA_SeatingDate.GetString(),            // will be driven from parm file
      tmpDate,
      hi_order,
      hi_file,
      hi_action,
      hi_line7);

   // now, whack out unwanted linefeeds
   for (i = 0; i < strlen(rollBuff) - 2; i++) {
      if (rollBuff[i] == '\n') {
         rollBuff[i] = ' ';
      }
   }

   if (!(rollFile = fopen(rollFilename, "wt"))) {
      if (!backup) {
         wsprintf(outBuff, "Unable to open roll call file <%s>!", rollFilename);
         AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
      }
      return;
   }

   // Write control record
   fputs(rollBuff, rollFile);

   // Build roll call record
   rollBuff[0] = '1';        // record type

                             // Get individual vote information
   for (i = 1; i < (MAX_SEATS + 1); i++) {
      rollBuff[i] = m_JA_MemberSeatingList[boardMap[i - 1]].vote + '0';
   }

   // Add padding
   for (i = 137; i < 141; i++) {
      rollBuff[i] = ' ';
   }

   rollBuff[141] = '\n';
   rollBuff[142] = EOS;

   // Write individual vote information
   fputs(rollBuff, rollFile);

   fclose(rollFile);
}

void JaWriteEmptyVote(CString voteFilename)
{
   FILE* voteFile;
   int count;
   unsigned char votes[MAX_VOTE_ARRAY];

   for (int i = 0; i < sizeof(votes) - 2; i++) {
      votes[i] = 0;
   }
   votes[136] = 0;
   votes[137] = 0;

   if (!(voteFile = fopen(voteFilename, "wb"))) {
      return;
   }

   count = fwrite(votes, 1, sizeof(votes), voteFile);

   fclose(voteFile);
}

void JaWriteCurrentVote(CString voteFilename)
{
   FILE* voteFile;
   int count;
   unsigned char votes[MAX_VOTE_ARRAY];
   static unsigned char votesLast[MAX_VOTE_ARRAY];
   bool votesChanged = false;

   for (int i = 0; i < MAX_SEATS; i++) {
      // Sergeant at arms does not vote (i = seat - 1 )
      if ((i != 68) && (i != 90)) {  // 69 && 91 
         votes[i] = m_JA_MemberSeatingList[boardMap[i]].vote;
         if (votes[i] != votesLast[i]) {
            votesChanged = true;
         }
      }
      else {
         votes[i] = 0;
      }
      votesLast[i] = votes[i];
   }
   votes[136] = yes_total;
   votes[137] = no_total;

   //TRACE("Before Open");

   voteFile = fopen(voteFilename, "wb");

   if (voteFile != NULL) {
      count = fwrite(votes, 1, sizeof(votes), voteFile);

      if (count != sizeof(votes)) {
         TRACE("Unable to write Votes file!");
         return;
      }

      fflush(voteFile);
      fclose(voteFile);
   }
   else {
      TRACE("Unable to open Votes file");
   }
}

void JaSendCurrentVote(void)
{
  // unsigned char votes[MAX_VOTE_ARRAY];
   static unsigned char votesLast[MAX_VOTE_ARRAY];
   bool votesChanged = false;
   SockPacket VMPacket;

   for (int i = 0; i < MAX_SEATS; i++) {
      // Sergeant at arms does not vote (i = seat - 1 )
      if ((i != 68) && (i != 90)) {  // 69 && 91 
         VMPacket.dataByte[i] = m_JA_MemberSeatingList[boardMap[i]].vote;
         if (VMPacket.dataByte[i] != votesLast[i]) {
            votesChanged = true;
         }
      }
      else {
         VMPacket.dataByte[i] = 0;
      }
      votesLast[i] = VMPacket.dataByte[i];
   }
   VMPacket.dataByte[136] = yes_total;
   VMPacket.dataByte[137] = no_total;

   //TRACE("Before Send Current Vote");

   // Show Results

   VMPacket.packetType = VM_VM;
   VMPacket.VMAction = VM_VOTE_CURRENT_VOTE;
   m_JA_VMDialog->SendCurrentVoteMsg(&VMPacket);

}

//void JaWriteCurrentLocks(CString lockFilename)
//{
//   FILE* lockFile;
//   int count;
//
//   if (!(lockFile = fopen(lockFilename, "wb"))) {
//      return;
//   }
//
//   count = fwrite(station_lock, 1, sizeof(station_lock), lockFile);
//
//   fclose(lockFile);
//}

void JaWriteCurrentDisp(CString dispFilename)
{
   FILE* dispFile;

   int i;

   if (!(dispFile = fopen(dispFilename, "w+"))) {
      return;
   }

   fprintf(dispFile, "%s\n", "Lucida Console");

   fprintf(dispFile, "%d\n", 2);

   for (i = 0; i < 8; i++) {
      fprintf(dispFile, "%s\n", " ");
   }

   for (i = 0; i < 4; i++) {
      fprintf(dispFile, "%s\n", " ");
   }

   fclose(dispFile);
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
   FILE* tmpFile;

   char section[64];
   char key[64];
   char inBuff[200];

   unsigned int i, j, seat;

   // Does JaVote.Ini file exist?
   tmpFile = fopen("V:\\JaVote.Ini", "r+");
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
            strncpy_s(m_JA_MemberSeatingList[k].name, inBuff, 30);
         }
         m_JA_MemberSeatingList[k].seat = seat;
         m_JA_MemberSeatingList[k].seatID = seat + 2000; // Window Id
         k++;
      }
   }
   m_JA_MemberSeatingList[k].seat = 0;

   setlocale(LC_COLLATE, "en-029"); // set locale to english-carribean

   qsort((char*)m_JA_MemberSeatingList, (MAX_SEATS - 3), sizeof(LEX), CompareMembers);

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

bool killMainThread = false;

void KillVM(void)
{
   killMainThread = true;
}

UINT JaVMThreadFunction(LPVOID DlgObj)
{
   int system_ready = 0;

   m_JA_VMDialog = (CJaVMDlg*)DlgObj;    // Initialize global pointer to VM Dialog

   // Spawn a separate thread to write the vote file asynchronously
   WriteThread = AfxBeginThread(JaWriteThreadFunction, DlgObj);

   DWORD lastTickCount, currTickCount;
   bool motionOn = false;

   //HANDLE VMMB_Pipe;
   //HANDLE VMVideo_Pipe;
   //VM_Packet packet;

   SockVMPacket remotePacket;
   SockPacket   mbPacket;
   SockPacket   videoPacket;

   TRACE("Enter");

   JaWriteEmptyVote(JaVoteDat);
//   JaWriteCurrentLocks(JaLockDat);
   JaWriteCurrentDisp(JaDispDat);

   try {
      opto = gcnew JaOpto;
   }
   catch (int card) {
      // CString except = e->ToString();
      CString except = "Error opening Opto card " + card.ToString();

      AfxMessageBox(except, MB_ICONINFORMATION | MB_OK | MB_APPLMODAL);
      exit(0);
   }
   //catch (...) {
   //   CString except = "Error opening Opto card"; // opto->getException(); // e->ToString();
   //   AfxMessageBox(except, MB_ICONINFORMATION | MB_OK | MB_APPLMODAL);
   //   exit(0);
   //}

   // Clear and initialize system components
   initialize();
   opto->Initialize();

   // Clear the debate queue on Startup
   for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
      debateQueue[i] = 0;
   }
   gDebateQueueChanged = true;

   yes_total = 0;
   no_total = 0;
   absent_total = 134;

   system_ready = 1;

   JaInitializeHeaderParms(); // Set up time and sequence stuff

   if (opto->displayOn()) {
   }

   //while (!opto->voteOn()) {
   //   wsprintf(outBuff, "WAITING FOR SYSTEM TO TURN ON...");
   //   JaUpdateStatusWindow(outBuff);
   //   Sleep(500);
   //}

   Sleep(500);

   wsprintf(outBuff, "SYSTEM IS ACTIVE...");
   JaUpdateStatusWindow(outBuff);

   while (!killMainThread) {

      wsprintf(outBuff, "WAITING FOR CONNECTION TO MESSAGE BOARD...");
      JaUpdateStatusWindow(outBuff);

      wsprintf(outBuff, "WAITING FOR CONNECTION TO VIDEO DISPLAY...");
      JaUpdateStatusWindow(outBuff);

      // Turn on end displays
      Sleep(500);

      if (m_TestMode) {
         wsprintf(outBuff, "TEST MODE:  ELECTRONIC VOTING NOW ENABLED.");
         JaUpdateStatusWindow(outBuff);
      }
      else {
         // Let them know it is happening
         wsprintf(outBuff, "MINNESOTA HOUSE OF REPRESENTATIVES:  ELECTRONIC VOTING NOW ENABLED.");
         JaUpdateStatusWindow(outBuff);
         Sleep(500);
         wsprintf(outBuff, "THE SEQUENCE COUNTER NOW READS: %d.", hi_sequence);
         JaUpdateStatusWindow(outBuff);

      }

      m_JA_VotingMachineActive = TRUE;

      while (m_JA_VotingMachineActive) {
         //killMainThread = true;

         // check for vote open signal
         if (system_ready && opto->voteOpenCheck(true)) {

            m_VoteOpen = true;
            m_VoteOpenTime = CTime::GetCurrentTime();

            // Clear the debate queue on Vote Open
            for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
               debateQueue[i] = 0;
            }
            gDebateQueueChanged = true;

            // Open the Remote Vote Stations
            remotePacket.packetType = VM_VM;
            remotePacket.VMAction = VM_VOTE_OPEN_REMOTE;
            m_JA_VMDialog->SendSockRemoteMsg(&remotePacket);

            JaSetCurrentTime();  // Set current time for printing

            wsprintf(outBuff, "THE VOTE IS NOW OPEN");
            JaUpdateStatusWindow(outBuff);

            // Freeze the message display transmission mechanisms
            mbPacket.packetType = VM_MB;
            mbPacket.MBAction = VM_MB_VOTE_ACTIVE;
            m_JA_VMDialog->SendMBMsg(&mbPacket);

            wsprintf(outBuff, "OPENING VOTE: SEQUENCE #%ld", hi_sequence);
            JaUpdateStatusWindow(outBuff);

            // Show RealTime
            videoPacket.packetType = VM_VIDEO;
            videoPacket.VideoAction = VM_VIDEO_SHOW_REALTIME;
            m_JA_VMDialog->SendVideoMsg(&videoPacket);

            switch (vote(opto)) {                       /***WILL GET RETURN CODE***/
            case PRINT_RESULT:
               wsprintf(outBuff, "PRINTING THE RESULTS");
               JaUpdateStatusWindow(outBuff);

               // Send the totals to communications central (MB) for message display
               mbPacket.packetType  = VM_MB;
               mbPacket.MBAction    = VM_MB_VOTE_RESULTS;
               mbPacket.dataByte[0] = yes_total;
               mbPacket.dataByte[1] = no_total;
               m_JA_VMDialog->SendMBMsg(&mbPacket);

               // Pause briefly to let the MB app send the Header
               Sleep(100);

               if (!m_TestMode) {
                  m_JA_PrinterName = m_JA_PrinterPrimary;
                  JaPrintVoteResults(2);   // print 2 pages

                  m_JA_PrinterName = m_JA_PrinterSecondary;
                  JaPrintVoteResults(2);   // print 2 pages
               }

               if (!system_test) {
                  char filename[32];
                  wsprintf(filename, "H%02.2d%04.4d.VOT", m_JA_SessionNumber, hi_sequence);

                  CString tmpFileName = m_OriginalVoteDir + filename;
                  JaWriteRollCall(tmpFileName, FALSE);

                  tmpFileName = m_StevesVoteDir + filename;
                  JaWriteRollCall(tmpFileName, TRUE);
               }
               else {
                  wsprintf(outBuff, "TEST ROLL CALL... NO DATA GENERATED");
                  JaUpdateStatusWindow(outBuff);
               }

               //Sleep(2000);   // IMPOSE PATIENCE ON THE SYSTEM

               // Show Results
               videoPacket.packetType = VM_VIDEO;
               videoPacket.VideoAction = VM_VIDEO_SHOW_RESULTS;
               m_JA_VMDialog->SendVideoMsg(&videoPacket);

               //Sleep(2000);

               wsprintf(outBuff, "THERE IS ROOM FOR %d VOTE FILES ON THIS SYSTEM.", 99999);
               JaUpdateStatusWindow(outBuff);

               break;

            case TOTALS_ONLY:
               wsprintf(outBuff, "JUST ON THE MESSAGE BOARD");
               JaUpdateStatusWindow(outBuff);

               // Send the totals to communications central (MB) for message display
               mbPacket.packetType = VM_MB;
               mbPacket.MBAction = VM_MB_VOTE_RESULTS;
               mbPacket.dataByte[0] = yes_total;
               mbPacket.dataByte[1] = no_total;
               m_JA_VMDialog->SendMBMsg(&mbPacket);

               // Pause briefly to let the MB app send the Header
               Sleep(100);

               //Sleep(5000);   // IMPOSE PATIENCE ON THE SYSTEM

               // Show Results
               videoPacket.packetType = VM_VIDEO;
               videoPacket.VideoAction = VM_VIDEO_SHOW_RESULTS;
               m_JA_VMDialog->SendVideoMsg(&videoPacket);

               //Sleep(2000);

               break;

            default:
               Sleep(1000);
               break;
            }
            m_VoteOpen = false;

            system_ready = 0;
         }
         else if (!system_ready && opto->voteResetCheck()) {
            initialize();

            m_JA_VMDialog->JaReadCallIns();  // zeroed out by initialize
            m_JA_VMDialog->JaReadRemotes();

            opto->Initialize();

            opto->collect(); // Force Garbage Collection in managed code.

            Sleep(10);

            // Zero out the vote. This also causes the displays to reset
            for (int i = 0; i < (MAX_SEATS - 3); i++) {     // TBD - (MAX_SEATS - 2) ?
               m_JA_VMDialog->SetMemberVote(i, m_JA_MemberSeatingList[i].seat, 0);
               m_JA_MemberSeatingList[i].vote = 0;
            }

            // Clear the debate queue on Reset
            for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
               debateQueue[i] = 0;
            }
            gDebateQueueChanged = true;

            yes_total = 0;
            no_total = 0;
            absent_total = 134;

            // Signal the write thread to write the vote
            gWriteTheVote = TRUE;

            if (!system_test) {
               JaUpdateSequence();
            }

            // Eliminate vote information from the message_display
            // Thaw the message display transmission mechanisms
            mbPacket.packetType = VM_MB;
            mbPacket.MBAction = VM_MB_VOTE_OPEN;
            m_JA_VMDialog->SendMBMsg(&mbPacket);
            CTime  resetTime = CTime::GetCurrentTime();

            __time64_t elapsedTime = resetTime.GetTime() - m_VoteOpenTime.GetTime();
            if (elapsedTime < 35) {
               Sleep((35 - elapsedTime) * 1000);
            }

            // Reset the Remote Vote Stations
            remotePacket.packetType = VM_VM;
            remotePacket.VMAction = VM_VOTE_RESET_REMOTE;
            m_JA_VMDialog->SendSockRemoteMsg(&remotePacket);

            system_ready = 1;
         }
         else if (!system_ready && opto->votePrintCheck()) {
            if (!m_TestMode) {
               m_JA_PrinterName = m_JA_PrinterPrimary;
               JaPrintVoteResults(1);   // print 1 pages

               m_JA_PrinterName = m_JA_PrinterSecondary;
               JaPrintVoteResults(1);   // print 1 pages
            }
         }

         if (m_TestMode) {
            Sleep(500);  // Pause for 500 milliseconds between iterations for testing
         }
         else {
            Sleep(10);  // Pause for 10 milliseconds between iterations
         }

         // Update motion switches and toggle lights
         currTickCount = GetTickCount();
         if (currTickCount - lastTickCount > 1000) {
            opto->getMotionSwitches();
            lastTickCount = currTickCount;
            motionOn = !motionOn;
            opto->setMotionLights(motionOn);
         }

         //if (opto->voteOff()) {
         //   opto->Initialize(); // Clear outputs
         //   for (int i = 0; i < (MAX_SEATS - 3); i++) {
         //      m_JA_VMDialog->SetMemberVote(i, m_JA_MemberSeatingList[i].seat, 0);
         //   }

         //   //m_JA_VotingMachineActive = false;
         //}

      } // end while(m_JA_VotingMachineActive)

      // Disconnect Remote Vote Stations
      remotePacket.packetType = VM_VM;
      remotePacket.VMAction = VM_VOTE_DISCONNECT_REMOTE;
      m_JA_VMDialog->SendSockRemoteMsg(&remotePacket);

      opto->Initialize(); // Clear outputs

      if (opto->displayOff()) {
      }

      for (int i = 0; i < (MAX_SEATS - 3); i++) {
         m_JA_VMDialog->SetMemberVote(i, m_JA_MemberSeatingList[i].seat, 0);
      }

      // Signal the write thread to write the vote
      gWriteTheVote = TRUE;


      // Shut down Video apps
      videoPacket.packetType = VM_VIDEO;
      videoPacket.VideoAction = VM_VIDEO_SHUTDOWN;
      m_JA_VMDialog->SendVideoMsg(&videoPacket);

      exit(0); // Just get the heck out

   } // end while(!killMainThread);

   // signal the write thread to end
   killWriteThread = TRUE;

   delete opto;

   return 0;
}


void JaSetHeader(SockPacket *mbPacket)
{
   strcpy_s(hi_order,  mbPacket->header[0]);
   strcpy_s(hi_file,   mbPacket->header[1]);
   strcpy_s(hi_action, mbPacket->header[2]);
   strcpy_s(hi_line7,  mbPacket->header[3]);

   // if action is TEST, we neither increment sequence number nor transmit information to editing machine
   system_test = !strncmp(hi_action, "TEST", 4);
}

void JaWriteDebateQueue(CString DQFilename)
{
   FILE* dqFile;
   int count;
   unsigned char dq[MAX_VOTE_ARRAY];

   for (int i = 0; i < sizeof(dq) - 2; i++) {
      // Sergeant at arms does not vote (i = seat - 1 )
      if ((i != 68) && (i != 90)) {  // 69 && 91 
         dq[i] = debateQueue[boardMap[i]];
      }
      else {
         dq[i] = 0;
      }
   }

   dqFile = fopen(DQFilename, "wb");

   if (dqFile != NULL) {
      count = fwrite(dq, 1, sizeof(dq), dqFile);

      if (count != sizeof(dq)) {
         TRACE("Unable to write Debate Queue file!");
         return;
      }

      fflush(dqFile);
      fclose(dqFile);
   }
   else {
      TRACE("Unable to open Debate Queue file");
   }
}

void JaWriteRemoteActive(CString RAFilename)
{
   FILE* raFile;
   int count;
   unsigned char ra[MAX_VOTE_ARRAY];

   for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
      ra[i] = 0;
   }
   for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
      if (m_JA_VMDialog->VMSockRemote->m_RemoteClient[i] != -1) {
         if (m_JA_VMDialog->VMSockRemote->m_RemoteClient[i].m_Seat > 0) {
            ra[m_JA_VMDialog->VMSockRemote->m_RemoteClient[i].m_Seat - 1] = 1;
         }
      }
   }

   raFile = fopen(RAFilename, "wb");

   if (raFile != NULL) {
      count = fwrite(ra, 1, sizeof(ra), raFile);

      if (count != sizeof(ra)) {
         TRACE("Unable to write Remote Active file!");
         return;
      }

      fflush(raFile);
      fclose(raFile);
   }
   else {
      TRACE("Unable to open Remote Active file");
   }
}

UINT JaWriteThreadFunction(LPVOID voidObj)
{
   TRACE("Entry");

   int forceTimer = 8;
   int RTSTimer = 8;

   while (!killWriteThread) {

      if (gWriteTheVote) {
         JaWriteCurrentVote(JaVoteDat);
         JaSendCurrentVote(); // Send current vote data to all clients
         gWriteTheVote = false;
         forceTimer = 8;
      }

      // If 2 seconds have elapsed force write the vote files
      if (forceTimer == 0) {
         JaWriteCurrentVote(JaVoteDat);
         JaSendCurrentVote(); // Send current vote data to all clients
         JaWriteRemoteActive(JaRADat);
         forceTimer = 8;
      }

      if (gDebateQueueChanged) {
         JaWriteDebateQueue(JaDQDat);
         gDebateQueueChanged = false;
      }

      if (RTSTimer == 0) {
         if (!m_VoteOpen) {
            SockRemotePacket SockResponseData;
            SockResponseData.packetType = VM_RESPONSE;
            SockResponseData.response = VM_VOTE_RTS_ACK;
            m_JA_VMDialog->SendSockRemoteMsgToRTS(&SockResponseData);
         }
         RTSTimer = 8;
      }

      forceTimer--;
      RTSTimer--;

      Sleep(250);
   }

   return 1;
}
