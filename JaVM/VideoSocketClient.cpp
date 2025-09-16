// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "afxmt.h"
#include "JaVM.h"
#include "VideoSocketClient.h"
#include "VideoSocketServer.h"

#include "trace.h"
#include "house.h"

#include "JaVMDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CClientSocket

CVideoSocketClient::CVideoSocketClient()
{
}

CVideoSocketClient::~CVideoSocketClient()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CVideoSocketClient, CSocket)
   //{{AFX_MSG_MAP(CVideoSocketClient)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CVideoSocketClient member functions

void CVideoSocketClient::OnReceive(int nErrorCode)
{
   TRACE("Entry");

   // CVideoSocketServer* tmpServer = (CVideoSocketServer*)VMServer;

   CSocket::OnReceive(nErrorCode);
}

void CVideoSocketClient::SendVoteOpen(int voteOpen)
{
   //TRACE("Entry");

   SockPacket* videoPacket = new SockPacket();
   videoPacket->packetType = VM_VIDEO;
   if (voteOpen) {
      videoPacket->VideoAction = VM_VIDEO_SHOW_REALTIME;
   }
   else {
      videoPacket->VideoAction = VM_VIDEO_SHOW_RESULTS;
   }
  
   Send((char*)videoPacket, sizeof(SockPacket));

   delete videoPacket;
}

void CVideoSocketClient::OnClose(int nErrorCode)
{
   // CVideoSocketServer* tmpServer = (CVideoSocketServer*)VMServer;

   m_Index = -1;

   CSocket::OnClose(nErrorCode);
}
