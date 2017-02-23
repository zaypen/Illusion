#include "Sensor.h"

const D2D1_SIZE_U Sensor::DEPTH_SIZE = D2D1::SizeU(512, 424);
const D2D1_SIZE_U Sensor::COLOR_SIZE = D2D1::SizeU(1920, 1080);

Sensor::Sensor() : m_bInitialized(false), m_pKinectSensor(nullptr), m_pCoordinateMapper(nullptr), m_pMultiSourceFrameReader(nullptr)
{
}


Sensor::~Sensor()
{
	SafeRelease(m_pMultiSourceFrameReader);
	SafeRelease(m_pCoordinateMapper);

	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}

	SafeRelease(m_pKinectSensor);
}

bool Sensor::Initialize()
{
	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return false;
	}

	if (m_pKinectSensor)
	{
		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_BodyIndex | FrameSourceTypes::FrameSourceTypes_Body,
				&m_pMultiSourceFrameReader
				);
		}
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		return false;
	}

	return m_bInitialized = true;
}
