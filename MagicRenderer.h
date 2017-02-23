#pragma once
#include "Renderer.h"
#include <cstdint>

class MagicRenderer : public Renderer
{
public:
	MagicRenderer();
	~MagicRenderer();

	void Initialize() override;
	void Deinitialize() override;
	void Render(RGBQUAD* pColor, IBody** ppBodies) override;
private:
	uint32_t frame;
	ID2D1Bitmap** m_pBitmaps;
};

