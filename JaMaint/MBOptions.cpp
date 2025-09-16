// CMBOptions.cpp : implementation file
//

#include "stdafx.h"
#include "JaMaint.h"
#include "MBOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString JaMBOptIni  = "V:\\JaMBOpt.Ini";

/////////////////////////////////////////////////////////////////////////////
// CMBOptions dialog


CMBOptions::CMBOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CMBOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMBOptions)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMBOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMBOptions)
	DDX_Control(pDX, IDC_EDIT_LIST, m_EditList);
	DDX_Control(pDX, IDC_COMBO_LISTSEL, m_ComboListSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMBOptions, CDialog)
	//{{AFX_MSG_MAP(CMBOptions)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnButtonUpdate)
	ON_CBN_SELCHANGE(IDC_COMBO_LISTSEL, OnSelchangeComboListSel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMBOptions message handlers

BOOL CMBOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ComboListSel.SetCurSel(0);

	LoadMBOptions();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMBOptions::OnSelchangeComboListSel() 
{
    LoadMBOptions();	
}

void CMBOptions::OnButtonUpdate() 
{
    StoreMBOptions();	
}

typedef struct {
	int   count;
	CHAR  strings[40][64];
} myData;

typedef struct {
	int   count;
	CHAR  strings[40][64];
} myData2;



bool CMBOptions::LoadMBOptions()
{
    FILE *tmpFile;

    char section[32];
    char key[32];

	int i, j;

	myData2 OOBData;
	CString tmpStr = "";

    // Does JaMBOpt.Ini file exist?
    if ((tmpFile = fopen(JaMBOptIni, "r+")) == NULL) {
       return FALSE;
    }
    else {
       fclose(tmpFile);
    }

	for (i = 0; i < 40; i++) {
    	for (j = 0; j < 64; j++) {
    	   OOBData.strings[i][j] = 0;
		}
	}

    wsprintf(section, "MBOptions");

	switch(m_ComboListSel.GetCurSel()) {
    	case 3:
           wsprintf(key, "Fonts");
		   break;
    	case 2:
           wsprintf(key, "Actions");
		   break;
    	case 1:
           wsprintf(key, "Files/Resolutions");
		   break;
    	case 0:
           wsprintf(key, "OrderOfBusiness");
		   break;
	}

    if (GetPrivateProfileStruct(section, key, &OOBData, sizeof(myData),  JaMBOptIni)){
	    for (i = 0; i < OOBData.count; i++) {
    	    tmpStr += OOBData.strings[i];
        	tmpStr += "\r\n";
		}
    	m_EditList.SetWindowText(tmpStr);
	}

	return TRUE;
}

void CMBOptions::StoreMBOptions()
{
    char section[32];
    char key[32];

	int i, j;

	myData2 OOBData;

	for (i = 0; i < 40; i++) {
    	for (j = 0; j < 64; j++) {
    	   OOBData.strings[i][j] = 0;
		}
	}

    wsprintf(section, "MBOptions");

	switch(m_ComboListSel.GetCurSel()) {
    	case 3:
           wsprintf(key, "Fonts");
		   break;
    	case 2:
           wsprintf(key, "Actions");
		   break;
    	case 1:
           wsprintf(key, "Files/Resolutions");
		   break;
    	case 0:
           wsprintf(key, "OrderOfBusiness");
		   break;
	}

	OOBData.count = m_EditList.GetLineCount();

	for (i = 0; i < OOBData.count; i++) {
		int length;
    	length = m_EditList.GetLine(i, OOBData.strings[i], 64);
		OOBData.strings[i][length] = 0;
	}

    WritePrivateProfileStruct(section, key,  &OOBData, sizeof(myData), JaMBOptIni); 
}
