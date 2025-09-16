// groov EPIC.cpp : Defines the entry point for the console application.
//
#ifdef _WIN32
#include "targetver.h"
#include <tchar.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string>

#ifdef _WIN32
#include <WS2tcpip.h>
#include <WinSock2.h>
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include "O22SIOMM.h"
#include "O22STRCT.h"

/*
	Installed Module and Locations
	Module 0	GRV-IDC-24
	Module 4	GRV-ODCSRC-24
	Module 9	GRV-ITMI-8
	              K-Thermocouple installed on channel 0
	Module 10   GRV-OVMALC-8
*/

int main(int nArgs, char *pcharyArgs[])
{
	O22SnapIoMemMap	objEpic;
	const int i32cGrvIdc24Module = 0;
	const int i32cGrvOdc24SrcModule = 4;
	const int i32cGrvItmi8Module = 9;
	const int i32cChannel0 = 0;
	const int i32cChannel2 = 2;
	const int i32cChannel3 = 3;
	int nResult;

	// check if there are enough arguments...
	if (nArgs < 2)
	{
		printf("please provide an ip address or hostname; groovEPIC opto-06-01-22 or groovEPIC 10.1.1.3\n");
		exit(0);
	}

	// open a TCP connection
	nResult = objEpic.OpenEnet2(pcharyArgs[1], 2001, 10000, 1, SIOMM_TCP);
	if (nResult != SIOMM_OK) {
		printf("OpenEnet2 reports result %d, exiting.\n", nResult);
		exit(0);
	}

	// wait for the TCP connect to complete, not necessary for UDP
#ifdef _WIN32
	Sleep(100);
#elif _LINUX
	usleep(100000);
#endif

	// configure a discrete point on a GRV-IDC-24 (module 0), channel 2 as a counter
	// see 1465, Appendix A for additional information for the point feature enumerations
	{
		nResult = objEpic.SetPointFeatureEx(i32cGrvIdc24Module, i32cChannel2, 1);
		if (nResult != SIOMM_OK) {
			printf("SetPointFeatureEx reports result %d, exiting.\n", nResult);
			exit(0);
		}
		int i32FeatureEnum = 0;
		nResult = objEpic.GetPointFeatureEx(i32cGrvIdc24Module, i32cChannel2, &i32FeatureEnum);
		if (nResult != SIOMM_OK) {
			printf("GetPointFeatureEx reports result %d, exiting.\n", nResult);
			exit(0);
		}
		printf("GetPointFeatureEx reports feature %d.\n", i32FeatureEnum);
	}

	// read all of the discrete states, channel qualities, and latches.
	{
		SIOMM_EpicPackedDigitalRead arystructEpicPackedDigital[16];
		nResult = objEpic.EpicGetDigitalPacked(arystructEpicPackedDigital);
		if (nResult != SIOMM_OK) {
			printf("EpicGetDigitalPacked reports result %d, exiting.\n", nResult);
			exit(0);
		}
		// print a state of a specific point
		bool bChannelState = (arystructEpicPackedDigital[i32cGrvIdc24Module].un.mod.uStates & (0x1 << i32cChannel3)) != 0;
		printf("GRV-IDC-24 (From Packed) Channel %d State is %s\n", i32cChannel3,
			bChannelState ? "on" : "off");
	}

	// read a discrete point from a GRV-IDC-24 (module 0), channel 2
	{
		SIOMM_EpicDigitalChannelRead objEpicChannel;
		nResult = objEpic.EpicGetDigitalChannel(i32cGrvIdc24Module, i32cChannel2, &objEpicChannel);
		if (nResult != SIOMM_OK) {
			printf("EpicGetDigitalChannel reports result %d, exiting.\n", nResult);
			exit(0);
		}
		// emit the feature value (the counter)
		printf("GRV-IDC-24 Channel %d State is %s Feature Value is %u\n", i32cChannel2,
			objEpicChannel.un.uAttributes.uState ? "on" : "off",
			objEpicChannel.un.uAttributes.un.uValue);
	}

	// configure an analog point
	{
		nResult = objEpic.SetPointFeatureEx(i32cGrvItmi8Module, i32cChannel0, 0x6000000f);
		if (nResult != SIOMM_OK) {
			printf("SetPointFeatureEx reports result %d, exiting.\n", nResult);
			exit(0);
		}
	}
	// if the point feature changed, allow the value to update
#ifdef _WIN32
	Sleep(500);
#elif _LINUX
	usleep(500000);
#endif

	// read the analog point value
	{
		float f32Value;
		nResult = objEpic.GetAnalogPointValueEx(i32cGrvItmi8Module, i32cChannel0, &f32Value);
		if (nResult != SIOMM_OK) {
			printf("GetAnalogPointValueEx reports result %d, exiting.\n", nResult);
			exit(0);
		}
		printf("GetAnalogPointValueEx reports module %d, channel %d value is %f.\n", i32cGrvItmi8Module, i32cChannel0, f32Value);
	}

	// fetch module quality
	{
		SIOMM_ModuleQuality structModuleQuality;
		nResult = objEpic.EpicGetModuleQuality(&structModuleQuality);
		if (nResult != SIOMM_OK) {
			printf("EpicGetModuleQuality reports result %d, exiting.\n", nResult);
			exit(0);
		}
		printf("Module Quality Bitmask %08x.\n", structModuleQuality.uModules31To00Bitmask);
	}

	// fetch channel quality
	{
		SIOMM_ChannelQuality structChannelQuality;
		nResult = objEpic.EpicGetChannelQuality(i32cGrvOdc24SrcModule, &structChannelQuality);
		if (nResult != SIOMM_OK) {
			printf("EpicGetChannelQuality reports result %d, exiting.\n", nResult);
			exit(0);
		}
		for (int i = 0; i < 24; i++)
		{
			printf("Module %d, Channel %d; Quality = %08x.\n", i32cGrvOdc24SrcModule, i, structChannelQuality.uaryChannelQuality[i]);
		}
	}
	return 0;
}

