#include <qdiligent/render_window_factory.hpp>

#include "render_window_private.hpp"

#include <map>

#include <dlfcn.h>
#include <libgen.h>    // dirname

namespace Diligent {


IRenderDevice* RenderWindowListener::device()
{
	return _private->device;
}

IDeviceContext* RenderWindowListener::context()
{
	return _private->immediateContext;
}

ISwapChain* RenderWindowListener::swapChain()
{
	return _private->swapChain;
}

IEngineFactory* RenderWindowListener::engineFactory()
{
	return _private->engineFactory;
}


void RenderWindowFactory::setupListener(RenderWindowListener* listener, RenderWindowPrivate* p)
{
	listener->_private = p;
}


typedef std::map<std::string, RenderWindowFactory*> FactoryRegistryMap;

FactoryRegistryMap& factoryRegistry()
{
	static FactoryRegistryMap s_factoryRegistry;
	return s_factoryRegistry;
}

std::string diligent_getLibraryPath(const std::string& backendName)
{
    // obtain library information containing this method
    Dl_info info;
    int res = dladdr((void*)&diligent_getLibraryPath, &info);

    if (!res)
        return backendName;

    // get dir from library name
    char* libname = strdup(info.dli_fname);
    char* path = dirname(libname);
    std::string spath(path);
    free(libname);

    return spath+"/libqdiligent-"+backendName+".so";
}

void loadLibrary(const std::string& filename)
{
    dlerror();    // clear any existing error
    void* handle = dlopen(filename.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (handle == nullptr)
        std::cerr << "Failed to load library '" << filename << "' :" << dlerror() << std::endl;
}


RenderWindowFactory* RenderWindowFactory::getFactory(const std::string& renderBackend)
{
    loadLibrary(diligent_getLibraryPath(renderBackend));

	FactoryRegistryMap& registry = factoryRegistry();
	auto it = registry.find(renderBackend);
	if(it==registry.end())
		return nullptr;
	return it->second;
}

void RenderWindowFactory::registerFactory(const std::string& renderBackend, RenderWindowFactory* factory)
{
	//std::cout << "Registering RenderWindowFactory " << renderBackend << ", " << factory << std::endl;
	FactoryRegistryMap& registry = factoryRegistry();
	registry[renderBackend] = factory;
}

void RenderWindowFactory::unregisterFactory(RenderWindowFactory* factory)
{
	//std::cout << "Unregistering RenderWindowFactory " << factory << std::endl;
	FactoryRegistryMap& registry = factoryRegistry();
	for(auto& p : registry)
		if(p.second == factory)
			p.second = nullptr;
}


}
