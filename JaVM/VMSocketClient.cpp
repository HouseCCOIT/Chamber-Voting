// VMSocketClient.cpp : implementation file
//

#include "stdafx.h"
#include "afxmt.h"
#include "JaVM.h"
#include "VMSocketClient.h"
#include "VMSocketServer.h"
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

CVMSocketClient::CVMSocketClient()
{
}

CVMSocketClient::~CVMSocketClient()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CVMSocketClient, CSocket)
   //{{AFX_MSG_MAP(CVMSocketClient)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CVMSocketClient member functions

void CVMSocketClient::OnReceive(int nErrorCode)
{
   TRACE("Entry");

   SockPacket* sockPacket = new SockPacket();

   int bytesReceived = Receive(sockPacket, sizeof(SockPacket));

	if (sockPacket->packetType == VM_VM) {
		switch (sockPacket->VMAction) {
		case VM_MB_SET_HEADER:
			//((CJaMBDlg*)pDlg)->JaMBVoteOpen();
			//JaSetHeader(sockPacket);
			break;

		default:
			break;
		}
	}

   delete sockPacket;

   CSocket::OnReceive(nErrorCode);
}

void CVMSocketClient::OnClose(int nErrorCode)
{
   // CVMSocketServer* tmpServer = (CVMSocketServer*)VMServer;

   m_Index = -1;

   CSocket::OnClose(nErrorCode);
}
