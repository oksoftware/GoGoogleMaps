/*
 * Go Goole Maps! -- Jump a place where you stay now on Google Maps.
 *
 * Copyright(C) 2009 oksoftware. All rights reserved.
 *
 * Some codes aren't written by me.
 *
 * Requires:
 *  * GPS device using NMEA Protocol
 *
 * How to compile it:
 *  g++ -mwin32 -mwindows -o GoGoogleMaps.exe GoGoogleMaps.cpp
 *
 * If you want to use this program on another COM port, change the source and compile it.
 *
 */

#define PORT "COM7"

#include <string>
#include <vector>
#include <sstream>
#include <windows.h>
using namespace std;

vector<string> split( string s, string c )
{
	vector<string> ret;
	for( int i=0, n; i <= s.length(); i=n+1 ){
		n = s.find_first_of( c, i );
		if( n == string::npos ) n = s.length();
		string tmp = s.substr( i, n-i );
		ret.push_back(tmp);
	}
	return ret;
}

string itos(int num)
{
  stringstream ss;
  ss << num;
  return ss.str();
}

string ConvertNMEAtoGoogle(string sLatitude, string sLongitude)
{
	string sLatitudeDegree, sLongitudeDegree;
	string sLatitudeDecimal, sLongitudeDecimal;
	unsigned int iLatitudeDecimal, iLongitudeDecimal;
	string sLatitudeDest, sLongitudeDest;
	
	sLatitudeDegree = sLatitude.substr(0, 2);
	sLongitudeDegree = sLongitude.substr(0, 3);
	
	sLatitudeDecimal = sLatitude.substr(2, 2) + sLatitude.substr(5);
	sLongitudeDecimal = sLongitude.substr(3, 2) + sLongitude.substr(6);
	
	iLatitudeDecimal = atoi(sLatitudeDecimal.c_str());
	iLongitudeDecimal = atoi(sLongitudeDecimal.c_str());
	
	iLatitudeDecimal = iLatitudeDecimal * 5 / 3;
	iLongitudeDecimal = iLongitudeDecimal * 5 / 3;
	
	sLatitudeDest = sLatitudeDegree + "." + itos(iLatitudeDecimal);
	sLongitudeDest = sLongitudeDegree + "." + itos(iLongitudeDecimal);
	
	return sLatitudeDest + "," + sLongitudeDest;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HANDLE hCom;
	DCB dcb;
	COMMTIMEOUTS commTimeOuts;
	char sBuf;
	string sLineBuf = "";
	unsigned long nBytesRead;
	vector<string> sLineSplit;
	string sLatitude, sLongitude;
	string sGoogleMapsURL;
	
	hCom = CreateFile(PORT, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	
	if(hCom == INVALID_HANDLE_VALUE){
		MessageBox(NULL, "Cannot open " PORT " port.\n", NULL, MB_OK | MB_ICONERROR);
		return -1;
	}
	
	GetCommState(hCom, &dcb);
	
	dcb.BaudRate = 38400;
	
	SetCommState(hCom, &dcb);
	
	GetCommTimeouts(hCom, &commTimeOuts);
	
	commTimeOuts.ReadIntervalTimeout			= 1000;
	commTimeOuts.ReadTotalTimeoutMultiplier		= 0;
	commTimeOuts.ReadTotalTimeoutConstant		= 1000;
	commTimeOuts.WriteTotalTimeoutMultiplier	= 0;
	commTimeOuts.WriteTotalTimeoutConstant		= 0;
	
	SetCommTimeouts(hCom, &commTimeOuts);
	
	while(1){
		ReadFile(hCom, (LPVOID)&sBuf, 1, &nBytesRead, 0);
		if(nBytesRead == 1){
			sLineBuf += sBuf;
			if(sLineBuf.find("\n") != string::npos){
				
				if(sLineBuf.find("$GPGGA") == 0){
					sLineSplit = split(sLineBuf, ",");
					
					sLatitude = sLineSplit[2];
					sLongitude = sLineSplit[4];
					
					sGoogleMapsURL = "http://maps.google.co.jp/maps?q=(" + ConvertNMEAtoGoogle(sLatitude,sLongitude) + ")";
					
					ShellExecute(NULL, "open", sGoogleMapsURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
					
					CloseHandle(hCom);
					
					return 0;
				}
				sLineBuf = "";
			}
		}else{
			Sleep(5);
		}
	}
	
	return 0;
}
