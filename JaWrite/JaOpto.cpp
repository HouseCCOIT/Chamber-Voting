#include "JaOpto.h"
#include "house.h"

//#define TEST_ONLY 1
#ifdef TEST_ONLY
#define numModules 1
#else
#define numModules 16
#endif

// 0  "192.168.10.20" - Desk Switches
// 1  "192.168.10.21"
// 2  "192.168.10.22"
// 3  "192.168.10.23"
// 4  "192.168.10.24"
// 5  "192.168.10.25"
// 6  "192.168.10.26"
// 7  "192.168.10.27" - Desk Lights
// 8  "192.168.10.28"
// 9  "192.168.10.29"
// 10 "192.168.10.30"
// 11 "192.168.10.31" - Desk Lights + Name Board Lights
// 12 "192.168.10.32" - Name Board Lights
// 13 "192.168.10.33"
// 14 "192.168.10.34"
// 15 "192.168.10.35"


JaOpto::JaOpto()
{
   int i32Result;

   int module = 20;

   objMmp = gcnew array<OptoMMP^>(numModules);
   motion = gcnew array<bool>(4);

   for (int i = 0; i < numModules; i++) {
      System::String ^IPAddr = "192.168.10." + module.ToString();

      objMmp[i] = gcnew OptoMMP;

	  for (int ii = 0; ii < 100; ii++) {
		  array<OptoMMP^>^ objMmp2 = gcnew array<OptoMMP^>(numModules);
	  }

      //i32Result = objMmp[i]->Open(IPAddr, 2001, OptoMMP::Connection::Udp, 6000, true);

      //if (i32Result < 0) {
      //   System::String ^Error = "";
      //   exString = "Blah";

      //   bool commFault, protocolFault, argumentFault;

      //   objMmp[i]->GetExtendedErrorInformation(exString, commFault, protocolFault, argumentFault);

      //   if (commFault) {
      //      exString += " - " + "Comm Fault on " + IPAddr;
      //   }
      //   else if (protocolFault) {
      //      exString += " - " + "Protocol Fault on " + IPAddr;
      //   }
      //   else if(argumentFault) {
      //      exString += " - " + "Argument Fault on " + IPAddr;
      //   }
      //   else {
      //      exString += " - " + "Unknown Fault on " + IPAddr;
      //   }
      //   //throw(exString); // TBD - figure out how to throw a String
      //   throw(i);
      //}

      module++;
   }
}
JaOpto::~JaOpto() 
{
   // Close all connections to Opto gear
   for (int i = 0; i < numModules; i++) {
      objMmp[i]->Close();
      delete[] objMmp[i];
   }
}

System::String ^JaOpto::getException()
{
   return exString;
}

unsigned int JaOpto::Initialize(void) 
{
   bool latchState = false;
   static bool initialStartup = true;

   //if (m_TestMode) {
   //   if (m_TestVoteOpen) {
   //      return 0x10;
   //   }
   //   else {
   //      return 0x00;
   //   }
   //}

   // Clear all Desk Switch latches
   for (int i = 0; i < 7; i++) {
      for (int k = 0; k < 64; k++) {
         System::String ^Point = "D" + k.ToString();
         objMmp[i]->WriteDigitalPointConfiguration64(k, false, (OptoMMP::eDigitalFeature)0, false, false, Point);
         objMmp[i]->ReadClearDigitalLatch64(k, true, latchState);  // Read and Clear the ON latch
         objMmp[i]->ReadClearDigitalLatch64(k, false, latchState); // Read and Clear the OFF latch
      }
   }
   for (int k = 36; k < 43; k++) { // vo vl pr rt tl on off switches
      System::String ^Point = "D" + k.ToString();
      objMmp[11]->WriteDigitalPointConfiguration64(k, false, (OptoMMP::eDigitalFeature)0, false, false, Point);
      objMmp[11]->ReadClearDigitalLatch64(k, true, latchState);  // Read and Clear the ON latch
      objMmp[11]->ReadClearDigitalLatch64(k, false, latchState); // Read and Clear the OFF latch
   }
   for (int k = 44; k < 48; k++) { // motion switches
      System::String ^Point = "D" + k.ToString();
      objMmp[11]->WriteDigitalPointConfiguration64(k, false, (OptoMMP::eDigitalFeature)0, false, false, Point);
      if (initialStartup) {
         objMmp[11]->ReadClearDigitalLatch64(k, true, latchState);  // Read and Clear the ON latch
         objMmp[11]->ReadClearDigitalLatch64(k, false, latchState); // Read and Clear the OFF latch
      }
   }

   // Clear all Desk Light latches
   for (int i = 7; i < 11; i++) {
      for (int k = 0; k < 64; k++) {
         System::String ^Point = "D" + k.ToString();
         objMmp[i]->WriteDigitalPointConfiguration64(k, true, (OptoMMP::eDigitalFeature)0, false, false, Point);
         objMmp[i]->WriteDigitalState64(k, false);  // Turn output off
      }
   }
   for (int k = 0; k < 16; k++) { // Remaining desk lights on IP address 192.168.10.31
      System::String ^Point = "D" + k.ToString();
      objMmp[11]->WriteDigitalPointConfiguration64(k, true, (OptoMMP::eDigitalFeature)0, false, false, Point);
      objMmp[11]->WriteDigitalState64(k, false);  // Turn output off
   }

   // Clear all Name Board latches
   for (int i = 12; i < 16; i++) {
      for (int k = 0; k < 64; k++) {
         System::String ^Point = "D" + k.ToString();
         objMmp[i]->WriteDigitalPointConfiguration64(k, true, (OptoMMP::eDigitalFeature)0, false, false, Point);
         objMmp[i]->WriteDigitalState64(k, false);  // Turn output off
      }
   }
   for (int k = 16; k < 32; k++) { // Remaining name board lights
      System::String ^Point = "D" + k.ToString();
      objMmp[11]->WriteDigitalPointConfiguration64(k, true, (OptoMMP::eDigitalFeature)0, false, false, Point);
      objMmp[11]->WriteDigitalState64(k, false);  // Turn output off
   }

   // Clear Vote Open and Vote Lock latches
   for (int k = 32; k < 34; k++) { // 32 && 33 = Vote Open and Vote Lock Lights
      System::String ^Point = "D" + k.ToString();
      objMmp[11]->WriteDigitalPointConfiguration64(k, true, (OptoMMP::eDigitalFeature)0, false, false, Point);
      objMmp[11]->WriteDigitalState64(k, false);  // Turn output off
   }
   // Clear Motion latches
   for (int k = 48; k < 52; k++) { // 48, 49, 50, and 51 = Motion Lights
      System::String ^Point = "D" + k.ToString();
      objMmp[11]->WriteDigitalPointConfiguration64(k, true, (OptoMMP::eDigitalFeature)0, false, false, Point);
      objMmp[11]->WriteDigitalState64(k, false);  // Turn output off
   }
   if (initialStartup) {
      // Clear End Display Power latches
      for (int k = 52; k < 56; k++) { // 52, 53, 54, and 56 = Display switches
         System::String ^Point = "D" + k.ToString();
         objMmp[11]->WriteDigitalPointConfiguration64(k, true, (OptoMMP::eDigitalFeature)0, false, false, Point);
         objMmp[11]->WriteDigitalState64(k, false);  // Turn output off
      }
   }

   resetMotionLights();

   initialStartup = false;

   return 0;
}

bool JaOpto::collect(void) 
{
	//System::coll //
	
	GC::Collect();

	return true;
}

bool JaOpto::readSwitchLatches(unsigned int module)
{
   // Get the state of the switches for this module

   inOpto = objMmp[module];

   inOpto->ReadClearDigitalLatches64(true, seatInputMask);

   return true;
}

unsigned int JaOpto::voteInput(unsigned int seat)
{
   unsigned int voteIn = 4;

   // Get the vote for this module

   //inOpto = objMmp[seat / 21]; // Convert seat # to module index

   for (int i = 0; i < 3; i++) {
      bool latchOn = true;
      unsigned int shift = 0;

      shift = ((seat % 21) * 3) + i;

      latchOn = (seatInputMask >> shift) & 0x01;

      // inOpto->ReadClearDigitalLatch64((seat % 21) * 3 + i, true, latchOn);

      if (latchOn) {
		 //Test System::Threading::Thread::Sleep(5);
         switch (i % 3) {
            bool latchState;
            case 0:
               voteIn = 2; // Yea
               inOpto->ReadClearDigitalLatch64((seat % 21) * 3 + 1, true, latchState);
               inOpto->ReadClearDigitalLatch64((seat % 21) * 3 + 2, true, latchState);
               break;

            case 1:
               voteIn = 1; // Nay
               inOpto->ReadClearDigitalLatch64((seat % 21) * 3 + 0, true, latchState);
               inOpto->ReadClearDigitalLatch64((seat % 21) * 3 + 2, true, latchState);
               break;

            case 2:
               voteIn = 0; // Reset
               inOpto->ReadClearDigitalLatch64((seat % 21) * 3 + 0, true, latchState);
               inOpto->ReadClearDigitalLatch64((seat % 21) * 3 + 1, true, latchState);
               break;

            default:
               break;
         }
         break; // exit for loop
      }
   }
 
   return voteIn;
}

unsigned int JaOpto::voteDeskLights(unsigned int seat, unsigned int voteIn)
{
   // Set the desk lights for this seat

	bool latchOn[3];
	unsigned int shift = 0;

	for (int i = 0; i < 3; i++) {
		shift = ((seat % 21) * 3) + i;
		latchOn[i] = (seatInputMask >> shift) & 0x01;
	}

	if (latchOn[0] || latchOn[1] || latchOn[2]) {
		//Test System::Threading::Thread::Sleep(5);
		outOpto = objMmp[(seat / 32) + 7]; // Convert seat # to module index

		unsigned int index = ((seat % 32) * 2);

		switch (voteIn) {
		case 0: // Reset
			outOpto->WriteDigitalState64(index + 0, false);
			outOpto->WriteDigitalState64(index + 1, false);
			break;

		case 1: // Nay 
			outOpto->WriteDigitalState64(index + 0, false);
			outOpto->WriteDigitalState64(index + 1, true);
			break;

		case 2: // Yea
			outOpto->WriteDigitalState64(index + 0, true);
			outOpto->WriteDigitalState64(index + 1, false);
			break;

		default:
			break;
		}
	}

   return voteIn;
}

unsigned int JaOpto::voteBoardLights(unsigned int seat, unsigned int voteIn, unsigned int seat2)
{
	// Set the desk lights for this seat

	bool latchOn[3];
	unsigned int shift = 0;

	for (int i = 0; i < 3; i++) {
		shift = ((seat2 % 21) * 3) + i;
		latchOn[i] = (seatInputMask >> shift) & 0x01;
	}

	if (latchOn[0] || latchOn[1] || latchOn[2]) {
	   unsigned int index = 0;

	   if ((seat >= 0) && (seat < 32)) {
		   outOpto = objMmp[12]; // Convert seat # to module index
		   index = (((seat) % 32) * 2);
	   }
	   else if ((seat >= 32) && (seat < 64)) {
		   outOpto = objMmp[13]; // Convert seat # to module index
		   index = (((seat) % 32) * 2);
	   }
	   else if ((seat >= 64) && (seat < 96)) {
		   outOpto = objMmp[14]; // Convert seat # to module index
		   index = (((seat) % 32) * 2);
	   }
	   else if ((seat >= 96) && (seat < 128)) {
		   outOpto = objMmp[15]; // Convert seat # to module index
		   index = (((seat) % 32) * 2);
	   }
	   else if ((seat >= 128) && (seat <= 136)) {
		   outOpto = objMmp[11]; // Convert seat # to module index
		   index = ((seat % 32) * 2 + 16);
	   }

	   switch (voteIn) {
	   case 0: // Reset
		   outOpto->WriteDigitalState64(index + 0, false);
		   outOpto->WriteDigitalState64(index + 1, false);
		   break;

	   case 1: // Nay 
		   outOpto->WriteDigitalState64(index + 0, false);
		   outOpto->WriteDigitalState64(index + 1, true);
		   break;

	   case 2: // Yea
		   outOpto->WriteDigitalState64(index + 0, true);
		   outOpto->WriteDigitalState64(index + 1, false);
		   break;

	   default:
		   break;
	   }
   }

   return voteIn;
}

unsigned int JaOpto::voteCloseCheck(void)
{
   short vote_close = 0;
   bool latchOn = false;

   inOpto = objMmp[11]; // 31

   // Check Print Results button

   inOpto->ReadClearDigitalLatch64(38, false, latchOn);  // Print Result - Leave latched

   if (latchOn) {
      inOpto->ReadClearDigitalLatch64(39, true, latchOn);  // Clear Reset Latch
      inOpto->ReadClearDigitalLatch64(39, false, latchOn); // Clear Reset Latch
      inOpto->ReadClearDigitalLatch64(40, true, latchOn);  // Clear Totals Only Latch
      vote_close = 0x80;  // Print Result
   }

   //Check Totals Only button

   inOpto->ReadClearDigitalLatch64(40, false, latchOn);  // Totals Only - Leave latched

   if (latchOn) {
      inOpto->ReadClearDigitalLatch64(39, true, latchOn);  // Clear Reset Latch
      inOpto->ReadClearDigitalLatch64(39, false, latchOn); // Clear Reset Latch
      inOpto->ReadClearDigitalLatch64(38, true, latchOn);  // Clear Print Result latch
      vote_close = 0x01;  // Totals Only
   }

   if (vote_close & 0x80) {
      return(PRINT_RESULT);
   }
   else if (vote_close & 0x01) {
      return(TOTALS_ONLY);
   }
   else {
      return(0);
   }
}

unsigned int JaOpto::voteLockCheck(void)
{
   short vote_lock = 0;
   
   inOpto  = objMmp[11]; // 31
   outOpto = objMmp[11]; // 31

   // Read Vote Lock buttons (There are two of them)

   bool latchOn = false;
   inOpto->ReadClearDigitalLatch64(37, false, latchOn); // Leave latched

   if (latchOn) {
      inOpto->ReadClearDigitalLatch64(36, true, latchOn);  // Clear Vote Open Latch
      inOpto->ReadClearDigitalLatch64(38, true, latchOn);  // Clear Print Result latch
      inOpto->ReadClearDigitalLatch64(38, false, latchOn); // Clear Print Result latch
      inOpto->ReadClearDigitalLatch64(39, true, latchOn);  // Clear Reset Latch
      inOpto->ReadClearDigitalLatch64(39, false, latchOn); // Clear Reset Latch
      inOpto->ReadClearDigitalLatch64(40, true, latchOn);  // Clear Totals Only Latch
      inOpto->ReadClearDigitalLatch64(40, false, latchOn); // Clear Totals Only Latch
      outOpto->WriteDigitalState64(33, (bool)true);  // turn on vote lock LED
      outOpto->WriteDigitalState64(32, (bool)false); // Turn off vote open LED
      vote_lock = 0x20;
   }

   return (vote_lock);
}

unsigned int JaOpto::voteOpenCheck(bool resetLatch)
{
   short vote_open = 0;

   //// TBD - Do we need test mode?
   //if (m_TestMode) {
   //   if (m_TestVoteOpen) {
   //      return 0x10;
   //   }
   //   else {
   //      return 0x00;
   //   }
   //}

   inOpto  = objMmp[11]; // 31
   outOpto = objMmp[11]; // 31

   // Read Vote Open buttons (There are two of them)
   bool latchOn = false;
   inOpto->ReadClearDigitalLatch64(36, false, latchOn); // Leave latched

   if (latchOn) {
      inOpto->ReadClearDigitalLatch64(37, true, latchOn); // clear latch on Vote Lock
      outOpto->WriteDigitalState64(32, (bool)true);  // Turn on vote open LED
      outOpto->WriteDigitalState64(33, (bool)false); // Turn off vote lock LED
      vote_open = 0x10;
   }

   return(vote_open);
}

unsigned int JaOpto::votePrintCheck(void)
{
   short vote_print = 0;

   inOpto = objMmp[11]; // 31

   bool latchOn = false;
   inOpto->ReadClearDigitalLatch64(38, false, latchOn); // Leave latched

   if (latchOn) {
      inOpto->ReadClearDigitalLatch64(39, true, latchOn); // Clear Reset Latch
      inOpto->ReadClearDigitalLatch64(39, false, latchOn); // Clear Reset Latch
      inOpto->ReadClearDigitalLatch64(40, true, latchOn);  // Clear Totals Only Latch
      vote_print = 0x80;
   }

   return(vote_print);
}

unsigned int JaOpto::voteResetCheck(void)
{
   short vote_reset = 0;

   inOpto = objMmp[11]; // 31

   // Read Vote Lock buttons (There are two of them)
   bool latchOn = false;
   inOpto->ReadClearDigitalLatch64(39, false, latchOn); // Leave latched

   if (latchOn) {
      // TBD outOpto->WriteDigitalState64(TBD, true);
      vote_reset = 0x02;
   }

   return(vote_reset);
}

bool JaOpto::displayOn(void)
{
   outOpto = objMmp[11]; // 31

   outOpto->WriteDigitalState64(52, true);
   outOpto->WriteDigitalState64(53, false);
   outOpto->WriteDigitalState64(54, true);
   outOpto->WriteDigitalState64(55, false);

   return(true);
}

bool JaOpto::displayOff(void)
{
   outOpto = objMmp[11]; // 31

   outOpto->WriteDigitalState64(52, false);
   outOpto->WriteDigitalState64(53, true);
   outOpto->WriteDigitalState64(54, false);
   outOpto->WriteDigitalState64(55, true);

   return(true);
}

bool JaOpto::getMotionSwitches(void)
{
   bool success = false;

   inOpto = objMmp[11]; // 31

   // Read Motion switches
   for (unsigned int i = 0; i < 4; i++) {
      bool latchOn = false;
      bool latchOff = false;
      inOpto->ReadDigitalLatch64(44 + i, latchOn, latchOff);

      if (latchOn) {
         motion[i] = true;
      }
      if (latchOff) {
         inOpto->ReadClearDigitalLatch64(44 + i, true, latchOn);
         inOpto->ReadClearDigitalLatch64(44 + i, false, latchOff);
         motion[i] = false;
      }
   }

   success = true;

   return(success);
}

bool JaOpto::setMotionLights(bool lightOn)
{
   bool success = false;

   outOpto = objMmp[11]; // 31

   // Turn Motion lights on or off
   for (unsigned int i = 0; i < 4; i++) {
      if (motion[i] && lightOn) {
         outOpto->WriteDigitalState64(48 + i, true);
      }
      else {
         outOpto->WriteDigitalState64(48 + i, false);
      }
   }

   success = true;

   return(success);
}

bool JaOpto::resetMotionLights(void)
{
   bool success = false;

   outOpto = objMmp[11]; // 31

   // Turn Motion lights off
   for (unsigned int i = 0; i < 4; i++) {
      outOpto->WriteDigitalState64(48 + i, false);
      motion[i] = false;
   }

   success = true;

   return(success);
}