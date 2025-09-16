; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CJaVEDlg
LastTemplate=CWinThread
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "JaVE.h"

ClassCount=4
Class1=CJaVEApp
Class2=CJaVEDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class4=VEThread
Resource3=IDD_JAVE_DIALOG

[CLS:CJaVEApp]
Type=0
HeaderFile=JaVE.h
ImplementationFile=JaVE.cpp
Filter=N

[CLS:CJaVEDlg]
Type=0
HeaderFile=JaVEDlg.h
ImplementationFile=JaVEDlg.cpp
Filter=D
LastObject=IDC_EDIT_ACTION
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=JaVEDlg.h
ImplementationFile=JaVEDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_JAVE_DIALOG]
Type=1
Class=CJaVEDlg
ControlCount=22
Control1=IDC_COMBO_SEQUENCE,combobox,1344340227
Control2=IDC_COMBO_NAME,combobox,1344340227
Control3=IDC_RADIO_YES,button,1342308361
Control4=IDC_RADIO_NO,button,1342177289
Control5=IDC_RADIO_ABSENT,button,1342177289
Control6=IDC_BUTTON_SEND,button,1342242816
Control7=IDCANCEL,button,1342177280
Control8=IDC_BUTTON_RESET,button,1342177280
Control9=IDC_EDIT_EDITED,edit,1350568064
Control10=IDC_STATIC_DATE,static,1342308354
Control11=IDC_STATIC_FILE,static,1342308354
Control12=IDC_STATIC_SEQUENCE,static,1342308354
Control13=IDC_STATIC_ORDER,static,1342308354
Control14=IDC_STATIC_ACTION,static,1342308354
Control15=IDC_EDIT_DATE,edit,1350568064
Control16=IDC_EDIT_ORDER,edit,1350568064
Control17=IDC_EDIT_FILE,edit,1350568064
Control18=IDC_EDIT_ACTION,edit,1350568064
Control19=IDC_STATIC_GROUP,button,1342177287
Control20=IDC_STATIC_ORIGINAL,static,1342308864
Control21=IDC_STATIC_EDITED,static,1342308864
Control22=IDOK,button,1208025088

[CLS:VEThread]
Type=0
HeaderFile=VEThread.h
ImplementationFile=VEThread.cpp
BaseClass=CWinThread
Filter=N

