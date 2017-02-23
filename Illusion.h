//------------------------------------------------------------------------------
// <copyright file="BodyBasics.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "resource.h"
#include "Sensor.h"
#include "RenderDevice.h"

class CIllusion
{
    static const int        cDepthWidth  = 512;
    static const int        cDepthHeight = 424;

public:

    CIllusion();

    ~CIllusion();

    static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT CALLBACK        DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    int                     Run(HINSTANCE hInstance, int nCmdShow);

private:
    HWND                    m_hWnd;
    INT64                   m_nStartTime;
    INT64                   m_nLastCounter;
    double                  m_fFreq;
    INT64                   m_nNextStatusTime;
    DWORD                   m_nFramesSinceUpdate;

	Sensor*					m_pSensor;
	RenderDevice*			m_pRenderDevice;

	RGBQUAD*                m_pColorRGBX;

    void                    Update();

	HRESULT					AcquireColorFrame(IMultiSourceFrame* pMultiSourceFrame, IColorFrame** pFrame);
	HRESULT					AcquireDepthFrame(IMultiSourceFrame* pMultiSourceFrame, IDepthFrame** pFrame);
	HRESULT					AcquireBodyIndexFrame(IMultiSourceFrame* pMultiSourceFrame, IBodyIndexFrame** pFrame);
	HRESULT					AcquireBodyFrame(IMultiSourceFrame* pMultiSourceFrame, IBodyFrame** pFrame);

	void					UpdateFps(INT64 nTime);
    
	void                    ProcessFrame(RGBQUAD* ppColor, IBody** ppBodies);

    bool                    SetStatusMessage(_In_z_ WCHAR* szMessage, DWORD nShowTimeMsec, bool bForce);

    HRESULT					EnsureDirect2DResources();

};

