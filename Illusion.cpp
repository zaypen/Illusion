//------------------------------------------------------------------------------
// <copyright file="BodyBasics.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "stdafx.h"
#include <strsafe.h>
#include "resource.h"
#include "Illusion.h"
#include "Sensor.h"
#include "BodyRenderer.h"
#include "CameraRenderer.h"
#include "MagicRenderer.h"

CIllusion::CIllusion() :
m_hWnd(nullptr),
	m_nStartTime(0),
	m_nLastCounter(0),
	m_fFreq(0),
	m_nNextStatusTime(0LL),
	m_nFramesSinceUpdate(0),
	m_pSensor(nullptr),
	m_pRenderDevice(nullptr), 
	m_pColorRGBX(nullptr)
{
	LARGE_INTEGER qpf = {0};
	if (QueryPerformanceFrequency(&qpf))
	{
		m_fFreq = double(qpf.QuadPart);
	}

	m_pSensor = new Sensor();
	m_pRenderDevice = new RenderDevice();

	m_pColorRGBX = new RGBQUAD[Sensor::COLOR_FRAME_SIZE.width * Sensor::COLOR_FRAME_SIZE.height];
}


/// <summary>
/// Destructor
/// </summary>
CIllusion::~CIllusion()
{
	if (m_pColorRGBX)
	{
		delete[] m_pColorRGBX;
		m_pColorRGBX = NULL;
	}

	delete m_pRenderDevice;

	m_pRenderDevice = nullptr;

	delete m_pSensor;

	m_pSensor = nullptr;
}

/// <summary>
/// Creates the main window and begins processing
/// </summary>
/// <param name="hInstance">handle to the application instance</param>
/// <param name="nCmdShow">whether to display minimized, maximized, or normally</param>
int CIllusion::Run(HINSTANCE hInstance, int nCmdShow)
{
    MSG       msg = {0};
    WNDCLASS  wc;

    // Dialog custom window class
    ZeroMemory(&wc, sizeof(wc));
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra    = DLGWINDOWEXTRA;
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.lpfnWndProc   = DefDlgProcW;
    wc.lpszClassName = L"BodyBasicsAppDlgWndClass";

    if (!RegisterClassW(&wc))
    {
        return 0;
    }

    // Create main application window
    HWND hWndApp = CreateDialogParamW(
        NULL,
        MAKEINTRESOURCE(IDD_APP),
        NULL,
        (DLGPROC)CIllusion::MessageRouter, 
        reinterpret_cast<LPARAM>(this));

    // Show window
    ShowWindow(hWndApp, nCmdShow);

    // Main message loop
    while (WM_QUIT != msg.message)
    {
        Update();

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If a dialog message will be taken care of by the dialog proc
            if (hWndApp && IsDialogMessageW(hWndApp, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK CIllusion::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CIllusion* pThis = NULL;

	if (WM_INITDIALOG == uMsg)
	{
		pThis = reinterpret_cast<CIllusion*>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
	}
	else
	{
		pThis = reinterpret_cast<CIllusion*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pThis)
	{
		return pThis->DlgProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

/// <summary>
/// Handle windows messages for the class instance
/// </summary>
/// <param name="hWnd">window message is for</param>
/// <param name="uMsg">message</param>
/// <param name="wParam">message data</param>
/// <param name="lParam">additional message data</param>
/// <returns>result of message processing</returns>
LRESULT CALLBACK CIllusion::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
	{
		// Bind application window handle
		m_hWnd = hWnd;

		// Get and Initialize the default Kinect sensor
		m_pSensor->Initialize();
	}
	break;

	// If the titlebar X is clicked, destroy app
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		// Quit the main message pump
		PostQuitMessage(0);
		break;
	}

	return FALSE;
}

void CIllusion::Update()
{
    if (!m_pSensor->Initialized())
    {
        return;
    }

	IMultiSourceFrame* pMultiSourceFrame = nullptr;
	IColorFrame* pColorFrame = nullptr;
	IDepthFrame* pDepthFrame = nullptr;
	IBodyIndexFrame* pBodyIndexFrame = nullptr;
	IBodyFrame* pBodyFrame = nullptr;

	HRESULT hr = m_pSensor->MultiSourceFrameReader()->AcquireLatestFrame(&pMultiSourceFrame);

	if (SUCCEEDED(hr))
	{
		hr = AcquireColorFrame(pMultiSourceFrame, &pColorFrame);
	}

	if (SUCCEEDED(hr))
	{
		hr = AcquireDepthFrame(pMultiSourceFrame, &pDepthFrame);
	}

	if (SUCCEEDED(hr))
	{
		hr = AcquireBodyIndexFrame(pMultiSourceFrame, &pBodyIndexFrame);
	}

	if (SUCCEEDED(hr))
	{
		hr = AcquireBodyFrame(pMultiSourceFrame, &pBodyFrame);
	}

	if (SUCCEEDED(hr))
	{
		INT64 nTime = 0;

		hr = pBodyFrame->get_RelativeTime(&nTime);
		if (SUCCEEDED(hr))
		{
			UpdateFps(nTime);
		}

		IFrameDescription* pColorFrameDescription = nullptr;
		int nColorWidth = 0;
		int nColorHeight = 0;
		ColorImageFormat imageFormat = ColorImageFormat_None;
		UINT nColorBufferSize = 0;
		RGBQUAD *pColorBuffer = nullptr;

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_FrameDescription(&pColorFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameDescription->get_Width(&nColorWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameDescription->get_Height(&nColorHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
		}

		if (SUCCEEDED(hr))
		{
			if (imageFormat == ColorImageFormat_Bgra)
			{
				hr = pColorFrame->AccessRawUnderlyingBuffer(&nColorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));
			}
			else if (m_pColorRGBX)
			{
				pColorBuffer = m_pColorRGBX;
				nColorBufferSize = Sensor::COLOR_FRAME_SIZE.width * Sensor::COLOR_FRAME_SIZE.height * sizeof(RGBQUAD);
				hr = pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, reinterpret_cast<BYTE*>(pColorBuffer), ColorImageFormat_Bgra);
			}
			else
			{
				hr = E_FAIL;
			}
		}

        IBody* ppBodies[BODY_COUNT] = {0};

        if (SUCCEEDED(hr))
        {
            hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
        }

		if (SUCCEEDED(hr) && pColorBuffer && nColorWidth == Sensor::COLOR_FRAME_SIZE.width && nColorHeight == Sensor::COLOR_FRAME_SIZE.height)
        {
			ProcessFrame(pColorBuffer, ppBodies);
        }

        for (int i = 0; i < _countof(ppBodies); ++i)
        {
            SafeRelease(ppBodies[i]);
        }
    }

	SafeRelease(pDepthFrame);
	SafeRelease(pColorFrame);
	SafeRelease(pBodyIndexFrame);
	SafeRelease(pBodyFrame);
	SafeRelease(pMultiSourceFrame);
}

HRESULT CIllusion::AcquireColorFrame(IMultiSourceFrame* pMultiSourceFrame, IColorFrame** pFrame)
{
	IColorFrameReference* pFrameReference = nullptr;

	HRESULT hr = pMultiSourceFrame->get_ColorFrameReference(&pFrameReference);
	if (SUCCEEDED(hr))
	{
		hr = pFrameReference->AcquireFrame(pFrame);
	}

	SafeRelease(pFrameReference);

	return hr;
}

HRESULT CIllusion::AcquireDepthFrame(IMultiSourceFrame* pMultiSourceFrame, IDepthFrame** pFrame)
{
	IDepthFrameReference* pFrameReference = nullptr;

	HRESULT hr = pMultiSourceFrame->get_DepthFrameReference(&pFrameReference);
	if (SUCCEEDED(hr))
	{
		hr = pFrameReference->AcquireFrame(pFrame);
	}

	SafeRelease(pFrameReference);

	return hr;
}

HRESULT CIllusion::AcquireBodyIndexFrame(IMultiSourceFrame* pMultiSourceFrame, IBodyIndexFrame** pFrame)
{
	IBodyIndexFrameReference* pFrameReference = nullptr;

	HRESULT hr = pMultiSourceFrame->get_BodyIndexFrameReference(&pFrameReference);
	if (SUCCEEDED(hr))
	{
		hr = pFrameReference->AcquireFrame(pFrame);
	}

	SafeRelease(pFrameReference);

	return hr;
}

HRESULT CIllusion::AcquireBodyFrame(IMultiSourceFrame* pMultiSourceFrame, IBodyFrame** pFrame)
{
	IBodyFrameReference* pFrameReference = nullptr;

	HRESULT hr = pMultiSourceFrame->get_BodyFrameReference(&pFrameReference);
	if (SUCCEEDED(hr))
	{
		hr = pFrameReference->AcquireFrame(pFrame);
	}

	SafeRelease(pFrameReference);

	return hr;
}

void CIllusion::UpdateFps(INT64 nTime)
{
	if (m_hWnd)
	{
		if (!m_nStartTime)
		{
			m_nStartTime = nTime;
		}

		double fps = 0.0;

		LARGE_INTEGER qpcNow = { 0 };
		if (m_fFreq)
		{
			if (QueryPerformanceCounter(&qpcNow))
			{
				if (m_nLastCounter)
				{
					m_nFramesSinceUpdate++;
					fps = m_fFreq * m_nFramesSinceUpdate / double(qpcNow.QuadPart - m_nLastCounter);
				}
			}
		}

		WCHAR szStatusMessage[64];
		StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L" FPS = %0.2f    Time = %I64d", fps, (nTime - m_nStartTime));

		if (SetStatusMessage(szStatusMessage, 1000, false))
		{
			m_nLastCounter = qpcNow.QuadPart;
			m_nFramesSinceUpdate = 0;
		}
	}
}

void CIllusion::ProcessFrame(RGBQUAD* pColor, IBody** ppBodies)
{
    if (m_hWnd)
    {
        HRESULT hr = EnsureDirect2DResources();

        if (SUCCEEDED(hr) && m_pRenderDevice->Initialized() && m_pSensor->Initialized())
        {
			m_pRenderDevice->RenderTarget()->BeginDraw();
			m_pRenderDevice->RenderTarget()->Clear();

			m_pRenderDevice->Render(pColor, ppBodies);
			hr = m_pRenderDevice->RenderTarget()->EndDraw();

            // Device lost, need to recreate the render target
            // We'll dispose it now and retry drawing
            if (D2DERR_RECREATE_TARGET == hr)
            {
                hr = S_OK;
				// TODO: m_pRenderDevice->Deinitialized();
            }
        }

    }
}

bool CIllusion::SetStatusMessage(_In_z_ WCHAR* szMessage, DWORD nShowTimeMsec, bool bForce)
{
    INT64 now = GetTickCount64();

    if (m_hWnd && (bForce || (m_nNextStatusTime <= now)))
    {
        SetDlgItemText(m_hWnd, IDC_STATUS, szMessage);
        m_nNextStatusTime = now + nShowTimeMsec;

        return true;
    }

    return false;
}

HRESULT CIllusion::EnsureDirect2DResources()
{
    HRESULT hr = S_OK;

    if (m_pRenderDevice && !m_pRenderDevice->Initialized())
    {
		HWND hWnd = GetDlgItem(m_hWnd, IDC_VIDEOVIEW);
		bool initialized = m_pRenderDevice->Initialize(hWnd, Sensor::COLOR_FRAME_SIZE.width, Sensor::COLOR_FRAME_SIZE.height);

		CameraRenderer* cameraRenderer = new CameraRenderer();
		cameraRenderer->Setup(m_pSensor, m_pRenderDevice);
		m_pRenderDevice->AddRenderer(cameraRenderer);

		/*BodyRenderer* bodyRenderer = new BodyRenderer();
		bodyRenderer->Setup(m_pSensor, m_pRenderDevice);
		m_pRenderDevice->AddRenderer(bodyRenderer);*/

		MagicRenderer* magicRenderer = new MagicRenderer();
		magicRenderer->Setup(m_pSensor, m_pRenderDevice);
		m_pRenderDevice->AddRenderer(magicRenderer);

		if (!initialized)
        {
            SetStatusMessage(L"Couldn't create Direct2D render target!", 10000, true);
            return hr;
        }

    }

    return hr;
}

