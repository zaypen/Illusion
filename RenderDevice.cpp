#include "RenderDevice.h"
#include <cstdint>


RenderDevice::RenderDevice(): 
	m_bInitialized(false), 
	m_pD2DFactory(nullptr), 
	m_pRenderTarget(nullptr), 
	m_pImagingFactory(nullptr)
{
}


RenderDevice::~RenderDevice()
{
	for (auto &renderer : m_vRenderers)
	{
		renderer->Deinitialize();
	}
	SafeRelease(m_pRenderTarget);
	SafeRelease(m_pD2DFactory);
//	SafeRelease(m_pImagingFactory);
}

bool RenderDevice::Initialize(HWND m_hWnd, uint32_t width, uint32_t height)
{
	HRESULT hr = S_OK;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (FAILED(hr))
	{
		return false;
	}

	m_ScreenSize = D2D1::SizeU(width, height);

	D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
	rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
	rtProps.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;

	hr = m_pD2DFactory->CreateHwndRenderTarget(
		rtProps,
		D2D1::HwndRenderTargetProperties(m_hWnd, m_ScreenSize),
		&m_pRenderTarget
	);

	if (FAILED(hr))
	{
		return false;
	}

	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<LPVOID*>(&m_pImagingFactory)
		);

	if (FAILED(hr))
	{
		return false;
	}
	return m_bInitialized = true;
}

HRESULT RenderDevice::LoadImageBitmap(LPCWSTR wzFilename, ID2D1Bitmap** pBitmap)
{
	IWICBitmapDecoder *pBitmapDecoder = nullptr;
	IWICFormatConverter *pFormatConverter = nullptr;
	IWICBitmapFrameDecode *pFrame = nullptr;

	UINT width, height;

	HRESULT hr = m_pImagingFactory->CreateDecoderFromFilename(
		wzFilename,
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&pBitmapDecoder);

	if (SUCCEEDED(hr))
	{
		hr = pBitmapDecoder->GetFrame(0, &pFrame);
	}

	if (SUCCEEDED(hr))
	{
		hr = pFrame->GetSize(&width, &height);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pImagingFactory->CreateFormatConverter(&pFormatConverter);
	}

	if (SUCCEEDED(hr))
	{
		hr = pFormatConverter->Initialize(
			pFrame,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			0.f,
			WICBitmapPaletteTypeCustom
			);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pRenderTarget->CreateBitmapFromWicBitmap(pFormatConverter, nullptr, pBitmap);
	}

	SafeRelease(pBitmapDecoder);
	return hr;
}
