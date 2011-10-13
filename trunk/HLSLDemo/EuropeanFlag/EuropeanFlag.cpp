//***************************************************
// 
// Project: GPU Gems Demo
//  
//  
//  
// System: Intel Pentium D 915, 1536 DDR, Intel GMA X3100, Windows7, MSVC++ 9.0
// 
// FileName:   ProceduralTerrain.cpp
// Created:    2009-11-30(11:19),by aldrich
// Purpose:    
// E-mail:     hpsoar@gmail.com
// URL:        N/A
// 
// CopyRight(c):ccmtc@hust
// All Right Reserved.
//***************************************************

#include "d3dUtility.h"
#include <vector>

//
// Globals
//

IDirect3DDevice9* Device = 0;

const int Width = 640;
const int Height = 480;

ID3DXEffect* VSEffect = 0;
D3DXHANDLE TechHandle = 0;
D3DXHANDLE WVPHandle = 0;
D3DXHANDLE TimeHandle = 0;
D3DXHANDLE TexHandle    = 0;

ID3DXMesh* Mesh = 0;
std::vector<D3DMATERIAL9> Mtrls(0);
std::vector<IDirect3DTexture9*> Textures(0);

//
// Framework Functions
//

bool Setup()
{
	HRESULT hr = 0;

	ID3DXBuffer* mtrlBuffer = 0;
	DWORD numMtrls = 0;

	hr = D3DXLoadMeshFromX(
		"flag.x",
		D3DXMESH_MANAGED,
		Device,
		0,
		&mtrlBuffer,
		0,
		&numMtrls,
		&Mesh);

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXLoadMeshFromX() - FAILED", 0, 0);
		return false;
	}

	if (0 != mtrlBuffer && 0 != numMtrls)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();
		for (int i = 0; i < numMtrls; ++i)
		{
			//mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			Mtrls.push_back(mtrls[i].MatD3D);

			if (0 != mtrls[i].pTextureFilename)
			{
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFile(
					Device,					
					mtrls[i].pTextureFilename,
					&tex);
				Textures.push_back(tex);
			}
			else
			{
				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFile(
					Device,
					"EuropeFlag.jpg",
					&tex);
				Textures.push_back(tex);
			}
		}
	}

	d3d::Release<ID3DXBuffer*>(mtrlBuffer);

	ID3DXBuffer* errorBuffer = 0;	

	hr = D3DXCreateEffectFromFile(
		Device,
		"flag.fx",
		0,
		0,
		D3DXSHADER_DEBUG,
		0,
		&VSEffect,
		&errorBuffer);	

	if (errorBuffer)
	{
		::MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		d3d::Release<ID3DXBuffer*>(errorBuffer);
	}

	if (FAILED(hr))
	{
		::MessageBox(0, "D3DXCreateEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	TexHandle = VSEffect->GetParameterByName(0, "Tex0");	
	WVPHandle = VSEffect->GetParameterByName(0, "matWorldViewProj");
	TimeHandle = VSEffect->GetParameterByName(0, "time");
	TechHandle = VSEffect->GetTechniqueByName("TVertexShaderOnly");

	return true;
}

void Cleanup()
{
	d3d::Release<ID3DXMesh*>(Mesh);

	for (int i = 0; i < Textures.size(); ++i)
	{
		d3d::Release<IDirect3DTexture9*>(Textures[i]);
	}

	d3d::Release<ID3DXEffect*>(VSEffect);
}

bool Display(float timeDelta)
{
	if( Device ) // Only use Device methods if we have a valid device.
	{
		static float angle  = (3.0f * D3DX_PI) / 2.0f;
		static float radius = 30.0f;		
		static float time = 0.0f;

		time += timeDelta;

		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			angle -= 0.5f * timeDelta;

		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			angle += 0.5f * timeDelta;

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			radius -= 2.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			radius += 2.0f * timeDelta;		

		D3DXMATRIX P, W, V;

		D3DXMatrixIdentity(&W);

		D3DXMatrixPerspectiveFovLH(
			&P, D3DX_PI * 0.25f,
			(float)Width / (float)Height, 1.0f, 1000.0f);		

		D3DXVECTOR3 position( cosf(angle) * radius, 0.0f, sinf(angle) * radius );
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMatrixLookAtLH(&V, &position, &target, &up);						

		D3DXMATRIX WV, WVP;

		D3DXMatrixMultiply(&WV, &W, &V);
		D3DXMatrixMultiply(&WVP, &WV, &P);
		VSEffect->SetMatrix(WVPHandle, &WVP);				
		VSEffect->SetFloat(TimeHandle, time);

		// Instruct the device to set each pixel on the back buffer black -
		// D3DCLEAR_TARGET: 0x00000000 (black) - and to set each pixel on
		// the depth buffer to a value of 1.0 - D3DCLEAR_ZBUFFER: 1.0f.
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);

		Device->BeginScene();


		// Set the technique to use
		VSEffect->SetTechnique(TechHandle);

		UINT numPasses = 0;
		VSEffect->Begin(&numPasses, 0);

		for (int i = 0; i < numPasses; ++i)
		{
			VSEffect->BeginPass(i);

			for (int j = 0; j < Mtrls.size(); ++j)
			{			
				VSEffect->SetTexture(TexHandle, Textures[j]);	
				Mesh->DrawSubset(j);
			}

			VSEffect->EndPass();
		}

		Device->EndScene();
		Device->Present(0, 0, 0, 0);				
	}

	return true;
}

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	HINSTANCE h = hinstance;
	if(!d3d::InitD3D(hinstance,
		640, 480, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}