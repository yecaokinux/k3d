QT4_WRAP_CPP(MOC_BUILT_SOURCES window.h)
QT4_WRAP_UI(UI_BUILT_SOURCES window.ui)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

K3D_BUILD_QTUI_MODULE(k3d-qtui-text-editor EXTRA_SOURCES ${MOC_BUILT_SOURCES} ${UI_BUILT_SOURCES})
K3D_CREATE_MODULE_PROXY(k3d-qtui-text-editor)

