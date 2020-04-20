#include <qdiligent/render_window_factory.hpp>

#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h>

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLWindow>
#include <QtX11Extras/QX11Info>
#include <QtGui/QResizeEvent>

#include "render_window_private.hpp"


namespace Diligent
{

class RenderWindow_lnx_opengl : public QOpenGLWindow
{
public:

	RenderWindowListener* _listener;
	RenderWindowPrivate p;

public:

	RenderWindow_lnx_opengl(RenderWindowListener* listener) : _listener(listener)
	{

		QSurfaceFormat format = QSurfaceFormat::defaultFormat();

		std::cout << format.depthBufferSize() << std::endl;
		std::cout << format.stencilBufferSize() << std::endl;
		std::cout << format.swapBehavior() << std::endl;
		format.setVersion(4,3);
		format.setStencilBufferSize(8);
		format.setDepthBufferSize(24);
		//format.setSwapInterval(0);
		format.setSamples(4);
//        format.setSwapBehavior(QSurfaceFormat::SingleBuffer);
		setFormat(format);
		create();
		resize(800,600);
	}

protected:

	void resizeEvent(QResizeEvent *event) override
	{
		std::cout << "resizeEvent: " << event->size().width() << ", " << event->size().height() << std::endl;
	    if (p.swapChain)
	    	p.swapChain->Resize(event->size().width(), event->size().height());

		_listener->resize(event->size().width(), event->size().height());
	}

	virtual void initializeGL() override
	{
		QSurfaceFormat format = requestedFormat();

		std::cout << format.depthBufferSize() << std::endl;
		std::cout << format.stencilBufferSize() << std::endl;
		std::cout << format.swapBehavior() << std::endl;

		format = this->format();

		std::cout << format.majorVersion() << "." << format.minorVersion()<< std::endl;
		std::cout << format.depthBufferSize() << std::endl;
		std::cout << format.stencilBufferSize() << std::endl;
		std::cout << format.swapBehavior() << std::endl;


		SwapChainDesc SCDesc;
		//SCDesc.DepthBufferFormat = TEX_FORMAT_D32_FLOAT_S8X24_UINT;
        SCDesc.DepthBufferFormat = TEX_FORMAT_D24_UNORM_S8_UINT;

		auto* factory = GetEngineFactoryOpenGL();

		EngineGLCreateInfo CreationAttribs;
		CreationAttribs.Window.WindowId = winId();
		CreationAttribs.Window.pDisplay = QX11Info::display();

		factory->CreateDeviceAndSwapChainGL(
				CreationAttribs, &p.device, &p.immediateContext, SCDesc, &p.swapChain);
		p.engineFactory = factory;

		_listener->initialize();
	}

	virtual void paintGL() override
	{
		_listener->render();
		update();
	}

	virtual void resizeGL(int w, int h) override
	{
		std::cout << "resizeGL: " << w << ", " << h << std::endl;
		//_listener->resize(w, h);
	}

};



class RenderWindowFactory_lnx_opengl : public RenderWindowFactory
{
public:

	RenderWindowFactory_lnx_opengl()
	{
		registerFactory("opengl", this);
	}

	~RenderWindowFactory_lnx_opengl()
	{
		unregisterFactory(this);
	}

	virtual QWindow* createRenderWindow(RenderWindowListener* listener, QWindow* parent = nullptr) override
	{
		RenderWindow_lnx_opengl* win = new RenderWindow_lnx_opengl(listener);
		setupListener(listener, &win->p);
		return win;

	}

};

RenderWindowFactory_lnx_opengl g_RenderWindowFactory_lnx_opengl;

}
