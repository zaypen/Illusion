#pragma once
#include "Renderer.h"

class BodyRenderer : public Renderer
{
public:
	BodyRenderer();
	~BodyRenderer();

	void Initialize() override;
	void Deinitialize() override;
	void Render(RGBQUAD* pColor, IBody** ppBodies) override;

private:
	D2D1_POINT_2F			MapCamaraSpaceToScreen(const CameraSpacePoint& bodyPoint) const;
	void                    DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints);
	void                    DrawHand(HandState handState, const D2D1_POINT_2F& handPosition);
	void                    DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1);

	ID2D1SolidColorBrush*   m_pBrushJointTracked;
	ID2D1SolidColorBrush*   m_pBrushJointInferred;
	ID2D1SolidColorBrush*   m_pBrushBoneTracked;
	ID2D1SolidColorBrush*   m_pBrushBoneInferred;
	ID2D1SolidColorBrush*   m_pBrushHandClosed;
	ID2D1SolidColorBrush*   m_pBrushHandOpen;
	ID2D1SolidColorBrush*   m_pBrushHandLasso;
};

