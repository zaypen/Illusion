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
	uint32_t m_nFireFrame;
	uint32_t m_nStarFrame;
	ID2D1Bitmap** m_pFireBitmaps;
	ID2D1Bitmap** m_pStarBitmaps;
};

