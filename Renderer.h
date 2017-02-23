#pragma once

#include "stdafx.h"

class Sensor;
class RenderDevice;

class Renderer
{
public:

	Renderer(): m_pSensor(nullptr), m_pRenderDevice(nullptr)
	{
	}

	virtual ~Renderer()
	{
	}

	void Setup(Sensor* pSensor, RenderDevice* pRenderDevice)
	{
		m_pSensor = pSensor;
		m_pRenderDevice = pRenderDevice;
	}

	virtual void Initialize() = 0;
	virtual void Deinitialize() = 0;
	virtual void Render(RGBQUAD* pColor, IBody** ppBodies) = 0;

protected:
	Sensor*			m_pSensor;
	RenderDevice*	m_pRenderDevice;
};

