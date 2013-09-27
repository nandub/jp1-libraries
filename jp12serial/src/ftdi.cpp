// ftdi.cpp : implementation file
//

/*
	Copyright (C) 2009 Graham Dixon

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "stdafx.h"
#include "ftd2xx.h"
#include "ftdi.h"

#define CBUS_CLK6 0x09

typedef FTD2XX_API FT_STATUS (WINAPI *typeFT_Open)(
	int deviceNumber,
	FT_HANDLE *pHandle
	);

typedef FTD2XX_API FT_STATUS (WINAPI *typeFT_OpenEx)(
    PVOID pArg1,
    DWORD Flags,
    FT_HANDLE *pHandle
    );

typedef FTD2XX_API FT_STATUS (WINAPI *typeFT_Close)(
    FT_HANDLE ftHandle
    );

typedef FTD2XX_API FT_STATUS (WINAPI *typeFT_CreateDeviceInfoList)(
	LPDWORD lpdwNumDevs
	);

typedef FTD2XX_API FT_STATUS (WINAPI *typeFT_GetDeviceInfoDetail)(
	DWORD dwIndex,
	LPDWORD lpdwFlags,
	LPDWORD lpdwType,
	LPDWORD lpdwID,
	LPDWORD lpdwLocId,
	LPVOID lpSerialNumber,
	LPVOID lpDescription,
	FT_HANDLE *pftHandle
	);


typedef FTD2XX_API FT_STATUS (WINAPI *typeFT_EE_Read)(
    FT_HANDLE ftHandle,
	PFT_PROGRAM_DATA pData
	);

typedef FTD2XX_API FT_STATUS (WINAPI *typeFT_GetComPortNumber)(
    FT_HANDLE ftHandle,
	LPLONG	lpdwComPortNumber
	);

typeFT_Open m_FT_Open;
typeFT_OpenEx m_FT_OpenEx;
typeFT_Close m_FT_Close;
typeFT_EE_Read m_FT_EE_Read;
typeFT_GetComPortNumber m_FT_GetComPortNumber;
typeFT_CreateDeviceInfoList m_FT_CreateDeviceInfoList;
typeFT_GetDeviceInfoDetail m_FT_GetDeviceInfoDetail;


BOOL SetFunctions(HINSTANCE hFTDILib)
{
	int missing;
#ifndef FTDI_NOERROR
	CString errStr;
#endif
	m_FT_OpenEx=reinterpret_cast<typeFT_OpenEx>(GetProcAddress(hFTDILib,"FT_OpenEx"));
	m_FT_Close=reinterpret_cast<typeFT_Close>(GetProcAddress(hFTDILib,"FT_Close"));
	m_FT_EE_Read=reinterpret_cast<typeFT_EE_Read>(GetProcAddress(hFTDILib,"FT_EE_Read"));
	m_FT_GetComPortNumber=reinterpret_cast<typeFT_GetComPortNumber>(GetProcAddress(hFTDILib,"FT_GetComPortNumber"));
	m_FT_CreateDeviceInfoList=reinterpret_cast<typeFT_CreateDeviceInfoList>(GetProcAddress(hFTDILib,"FT_CreateDeviceInfoList"));
	m_FT_GetDeviceInfoDetail=reinterpret_cast<typeFT_GetDeviceInfoDetail>(GetProcAddress(hFTDILib,"FT_GetDeviceInfoDetail"));
	missing = (		!m_FT_OpenEx
				|	!m_FT_Close<<1
				|	!m_FT_EE_Read<<2
				|	!m_FT_GetComPortNumber<<3
				|	!m_FT_CreateDeviceInfoList<<4
				|	!m_FT_GetDeviceInfoDetail<<5 );
#ifndef FTDI_NOERROR
	errStr.Format("Missing DLL functions.  Error %i.\n\n", missing);
	errStr += "You need version 2.04.06 of the FTDI driver \n(issued March 13, 2008) ";
	errStr += "or later for \nAutolocate Widget to work.";
	if (missing) AfxMessageBox(errStr);
#endif

	return !missing;
}

int GetNumFTDIDevices()
{
	DWORD numDevs;
	int result;

	if (m_FT_CreateDeviceInfoList(&numDevs) == FT_OK) { 
		result = (int)numDevs;
	}
	else {
		result = -1;
	}

#ifdef FTDITEST
	CString nstr;
	nstr.Format("Number of FTDI chips found: %i", result);
	AfxMessageBox(result == -1 ? "Search for FTDI chips failed" :
		result == 0 ? "No FTDI chips found" : nstr);
#endif

	return result;
}

int TestFTDIDevice(int ndx, LONG* pComPort)
{
	int widgetTest = -1;
	FT_HANDLE ftHandle;
	FT_PROGRAM_DATA ftData;
	DWORD Flags;
	DWORD ID; 
	DWORD Type; 
	DWORD LocId;
	char ManufacturerBuf[32]; 
	char ManufacturerIdBuf[16]; 
	char DescriptionBuf[64]; 
	char SerialNumberBuf[16];
#ifndef FTDI_NOERROR
	CString teststr;
#endif

	if (m_FT_GetDeviceInfoDetail(ndx, &Flags, &Type, &ID, &LocId, 
			SerialNumberBuf, DescriptionBuf, &ftHandle) != FT_OK ) {

#ifdef FTDITEST
		teststr.Format("Chip %i could not be identified", ndx+1);
		AfxMessageBox(teststr);
#endif

		return -1;
	}

	if ( Type != FT_DEVICE_232R ){

#ifdef FTDITEST
		teststr.Format("Chip %i is type %i (not an FT232R)", ndx+1, Type);
		AfxMessageBox(teststr);
#endif

		return -1;
	}

	if (m_FT_OpenEx(SerialNumberBuf, FT_OPEN_BY_SERIAL_NUMBER, &ftHandle) != FT_OK) {

#ifdef FTDITEST
		teststr.Format("Chip %i could not be opened", ndx+1);
		AfxMessageBox(teststr);
#endif

		return -1;
	}

 	ftData.Signature1 = 0x00000000; 
	ftData.Signature2 = 0xffffffff; 
	ftData.Version = 0x00000002; // EEPROM structure with FT232R extensions
	ftData.Manufacturer = ManufacturerBuf; 
	ftData.ManufacturerId = ManufacturerIdBuf; 
	ftData.Description = DescriptionBuf; 
	ftData.SerialNumber = SerialNumberBuf; 
 
	if (m_FT_EE_Read(ftHandle, &ftData) == FT_OK) {
		widgetTest = (ftData.Cbus3 == CBUS_CLK6);

#ifdef FTDITEST
		if (!widgetTest) {
			teststr.Format("Chip %i has C3 parameter %i (not a Widget)", ndx+1, ftData.Cbus3);
		}
		else {
			teststr.Format("Success!  Chip %i is a Widget!", ndx+1);
		}
		AfxMessageBox(teststr);
	}
	else {
		teststr.Format("EEPROM of Chip %i could not be read", ndx+1);
		AfxMessageBox(teststr);
#endif

	}

#ifndef FTDI_NOERROR
	if (widgetTest >= 0 && m_FT_GetComPortNumber(ftHandle,pComPort) != FT_OK) {
		teststr = "COM port of Widget could not be determined.\n\n";
		teststr += "You need version 2.04.06 of the FTDI driver \n(issued March 13, 2008) ";
		teststr += "or later for \nAutolocate Widget to work.";
		AfxMessageBox(teststr);
	}
#else
	if (m_FT_GetComPortNumber(ftHandle,pComPort) != FT_OK) widgetTest = -1;
#endif

#ifdef FTDITEST
	else if (widgetTest >= 0) {
		teststr.Format("COM port of Chip %i is COM%i", ndx+1, *pComPort);
		AfxMessageBox(teststr);
	}
#endif

	m_FT_Close(ftHandle);
	return widgetTest; 
}

