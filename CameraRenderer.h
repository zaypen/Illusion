#pragma once
#include "Renderer.h"

class CameraRenderer : public Renderer
{
public:
	CameraRenderer();
	~CameraRenderer();

	void Initialize() override;
	void Deinitialize() override;
	void Render(RGBQUAD* pColor, IBody** ppBodies) override;

private:
	ID2D1Bitmap* m_pBitmap;
};

