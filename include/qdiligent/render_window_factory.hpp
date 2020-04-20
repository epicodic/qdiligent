#pragma once

#include <memory>
#include <QtGui/QWindow>


namespace Diligent
{

class IRenderDevice;
class IDeviceContext;
class ISwapChain;
class IEngineFactory;

class RenderWindowPrivate;
class RenderWindowFactory;

class RenderWindowListener
{
public:
	virtual ~RenderWindowListener() = default;

	virtual void initialize() = 0;
	virtual void render() = 0;
	virtual void resize(int width, int height) {}


public:

	IRenderDevice* device();
	IDeviceContext* context();
	ISwapChain* swapChain();
	IEngineFactory* engineFactory();

private:
	friend class RenderWindowFactory;
	RenderWindowPrivate* _private = nullptr;
};


class RenderWindowFactory
{
public:

	virtual ~RenderWindowFactory() = default;

public:

	virtual QWindow* createRenderWindow(RenderWindowListener* listener, QWindow* parent = nullptr) = 0;


	static RenderWindowFactory* getFactory(const std::string& renderBackend);

protected:

	void setupListener(RenderWindowListener* listener, RenderWindowPrivate* p);

	static void registerFactory(const std::string& renderBackend, RenderWindowFactory* factory);
	static void unregisterFactory(RenderWindowFactory* factory);

};


}
