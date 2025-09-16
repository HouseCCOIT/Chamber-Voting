// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "afxmt.h"
#include "JaVM.h"
#include "RemoteSocketClient.h"
#include "RemoteSocketServer.h"
#include "VM_Sock.h"
#include "trace.h"
#include "house.h"

#include "JaVMDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char station_lock[MAX_VOTE_ARRAY];    // != 0: bypass this vote station in polling sequence
extern char remoteVotes[MAX_VOTE_ARRAY];
extern char callInVotes[MAX_VOTE_ARRAY];
extern char pageButtons[MAX_VOTE_ARRAY];
extern char debateQueue[MAX_VOTE_ARRAY];

/////////////////////////////////////////////////////////////////////////////
// CRemoteSocketClient

CRemoteSocketClient::CRemoteSocketClient()
{
}

CRemoteSocketClient::~CRemoteSocketClient()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CRemoteSocketClient, CSocket)
   //{{AFX_MSG_MAP(CRemoteSocketClient)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CRemoteSocketClient member functions

extern LEX m_MemberSeatingList[];
extern LEX m_JA_MemberSeatingList[];
extern unsigned char boardMap[MAX_SEATS];
extern void DisplayVote(unsigned char seat, unsigned char vote);
extern void DisplayVoteRemote(unsigned char seat, unsigned char vote);

extern BOOL m_VoteOpen;
extern short vote_in_progress;
extern BOOL gDebateQueueChanged;

extern bool gWriteTheVote;

CMutex CallInMutex;

//extern unsigned char absent_total;
//extern unsigned char no_total;
//extern unsigned char yes_total;
//extern unsigned char oldVote[MAX_VOTE_ARRAY];

//extern unsigned char sockBoardMap[MAX_SEATS];

void CRemoteSocketClient::OnReceive(int nErrorCode)
{
   bool remote;

   //TRACE("Entry");

   CRemoteSocketServer* tmpServer = (CRemoteSocketServer*)VMServer;

   CSingleLock CallInSem(&CallInMutex);

   CallInSem.Lock(1000);

   SockRemotePacket* SockRemoteData = new SockRemotePacket();

   int bytesReceived = Receive(SockRemoteData, sizeof(SockRemotePacket));
   m_Vote = SockRemoteData->remoteAction;
   if (SockRemoteData->index > 999) {
      m_Seat = SockRemoteData->index - 1000;
      remote = false;
   }
   else {
      m_Seat = SockRemoteData->index;
      remote = true;
   }

   SockRemotePacket* SockResponseData = new SockRemotePacket();
   SockResponseData->packetType = VM_RESPONSE;
   SockResponseData->index = m_Index;

   if (SockRemoteData->remoteAction == VM_VOTE_GET) {

      if (m_VoteOpen) { // If the vote is open, return current vote status
         switch (m_JA_MemberSeatingList[boardMap[m_Seat - 1]].vote) {
         case VM_VOTE_YEA:
            SockResponseData->response = VM_VOTE_YEA_ACK;
            break;
         case VM_VOTE_NAY:
            SockResponseData->response = VM_VOTE_NAY_ACK;
            break;
         case VM_VOTE_RESET:
            SockResponseData->response = VM_VOTE_RESET_ACK;
            break;
         default:
            break;
         }
      }
      else { // Otherwise return the current RTS status
         if (debateQueue[boardMap[m_Seat - 1]] == 1) {
            SockResponseData->response = VM_VOTE_RTS_ACK;
         }
         else {
            SockResponseData->response = VM_VOTE_RTS_NAK;
            tmpServer->SendSockRemoteSeatMessage(m_Seat, SockResponseData);
         }
      }
   }
   else if (SockRemoteData->remoteAction == VM_VOTE_PAGE) {
      if (pageButtons[boardMap[m_Seat - 1]] == 0) {
         SockResponseData->response = VM_VOTE_PAGE_NAK; // ACK;
         pageButtons[boardMap[m_Seat - 1]] = 1;
      }
      else {
         SockResponseData->response = VM_VOTE_PAGE_NAK;
         pageButtons[boardMap[m_Seat - 1]] = 0;
      }
   }
   else if ((SockRemoteData->remoteAction == VM_VOTE_RTS) && !m_VoteOpen) {
      if (debateQueue[boardMap[m_Seat - 1]] == 0) {
         SockResponseData->response = VM_VOTE_RTS_ACK;
         debateQueue[boardMap[m_Seat - 1]] = 1;
      }
      else {
         SockResponseData->response = VM_VOTE_RTS_NAK;
         debateQueue[boardMap[m_Seat - 1]] = 0;
         tmpServer->SendSockRemoteSeatMessage(m_Seat, SockResponseData);
      }
      gDebateQueueChanged = true;
   }
   else if (m_VoteOpen && vote_in_progress && !station_lock[m_Seat - 1] && ( remote ? remoteVotes[m_Seat - 1] : callInVotes[m_Seat - 1])) {
      m_MemberSeatingList[boardMap[m_Seat - 1]].vote = m_Vote;
      m_JA_MemberSeatingList[boardMap[m_Seat - 1]].vote = m_Vote;

      // TBD - 02132021 - Reconcile this with setting in VMThread.cpp
      // This used to be gSockWriteTheVote
      //gWriteTheVote = true;

      switch (SockRemoteData->remoteAction) {
      case VM_VOTE_YEA:
         SockResponseData->response = VM_VOTE_YEA_ACK;
         break;
      case VM_VOTE_NAY:
         SockResponseData->response = VM_VOTE_NAY_ACK;
         break;
      case VM_VOTE_RESET:
         SockResponseData->response = VM_VOTE_RESET_ACK;
         break;
      default:
         break;
      }
   }
   else {
      switch (SockRemoteData->remoteAction) {
      case VM_VOTE_YEA:
         SockResponseData->response = VM_VOTE_YEA_NAK;
         break;
      case VM_VOTE_NAY:
         SockResponseData->response = VM_VOTE_NAY_NAK;
         break;
      case VM_VOTE_RESET:
         SockResponseData->response = VM_VOTE_RESET_NAK;
         break;
      default:
         break;
      }
   }

   Send((char*)SockResponseData, sizeof(SockRemotePacket));

   delete SockRemoteData;
   delete SockResponseData;

   CallInSem.Unlock();
   //TRACE("Exit");

   CSocket::OnReceive(nErrorCode);
}

void CRemoteSocketClient::SendVoteOpen(int voteOpen)
{
   //TRACE("Entry");

   //m_VoteOpen2 = voteOpen;
   m_Index = voteOpen;

   SockVMPacket* remotePacket = new SockVMPacket();
   remotePacket->packetType = VM_VM;
   remotePacket->VMAction = VM_VOTE_SETINDEX;
   remotePacket->index = voteOpen;

   Send((char*)remotePacket, sizeof(SockVMPacket));

   delete remotePacket;
}


void CRemoteSocketClient::OnClose(int nErrorCode)
{
   CRemoteSocketServer* tmpServer = (CRemoteSocketServer*)VMServer;

   // TBD - Do we want to zero out the vote if a remote app closes?
   //m_MemberSeatingList[m_Seat - 1].vote = 0;
   //m_JA_MemberSeatingList[boardMap[m_Seat - 1]].vote = 0;

   // TBD - don't clear display if remote app blinks away.
   //DisplayVoteRemote(m_Seat - 1, 0);  // Turn off lights at Desk and Display Board

   m_Seat = 0;
   m_Index = -1;

   CSocket::OnClose(nErrorCode);
}
