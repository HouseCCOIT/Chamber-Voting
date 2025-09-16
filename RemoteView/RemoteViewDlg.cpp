
// RemoteViewDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "RemoteView.h"
#include "RemoteViewDlg.h"
#include "afxdialogex.h"

#include "VM_Sock.h"
#include "alphasort.h"
#include "house.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString JaRemoteIni = "V:\\JaRemote.Ini";

LEX2 m_JA_MemberSeatingList[MAX_SEATS];

unsigned char remoteActive[MAX_VOTE_ARRAY];

CString RemoteActiveFile = "V:\\JaRA.dat";

// CRemoteViewDlg dialog

CRemoteViewDlg::CRemoteViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_REMOTEVIEW_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteViewDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDCANCEL2, m_Exit);
   DDX_Control(pDX, IDC_LIST_REMOTES, m_RemoteActiveList);
}

BEGIN_MESSAGE_MAP(CRemoteViewDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDCANCEL2, &CRemoteViewDlg::OnBnClickedCancel)
   ON_WM_TIMER()
END_MESSAGE_MAP()


// CRemoteViewDlg message handlers

BOOL CRemoteViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here


   JaLoadParametersAndSeating();

   JaReadRAFile();

  // m_RemoteActiveList.SetFont(&m_Calibri);
   m_RemoteActiveList.SetColumnWidth(120);

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if ((strlen(m_JA_MemberSeatingList[i].name) > 0) && remoteActive[m_JA_MemberSeatingList[i].seat - 1]) { // boardMap[i]]) {
         m_RemoteActiveList.AddString(m_JA_MemberSeatingList[i].name);
         int tmpItem = m_RemoteActiveList.FindString(-1, m_JA_MemberSeatingList[i].name);
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         m_RemoteActiveList.SetItemData(tmpItem, tmpSeat);
      }
   }
   m_RemoteActiveList.SetFocus();
   m_RemoteActiveList.SetCurSel(0);

   // Specify the timer interval for checking CallIn file.
   UINT    nElapse = 1000;
   SetTimer(1, nElapse, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRemoteViewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRemoteViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


int CompareMembers(const void* member1, const void* member2)
{
   const char apostrophe = (const char)'\'';

   CString name1 = ((LEX2*)member1)->name;
   CString name2 = ((LEX2*)member2)->name;

   // Remove apostrophe's prior to comparison
   name1.Remove(apostrophe);
   name2.Remove(apostrophe);

   // Comparing name in lower case guarantees telephone sort
   name1.MakeLower();
   name2.MakeLower();

   return(strcoll(name1, name2));
}

void CRemoteViewDlg::JaLoadParametersAndSeating(void)
{
   FILE* tmpFile;
   int fileError;

   char section[32];
   char key[32];
   char inBuff[81];
   char inBuff2[81];

   unsigned int i, seat;

   // Does JaRemote.Ini file exist?
   fileError = fopen_s(&tmpFile, JaRemoteIni, "r+");

   if (fileError != 0) {
      return;
   }
   else {
      fclose(tmpFile);
   }

   int k = 0;
   for (i = 0; i < MAX_SEATS; i++) {
      seat = i + 1;
      wsprintf(section, "MemberSeat_%03.3d", seat);

      wsprintf(key, "FullName");
      GetPrivateProfileString(section, key, "Hornsworthy", inBuff, 80, JaRemoteIni);

      wsprintf(key, "Name");
      GetPrivateProfileString(section, key, "Hornsworthy", inBuff2, 80, JaRemoteIni);

      if ((seat != 69) && (seat != 91)) {
         if (seat == 136) {
            strcpy_s(m_JA_MemberSeatingList[k].fullName, "Spkr. ");
            strncat_s(m_JA_MemberSeatingList[k].fullName, inBuff, 35);
            strcpy_s(m_JA_MemberSeatingList[k].name, "Spkr. ");
            strncat_s(m_JA_MemberSeatingList[k].name, inBuff2, 24);
         }
         else {
            strncpy_s(m_JA_MemberSeatingList[k].fullName, inBuff, 40);
            strncpy_s(m_JA_MemberSeatingList[k].name, inBuff2, 30);
         }
         m_JA_MemberSeatingList[k].seat = seat;
         k++;
      }
   }

   setlocale(LC_COLLATE, "en-029"); // set locale to english-carribean

   qsort((char*)m_JA_MemberSeatingList, (MAX_SEATS - 3), sizeof(LEX2), CompareMembers);

   // map members to board location
   //for (i = 0; m_JA_MemberSeatingList[i].seat; i++) {
   //   boardMap[m_JA_MemberSeatingList[i].seat - 1] = i;
   //}


   //boardMap[135] = 133;                                   // shame on you, Mr. Speaker
   //boardMap[134] = 132;                                  // shame on you, Mr. Speaker
}


FILETIME CRemoteViewDlg::JaGetFileTime(CString fName)
{
   HANDLE hFile = NULL;
   FILETIME ftCreate, ftAccess, ftWrite;

   ftWrite.dwHighDateTime = 0;
   ftWrite.dwLowDateTime = 0;

   hFile = CreateFile(fName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

   if (hFile != INVALID_HANDLE_VALUE) {
      // Retrieve the file times for the file.
      if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
         ftWrite.dwHighDateTime = 0;
         ftWrite.dwLowDateTime = 0;
      }

      CloseHandle(hFile);
   }

   return ftWrite;
}

BOOL CRemoteViewDlg::JaRAFileChanged()
{
	BOOL retVal = false;

	static FILETIME ftLastWrite;
	FILETIME ftWrite;

	ftWrite = JaGetFileTime(RemoteActiveFile);

	if (ftWrite.dwLowDateTime != ftLastWrite.dwLowDateTime) {
		ftLastWrite = ftWrite;
		retVal = true;
	}
	else {
		retVal = false;
	}

	return retVal;
}

void CRemoteViewDlg::JaReadRAFile(void)
{
	FILE* RAFile;
	int count;

	RAFile = fopen(RemoteActiveFile, "rb");

	if (RAFile != NULL) {
		count = fread(remoteActive, 1, sizeof(remoteActive), RAFile);
		fclose(RAFile);
	}
	else {
		TRACE("Unable to open Remote Active file!");
		return;
	}

	if (count != sizeof(remoteActive)) {
		TRACE("Unable to read Remote Active file!");
		return;
	}
}

void CRemoteViewDlg::OnTimer(UINT_PTR nIDEvent)
{
   static bool bail = false;

   if (bail) goto BailOut;

   bail = true;

   if (JaRAFileChanged()) {
      JaReadRAFile();

      m_RemoteActiveList.ResetContent();

      for (int i = 0; i < (MAX_SEATS - 2); i++) {
         if ((strlen(m_JA_MemberSeatingList[i].name) > 0) && remoteActive[m_JA_MemberSeatingList[i].seat - 1]) { // boardMap[i]]) {
            m_RemoteActiveList.AddString(m_JA_MemberSeatingList[i].name);
            int tmpItem = m_RemoteActiveList.FindString(-1, m_JA_MemberSeatingList[i].name);
            if (tmpItem != LB_ERR) {
               int tmpSeat = m_JA_MemberSeatingList[i].seat;
               m_RemoteActiveList.SetItemData(tmpItem, tmpSeat);
               //if (m_JA_MemberSeatingList[i].name == m_NameRTS) {
               //   m_SelectionRTS = tmpItem;
               //}
            }
         }
      }
      m_RemoteActiveList.SetFocus();

      int count = m_RemoteActiveList.GetCount();

      //if ((count > 0) && (m_SelectionRTS >= count)) {
      //   m_SelectionRTS = count - 1;
      //}

      //m_RemoteActiveList.SetCurSel(m_SelectionRTS);

      //OnLbnSelchangeListRTS();
   }

   bail = false;

BailOut:
   CDialog::OnTimer(nIDEvent);
}


void CRemoteViewDlg::OnBnClickedCancel()
{
   // TODO: Add your control notification handler code here
   CDialog::OnCancel();
}
