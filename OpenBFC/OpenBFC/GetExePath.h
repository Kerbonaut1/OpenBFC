#ifndef GETEXEPATH
#define GETEXEPATH

#define WIN32_LEAN_AND_MEAN
#include <string>
#include <windows.h>
using namespace std;

string GetExePath()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
}

#endif // !GETEXEPATH
