cmake_minimum_required (VERSION 3.8)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(qdiligent)

find_package(Qt5 COMPONENTS Widgets X11Extras OpenGL REQUIRED)




add_subdirectory(DiligentCore) #  EXCLUDE_FROM_ALL)
add_subdirectory(DiligentTools) # EXCLUDE_FROM_ALL)
add_subdirectory(DiligentFX) #    EXCLUDE_FROM_ALL)



add_library(qdiligent SHARED
  src/render_window_factory.cpp
)
set_target_properties(qdiligent PROPERTIES
  # Disallow missing direct and indirect dependencies to enssure that .so is self-contained
  LINK_FLAGS "-Wl,--no-undefined -Wl,--no-allow-shlib-undefined"
)
target_compile_features(qdiligent PUBLIC cxx_std_11)
set_target_properties(qdiligent PROPERTIES CXX_EXTENSIONS OFF)
set_common_target_properties(qdiligent)
target_compile_definitions(qdiligent PUBLIC ENGINE_DLL=1)
target_compile_definitions(qdiligent
  PUBLIC
    PLATFORM_LINUX
    GL_SUPPORTED
    VULKAN_SUPPORTED
)
target_include_directories(qdiligent
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DiligentCore/Common/interface/>  
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:include/>  
)
  
target_link_libraries(qdiligent
  PRIVATE
    ${CMAKE_DL_LIBS}
#    glslang
#    SPIRV
#    spirv-cross-core
    -Wl,--whole-archive 
    Diligent-Common
    Diligent-GraphicsEngineNextGenBase 
    Diligent-GraphicsAccessories
    Diligent-GraphicsTools
    Diligent-GraphicsEngine
    Diligent-TextureLoader
    Diligent-AssetLoader
    DiligentFX
    Diligent-Imgui
    -Wl,--no-whole-archive
  PUBLIC
    Qt5::Widgets
    X11
)

add_library(qdiligent-opengl SHARED
  src/render_window_factory_lnx_opengl.cpp
)
target_link_libraries(qdiligent-opengl
  PRIVATE
    qdiligent
    Qt5::X11Extras
    -Wl,--no-whole-archive
    Diligent-GraphicsEngineOpenGL-static
    -Wl,--no-whole-archive
  PUBLIC
)

add_library(qdiligent-vulkan SHARED
  src/render_window_factory_lnx_vulkan.cpp
)
target_link_libraries(qdiligent-vulkan
  PRIVATE
    qdiligent
    Qt5::X11Extras
    -Wl,--whole-archive 
    Diligent-GraphicsEngineVk-static
    -Wl,--no-whole-archive
  PUBLIC
)

install(TARGETS qdiligent qdiligent-opengl qdiligent-vulkan
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
  COMPONENT Develop
  DESTINATION include
)



set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "epicodic@web.de")
set(CPACK_PACKAGING_INSTALL_PREFIX=/opt/qdiligent)
set(CPACK_DEBIAN_PACKAGE_NAME=qdiligent)
set(CPACK_PACKAGE_VERSION "0.1")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Diligent Engine with Qt integration")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "qdiligent")
set(CPACK_PACKAGE_NAME "qdiligent")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

