
// JaSetupDlg.cpp : implementation file
//

#include <strsafe.h>

#include "pch.h"
#include "afxwin.h"
#include "framework.h"
#include "JaSetup.h"
#include "JaSetupDlg.h"
#include "afxdialogex.h"

#include "house.h"
#include "BusyForm.h"
#include "trace.h"
#include "VM_Sock.h"
#include "alphasort.h"

#include "NError.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_BUSY_FORM_FINISH (WM_USER + 1001)

// CJaSetupDlg dialog

CJaSetupDlg::CJaSetupDlg(CWnd* pParent /*=nullptr*/)
   : CDialog(IDD_JASETUP_DIALOG, pParent)
{
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJaSetupDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_LIST_STATIONS, m_StationList);
   DDX_Control(pDX, IDC_EDIT_PASSWORD, m_Password);
   DDX_Control(pDX, IDC_BUTTON_ACTIVATE, m_ButtonActivate);
   DDX_Control(pDX, IDC_BUTTON_ENROLL, m_ButtonEnroll);
   DDX_Control(pDX, IDC_BUTTON_VERIFY, m_ButtonVerify);
   DDX_Control(pDX, IDC_BUTTON_ENROLL2, m_ButtonEnroll_2);
   DDX_Control(pDX, IDC_BUTTON_VERIFY2, m_ButtonVerify_2);
}

BEGIN_MESSAGE_MAP(CJaSetupDlg, CDialog)
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDC_BUTTON_ACTIVATE, &CJaSetupDlg::OnBnClickedButtonActivate)
   ON_LBN_SELCHANGE(IDC_LIST_STATIONS, &CJaSetupDlg::OnLbnSelchangeListStations)
   ON_EN_CHANGE(IDC_EDIT_PASSWORD, &CJaSetupDlg::OnEnChangeEditPassword)
   ON_BN_CLICKED(IDC_BUTTON_ENROLL, &CJaSetupDlg::OnBnClickedButtonEnroll)
   ON_BN_CLICKED(IDC_BUTTON_VERIFY, &CJaSetupDlg::OnBnClickedButtonVerify)
   ON_BN_CLICKED(IDC_BUTTON_ENROLL2, &CJaSetupDlg::OnBnClickedButtonEnroll2)
   ON_BN_CLICKED(IDC_BUTTON_VERIFY2, &CJaSetupDlg::OnBnClickedButtonVerify2)
   ON_MESSAGE(WM_BUSY_FORM_FINISH, &CJaSetupDlg::OnBusyFormFinish)
   ON_COMMAND(IDC_SCAN_CANCEL, &CJaSetupDlg::OnScanCancel)
END_MESSAGE_MAP()


LEX2 m_JA_MemberSeatingList[MAX_SEATS];

// TBD file must be local to USB stick
CString JaRemoteIni = ".\\JaRemote.ini";
CString RemoteDataFile = ".\\JaRemote.Dat";

//CString JaRemoteIni = ".\\JaRemote.Ini";

unsigned char boardMap[MAX_SEATS];           // vectors absolute vote station numbers to display board locations
extern BOOL m_TestMode;

// CJaSetupDlg message handlers

BOOL CJaSetupDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   NResult result;

   char currDir[100];
   GetCurrentDirectory(100, (LPSTR)&currDir);
   TRACE(currDir);

   if (m_TestMode) {
      //m_ServerIPAddress.SetWindowTextA("192.168.10.140");
      RemoteDataFile = ".\\JaRemote.Dat";
      //SetCurrentDirectory("\\Program Files (x86)\\MHR\\RemoteVote"); // TBD remove for local debugging
   }
   else {
      //m_ServerIPAddress.SetWindowTextA("10.60.5.250");
      RemoteDataFile = ".\\JaRemote.Dat";
      SetCurrentDirectory("\\Program Files (x86)\\MHR\\RemoteVote");
   }

   //GetCurrentDirectory(100, (LPSTR)&currDir);
   TRACE(currDir);

   m_fpDB = ".\\mhrRemote.dat";
   m_pwd  = "MrSpeaker";
   m_fp   = "DigitalPersonaUareU";
   m_uDB  = "remoteDB.txt";

   NChar* szModules = NULL;

   AfxEnableControlContainer();

   try
   {
      CFile::Remove(m_fpDB);
   }
   catch (CFileException* pEx)
   {
      TRACE(_T("File %20s cannot be removed\n"), m_fpDB);
      pEx->Delete();
   }

   result = NffvGetAvailableScannerModules(&szModules);
   if (NFailed(result)) {
      return FALSE;
   }

   NffvFreeMemory(szModules);

   result = NffvInitialize((NAChar*)m_fpDB.GetString(), (NAChar*)m_pwd.GetString(), (NAChar*)m_fp.GetString());

   if (NFailed(result))
   {
      AfxMessageBox(_T("Failed to initialize Nffv or create/load database.\r\n")
         _T("Please check if:\r\n")
         _T(" - Provided password is correct;\r\n")
         _T(" - Database filename is correct;\r\n")
         _T(" - Scanners are used properly.\r\n"), MB_OK | MB_ICONERROR);
      return FALSE;
   }

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);			// Set big icon
   SetIcon(m_hIcon, FALSE);		// Set small icon

   // TODO: Add extra initialization here

   JaLoadParametersAndSeating();

   for (int i = 0; i < (MAX_SEATS - 2); i++) {
      if (strlen(m_JA_MemberSeatingList[i].name) > 0) {
         m_StationList.AddString(m_JA_MemberSeatingList[i].name);
         int tmpItem = m_StationList.FindString(-1, m_JA_MemberSeatingList[i].name);
         int tmpSeat = m_JA_MemberSeatingList[i].seat;
         m_StationList.SetItemData(tmpItem, tmpSeat);
      }
   }
   m_StationList.RedrawWindow();
   m_StationList.SetFocus();
   m_StationList.SetCurSel(0);
   OnLbnSelchangeListStations();

   try
   {
      CFile::Remove(RemoteDataFile);
   }
   catch (CFileException* pEx)
   {
      TRACE(_T("File %20s cannot be removed\n"), RemoteDataFile);
      pEx->Delete();
   }

   //	The crypto object.
   //m_crypto = new MFC::CCrypto();

   //	Set the password.
   m_strPassword = "Mr. Speaker";
   m_crypto.DeriveKey(m_strPassword);

   m_ButtonEnroll.EnableWindow(false);
   m_ButtonVerify.EnableWindow(false);
   m_ButtonEnroll_2.EnableWindow(false);
   m_ButtonVerify_2.EnableWindow(false);
   m_ButtonActivate.EnableWindow(false);

   m_iEnrollTimeout = 15000;
   m_iVerifyTimeout = 2000;

   return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJaSetupDlg::OnPaint()
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
HCURSOR CJaSetupDlg::OnQueryDragIcon()
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

void CJaSetupDlg::JaLoadParametersAndSeating(void)
{
   FILE* tmpFile;
   int fileError;

   char section[32];
   char key[32];
   char inBuff[81];
   char inBuff2[81];

   unsigned int i, j, seat;

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
      for (j = strlen(inBuff); j < 40; j++) {
         strcat_s(inBuff, " ");
      }

      wsprintf(key, "Name");
      GetPrivateProfileString(section, key, "Hornsworthy", inBuff2, 80, JaRemoteIni);
      for (j = strlen(inBuff2); j < 30; j++) {
         strcat_s(inBuff2, " ");
      }

      if ((seat != 69) && (seat != 91)) {
         strncpy_s(m_JA_MemberSeatingList[k].fullName, inBuff, 40);
         strncpy_s(m_JA_MemberSeatingList[k].name, inBuff2, 30);
         m_JA_MemberSeatingList[k].seat = seat;
         k++;
      }
   }

   setlocale(LC_COLLATE, "en-029"); // set locale to english-carribean

   qsort((char*)m_JA_MemberSeatingList, (MAX_SEATS - 3), sizeof(LEX2), CompareMembers);

   // map members to board location
   for (i = 0; m_JA_MemberSeatingList[i].seat; i++) {
      boardMap[m_JA_MemberSeatingList[i].seat - 1] = i;
   }

   boardMap[135] = 133;                                   // shame on you, Mr. Speaker
   //boardMap[134] = 132;                                  // shame on you, Mr. Speaker
}

extern DWORD cryptError;
void CJaSetupDlg::OnBnClickedButtonActivate()
{
   UpdateData();

   LPVOID lpMsgBuf;
   LPVOID lpDisplayBuf;

   //m_crypto = new MFC::CCrypto();

   //	Set the password.
   //m_strPassword = "Mr. Speaker";
   //m_crypto->DeriveKey(m_strPassword);

   //	Show a file save dialog.
   //CFileDialog dlg(FALSE, _T("dat"), _T("JaRemote"), OFN_HIDEREADONLY, _T("JaRemote (*.dat)|*.dat||"), this);
   //if (dlg.DoModal() == IDOK)
   //{
      //	Create a byte array to store the data.
      CByteArray arData;

      //	Write the data to the array.
      if (m_crypto.Encrypt(m_strData, arData) == false)
      {
         FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, cryptError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
         lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + 40) * sizeof(TCHAR));
         StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("Couldn't encrypt data. Error %d: %s"), cryptError, lpMsgBuf);

         MessageBox((LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

         LocalFree(lpMsgBuf);
         LocalFree(lpDisplayBuf);

         return;
      }

      //	Save the data to disk.
      CFile file;
      if (file.Open(RemoteDataFile, CFile::modeCreate | CFile::modeWrite) == FALSE)
      {
         AfxMessageBox(_T("Couldn't open file."));
         return;
      }

      file.Write(arData.GetData(), static_cast<UINT>(arData.GetCount()));
      file.Flush();
      file.Close();

      m_ButtonEnroll.EnableWindow(false);
      m_ButtonVerify.EnableWindow(false);
      m_ButtonEnroll_2.EnableWindow(false);
      m_ButtonVerify_2.EnableWindow(false);
      m_ButtonActivate.EnableWindow(false);

      //m_strData = _T("");
      UpdateData(FALSE);
 //  }
}


void CJaSetupDlg::OnLbnSelchangeListStations()
{
   int tmpItem = m_StationList.GetCurSel();
   int seat = m_StationList.GetItemData(tmpItem);
   CString name;
   m_StationList.GetText(tmpItem, m_Name);
   name = m_JA_MemberSeatingList[tmpItem].fullName;

  // CString ipAddr;
   char seatStr[10];

   _itoa_s(seat, seatStr, 10);
   //m_ServerIPAddress.GetWindowTextA(ipAddr);

   m_strData = name + '\t' + seatStr; // +'\t' + ipAddr;

   if (m_StationList.GetCount() > 0) {
      for (int i = 0; i < m_StationList.GetCount(); i++) {
         int tmpSeat = m_StationList.GetItemData(i);

      }
   }
}


void CJaSetupDlg::OnEnChangeEditPassword()
{
   CString pwd;

   m_Password.GetWindowTextA(pwd);

   if (pwd == "mhr#932023") {
      m_ButtonEnroll.EnableWindow(true);
   }
}

struct EnrollParam
{
   HNffvUser* pHUser;
   CJaSetupDlg* pMainForm;
   CBusyForm* pBusyForm;
   NffvStatus engineStatus;
};

struct VerifyParam
{
   HNffvUser hUser;
   NInt* pScore;
   CJaSetupDlg* pMainForm;
   CBusyForm* pBusyForm;
   NffvStatus engineStatus;
};

UINT CJaSetupDlg::EnrollUserThread(LPVOID pParam)
{
   EnrollParam* pEnrollParam = (EnrollParam*)pParam;
   CJaSetupDlg* form = (CJaSetupDlg*)pEnrollParam->pMainForm;

   NffvSetQualityThreshold(25);

   NResult result = NffvEnroll(form->m_iEnrollTimeout, &pEnrollParam->engineStatus, pEnrollParam->pHUser);
   if (NFailed(result)) {} // throw result;

   form->PostMessage(WM_BUSY_FORM_FINISH, 0, (LPARAM)pEnrollParam->pBusyForm);

   return 0;   // thread completed successfully
}

UINT CJaSetupDlg::VerifyUserThread(LPVOID pParam)
{
   VerifyParam* pVerifyParam = (VerifyParam*)pParam;
   CJaSetupDlg* form = ((CJaSetupDlg*)pVerifyParam->pMainForm);

   NffvSetQualityThreshold(25);

   NResult result = NffvVerify(pVerifyParam->hUser, 20000, &pVerifyParam->engineStatus, pVerifyParam->pScore);
   if (NFailed(result)) {} // throw result;

   form->PostMessage(WM_BUSY_FORM_FINISH, 0, (LPARAM)pVerifyParam->pBusyForm);

   return 0;   // thread completed successfully
}

void CJaSetupDlg::OnBnClickedButtonEnroll()
{
   HNffvUser hUser;
   NResult result;
   NInt count;
   CBusyForm busy("Enrolling Primary Fingerprint...");

   EnrollParam p;
   p.pMainForm = this;
   p.pHUser = &hUser;
   p.pBusyForm = &busy;

   AfxBeginThread(EnrollUserThread, &p);
   busy.DoModal();

   if (p.engineStatus == nfesTemplateCreated)
   {
      m_ButtonEnroll.EnableWindow(false);
      m_ButtonVerify.EnableWindow(true);
   }
   else
   {
      CString description = GetNffvStatusDescription(p.engineStatus);
      CString errorMsg;
      errorMsg.Format(_T("Enrollment failed. Reason: %s"), description);
      MessageBox(errorMsg);
   }
}


void CJaSetupDlg::OnBnClickedButtonEnroll2()
{
   HNffvUser hUser;
   NResult result;
   NInt count;
   CBusyForm busy("Enrolling Secondary Fingerprint...");

   EnrollParam p;
   p.pMainForm = this;
   p.pHUser = &hUser;
   p.pBusyForm = &busy;

   AfxBeginThread(EnrollUserThread, &p);
   busy.DoModal();

   if (p.engineStatus == nfesTemplateCreated)
   {
      m_ButtonEnroll_2.EnableWindow(false);
      m_ButtonVerify_2.EnableWindow(true);
   }
   else
   {
      CString description = GetNffvStatusDescription(p.engineStatus);
      CString errorMsg;
      errorMsg.Format(_T("Enrollment failed. Reason: %s"), description);
      MessageBox(errorMsg);
   }
}


void CJaSetupDlg::OnBnClickedButtonVerify()
{
   NResult result;
   HNffvUser hUser;
   NInt score;
   CBusyForm busy("Verifying Primary Fingerprint...");

   result = NffvGetUser(0, &hUser);
   if (NFailed(result)) {} //throw result;

   VerifyParam p;
   p.pMainForm = this;
   p.pBusyForm = &busy;
   p.hUser = hUser;
   p.pScore = &score;

   AfxBeginThread(VerifyUserThread, &p);
   busy.DoModal();

   if (p.engineStatus == nfesTemplateCreated)
   {
      if (score > 0)
      {
         CString msg;
         msg.Format(_T("%s verified.\r\n") _T("Fingerprints match. Score: %d"), m_Name.GetString(), score);

         MessageBox(msg.GetString(), _T("Fingerprints verification"));

         m_ButtonVerify.EnableWindow(false);
         m_ButtonEnroll_2.EnableWindow(true);
      }
      else
      {
         MessageBox(_T("Fingerprints did not match"), _T("Fingerprints verification"));
      }
   }
   else
   {
      CString description = GetNffvStatusDescription(p.engineStatus);
      CString errorMsg;
      errorMsg.Format(_T("Verification failed. Reason: %s"), description);
      MessageBox(errorMsg);
   }
}


void CJaSetupDlg::OnBnClickedButtonVerify2()
{
   NResult result;
   HNffvUser hUser;
   NInt score;
   CBusyForm busy("Verifying Secondary FingerPrint...");

   result = NffvGetUser(1, &hUser);
   if (NFailed(result)) {} //throw result;

   VerifyParam p;
   p.pMainForm = this;
   p.pBusyForm = &busy;
   p.hUser = hUser;
   p.pScore = &score;

   AfxBeginThread(VerifyUserThread, &p);
   busy.DoModal();

   if (p.engineStatus == nfesTemplateCreated)
   {
      if (score > 0)
      {
         CString msg;
         msg.Format(_T("%s verified.\r\n") _T("Fingerprints match. Score: %d"), m_Name.GetString(), score);

         MessageBox(msg.GetString(), _T("Fingerprints verification"));
         m_ButtonVerify_2.EnableWindow(false);
         m_ButtonActivate.EnableWindow(true);
      }
      else
      {
         MessageBox(_T("Fingerprints did not match"), _T("Fingerprints verification"));
      }
   }
   else
   {
      CString description = GetNffvStatusDescription(p.engineStatus);
      CString errorMsg;
      errorMsg.Format(_T("Verification failed. Reason: %s"), description);
      MessageBox(errorMsg);
   }
}


afx_msg LRESULT CJaSetupDlg::OnBusyFormFinish(WPARAM wParam, LPARAM lParam)
{
   CBusyForm* busyForm = (CBusyForm*)lParam;
   busyForm->Stop();

   return 0;
}


void CJaSetupDlg::OnScanCancel()
{
   NffvCancel();
}

const TCHAR* CJaSetupDlg::GetNffvStatusDescription(NffvStatus status)
{
   switch (status)
   {
   case nfesNone: return _T("None");
   case nfesTemplateCreated: return _T("Template created");
   case nfesNoScanner: return _T("No scanner");
   case nfesScannerTimeout: return _T("Scanner timeout");
   case nfesUserCanceled: return _T("User cancelled.");
   case nfesQualityCheckFailed: return _T("Quality check failed");
   default: return _T("");
   }
}


