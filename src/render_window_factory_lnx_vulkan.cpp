#include <qdiligent/render_window_factory.hpp>

#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>

#include <QtX11Extras/QX11Info>
#include <QtGui/QResizeEvent>

#include "render_window_private.hpp"


namespace Diligent
{

class RenderWindow_lnx_vulkan : public QWindow
{
public:

	RenderWindowListener* _listener;
	RenderWindowPrivate p;
	bool _initialized = false;

public:

	RenderWindow_lnx_vulkan(RenderWindowListener* listener) : _listener(listener)
	{
		setSurfaceType(QWindow::OpenGLSurface);
		resize(800,600);
	}

protected:

	bool event(QEvent *event) override
	{
	    switch (event->type()) {
	    case QEvent::UpdateRequest:
	    	renderNow();
	        return true;
	    default:
	        return QWindow::event(event);
	    }
	}

    void exposeEvent(QExposeEvent *event) override
    {
        Q_UNUSED(event);
        renderNow();
    }

    void resizeEvent(QResizeEvent *event) override
    {
    	_initialize();
        if (p.swapChain)
        {
        	p.swapChain->Resize(event->size().width(), event->size().height());
    		_listener->resize(event->size().width(), event->size().height());
        }

        renderNow();
    }


    void renderNow()
    {
        if (!isExposed())
            return;

		_initialize();
        _listener->render();
        requestUpdate();
    }

	void _initialize()
	{
		if(_initialized)
			return;
		_initialized = true;

		SwapChainDesc SCDesc;
		SCDesc.DepthBufferFormat = TEX_FORMAT_D32_FLOAT_S8X24_UINT;
		//SCDesc.DepthBufferFormat = TEX_FORMAT_D24_UNORM_S8_UINT;

		auto* factory = GetEngineFactoryVk();

		EngineVkCreateInfo EngVkAttribs;
		EngVkAttribs.DynamicHeapSize *= 2;
		//EngVkAttribs.DeviceLocalMemoryReserveSize /= 4;
		//EngVkAttribs.HostVisibleMemoryReserveSize /= 4;
		EngVkAttribs.DeviceLocalMemoryPageSize /= 8;
		EngVkAttribs.HostVisibleMemoryPageSize /= 8;

		factory->CreateDeviceAndContextsVk(EngVkAttribs, &p.device, &p.immediateContext);

		LinuxNativeWindow XCBWindow;
		XCBWindow.WindowId       = winId();
		XCBWindow.pXCBConnection = QX11Info::connection();
		factory->CreateSwapChainVk(p.device, p.immediateContext, SCDesc, XCBWindow, &p.swapChain);

		p.engineFactory = factory;
		_listener->initialize();
	}


};



class RenderWindowFactory_lnx_vulkan : public RenderWindowFactory
{
public:

	RenderWindowFactory_lnx_vulkan()
	{
		registerFactory("vulkan", this);
	}

	~RenderWindowFactory_lnx_vulkan()
	{
		unregisterFactory(this);
	}

	virtual QWindow* createRenderWindow(RenderWindowListener* listener, QWindow* parent = nullptr) override
	{
		RenderWindow_lnx_vulkan* win = new RenderWindow_lnx_vulkan(listener);
		setupListener(listener, &win->p);
		return win;

	}

};

RenderWindowFactory_lnx_vulkan g_RenderWindowFactory_lnx_vulkan;

}
