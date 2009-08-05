GoGoogleMaps.exe : GoGoogleMaps.cpp
	g++ -mwin32 -mwindows -o GoGoogleMaps.exe GoGoogleMaps.cpp
	strip GoGoogleMaps.exe
	
clean :
	rm -f GoGoogleMaps.exe *o *~
	
test : GoGoogleMaps.exe
	.\\GoGoogleMaps.exe