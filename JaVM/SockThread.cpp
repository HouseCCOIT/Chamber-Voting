// SockThread.cpp : implementation file
//

#include "stdafx.h"
#include "JaVM.h"
#include "JaVMDlg.h"
#include "JaOpto.h"
#include "server32.h"
#include "trace.h"
#include "RemoteSocketServer.h"
#include "RemoteSocketClient.h"
//#include "vmthread.h"
#include "house.h"
#include "alphasort.h"

#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

extern CJaVMDlg* m_JA_VMDialog;
bool killSockThread = false;

LEX m_MemberSeatingList[MAX_VOTE_ARRAY];

//extern char station_lock[MAX_VOTE_ARRAY];                 // != 0: bypass this vote station in polling sequence
//extern char remoteVotes[MAX_VOTE_ARRAY];
//extern char callInVotes[MAX_VOTE_ARRAY];
//extern char pageButtons[MAX_VOTE_ARRAY];
extern char debateQueue[MAX_VOTE_ARRAY];

extern UINT JaWriteThreadFunction(LPVOID voidObj);

extern BOOL gDebateQueueChanged;

extern unsigned char yes_total, no_total, absent_total;
extern bool gWriteTheVote;

using namespace System;

CWinThread* SockWriteThread;
bool killSockWriteThread = false;

bool gSockWriteTheVote = false;

unsigned char abs_total = 134;
unsigned char nay_total, yea_total;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//CString StationLockFile = "V:\\JaLock.dat";
//CString RemoteVotesFile = "V:\\JaRemoteVotes.dat";
//CString CallInVotesFile = "V:\\JaCallInVotes.dat";

CString JaSockVoteIni = "V:\\JaVote.Ini";

extern BOOL m_VoteOpen;
         
extern unsigned char boardMap[MAX_SEATS];   // vectors absolute vote station numbers to display board locations

int voteSock(CRemoteSocketServer *VMSocket)
{
   return 1;
}
 

void JaSockLoadParametersAndSeating(void)
{
   FILE* tmpFile;

   char section[64];
   char key[64];
   char inBuff[200];

   unsigned int i, j, seat;

   // Does JaVote.Ini file exist?
   tmpFile = fopen(JaSockVoteIni, "r+");
   if (tmpFile == NULL) {
      DWORD error = GetLastError();
      return;
   }
   else {
      fclose(tmpFile);
   }

   int k = 0;
   for (i = 0; i < MAX_SEATS; i++) {
      seat = i + 1;
      wsprintf(section, "MemberSeat_%03.3d", seat);

      wsprintf(key, " Name");
      GetPrivateProfileString(section, key, "Hornsworthy", inBuff, 80, JaSockVoteIni);
      for (j = strlen(inBuff); j < 30; j++) {
         strcat_s(inBuff, " ");
      }

      if ((seat != 69) && (seat != 91)) {
         if (seat == 136) {
            strcpy_s(m_MemberSeatingList[k].name, "Speaker");
            strncat(m_MemberSeatingList[k].name, inBuff, 25);
         }
         else {
            strncpy_s(m_MemberSeatingList[k].name, inBuff, 30);
         }
         m_MemberSeatingList[k].seat = seat;
         m_MemberSeatingList[k].seatID = seat + 2000; // Window Id
         k++;
      }
   }
   m_MemberSeatingList[k].seat = 0;

   setlocale(LC_COLLATE, "en-029"); // set locale to english-carribean

   qsort((char*)m_MemberSeatingList, (MAX_SEATS - 3), sizeof(LEX), CompareMembers);

   // map members to board location
   for (i = 0; m_MemberSeatingList[i].seat; i++) {
      boardMap[m_MemberSeatingList[i].seat - 1] = i;
      m_MemberSeatingList[i].name[NAME_PRINTABLE] = '\0';  // truncate the padded name for columnation on printout
   }

   boardMap[135] = 133;                                  // shame on you, Mr. Speaker
   //boardMap[134] = 132;                                  // shame on you, Mr. Speaker

   m_MemberSeatingList[i].name[NAME_PRINTABLE] = '\0';

}

void KillSock(void)
{
   killSockThread = true;
}


int voteTest()
{
   no_total = 0;
   yes_total = 0;
   absent_total = 0;

   static unsigned char last_no_total, last_yes_total;

   for (int i = 0; i < MAX_SEATS; i++) {
 
      // Sergeant at arms does not vote (i = seat - 1 )
      if ((i != 68) && (i != 90)) {  // 69 && 91 
         m_JA_VMDialog->SetMemberVote(i, i + 1, m_MemberSeatingList[boardMap[i]].vote);
         if (debateQueue[boardMap[i]]) {
            m_JA_VMDialog->SetMemberVote(i, i + 1, 3);
         }
      }

      switch (m_MemberSeatingList[i].vote) {
      case 2:
         yes_total++;
         break;

      case 1:
         no_total++;
         break;

      case 0:
      default:
         absent_total++;
         break;
      }
   }

   if ((yes_total != last_yes_total) || (no_total != last_no_total)) {
      gWriteTheVote = true;
   }
   last_yes_total = yes_total;
   last_no_total = no_total;

   return(PRINT_RESULT);
}


UINT JaSockThreadFunction(LPVOID DlgObj)
{
   TRACE("Entry");

   m_JA_VMDialog = (CJaVMDlg *)DlgObj;    // Initialize global pointer to VM Dialog
   
   SockVMPacket remotePacket;
   SockPacket   videoPacket;

   // Clear the debate queue on Startup
   for (int i = 0; i < MAX_VOTE_ARRAY; i++) {
      debateQueue[i] = 0;
   }
   gDebateQueueChanged = true;

   JaSockLoadParametersAndSeating();

   // Spawn a separate thread to write the vote file asynchronously
   SockWriteThread = AfxBeginThread(JaWriteThreadFunction, DlgObj);

   while (!killSockThread) {

      //TRACE("In a little while sir...");

     // Sleep(1000);

      Sleep(500);

      voteTest();

      //gSockWriteTheVote = true;

      //TRACE("Now!");

      //if (m_VoteOpen) {
      //   SockVMPacket SockVMOpen;
      //   SockVMOpen.packetType = VM_VM;
      //   SockVMOpen.VMAction = VM_VOTE_OPEN_REMOTE;
      //   m_JA_VMDialog2->SendSockRemoteMsg(&SockVMOpen);
      //}
      //else {
      //    // Reset the Remote Vote Stations
      //    SockVMPacket SockVMReset;
      //    SockVMReset.packetType = VM_VM;
      //    SockVMReset.VMAction = VM_VOTE_RESET_REMOTE;
      //    m_JA_VMDialog2->SendSockRemoteMsg(&SockVMReset);
      //}

	  //exit(0); // Just get the heck out

   } // end while(!killSockThread);

   // Disconnect the Remote Vote Stations
   remotePacket.packetType = VM_VM;
   remotePacket.VMAction = VM_VOTE_DISCONNECT_REMOTE;
   m_JA_VMDialog->SendSockRemoteMsg(&remotePacket);

   // Shut down Video apps
   videoPacket.packetType = VM_VIDEO;
   videoPacket.VideoAction = VM_VIDEO_SHUTDOWN;
   m_JA_VMDialog->SendVideoMsg(&videoPacket);

   return 0;
}
