#include "CameraRenderer.h"

#include "Sensor.h"
#include "RenderDevice.h"

CameraRenderer::CameraRenderer(): m_pBitmap(nullptr)
{
}


CameraRenderer::~CameraRenderer()
{
}

void CameraRenderer::Initialize()
{
	m_pRenderDevice->RenderTarget()->CreateBitmap(
		Sensor::COLOR_SIZE,
		D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)),
		&m_pBitmap
		);
}

void CameraRenderer::Deinitialize()
{
	SafeRelease(m_pBitmap);
}

void CameraRenderer::Render(RGBQUAD* pColor, IBody** ppBodies)
{
	HRESULT hr = S_OK;

	hr = m_pBitmap->CopyFromMemory(nullptr, pColor, Sensor::COLOR_SIZE.width * sizeof(RGBQUAD));

	if (SUCCEEDED(hr))
	{
		m_pRenderDevice->RenderTarget()->DrawBitmap(m_pBitmap);
	}
}
