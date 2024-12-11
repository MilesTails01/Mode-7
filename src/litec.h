//////////////////////////////////////////////////////////////////////
// Common header file for WIN32 and for C-Script
//////////////////////////////////////////////////////////////////////
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <stdio.h>

#define WINARGS HINSTANCE hinstance,HINSTANCE hprevinst,LPSTR lpcmdline,int ncmdshow

#else ////////////////////////////////////////////////////////////////
#ifndef litec_h
#define litec_h
#define WINARGS 

#define NULL	0
#define TRUE	1
#define FALSE	0
#define true	1
#define false	0

typedef char	byte;
typedef long	HINSTANCE;
typedef long	HMODULE;
typedef long	HWND;
typedef long	HDC;
typedef long	BOOL;
typedef long	LRESULT;
typedef long	UINT;
typedef long	WPARAM;
typedef long	LPARAM;
typedef long	DWORD;
typedef void	VOID;
typedef long	HRESULT;
typedef long	LONG;
typedef void*	HANDLE;
typedef void*	LPVOID;
typedef long	HBITMAP;
typedef char	BYTE;
typedef short	WORD;
typedef char*	LPCSTR;

typedef char*	LPSTR;
typedef long 	HMENU;
typedef long 	HBRUSH;
typedef void* 	UINT_PTR;

typedef struct _RECT{
	long	left;
	long	top;
	long	right;
	long	bottom;
}RECT;

//////////////////////////////////////////////////////////////////////
//clib functions - internally defined in the compiler

double  __cdecl acos(double);
double  __cdecl asin(double);
double  __cdecl atan(double);
double  __cdecl atan2(double,double);
double  __cdecl cos(double);
double  __cdecl cosh(double);
double  __cdecl exp(double);
double  __cdecl abs(double);
double  __cdecl fabs(double);
long    __cdecl labs(long);
double  __cdecl fmod(double, double);
double  __cdecl log(double);
double  __cdecl log10(double);
double  __cdecl pow(double, double);
double  __cdecl sin(double);
double  __cdecl sinh(double);
double  __cdecl tan(double);
double  __cdecl tanh(double);
double  __cdecl sqrt(double);
int		__cdecl rand();
void	__cdecl srand(int seed);

#define _pi 3.14159265

int     __cdecl memcmp(void *, void *, long);
void *  __cdecl memcpy(void *, void *, long);
void *  __cdecl memset(void *, int, long);
void *  __cdecl malloc(long);
void	__cdecl free(void*);
void *	__cdecl realloc(void*,long);

char *  __cdecl strcpy(char *, const char *);
char *  __cdecl strcat(char *, const char *);
//char *  __cdecl _strdup(const char *);
int     __cdecl strcmp(const char *, const char *);
int     __cdecl strlen(const char *);

//int     __cdecl getc();
//int     __cdecl kbhit();
int     __cdecl Sleep(int);

long	printf(char*,...);
long	__stdcall DefineApi(long);

#define sizeof(t) ((int)_sizeof(t))

#define API(name,lib) #define PRAGMA_API name;lib!name
#define APIA(name,lib) #define PRAGMA_API name;lib!name##A

#ifndef acknex_h
#include <windows.h>
#endif

#endif // litec_h
#endif ////////////////////////////////////////////////////////////

