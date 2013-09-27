// ftdi.h : header file
//

/////////////////////////////////////////////////////////////////////////////

//#define FTDITEST

BOOL SetFunctions(HINSTANCE hFTDILib);
int GetNumFTDIDevices();
int TestFTDIDevice(int ndx, LONG* pComPort);

