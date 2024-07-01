//
// Created by yangbin on 2022/1/11.
//

#ifndef DESKTOP_MULTI_WINDOW_WINDOWS_MULTI_WINDOW_MANAGER_H_
#define DESKTOP_MULTI_WINDOW_WINDOWS_MULTI_WINDOW_MANAGER_H_

#include <cstdint>
#include <string>
#include <map>

#include "base_flutter_window.h"
#include "flutter_window.h"

class MultiWindowManager : public std::enable_shared_from_this<MultiWindowManager>, public FlutterWindowCallback {

 public:
  static MultiWindowManager *Instance();

  MultiWindowManager();

  int64_t Create(std::string args);

  void AttachFlutterMainWindow(HWND main_window_handle, std::unique_ptr<WindowChannel> window_channel);

  void Show(int64_t id);

  void Hide(int64_t id);

  void Close(int64_t id);

  void SetFrame(int64_t id, double_t x, double_t y, double_t width, double_t height);

  void Center(int64_t id);

  void SetTitle(int64_t id, const std::string &title);

  std::vector<int64_t> GetAllSubWindowIds();

  void OnWindowClose(int64_t id) override;

  void OnWindowDestroy(int64_t id) override;

  //add FlashWindow
  void FlashWindow(int64_t id);

  //add Focus
  void Focus(int64_t id);

  //add IsFocused
  bool IsFocused(int64_t id);

  //add GetBounds
  flutter::EncodableMap GetBounds(int64_t id, const flutter::EncodableMap& args);

  //add SetTitleBarHidden
  void SetTitleBarHidden(int64_t id);

  //add Set Window Minimum size
  void SetMinimumSize(int64_t id, int64_t width, int64_t height);

 private:

  std::map<int64_t, std::unique_ptr<BaseFlutterWindow>> windows_;

  void HandleWindowChannelCall(
      int64_t from_window_id,
      int64_t target_window_id,
      const std::string &call,
      flutter::EncodableValue *arguments,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result
  );

};

#endif //DESKTOP_MULTI_WINDOW_WINDOWS_MULTI_WINDOW_MANAGER_H_
