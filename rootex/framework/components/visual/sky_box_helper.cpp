#include "sky_box_helper.h"

#include "renderer/rendering_device.h"

SkyBoxHelper::SkyBoxHelper()
{
	RenderingDevice::GetSingleton()->enableSkyDepthStencilState();
}

SkyBoxHelper::~SkyBoxHelper()
{
	RenderingDevice::GetSingleton()->disableSkyDepthStencilState();
}
