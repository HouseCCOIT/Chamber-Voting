// JaFile.h: interface for the JaFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JAFILE_H__37C12BA1_4027_11D2_9F54_00104B9A2A27__INCLUDED_)
#define AFX_JAFILE_H__37C12BA1_4027_11D2_9F54_00104B9A2A27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class JaFile  
{
public:
    JaFile();
    virtual ~JaFile();

};

void SetJaVoteIniFilename(CString Filename);
void SetJaSequenceFilename(CString Filename);


void JaTransferRollCall(BOOL ToDiskette, BOOL RemSrc);
void JaLoadSeating(void);
void JaLoadParmFile(void);
void JaLoadSequenceNumber(void);
void JaStoreSequenceNumber(void);
void JaUpdateSeating(void);
void JaLoadParametersAndSeating(void);
void JaStoreParameters(void);
void JaStoreSeating(void);

#endif // !defined(AFX_JAFILE_H__37C12BA1_4027_11D2_9F54_00104B9A2A27__INCLUDED_)
