#pragma once

#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>

namespace Diligent {

class RenderWindowPrivate
{
public:
	RefCntAutoPtr<IRenderDevice>  device;
	RefCntAutoPtr<IDeviceContext> immediateContext;
	RefCntAutoPtr<ISwapChain>     swapChain;
	IEngineFactory*               engineFactory;
};

}
