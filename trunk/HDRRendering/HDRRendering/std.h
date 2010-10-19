#ifndef __STD_H__
#define __STD_H__

#include <iostream>
#include <map>
#include <assert.h>
#include <exception>
#include <cmath>
#include <string>

#include "IL/il.h"
#include "IL/ilu.h"
#pragma comment(lib,"devil.lib")
#pragma comment(lib, "ilu.lib")

#define RET(b) \
	if ( !(b) ) return;

#define F_RET(b) \
	if ( !(b) ) return false;

#define V_RET(b) F_RET(b)

#define T_RET(b) \
	if ( (b) ) return true;

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

typedef unsigned char	uchar;
typedef unsigned char	uint8;
typedef signed char		sint8;
typedef unsigned short	uint16;
typedef short			sint16;
typedef unsigned int	uint32;
typedef int				sint32;

#else
#endif//__STD_H__