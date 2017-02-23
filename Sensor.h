#pragma once

#include "stdafx.h"

class Sensor
{
public:
	static const D2D1_SIZE_U DEPTH_SIZE;
	static const D2D1_SIZE_U COLOR_SIZE;

	Sensor();
	~Sensor();

	bool Initialize();

	bool Initialized() const
	{
		return m_bInitialized;
	}

	ICoordinateMapper* CoordinateMapper() const
	{
		return m_pCoordinateMapper;
	}

	IMultiSourceFrameReader* MultiSourceFrameReader() const
	{
		return m_pMultiSourceFrameReader;
	}

private:
	bool						m_bInitialized;

	// Current Kinect
	IKinectSensor*				m_pKinectSensor;
	ICoordinateMapper*			m_pCoordinateMapper;

	IMultiSourceFrameReader*	m_pMultiSourceFrameReader;
};

