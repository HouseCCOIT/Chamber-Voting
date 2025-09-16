#pragma once
using namespace Opto22::OptoMMP4;
using namespace System;

ref class JaOpto
{
public:
   JaOpto();
   ~JaOpto();

   unsigned int Initialize(void);
   bool collect(void);
   bool readSwitchLatches(unsigned int module);
   unsigned int voteInput(unsigned int seat);
   unsigned int voteDeskLights(unsigned int seat, unsigned int voteIn);
   unsigned int voteBoardLights(unsigned int seat, unsigned int voteIn, unsigned int seat2);
   unsigned int voteCloseCheck(void);
   unsigned int voteLockCheck(void);
   unsigned int voteOpenCheck(bool resetLatch);
   unsigned int votePrintCheck(void);
   unsigned int voteResetCheck(void);
   bool displayOn(void);
   bool displayOff(void);
   bool getMotionSwitches();
   bool setMotionLights(bool LightsOn);
   bool resetMotionLights(void);

   System::String ^getException();

private:
   array<OptoMMP^>^ objMmp;
   OptoMMP ^inOpto;
   OptoMMP ^outOpto;
   System::String ^exString;

   unsigned long long seatInputMask;

   array<bool>^ motion;
};

