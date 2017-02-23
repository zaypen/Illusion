#pragma once

#include "stdafx.h"
#include <cstdint>
#include <vector>
#include "Renderer.h"
#include "Wincodec.h"

class RenderDevice
{
public:
	RenderDevice();
	~RenderDevice();

	bool Initialize(HWND m_hWnd, uint32_t width, uint32_t height);

	bool Initialized() const
	{
		return m_bInitialized;
	}

	ID2D1HwndRenderTarget* RenderTarget() const
	{
		return m_pRenderTarget;
	}

	D2D1_SIZE_U ScreenSize() const
	{
		return m_ScreenSize;
	}

	void AddRenderer(Renderer* pRenderer)
	{
		pRenderer->Initialize();
		m_vRenderers.push_back(pRenderer);
	}

	void Render(RGBQUAD* pColor, IBody** ppBodies)
	{
		for (auto &renderer : m_vRenderers)
		{
			renderer->Render(pColor, ppBodies);
		}
	}

	HRESULT LoadImageBitmap(LPCWSTR wzFilename, ID2D1Bitmap** pBitmap);

private:

	std::vector<Renderer*>	m_vRenderers;

	bool					m_bInitialized;
	D2D1_SIZE_U				m_ScreenSize;

	ID2D1Factory*           m_pD2DFactory;
	ID2D1HwndRenderTarget*  m_pRenderTarget;

	IWICImagingFactory*		m_pImagingFactory;
};

