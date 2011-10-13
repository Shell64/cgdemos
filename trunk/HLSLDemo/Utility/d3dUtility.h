//***************************************************
// 
// Project: GPU Gems Demo
//  
//  
//  
// System: Intel Pentium D 915, 1536 DDR, Intel GMA X3100, Windows7, MSVC++ 9.0
// 
// FileName:   d3dUtility.h
// Created:    2009-11-30(10:47),by aldrich
// Purpose:    
// E-mail:     hpsoar@gmail.com
// URL:        N/A
// 
// CopyRight(c):ccmtc@hust
// All Right Reserved.
//***************************************************


#ifndef __D3D_UTILITY_H__
#define __D3D_UTILITY_H__

#include <d3dx9.h>
#include <string>
#include <limits>

namespace d3d
{

	//************************************
	// Method:    InitD3D
	// FullName:  d3d::InitD3D
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: HINSTANCE hInstance
	// Parameter: int width
	// Parameter: int height
	// Parameter: bool windowed
	// Parameter: D3DDEVTYPE deviceType
	// Parameter: IDirect3DDevice9 * * device
	//************************************	
	bool InitD3D(HINSTANCE hInstance, 
		int width, int height, 
		bool windowed, 
		D3DDEVTYPE deviceType, 
		IDirect3DDevice9** device);  

	int EnterMsgLoop(bool (*ptr_display)(float timeDelta));

	LRESULT CALLBACK WndProc(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam);	

	//
	// Cleanup
	//
	template<class T> void Release(T t)
	{
		if( t )
		{
			t->Release();
			t = 0;
		}
	}

	template<class T> void Delete(T t)
	{
		if( t )
		{
			delete t;
			t = 0;
		}
	}
}

#endif //__D3D_UTILITY_H__