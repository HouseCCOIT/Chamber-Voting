; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CJaMaintDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "JaMaint.h"

ClassCount=8
Class1=CJaMaintApp
Class2=CJaMaintDlg
Class3=CAboutDlg

ResourceCount=7
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_MEMBER_SEATING_DIALOG
Resource4=IDD_VP_TITLE_DIALOG
Class4=CMemberSeating
Class5=CVPTitle
Resource5=IDD_DIALOG_TRANSFER_ROLL_CALL
Class6=CTransferRollCall
Class7=CMemberSeatingNew
Resource6=IDD_JAMAINT_DIALOG
Class8=CMBOptions
Resource7=IDD_MBOPTIONS_DIALOG

[CLS:CJaMaintApp]
Type=0
HeaderFile=JaMaint.h
ImplementationFile=JaMaint.cpp
Filter=N

[CLS:CJaMaintDlg]
Type=0
HeaderFile=JaMaintDlg.h
ImplementationFile=JaMaintDlg.cpp
Filter=D
LastObject=IDC_EDIT_JA_SPECIALSESSION
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=JaMaintDlg.h
ImplementationFile=JaMaintDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_JAMAINT_DIALOG]
Type=1
Class=CJaMaintDlg
ControlCount=19
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308865
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_JA_SESSIONNUMBER,edit,1350631552
Control7=IDC_EDIT_JA_SEQUENCENUMBER,edit,1350631552
Control8=IDC_SPIN_JA_SESSIONNUMBER,msctls_updown32,1342177312
Control9=IDC_SPIN_JA_SEQUENCENUMBER,msctls_updown32,1342177312
Control10=IDC_BUTTON_UPDATE_MEMBER_SEATING,button,1342242816
Control11=IDC_BUTTON_CHANGE_PRINTOUT_TITLE,button,1342242816
Control12=IDC_BUTTON_TRANSFER_ROLL_CALL,button,1342242816
Control13=IDC_STATIC,button,1342177287
Control14=IDC_BUTTON_NETWORK_SETTINGS,button,1342242816
Control15=IDC_CHECK_SCRATCH,button,1342242819
Control16=IDC_BUTTON_MB_OPTIONS,button,1342242816
Control17=IDC_STATIC,static,1342308352
Control18=IDC_EDIT_JA_SPECIALSESSION,edit,1350631552
Control19=IDC_SPIN_JA_SPECIALSESSION,msctls_updown32,1342177312

[DLG:IDD_VP_TITLE_DIALOG]
Type=1
Class=CVPTitle
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308865
Control4=IDC_STATIC,button,1342177287
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT_JA_VP_TITLE1,edit,1350631552
Control9=IDC_EDIT_JA_VP_TITLE2,edit,1350631552
Control10=IDC_EDIT_JA_VP_TITLE3,edit,1350631552

[DLG:IDD_MEMBER_SEATING_DIALOG]
Type=1
Class=CMemberSeating
ControlCount=24
Control1=IDC_EDIT_JA_SEATNUMBER1,edit,1350631552
Control2=IDC_EDIT_JA_SEATNUMBER2,edit,1350631552
Control3=IDC_COMBO_JA_NAME,combobox,1344340226
Control4=IDC_COMBO_PARTY,combobox,1344339971
Control5=IDOK,button,1342242817
Control6=IDCANCEL,button,1342242816
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_SPIN_JA_SEATNUMBER1,msctls_updown32,1342177320
Control14=IDC_SPIN_JA_SEATNUMBER2,msctls_updown32,1342177320
Control15=IDC_BUTTON_ADD_MEMBER,button,1342242816
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_BUTTON_TRANSFER,button,1342242816
Control20=IDC_EDIT_JA_DISTRICT2,edit,1350633600
Control21=IDC_EDIT_JA_PARTY2,edit,1350633600
Control22=IDC_COMBO_JA_NAME_OLD,combobox,1344340227
Control23=IDC_CHECK_JA_AUTOINCREMENT,button,1342242819
Control24=IDC_COMBO_DISTRICT,combobox,1344340227

[CLS:CVPTitle]
Type=0
HeaderFile=VPTitle.h
ImplementationFile=VPTitle.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC

[CLS:CMemberSeating]
Type=0
HeaderFile=memberseating.h
ImplementationFile=memberseating.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CMemberSeating

[DLG:IDD_DIALOG_TRANSFER_ROLL_CALL]
Type=1
Class=CTransferRollCall
ControlCount=13
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_RADIO_TO_DISKETTE,button,1342308361
Control4=IDC_RADIO_FROM_DISKETTE,button,1342177289
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_JA_SEQUENCENUMBER_START,edit,1350631552
Control7=IDC_SPIN_JA_SEQUENCENUMBER_START,msctls_updown32,1342177312
Control8=IDC_STATIC,static,1342308352
Control9=IDC_EDIT_JA_SEQUENCENUMBER_END,edit,1350631552
Control10=IDC_SPIN_JA_SEQUENCENUMBER_END,msctls_updown32,1342177312
Control11=IDC_CHECK_DELETE_FROM_SOURCE,button,1342242819
Control12=IDC_BUTTON_TRANSFER,button,1342242816
Control13=IDC_STATIC,button,1342177287

[CLS:CTransferRollCall]
Type=0
HeaderFile=TransferRollCall.h
ImplementationFile=TransferRollCall.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_BUTTON_TRANSFER

[CLS:CMemberSeatingNew]
Type=0
HeaderFile=MemberSeatingNew.h
ImplementationFile=MemberSeatingNew.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CMemberSeatingNew

[DLG:IDD_MBOPTIONS_DIALOG]
Type=1
Class=CMBOptions
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDC_EDIT_LIST,edit,1352728580
Control3=IDC_COMBO_LISTSEL,combobox,1344339971
Control4=IDC_BUTTON_UPDATE,button,1342242816

[CLS:CMBOptions]
Type=0
HeaderFile=mboptions.h
ImplementationFile=mboptions.cpp
BaseClass=CDialog
LastObject=IDC_EDIT_LIST

