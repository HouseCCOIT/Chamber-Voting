// tblResults.h : Implementation of the CtblResults class



// CtblResults implementation

// code generated on Saturday, February 20, 2016, 11:49 AM

#include "stdafx.h"
#include "tblResults.h"
IMPLEMENT_DYNAMIC(CtblResults, CRecordset)

CtblResults::CtblResults(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_id = 0;
	m_strLine1_ = L"";
	m_strLine2_ = L"";
	m_strLine3_ = L"";
	m_strLine4_ = L"";
	m_strLine5_ = L"";
	m_strLine6_ = L"";
	m_strLine7_ = L"";
	m_dtUpdate;
	m_nFields = 9;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CtblResults::GetDefaultConnect()
{
	//return _T("DSN=House1;Description=House Chamber;Trusted_Connection=Yes;APP=Microsoft\x00ae Visual Studio\x00ae 2015;WSID=ROSSUM;DATABASE=House1;");
   return _T("DSN=House1;Description=House Chamber;UID=DisplayU;PWD=ChamberUpdate1!.;APP=Microsoft\x00ae Visual Studio\x00ae 2015;WSID=156.98.78.174\House;DATABASE=House1;");
}

CString CtblResults::GetDefaultSQL()
{
	return _T("[dbo].[tblChamberDisplay]");
}

void CtblResults::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Int() are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long(pFX, _T("[id]"), m_id);
	RFX_Text(pFX, _T("[strLine1_]"), m_strLine1_);
	RFX_Text(pFX, _T("[strLine2_]"), m_strLine2_);
	RFX_Text(pFX, _T("[strLine3_]"), m_strLine3_);
	RFX_Text(pFX, _T("[strLine4_]"), m_strLine4_);
	RFX_Text(pFX, _T("[strLine5_]"), m_strLine5_);
	RFX_Text(pFX, _T("[strLine6_]"), m_strLine6_);
	RFX_Text(pFX, _T("[strLine7_]"), m_strLine7_);
	RFX_Date(pFX, _T("[dtUpdate]"), m_dtUpdate);

}
/////////////////////////////////////////////////////////////////////////////
// CtblResults diagnostics

#ifdef _DEBUG
void CtblResults::AssertValid() const
{
	CRecordset::AssertValid();
}

void CtblResults::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG


