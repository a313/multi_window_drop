//
//  Generated file. Do not edit.
//

// clang-format off

#include "generated_plugin_registrant.h"

#include <desktop_drop/desktop_drop_plugin.h>
#include <desktop_multi_window/desktop_multi_window_plugin.h>
#include <drag_and_drop_windows/drag_and_drop_windows_plugin.h>
#include <dragdropwindows/dragdropwindows_plugin.h>

void RegisterPlugins(flutter::PluginRegistry* registry) {
  DesktopDropPluginRegisterWithRegistrar(
      registry->GetRegistrarForPlugin("DesktopDropPlugin"));
  DesktopMultiWindowPluginRegisterWithRegistrar(
      registry->GetRegistrarForPlugin("DesktopMultiWindowPlugin"));
  DragAndDropWindowsPluginRegisterWithRegistrar(
      registry->GetRegistrarForPlugin("DragAndDropWindowsPlugin"));
  DragdropwindowsPluginRegisterWithRegistrar(
      registry->GetRegistrarForPlugin("DragdropwindowsPlugin"));
}
