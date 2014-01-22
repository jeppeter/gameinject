
#ifndef __INJECT_COMMON_H__
#define __INJECT_COMMON_H__


#include <Windows.h>
#include <stdint.h>

#define  CURSOR_COLOR_BITMAPINFO        101
#define  CURSOR_COLOR_BITDATA           102

typedef unsigned long ptr_t;

typedef struct
{
	HANDLE thread;
	unsigned long threadid;
	HANDLE exitevt;
	int running;
	int exited;	
} thread_control_t;

typedef struct __share_data
{
	int datalen;
	int datatype;
	unsigned char data[1];
} SHARE_DATA,*LPSHARE_DATA;



typedef DWORD (WINAPI *ThreadFunc_t)(LPVOID lpParam);

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))


#endif /*__INJECT_COMMON_H__*/


