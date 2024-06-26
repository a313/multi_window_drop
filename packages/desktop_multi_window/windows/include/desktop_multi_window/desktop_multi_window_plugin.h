#ifndef FLUTTER_PLUGIN_DESKTOP_MULTI_WINDOW_PLUGIN_H_
#define FLUTTER_PLUGIN_DESKTOP_MULTI_WINDOW_PLUGIN_H_

#include <flutter_plugin_registrar.h>

#ifdef FLUTTER_PLUGIN_IMPL
#define FLUTTER_PLUGIN_EXPORT __declspec(dllexport)
#else
#define FLUTTER_PLUGIN_EXPORT __declspec(dllimport)
#endif

#if defined(__cplusplus)
extern "C" {
#endif

// DeskTopDropPluginRegister
FLUTTER_PLUGIN_EXPORT void DesktopDropPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar);

FLUTTER_PLUGIN_EXPORT void DesktopMultiWindowPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar);

// flutter_view_controller: pointer to the flutter::FlutterViewController
typedef void (*WindowCreatedCallback)(void *flutter_view_controller);
FLUTTER_PLUGIN_EXPORT void DesktopMultiWindowSetWindowCreatedCallback(WindowCreatedCallback callback);

#if defined(__cplusplus)
}  // extern "C"
#endif

#endif  // FLUTTER_PLUGIN_DESKTOP_MULTI_WINDOW_PLUGIN_H_
