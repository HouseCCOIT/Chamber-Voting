
OptoMMP C++ Software Development Kit
Copyright Opto 22 2018

Part Number: PAC-DEV-OPTOMMP-CPLUS

Date:    May 18, 2018
Version: 4.0a


==========================================================================

Introduction

This file describes Opto 22's OptoMMP C++ Software Development Kit, a toolkit
provided by Opto 22 for communicating with devices that support the OptoMMP
Protocol. These devices include groov EPIC, SNAP PAC brains and controllers,
older Ethernet-based brains and controllers, and E1 and E2 brain boards.

The toolkit consists of C++ classes that make it easy to communicate with
these Opto 22 devices by hiding the details of Ethernet communications
and the OptoMMP protocol.

Examples are included of using the toolkit in Visual C++, and Linux.

==========================================================================

Documentation

Documentation can be found in the "Documentation" folder. Double-click
index.html to open. The complete memory map is documented in the
"OptoMMP Protocol Guide" (Form #1465), which may be found on the
Opto 22 Web site at
http://www.opto22.com/documents/1465_OptoMMP_Protocol_Guide.pdf

==========================================================================

= RELEASE HISTORY =

== Version: 4.0a; May 18, 2018 ==

SDK named as "PAC-DEV-OPTOMMP-CPLUS".

This SDK is a strictly C++ OptoMMP SDK. ActiveX is no longer provided
with this package.

Visual Studio 2015 Professional Update 3 was used to build and validate
the _WIN32 portion of the code. Windows builds are unmanaged code.

Ubuntu 16.04 LTS/g++ 5.4.0 was used to test the _LINUX portion of the
code.

New methods in O22SnapIoMemMap:
	=== Specifically groov EPIC ===
	EpicGetDigitalChannel()
	EpicGetDigitalPacked()
	EpicGetAnalogChannel()
	EpicGetModuleQuality()
	EpicGetChannelQuality()

	=== SNAP-PAC ===
	SetPointFeatureEx() - includes EPIC
	GetPointFeatureEx() - includes EPIC
	GetCustomAreaAddresses()
	SetCustomAreaAddresses()
	GetCustomAreaData()
	SetCustomAreaData()

	=== Any Byte Oriented Areas ===
	WriteBytes()
	ReadBytes()

== Version: 3.0b   11/07/2014 ==

  FIXES:
    + The Makefile necessary for building the library under Linux or Mac OS X was missing
      in the R3.0a release.

Version: 3.0a   09/01/2013

  NEW FEATURES and ENHANCEMENTS:

    + The toolkit was originally released as IOP-DEV-OPTOMMP for Windows development,
      providing both C++ and and ActiveX SDKs, and IOP-DEV-OPTOMMP-LX for Linux
      development, providing a C++ SDK. It is now being released as PAC-DEV-OPTOMMP-C++,
      providing a C++ SDK that works in Windows, Linux and Mac OS X, and
      PAC-DEV-OPTOMMP-ACTIVEX, providing an ActiveX SDK on Windows.

    + Improved error checking. All method inputs are now checked for validity.

    + Updated the toolkit to work properly on both 32 and 64 bit systems.

    + Added support for serial and serial pass-through connections.

    + Added an extensive C++ walkthrough tutorial that shows the use of various parts
      of the toolkit working against the SNAP PAC Learning Center hardware.

    + Added the following C++ and ActiveX methods:
        - OpenSerial
        - OpenSerialPassThru
        - ReadBlockAsIntegers
        - WriteBlockAsIntegers
        - ReadBlockAsFloats
        - WriteBlockAsFloats
        - GetStatusWriteEx
        - SetStatusWriteEx
        - GetPtConfigurationEx3
        - SetPtConfigurationEx3
        - GetHDDigitalPointState
        - GetHDDigitalPointOnLatch
        - GetHDDigitalPointOffLatch
        - GetHDDigitalPointCounts
        - SetHDDigitalPointStatue
        - ReadClearHDDigitalPointCounts
        - ReadClearHDDigitalPointOnLatch
        - ReadClearHDDigitalPointOffLatch
        - GetAnaBanksEx
        - GetAnalogPointValueEx
        - GetAnalogPointCountsEx
        - GetAnalogPointMinValueEx
        - GetAnalogPointMaxValueEx
        - GetAnalogPointTpoPeriodEx
        - GetAnalogPointReadAreaEx
        - SetAnalogPointValueEx
        - SetAnalogPointCountsEx
        - SetAnalogPointTpoPeriodEx
        - ReadClearAnalogPointMinValueEx
        - ReadClearAnalogPointMaxValueEx
        - SetPidLoopConfigurationEx
        - SetTpoConfigurationEx
        - GetTpoConfigurationEx
        - SetEventMsgConfigurationEx
        - SetDigitalEventConfigurationEx
        - SetAlarmEventConfigurationEx
        - SetSerialEventConfigurationEx
        - SetTimerEventConfigurationEx
        - GetScratchPadInteger64Area
        - SetScratchPadInteger64Area
        - GetDataLoggingSamplesEx
        - ConfigurePoint
        - GetModuleType
        - GetPointConfigurationEx4
        - GetPointGain
        - GetPointOffset
        - SetPointConfigurationEx4
        - SetPointFilterWeight
        - SetPointName
        - SetPointOffset
        - SetPointScale
        - SetPointWatchdog
        - GetDigitalPointReadAreaEx
        - CalcSetAnalogPointOffsetEx
        - CalcSetAnalogPointGainEx
        - SetAnalogOutputClamp
        - GetAnalogOutputClamp

    + Updated the Internet Explorer ActiveX demo to work properly on modern IE.

  BUG FIXES:

    + The ActiveX function CO22SnapIoMemMapX::VGetPtConfiguration was returning the wrong value for
      the watchdog enabled field.

Version: 2.1a  04/01/2006

  The toolkit has been renamed from "SNAP Ethernet I/O Driver Toolkit" to
  "OptoMMP Communication Toolkit".

  NEW FEATURES and ENHANCEMENTS:

    + Improved error checking. The following new error codes were created:
        SIOMM_ERROR_LENGTH_BAD                -11
        SIOMM_ERROR_INDEX_BAD                 -12
        SIOMM_ERROR_PARTIAL_SUCCESS           -13
        SIOMM_ERROR_INCORRECT_LENGTH_SENT     -14
        SIOMM_ERROR_UNEXPECTED_SELECT_RESULT  -15
        SIOMM_ERROR_GRACEFULLY_CLOSED         -16
        SIOMM_ERROR_WRONG_NUM_BYTES_RETURNED  -17
        SIOMM_ERROR_UNEXPECTED_RECV_RESULT    -18
        SIOMM_ERROR_INCORRECT_TCODE_RECIEVED  -19
        SIOMM_ERROR_INCORRECT_TLABEL_RECIEVED -20
        SIOMM_ERROR_UNEXPECTED_CLOSE_RESULT   -21

        Also, in some situations the socket errors from the underlying
        operating system are now returned. For Windows, this is the
        function call "WSAGetLastError()" and for Linux, it is the
        "errno" variable.

    + Added a Visual Basic .NET example using Visual Studio 2003.

    + Added the following C++ commands:
        ReadBlockAsIntegers
        ReadBlockAsFloats
        WriteBlockAsIntegers
        WriteBlockAsFloats
        GetStatusWriteEx
        SetStatusWriteEx
        GetPtConfigurationEx3
        SetPtConfigurationEx3
        SetAnaPtConfiguration
        GetAnaBanksEx
        SetPidLoopConfigurationEx
        SetTpoConfigurationEx
        SetEventMsgConfigurationEx
        SetDigitalEventConfigurationEx
        SetAlarmEventConfigurationEx
        SetSerialEventConfigurationEx
        SetTimerEventConfigurationEx
        GetDataLoggingSamplesEx


  BUG FIXES:

    + The function ReadBlock() was often returning a generic error code
      instead of a specific one.

    + The following ActiveX commands did not work properly in all
      environments, including .NET.
          ReadBlock
          SetLongAtBlockIndex
          SetFloatAtBlockIndex
          GetStreamReadArea





Version: R2.0a  August 26, 2002

  NEW FEATURES:

    + Added the following commands to the OptoSnapIoMemMapX ActiveX component
      and O22SnapIoMemMap C++ class:

      SetStreamConfiguration
      SetStreamTarget
      GetStreamConfiguration
      GetStreamTarget
      GetStreamReadAreaEx
      GetStreamReadArea

      WriteBlock            *
      ReadBlock             *
      GetLongAtBlockIndex   *
      GetFloatAtBlockIndex  *
      SetLongAtBlockIndex   *
      SetFloatAtBlockIndex  *

      OpenEnet2

      GetStatusWatchdogTime
      GetStatusHardware2     *
      GetStatusHardwareEx2   +
      GetStatusNetwork2      *
      GetStatusNetworkEx2    +

      GetDateTime
      SetDateTime

      GetSerialModuleConfigurationEx
      SetSerialModuleConfigurationEx

      SetAnaPtTpoPeriod
      GetAnaPtTpoPeriod

      SetPtConfiguration2    *
      GetPtConfiguration2    *
      SetPtConfigurationEx2  +
      GetPtConfigurationEx2  +

      GetScratchPadBitArea
      GetScratchPadIntegerArea
      GetScratchPadFloatArea
      GetScratchPadStringArea
      SetScratchPadBitArea
      SetScratchPadBitAreaMask
      SetScratchPadIntegerArea
      SetScratchPadFloatArea
      SetScratchPadStringArea
      SetScratchPadStringAreaBin   *

      * means this method was added only to the ActiveX component
      + means this method was added only to the C++ class



    + Added the OptoSnapIoStreamX ActiveX component and the
      O22SnapIoStream C++ class for receiving and handling stream packets
      from multiple SNAP brains.

    + Added UDP support. The new OptoEnet2() method has an argument for specifying
      TCP or UDP.

    + Added the following examples:

      + Microsoft Access 2000:     Demo Center
      + Borland Delphi 5:          Digital Bank
      + Visual Basic 6:            Scratch Pad, Streaming
      + Visual C++ 6.0 and Linux:  eiocl


  BUG FIXES:

    + The functions SetPtConfigurationEx() and SetPtConfiguration()
      were overwriting new fields in the point configuration area.

    + Timeouts of less than a second in the functions OpenEnet() and
      SetCommOptions() did not work correctly.



Version: R1.0b  August 11, 1999

  BUG FIXES:

    + All Demo Center examples had the wrong I/O configuration.



Version: R1.0a  June 18, 1999

  First official release.


==========================================================================

USE

The toolkit includes all source code, which may be modified, reused,
and borrowed at your own risk.  Opto 22 does not provide support for modifying
the code in this toolkit.


==========================================================================

DISTRIBUTION

The SNAP Ethernet I/O Driver Toolkit may be downloaded from the
Opto 22 Web site at http://www.opto22.com/products/driver_tool.asp.

There are two installations available:

1) For Windows, download OptoEnetDTK.exe. It will copy all files and register
   the ActiveX component.

   If you have any troubles using the toolkit, download and install
   OptoENETUtilities.exe, which will update various system files.

2) For Linux, download optoenetdtk.tar. It contains just the files
   needed for a Linux system.


==========================================================================

HOW TO GET HELP

If you have any questions about this product, contact Opto 22 Product
Support Monday through Friday, 8 a.m. to 5 p.m., Pacific Time.

Phone:  800-TEK-OPTO (835-6786)
        951-695-3080

Internet:
  Web site: www.opto22.com
  e-mail:   support@opto.com

When calling for technical support, be prepared to provide the following
information about your system to the Product Support engineer:

  - Version of this product
  - PC configuration
  - A complete description of your hardware system, including:
    - jumper configuration
    - accessories installed (such as daughter cards)
    - type of power supply
    - types of I/O units installed
    - third-party devices installed (e.g., barcode readers)
  - Controller firmware version
  - Any specific error messages seen

