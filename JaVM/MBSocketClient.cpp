// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "afxmt.h"
#include "JaVM.h"
#include "MBSocketClient.h"
#include "MBSocketServer.h"
#include "VMThread.h"

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

CMBSocketClient::CMBSocketClient()
{
}

CMBSocketClient::~CMBSocketClient()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CMBSocketClient, CSocket)
   //{{AFX_MSG_MAP(CMBSocketClient)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CMBSocketClient member functions

void CMBSocketClient::OnReceive(int nErrorCode)
{
   TRACE("Entry");

   SockPacket* mbPacket = new SockPacket();

   int bytesReceived = Receive(mbPacket, sizeof(SockPacket));

	if (mbPacket->packetType == VM_MB) {
		switch (mbPacket->MBAction) {
		case VM_MB_SET_HEADER:
			//((CJaMBDlg*)pDlg)->JaMBVoteOpen();
			JaSetHeader(mbPacket);
			break;

		default:
			break;
		}
	}

   delete mbPacket;

   CSocket::OnReceive(nErrorCode);
}

void CMBSocketClient::OnClose(int nErrorCode)
{
   // CMBSocketServer* tmpServer = (CMBSocketServer*)VMServer;

   m_Index = -1;

   CSocket::OnClose(nErrorCode);
}
