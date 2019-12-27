#include "sky_box.h"

#include "renderer/rendering_device.h"

SkyBoxHelper::SkyBoxHelper()
{
	RenderingDevice::GetSingleton()->enableSkyDepthStencilState();
}

SkyBoxHelper::~SkyBoxHelper()
{
	RenderingDevice::GetSingleton()->disableSkyDepthStencilState();
}
