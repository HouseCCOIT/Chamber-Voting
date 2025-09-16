// tblResults.h : Declaration of the CtblResults

#pragma once

// code generated on Saturday, February 20, 2016, 11:49 AM

class CtblResults : public CRecordset
{
public:
	CtblResults(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CtblResults)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_id;
	CStringW	m_strLine1_;
	CStringW	m_strLine2_;
	CStringW	m_strLine3_;
	CStringW	m_strLine4_;
	CStringW	m_strLine5_;
	CStringW	m_strLine6_;
	CStringW	m_strLine7_;
	CTime	m_dtUpdate;

// Overrides
	// Wizard generated virtual function overrides
	public:
	virtual CString GetDefaultConnect();	// Default connection string

	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};


