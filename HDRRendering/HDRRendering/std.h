#ifndef __STD_H__
#define __STD_H__

#ifndef OEMRESOURCE
#define OEMRESOURCE
#include <Windows.h>
#undef OEMRESOURCE
#else
#include <Windows.h>
#endif

#include <iostream>
#include <map>
#include <assert.h>
#include <exception>

#include "il.h"
#include "ilu.h"
#pragma comment(lib,"devil.lib")
#pragma comment(lib, "ilu.lib")

#define RET(b) \
	if ( !(b) ) return;

#define F_RET(b) \
	if ( !(b) ) return FALSE;

#define V_RET(b) F_RET(b)

#define T_RET(b) \
	if ( (b) ) return TRUE;

#define C_BREAK_R(b) \
	if ( (b) ) break; \
	else return 0;

#define C_BREAK(b) \
	if ( (b) ) break;

#define SAFE_RELEASE(p) \
	if ( NULL != p ) { \
		delete p; \
		p = NULL; \
	}

#else
#error __STD_H__ already defined!
#endif//__STD_H__