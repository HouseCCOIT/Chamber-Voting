// JaFile.cpp: implementation of the JaFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "time.h"
#include "JaMaint.h"
#include "JaFile.h"
#include "VM_Sock.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

JaFile::JaFile()
{

}

JaFile::~JaFile()
{

}

char *JaTrim(char *s)
{
   char *p;

   for (p = s + strlen(s) - 1; *p == ' ' && p >= s; p--);

   *(p + 1) = 0;

   return (s);
}

#define ROLL_RECORD 215
#define ROLL_LINE_LENGTH 150

CString JaVoteIni  = "V:\\JaVote.Ini";
CString JaSequence = "V:\\VOTES\\SEQUENCE.VOT";


void SetJaVoteIniFilename(CString Filename)
{
	JaVoteIni = Filename;
}

void SetJaSequenceFilename(CString Filename)
{
	JaSequence = Filename;
}

void JaTransferRollCall(BOOL ToDiskette, BOOL RemSrc)
{
    char errBuff[81];

    unsigned int i, j, retry;
    CString SrcPath, DstPath;
    char SrcFullPath[255], DstFullPath[255], roll_info[2][ROLL_RECORD];
    FILE *roll_in, *roll_out;

    if (ToDiskette) {
       SrcPath = "V:\\VOTES\\";
       DstPath = "A:\\";
    }
    else {
       SrcPath = "A:\\";
       DstPath = "V:\\VOTES\\";
    }

    for (i = m_JA_SequenceNumberStart; i <= m_JA_SequenceNumberEnd; i++) {

        wsprintf(SrcFullPath, "%sH%02.2d%04.4ld.vot", SrcPath, m_JA_SessionNumber, i);        
        wsprintf(DstFullPath, "%sH%02.2d%04.4ld.vot", DstPath, m_JA_SessionNumber, i);

        if (roll_in = fopen(SrcFullPath, "r+")) {

            // read in the two records (control and vote)
            for (j = 0; j < 2; j++) {
                if (!fgets(roll_info[j], ROLL_RECORD, roll_in)) {
                    wsprintf(errBuff, "Could not read info on sequence number %ld.", i);
                    AfxMessageBox(errBuff, MB_ICONEXCLAMATION);
                }
            }

            /* close the input file */
            fclose(roll_in);

// The write sequence consists of a while loop that checks the variable "retry", which is initialized before loop
// entry to 1.  In the do loop within the while, we keep trying to open the target file until either we succeed, we
// detect that we had the error on the hard disk, or the user gives us an ESC.  If we succeed in opening the file
// and writing the two records, retry is cleared and the main backup sequence is resumed.  Otherwise, we erase any
// file flotsam that may have gotten onto the target.  Then, if the error happened on a floppy, we let the user insert
// a new one and try again. */

            retry = 1;
            while (retry) {
                do {
                    if (!(roll_out = fopen(DstFullPath, "w"))) {
                        if (ToDiskette) {
                            if (AfxMessageBox("Insert another diskette and click OK, or click Cancel to abort transfer.", MB_RETRYCANCEL) == IDRETRY) {
                               continue;
                            }
                            else {
                               return;
                            }
                        }
                        AfxMessageBox("Failure to create new file on hard disk.", MB_OK);
                        return;
                    }
                } while (!roll_out);

                // write out the two records
                for (j = 0; j < 2; j++) {
                    if (fputs(roll_info[j], roll_out) < 0) {
                        fclose(roll_out);
                        remove(DstFullPath);
                        if (ToDiskette) {
//                            if (errno == ENOSPC) {
                                AfxMessageBox("This floppy diskette is full.", MB_OK);
//                            }
//                            else {
//                                AfxMessageBox("This floppy diskette will not accept any more data.", MB_OK);
//                            }

                            if (AfxMessageBox("Insert another diskette and click Retry, or click Cancel to abort transfer.", MB_RETRYCANCEL) == IDRETRY) {
                               continue;
                            }
                            else {
                               return;
                            }
                            retry = 1;
                            break;
                        } else {
                            AfxMessageBox("Could not write to hard disk.", MB_OK);
                            return;
                        }
                    }
                    else {
                        retry = 0;
                    }
                } /* for */

            } /* while (retry) */

            // close the output file
            fclose(roll_out);

            // Erase the file?
            if (RemSrc) {
               remove(SrcFullPath);
            }
        } /* if (roll_in) */
    }
}

/************************************************************************
    MAINTENANCE NOTE:
    This version of load_seating differs markedly from that in mb.c & mv.c:
        1. There is no sorting by names in the seating array
        2. The prefix "Spk. " is not attached to the name of the Speaker
        3. Instead of LEX, we use a typedef EXTENDED_LEX, which has provisions
           for the district and party code.
        4. Although we do not allow the user to modify the Sargeant's desks,
           we do load them here.
*************************************************************************/

void JaLoadSeating(void)
{
    char outBuff[81];
    char tmpStr[4];
    char seatRecord[50];
    int  seatNumber;
    char recordNumber[5];

    char seatFilename[255];
    FILE *seatHandle;

    wsprintf(seatFilename, "V:\\VOTES\\H%s.SEA", m_JA_SeatingDate);

    if (!(seatHandle = fopen(seatFilename, "r"))) {
        wsprintf(outBuff, "JaLoadSeating - Unable to open %s!", seatFilename);
        AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
        return;
    }

    for (int i = 1; i < (MAX_SEATS + 1); i++) {
        if (!(fgets(seatRecord, 50, seatHandle))) {
            wsprintf(outBuff, "JaLoadSeating - Unable to read record %d from %s!", i, seatFilename);
            AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
            fclose(seatHandle);
            return;
        }

        strncpy_s(recordNumber, seatRecord, 3);
        recordNumber[3] = '\0';
        seatNumber = atoi(recordNumber);
        if (i != seatNumber) {
            wsprintf(outBuff, "JaLoadSeating - Synchronization Error - Line %d doesn't match record %d in %s!", i, seatNumber, seatFilename);
            AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
            fclose(seatHandle);
            return;
        }

        // Load the structure from the input buffer
        strncpy_s(m_JA_MemberSeatingListOld[i].name, seatRecord + 3, 30);
        m_JA_MemberSeatingListOld[i].seat = seatNumber;

        strncpy_s(tmpStr, seatRecord + 33, 3);
        JaTrim(tmpStr);
        if (strlen(tmpStr) == 3) {
           strcpy_s(m_JA_MemberSeatingListOld[i].district, tmpStr);
        }
        else {
           strcpy_s(m_JA_MemberSeatingListOld[i].district, "0");
           strcat_s(m_JA_MemberSeatingListOld[i].district, tmpStr);
        }

        strcpy_s(m_JA_MemberSeatingListOld[i].party, m_JA_PartyList[seatRecord[36] - '0']);

        JaTrim(m_JA_MemberSeatingListOld[i].name); // Remove unwanted spaces
    }

    fclose(seatHandle);
}


void JaLoadParmFile(void)
{
    char outBuff[81];
    char inBuff[81];

    char parmFilename[] = "V:\\parms.mn";
    FILE *parmHandle;

    if (!(parmHandle = fopen(parmFilename, "r"))) {
        wsprintf(outBuff, "JaLoadParmFile - Unable to open %s!", parmFilename);
        AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
        return;
    }

    if (!(fgets(inBuff, 81, parmHandle))) {
        wsprintf(outBuff, "JaLoadParmFile - Missing Seating Date in %s!", parmFilename);
        AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
        fclose(parmHandle);
        return;
    }
    inBuff[strlen(inBuff) - 1] = 0;     // Eliminate the newline 
    m_JA_SeatingDate = inBuff;

    if (!(fgets(inBuff, 81, parmHandle))) {
        wsprintf(outBuff, "JaLoadParmFile - Missing Title Line One in %s!", parmFilename);
        AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
        fclose(parmHandle);
        return;
    }
    inBuff[strlen(inBuff) - 1] = 0;     // Eliminate the newline 
    m_JA_VPTitle1 = inBuff;

    if (!(fgets(inBuff, 81, parmHandle))) {
        wsprintf(outBuff, "JaLoadParmFile - Missing Title Line Two in %s!", parmFilename);
        AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
        fclose(parmHandle);
        return;
    }
    inBuff[strlen(inBuff) - 1] = 0;     // Eliminate the newline 
    m_JA_VPTitle2 = inBuff;

    if (!(fgets(inBuff, 81, parmHandle))) {
        wsprintf(outBuff, "JaLoadParmFile - Missing Title Line Three in %s!", parmFilename);
        AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
        fclose(parmHandle);
        return;
    }
    inBuff[strlen(inBuff) - 1] = 0;     // Eliminate the newline 
    m_JA_VPTitle3 = inBuff;

    if (!(fgets(inBuff, 81, parmHandle))) {
        wsprintf(outBuff, "JaLoadParmFile - Missing Session Number in %s!", parmFilename);
        AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
        fclose(parmHandle);
        return;
    }
    inBuff[strlen(inBuff) - 1] = 0;     // Eliminate the newline 
    m_JA_SessionNumber = atoi(inBuff);

    fclose(parmHandle);
}


void JaLoadSequenceNumber(void)
{
    char outBuff[81];

    unsigned long seqNumber;

    FILE *seqHandle;

    if (!(seqHandle = fopen(JaSequence, "r"))) {
        wsprintf(outBuff, "Sequence File <%s> does not exist! \n Creating a new one", JaSequence);
        AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
        return;
    }

    fscanf(seqHandle, "%ld", &seqNumber);

    m_JA_SequenceNumber = seqNumber;

    fclose(seqHandle);

    return;
}


void JaStoreSequenceNumber(void)
{
    char outBuff[81];

    FILE *seqHandle;

    if (!(seqHandle = fopen(JaSequence, "w"))) {
        wsprintf(outBuff, "Unable to update Sequence File <%s>!", JaSequence);
        AfxMessageBox(outBuff, MB_ICONEXCLAMATION);
        return;
    }

    fprintf(seqHandle, "%ld", m_JA_SequenceNumber);

    fclose(seqHandle);

    return;
}


void JaStoreIniFile(void)
{
    JaStoreParameters();
    JaStoreSeating();
}


void JaStoreParameters(void)
{
    char section[32];
    char outBuff[81];

    wsprintf(section, "Parameters");

    WritePrivateProfileString (section, " SeatingDate",   m_JA_SeatingDate, JaVoteIni); 
    WritePrivateProfileString (section, " VPTitle1",      m_JA_VPTitle1,    JaVoteIni); 
    WritePrivateProfileString (section, " VPTitle2",      m_JA_VPTitle2,    JaVoteIni); 
    WritePrivateProfileString (section, " VPTitle3",      m_JA_VPTitle3,    JaVoteIni); 

    wsprintf(outBuff, "%.d", m_JA_SessionNumber);
    WritePrivateProfileString (section, " SessionNumber", outBuff, JaVoteIni); 

    wsprintf(outBuff, "%.d", m_JA_SpecialSession);
    WritePrivateProfileString (section, " SpecialSession", outBuff, JaVoteIni); 
}

void JaUpdateSeating(void)
{
    for (int i = 1; i < (MAX_SEATS + 1); i++) {
        strcpy_s(m_JA_MemberSeatingListOld[i].name, m_JA_MemberSeatingListNew[i].name);
        m_JA_MemberSeatingListOld[i].seat = m_JA_MemberSeatingListNew[i].seat;
        strcpy_s(m_JA_MemberSeatingListOld[i].district, m_JA_MemberSeatingListNew[i].district);
        strcpy_s(m_JA_MemberSeatingListOld[i].party, m_JA_MemberSeatingListNew[i].party);
		strcpy_s(m_JA_MemberSeatingListOld[i].county, m_JA_MemberSeatingListNew[i].county);
    }
}

void JaStoreSeating(void)
{
    char section[32];
    char key[32];
    char outBuff[81];

    // Output the new data to the seating file
    for (int i = 1; i < (MAX_SEATS + 1); i++) {

        wsprintf(section, "MemberSeat_%03.3d", i);

        wsprintf(key, " Name");
        wsprintf(outBuff, "%-30.30s", m_JA_MemberSeatingListOld[i].name);
        WritePrivateProfileString (section, key, NULL, JaVoteIni); 
        WritePrivateProfileString (section, key, outBuff, JaVoteIni); 

        wsprintf(key, " District");
        wsprintf(outBuff, "%3.3s", m_JA_MemberSeatingListOld[i].district);
        WritePrivateProfileString (section, key, NULL, JaVoteIni); 
        WritePrivateProfileString (section, key, outBuff, JaVoteIni); 

        wsprintf(key, " Party");
        wsprintf(outBuff, "%-30.30s", m_JA_MemberSeatingListOld[i].party);
        WritePrivateProfileString (section, key, NULL, JaVoteIni); 
        WritePrivateProfileString (section, key, outBuff, JaVoteIni);

		wsprintf(key, " County");
		wsprintf(outBuff, "%-30.30s", m_JA_MemberSeatingListOld[i].county);
		WritePrivateProfileString(section, key, NULL, JaVoteIni);
		WritePrivateProfileString(section, key, outBuff, JaVoteIni);
    }
}


void JaLoadParametersAndSeating(void)
{
    FILE *tmpFile;

    char section[32];
    char key[32];
    char inBuff[81];

    // Does JaVote.Ini file exist?
    if ((tmpFile = fopen( JaVoteIni, "r+" )) == NULL ) {
       return;
    }
    else {
       fclose(tmpFile);
    }

    wsprintf(section, "Parameters");

    GetPrivateProfileString (section, " SeatingDate", "970103", inBuff, 80,  JaVoteIni); 
    m_JA_SeatingDate = inBuff;

    GetPrivateProfileString (section, " VPTitle1",    "Title1",  inBuff, 80, JaVoteIni); 
    m_JA_VPTitle1 = inBuff;
    GetPrivateProfileString (section, " VPTitle2",    "Title2",  inBuff, 80, JaVoteIni); 
    m_JA_VPTitle2 = inBuff;
    GetPrivateProfileString (section, " VPTitle3",    "Title3",  inBuff, 80, JaVoteIni); 
    m_JA_VPTitle3 = inBuff;

    GetPrivateProfileString (section, " SessionNumber", "9999",  inBuff, 80, JaVoteIni); 
    m_JA_SessionNumber = atoi(inBuff);

    GetPrivateProfileString (section, " SpecialSession", "0",  inBuff, 80, JaVoteIni); 
    m_JA_SpecialSession = atoi(inBuff);

    for (int i = 1; i < (MAX_SEATS + 1); i++) {
        wsprintf(section, "MemberSeat_%03.3d", i);

        wsprintf(key, " Name");
        GetPrivateProfileString (section, key, "Hornsworthy", inBuff, 80, JaVoteIni); 
        strcpy_s(m_JA_MemberSeatingListOld[i].name, inBuff);

        wsprintf(key, " District");
        GetPrivateProfileString (section, key, "66A", inBuff, 80, JaVoteIni); 
        strcpy_s(m_JA_MemberSeatingListOld[i].district, inBuff);

        wsprintf(key, " Party");
        GetPrivateProfileString (section, key, "Wig", inBuff, 80, JaVoteIni); 
        strcpy_s(m_JA_MemberSeatingListOld[i].party, inBuff);

		wsprintf(key, " County");
		GetPrivateProfileString(section, key, "Ramsey", inBuff, 80, JaVoteIni);
		strcpy_s(m_JA_MemberSeatingListOld[i].county, inBuff);
    }
}
