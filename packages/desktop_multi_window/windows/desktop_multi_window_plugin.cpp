#include "include/desktop_multi_window/desktop_multi_window_plugin.h"
#include "multi_window_plugin_internal.h"

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>

#include "multi_window_manager.h"

#include <windows.h>
#include <sstream>
#include <algorithm>

namespace {
  // Add DeskTopDrop 
  using FlutterMethodChannel = std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>>;
  using namespace std::literals::string_literals;
  std::string ws2s(const std::wstring &wstr) {
        if (wstr.empty()) {
            return {};
        }
        size_t pos;
        size_t begin = 0;
        std::string ret;

        int size;
        pos = wstr.find(static_cast<wchar_t>(0), begin);
        while (pos != std::wstring::npos && begin < wstr.length()) {
            std::wstring segment = std::wstring(&wstr[begin], pos - begin);
            size = WideCharToMultiByte(CP_UTF8,
                                       WC_ERR_INVALID_CHARS,
                                       &segment[0],
                                       (int) segment.size(),
                                       nullptr,
                                       0,
                                       nullptr,
                                       nullptr);
            std::string converted = std::string(size, 0);
            WideCharToMultiByte(CP_UTF8,
                                WC_ERR_INVALID_CHARS,
                                &segment[0],
                                (int) segment.size(),
                                &converted[0],
                                (int) converted.size(),
                                nullptr,
                                nullptr);
            ret.append(converted);
            ret.append({0});
            begin = pos + 1;
            pos = wstr.find(static_cast<wchar_t>(0), begin);
        }
        if (begin <= wstr.length()) {
            std::wstring segment = std::wstring(&wstr[begin], wstr.length() - begin);
            size =
                    WideCharToMultiByte(CP_UTF8,
                                        WC_ERR_INVALID_CHARS,
                                        &segment[0],
                                        (int) segment.size(),
                                        nullptr,
                                        0,
                                        nullptr,
                                        nullptr);
            std::string converted = std::string(size, 0);
            WideCharToMultiByte(CP_UTF8,
                                WC_ERR_INVALID_CHARS,
                                &segment[0],
                                (int) segment.size(),
                                &converted[0],
                                (int) converted.size(),
                                nullptr,
                                nullptr);
            ret.append(converted);
        }

        return ret;
    }

  class DesktopMultiWindowPlugin : public flutter::Plugin {
  public:
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

    DesktopMultiWindowPlugin();

    ~DesktopMultiWindowPlugin() override;

  private:
    void HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue> &method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
  };

  class DesktopDropTarget : public IDropTarget {
  public:

      DesktopDropTarget(FlutterMethodChannel channel, HWND window_handle);

      HRESULT DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;

      HRESULT DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;

      HRESULT DragLeave() override;

      HRESULT Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;

      HRESULT QueryInterface(const IID &riid, void **ppvObject) override;

      ULONG AddRef() override;

      ULONG Release() override;

      virtual ~DesktopDropTarget();

  private:
      FlutterMethodChannel channel_;
      HWND window_handle_;
      LONG ref_count_;
      bool need_revoke_ole_initialize_;
  };

  class DesktopDropPlugin : public flutter::Plugin {

  public:
      static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

      explicit DesktopDropPlugin(DesktopDropTarget *target);

      ~DesktopDropPlugin() override;

  private:

      DesktopDropTarget *target_;

  };

  // static
  void DesktopMultiWindowPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar) {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "mixin.one/flutter_multi_window",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<DesktopMultiWindowPlugin>();

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result) {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });
    registrar->AddPlugin(std::move(plugin));

    HWND hwnd = nullptr;
    if (registrar->GetView()) {
        hwnd = registrar->GetView()->GetNativeWindow();
    }

    auto drop_target = new DesktopDropTarget(std::move(channel), hwnd);
    auto plugins = std::make_unique<DesktopDropPlugin>(drop_target);
    registrar->AddPlugin(std::move(plugins));
    
  }

  DesktopMultiWindowPlugin::DesktopMultiWindowPlugin() = default;

  DesktopMultiWindowPlugin::~DesktopMultiWindowPlugin() = default;

  void DesktopMultiWindowPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
    if (method_call.method_name() == "createWindow") {
      auto args = std::get_if<std::string>(method_call.arguments());
      auto window_id = MultiWindowManager::Instance()->Create(*args);
      

      result->Success(flutter::EncodableValue(window_id));
      return;
    } else if (method_call.method_name() == "show") {
      auto window_id = method_call.arguments()->LongValue();
      MultiWindowManager::Instance()->Show(window_id);
      result->Success();
      return;
    } else if (method_call.method_name() == "hide") {
      auto window_id = method_call.arguments()->LongValue();
      MultiWindowManager::Instance()->Hide(window_id);
      result->Success();
      return;
    } else if (method_call.method_name() == "close") {
      auto window_id = method_call.arguments()->LongValue();
      MultiWindowManager::Instance()->Close(window_id);
      result->Success();
      return;
    } else if (method_call.method_name() == "setFrame") {
      auto *arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
      auto window_id = arguments->at(flutter::EncodableValue("windowId")).LongValue();
      auto left = std::get<double_t>(arguments->at(flutter::EncodableValue("left")));
      auto top = std::get<double_t>(arguments->at(flutter::EncodableValue("top")));
      auto width = std::get<double_t>(arguments->at(flutter::EncodableValue("width")));
      auto height = std::get<double_t>(arguments->at(flutter::EncodableValue("height")));
      MultiWindowManager::Instance()->SetFrame(window_id, left, top, width, height);
      result->Success();
      return;
    } else if (method_call.method_name() == "center") {
      auto window_id = method_call.arguments()->LongValue();
      MultiWindowManager::Instance()->Center(window_id);
      result->Success();
      return;
    } else if (method_call.method_name() == "setTitle") {
      auto *arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
      auto window_id = arguments->at(flutter::EncodableValue("windowId")).LongValue();
      auto title = std::get<std::string>(arguments->at(flutter::EncodableValue("title")));
      MultiWindowManager::Instance()->SetTitle(window_id, title);
      result->Success();
      return;
    } else if (method_call.method_name() == "getAllSubWindowIds") {
      auto window_ids = MultiWindowManager::Instance()->GetAllSubWindowIds();
      result->Success(flutter::EncodableValue(window_ids));
      return;
    } else if (method_call.method_name() == "flashWindow") {
        auto window_id = method_call.arguments()->LongValue();
        MultiWindowManager::Instance()->FlashWindow(window_id);
        result->Success();
        return;
    } else if (method_call.method_name() == "focus") {  // add Foucs
        auto window_id = method_call.arguments()->LongValue();
        MultiWindowManager::Instance()->Focus(window_id);
        result->Success();
        return;
    } else if (method_call.method_name() == "isFocused") {  // add isFoucs
        auto window_id = method_call.arguments()->LongValue();
        auto value = MultiWindowManager::Instance()->IsFocused(window_id);
        result->Success(flutter::EncodableValue(value));
        return;
    } else if (method_call.method_name() == "getBounds") {  // add getBounds
        // auto window_id = method_call.arguments()->LongValue();
        const flutter::EncodableMap& args =
                std::get<flutter::EncodableMap>(*method_call.arguments());
        flutter::EncodableMap value = MultiWindowManager::Instance()->GetBounds(0,args);
        // flutter::EncodableMap value = window_manager->GetBounds(args);
        result->Success(flutter::EncodableValue(value));
        return;
    } else if (method_call.method_name() == "setTitleBarHidden"){
        auto window_id = method_call.arguments()->LongValue();
        MultiWindowManager::Instance()->SetTitleBarHidden(window_id);
        result->Success();
        return;
    } else if (method_call.method_name() == "setMinimumSize"){
        auto *arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
        auto window_id = arguments->at(flutter::EncodableValue("windowId")).LongValue();
        auto width = arguments->at(flutter::EncodableValue("width")).LongValue();
        auto height = arguments->at(flutter::EncodableValue("height")).LongValue();
        MultiWindowManager::Instance()->SetMinimumSize(window_id, width, height);
        result->Success();
        return;
    }
    result->NotImplemented();
  }

  DesktopDropTarget::DesktopDropTarget(FlutterMethodChannel channel, HWND window_handle) : channel_(
            std::move(channel)), window_handle_(window_handle), ref_count_(0), need_revoke_ole_initialize_(false) {
        auto ret = RegisterDragDrop(window_handle_, this);
        if (ret == E_OUTOFMEMORY) {
            OleInitialize(nullptr);
            ret = RegisterDragDrop(window_handle_, this);
            if (ret == 0) {
                need_revoke_ole_initialize_ = true;
            }
        }

        if (ret != 0) {
            // std::cout << "RegisterDragDrop failed: " << ret << std::endl;
        }
    }

  HRESULT DesktopDropTarget::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {
      POINT point = {pt.x, pt.y};
      ScreenToClient(window_handle_, &point);
      channel_->InvokeMethod("entered", std::make_unique<flutter::EncodableValue>(
              flutter::EncodableList{
                      flutter::EncodableValue(double(point.x)),
                      flutter::EncodableValue(double(point.y))
              }
      ));
      return 0;
  }

  HRESULT DesktopDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {
      POINT point = {pt.x, pt.y};
      ScreenToClient(window_handle_, &point);
      channel_->InvokeMethod("updated", std::make_unique<flutter::EncodableValue>(
              flutter::EncodableList{
                      flutter::EncodableValue(double(point.x)),
                      flutter::EncodableValue(double(point.y))
              }
      ));
      return 0;
  }

  HRESULT DesktopDropTarget::DragLeave() {
      channel_->InvokeMethod("exited", std::make_unique<flutter::EncodableValue>());
      return 0;
  }

  HRESULT DesktopDropTarget::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) {

      flutter::EncodableList list = {};

      // construct a FORMATETC object
      FORMATETC fmtetc = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
      STGMEDIUM stgmed;

      // See if the dataobject contains any TEXT stored as a HGLOBAL
      if (pDataObj->QueryGetData(&fmtetc) == S_OK) {
          // Yippie! the data is there, so go get it!
          if (pDataObj->GetData(&fmtetc, &stgmed) == S_OK) {
              // we asked for the data as a HGLOBAL, so access it appropriately
              PVOID data = GlobalLock(stgmed.hGlobal);
              if (data != nullptr) {
                  auto files = DragQueryFile(reinterpret_cast<HDROP>(data), 0xFFFFFFFF, nullptr, 0);
                  for (unsigned int i = 0; i < files; ++i) {
                      TCHAR filename[MAX_PATH];
                      DragQueryFile(reinterpret_cast<HDROP>(data), i, filename, sizeof(TCHAR) * MAX_PATH);
                      std::wstring wide(filename);
                      std::string path = ws2s(wide);
                    //   std::cout << "done: " << path << std::endl;
                      list.push_back(flutter::EncodableValue(path));
                  }
                  GlobalUnlock(stgmed.hGlobal);
              }

              // release the data using the COM API
              ReleaseStgMedium(&stgmed);
          }
      }
      channel_->InvokeMethod("performOperation", std::make_unique<flutter::EncodableValue>(list));

      return 0;
  }

  HRESULT DesktopDropTarget::QueryInterface(const IID &iid, void **ppvObject) {
      if (iid == IID_IDropTarget || iid == IID_IUnknown) {
          AddRef();
          *ppvObject = this;
          return S_OK;
      } else {
          *ppvObject = nullptr;
          return E_NOINTERFACE;
      }
  }

  ULONG DesktopDropTarget::AddRef() {
      return InterlockedIncrement(&ref_count_);
  }

  ULONG DesktopDropTarget::Release() {
      LONG count = InterlockedDecrement(&ref_count_);

      if (count == 0) {
          delete this;
          return 0;
      } else {
          return count;
      }
  }

  DesktopDropTarget::~DesktopDropTarget() {
      RevokeDragDrop(window_handle_);
      if (need_revoke_ole_initialize_) {
          OleUninitialize();
      }
  }

  DesktopDropPlugin::DesktopDropPlugin(DesktopDropTarget *target) : target_(target) {
        target_->AddRef();
    }

  DesktopDropPlugin::~DesktopDropPlugin() {
      target_->Release();
  }


  // static
  void DesktopDropPlugin::RegisterWithRegistrar(
          flutter::PluginRegistrarWindows *registrar) {
      auto channel =
              std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
                      registrar->messenger(), "desktop_drop",
                      &flutter::StandardMethodCodec::GetInstance());

      HWND hwnd = nullptr;
      if (registrar->GetView()) {
          hwnd = registrar->GetView()->GetNativeWindow();
      }

      if (hwnd == nullptr) {
          // no window, no drop.
          return;
      }

      channel->SetMethodCallHandler([](const auto &call, auto result) {
          result->NotImplemented();
      });

      auto drop_target = new DesktopDropTarget(std::move(channel), hwnd);

      auto plugin = std::make_unique<DesktopDropPlugin>(drop_target);

      registrar->AddPlugin(std::move(plugin));
  }

}  // namespace

void DesktopMultiWindowPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {

  InternalMultiWindowPluginRegisterWithRegistrar(registrar);

  // Attach MainWindow for
  auto hwnd = FlutterDesktopViewGetHWND(FlutterDesktopPluginRegistrarGetView(registrar));
  auto channel = WindowChannel::RegisterWithRegistrar(registrar, 0);
  MultiWindowManager::Instance()->AttachFlutterMainWindow(GetAncestor(hwnd, GA_ROOT),
                                                          std::move(channel));
}

void InternalMultiWindowPluginRegisterWithRegistrar(FlutterDesktopPluginRegistrarRef registrar) {
  DesktopMultiWindowPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

void DesktopDropPluginRegisterWithRegistrar(
        FlutterDesktopPluginRegistrarRef registrar) {
    DesktopDropPlugin::RegisterWithRegistrar(
            flutter::PluginRegistrarManager::GetInstance()
                    ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));

}

