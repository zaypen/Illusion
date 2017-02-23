#include "MagicRenderer.h"

#include "Sensor.h"
#include "RenderDevice.h"

static const LPCWSTR FILENAME_FORMAT = L"magic/fire%04d.tif";
static const int FRAME_COUNT = 60;
static const D2D1_SIZE_U FRAME_SIZE = D2D1::SizeU(160, 240);

MagicRenderer::MagicRenderer(): frame(0)
{
	m_pBitmaps = new ID2D1Bitmap* [FRAME_COUNT]{nullptr};
}


MagicRenderer::~MagicRenderer()
{
}

void MagicRenderer::Initialize()
{
	for (auto i = 0; i < FRAME_COUNT; ++i)
	{
		wchar_t buf[256];
		wsprintf(buf, FILENAME_FORMAT, i + 1);
		m_pRenderDevice->LoadImageBitmap(buf, &m_pBitmaps[i]);
	}
}

void MagicRenderer::Deinitialize()
{
	for (auto i = 0; i < FRAME_COUNT; ++i)
	{
//		SafeRelease(m_pBitmaps[i]);
	}
}

void MagicRenderer::Render(RGBQUAD* pColor, IBody** ppBodies)
{
	for (int i = 0; i < BODY_COUNT; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)
		{
			BOOLEAN bTracked = false;
			pBody->get_IsTracked(&bTracked);

			if (bTracked)
			{
				Joint joints[JointType_Count];
				JointOrientation jointOrientations[JointType_Count];
				D2D1_POINT_2F jointPoints[JointType_Count];
				HandState leftHandState = HandState_Unknown;
				HandState rightHandState = HandState_Unknown;

				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);
				pBody->GetJoints(_countof(joints), joints);
				pBody->GetJointOrientations(_countof(jointOrientations), jointOrientations);
				Vector4 orientation = jointOrientations[JointType_HandRight].Orientation;
				if (rightHandState == HandState_Open && orientation.y > 0.6f)
				{
					ColorSpacePoint colorPoint = { 0 };
					m_pSensor->CoordinateMapper()->MapCameraPointToColorSpace(joints[JointType_HandRight].Position, &colorPoint);
					D2D1_RECT_F rect = D2D1::RectF(colorPoint.X - FRAME_SIZE.width / 2, colorPoint.Y - FRAME_SIZE.height, colorPoint.X + FRAME_SIZE.width / 2, colorPoint.Y);
					m_pRenderDevice->RenderTarget()->DrawBitmap(m_pBitmaps[frame++], rect);
					frame %= FRAME_COUNT;
				}

			}
		}
	}
}


