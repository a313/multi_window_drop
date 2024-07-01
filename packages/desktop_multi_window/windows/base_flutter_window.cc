//
// Created by yangbin on 2022/1/27.
//

#include "base_flutter_window.h"

namespace {
void CenterRectToMonitor(LPRECT prc) {
  HMONITOR hMonitor;
  MONITORINFO mi;
  RECT rc;
  int w = prc->right - prc->left;
  int h = prc->bottom - prc->top;

  //
  // get the nearest monitor to the passed rect.
  //
  hMonitor = MonitorFromRect(prc, MONITOR_DEFAULTTONEAREST);

  //
  // get the work area or entire monitor rect.
  //
  mi.cbSize = sizeof(mi);
  GetMonitorInfo(hMonitor, &mi);

  rc = mi.rcMonitor;

  prc->left = rc.left + (rc.right - rc.left - w) / 2;
  prc->top = rc.top + (rc.bottom - rc.top - h) / 2;
  prc->right = prc->left + w;
  prc->bottom = prc->top + h;

}

std::wstring Utf16FromUtf8(const std::string &string) {
  int size_needed = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, nullptr, 0);
  if (size_needed == 0) {
    return {};
  }
  std::wstring wstrTo(size_needed, 0);
  int converted_length = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, &wstrTo[0], size_needed);
  if (converted_length == 0) {
    return {};
  }
  return wstrTo;
}

}

void BaseFlutterWindow::Center() {
  auto handle = GetWindowHandle();
  if (!handle) {
    return;
  }
  RECT rc;
  GetWindowRect(handle, &rc);
  CenterRectToMonitor(&rc);
  SetWindowPos(handle, nullptr, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void BaseFlutterWindow::SetBounds(double_t x, double_t y, double_t width, double_t height) {
  auto handle = GetWindowHandle();
  if (!handle) {
    return;
  }
  MoveWindow(handle, int32_t(x), int32_t(y),
             static_cast<int>(width),
             static_cast<int>(height),
             TRUE);
}

void BaseFlutterWindow::SetTitle(const std::string &title) {
  auto handle = GetWindowHandle();
  if (!handle) {
    return;
  }
  SetWindowText(handle, Utf16FromUtf8(title).c_str());
}

void BaseFlutterWindow::Close() {
  auto handle = GetWindowHandle();
  if (!handle) {
    return;
  }
  PostMessage(handle, WM_SYSCOMMAND, SC_CLOSE, 0);
}

void BaseFlutterWindow::Show() {
    auto handle = GetWindowHandle();
    if (!handle) {
        return;
    }

    DWORD gwlStyle = GetWindowLong(handle, GWL_STYLE);
    gwlStyle = gwlStyle | WS_VISIBLE;
    if ((gwlStyle & WS_VISIBLE) == 0) {
        SetWindowLong(handle, GWL_STYLE, gwlStyle);
        ::SetWindowPos(handle, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    }

    ShowWindowAsync(handle, SW_SHOW);
    SetForegroundWindow(handle);
}

//void BaseFlutterWindow::Show() {
//  auto handle = GetWindowHandle();
//  if (!handle) {
//    return;
//  }
//  ShowWindow(handle, SW_SHOW);
//
//}

void BaseFlutterWindow::Hide() {
  auto handle = GetWindowHandle();
  if (!handle) {
    return;
  }
  ShowWindow(handle, SW_HIDE);
}

//add Flash
void BaseFlutterWindow::FlashWindow(){
    auto handle = GetWindowHandle();
    if(!handle){
        return;
    }
    FLASHWINFO fi;
    fi.cbSize = sizeof(FLASHWINFO);
    fi.hwnd = handle;
    fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
    fi.uCount = 0;
    fi.dwTimeout = 0;
    ::FlashWindowEx(&fi);
}
//add focus
//add focus
void BaseFlutterWindow::Focus(){
    auto handle = GetWindowHandle();
    if(!handle) {
        return;
    }


    if (IsMinimized()) {
        Restore();
    }

    ::SetWindowPos(handle, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    SetForegroundWindow(handle);
}

//add IsFocused
bool BaseFlutterWindow::IsFocused(){
    return GetWindowHandle() == GetActiveWindow();
}

//add bounds
flutter::EncodableMap BaseFlutterWindow::GetBounds(
        const flutter::EncodableMap& args) {
    auto handle = GetWindowHandle();

    double devicePixelRatio =
            std::get<double>(args.at(flutter::EncodableValue("devicePixelRatio")));

    flutter::EncodableMap resultMap = flutter::EncodableMap();
    RECT rect;
    if (GetWindowRect(handle, &rect)) {
        double x = rect.left / devicePixelRatio * 1.0f;
        double y = rect.top / devicePixelRatio * 1.0f;
        double width = (rect.right - rect.left) / devicePixelRatio * 1.0f;
        double height = (rect.bottom - rect.top) / devicePixelRatio * 1.0f;

        resultMap[flutter::EncodableValue("x")] = flutter::EncodableValue(x);
        resultMap[flutter::EncodableValue("y")] = flutter::EncodableValue(y);
        resultMap[flutter::EncodableValue("width")] =
                flutter::EncodableValue(width);
        resultMap[flutter::EncodableValue("height")] =
                flutter::EncodableValue(height);
    }
    return resultMap;
}

// add IsMinimized
bool BaseFlutterWindow::IsMinimized() {
    auto handle = GetWindowHandle();
    // if(!handle) {
    //   return;
    // }
    WINDOWPLACEMENT windowPlacement;
    GetWindowPlacement(handle, &windowPlacement);

    return windowPlacement.showCmd == SW_SHOWMINIMIZED;
}
// add Restore
void BaseFlutterWindow::Restore() {
    auto handle = GetWindowHandle();
    if(!handle) {
        return;
    }
    WINDOWPLACEMENT windowPlacement;
    GetWindowPlacement(handle, &windowPlacement);

    if (windowPlacement.showCmd != SW_NORMAL) {
        PostMessage(handle, WM_SYSCOMMAND, SC_RESTORE, 0);
    }
}

// add titleBarHidden
void BaseFlutterWindow::SetTitleBarHidden() {
    auto handle = GetWindowHandle();
    if(!handle) {
        return;
    }

    RECT rect;

    GetWindowRect(handle, &rect);
    SetWindowLong(handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(handle, HWND_TOPMOST, rect.left, rect.top, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE |
                 SWP_SHOWWINDOW);


}
