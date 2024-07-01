//
// Created by yangbin on 2022/1/27.
//

#ifndef MULTI_WINDOW_WINDOWS_BASE_FLUTTER_WINDOW_H_
#define MULTI_WINDOW_WINDOWS_BASE_FLUTTER_WINDOW_H_

#include "window_channel.h"

class BaseFlutterWindow {

 public:

  virtual ~BaseFlutterWindow() = default;

  virtual WindowChannel *GetWindowChannel() = 0;

  void Show();

  void Hide();

  void Close();

  void SetTitle(const std::string &title);

  void SetBounds(double_t x, double_t y, double_t width, double_t height);

  void Center();

  //add FlashWindow
  void FlashWindow();

  //add focus
  void Focus();

  //add isFocus
  bool IsFocused();

  //add getBounds
  flutter::EncodableMap GetBounds(const flutter::EncodableMap& args);

  //add IsMinimized
  bool IsMinimized();

  //add Restore
  void Restore();

  // add titleBar
  void SetTitleBarHidden();


  void SetMinimumSize(int64_t width, int64_t height) {
      m_rcMinimumSize.x = (LONG) width;
      m_rcMinimumSize.y = (LONG) height;
      m_bHasMinSize = true;
  };

public:
    bool m_bHasMinSize = false;
    POINT m_rcMinimumSize = {0,0};

protected:

  virtual HWND GetWindowHandle() = 0;

};

#endif //MULTI_WINDOW_WINDOWS_BASE_FLUTTER_WINDOW_H_
