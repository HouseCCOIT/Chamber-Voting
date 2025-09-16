#pragma once
using namespace Opto22::OptoMMP4;

ref class JaOpto
{
public:
   JaOpto();
   ~JaOpto();

   unsigned int Initialize(void);
   bool readSwitchLatches(unsigned int module);
   unsigned int voteInput(unsigned int seat);
   unsigned int voteDeskLights(unsigned int seat, unsigned int voteIn);
   unsigned int voteBoardLights(unsigned int seat, unsigned int voteIn);
   unsigned int voteCloseCheck(void);
   unsigned int voteLockCheck(void);
   unsigned int voteOpenCheck(bool resetLatch);
   unsigned int votePrintCheck(void);
   unsigned int voteResetCheck(void);
   bool voteOn(void);
   bool voteOff(void);
   bool displayOn(void);
   bool displayOff(void);
   System::String ^getException();

private:
   array<OptoMMP^>^ objMmp;
   OptoMMP ^inOpto;
   OptoMMP ^outOpto;
   System::String ^exString;

   unsigned long long seatInputMask;
};

