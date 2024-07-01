import 'dart:io';
import 'dart:ui';

import 'package:flutter/services.dart';

import 'channels.dart';
import 'window_controller.dart';

class WindowControllerMainImpl extends WindowController {
  final MethodChannel _channel = multiWindowChannel;

  // the id of this window
  final int _id;

  WindowControllerMainImpl(this._id);

  @override
  int get windowId => _id;

  @override
  Future<void> close() {
    return _channel.invokeMethod('close', _id);
  }

  @override
  Future<void> hide() {
    return _channel.invokeMethod('hide', _id);
  }

  @override
  Future<void> show() {
    return _channel.invokeMethod('show', _id);
  }

  @override
  Future<void> center() {
    return _channel.invokeMethod('center', _id);
  }

  @override
  Future<void> setFrame(Rect frame) {
    return _channel.invokeMethod('setFrame', <String, dynamic>{
      'windowId': _id,
      'left': frame.left,
      'top': frame.top,
      'width': frame.width,
      'height': frame.height,
    });
  }

  @override
  Future<void> setTitle(String title) {
    return _channel.invokeMethod('setTitle', <String, dynamic>{
      'windowId': _id,
      'title': title,
    });
  }

  @override
  Future<void> resizable(bool resizable) {
    if (Platform.isMacOS) {
      return _channel.invokeMethod('resizable', <String, dynamic>{
        'windowId': _id,
        'resizable': resizable,
      });
    } else {
      throw MissingPluginException(
        'This functionality is only available on macOS',
      );
    }
  }

  @override
  Future<void> setFrameAutosaveName(String name) {
    return _channel.invokeMethod('setFrameAutosaveName', <String, dynamic>{
      'windowId': _id,
      'name': name,
    });
  }

  //add flashWindow
  @override
  Future<void> flashWindow() {
    return _channel.invokeMethod('flashWindow', _id);
  }

  //add foucs
  @override
  Future<void> focus() {
    return _channel.invokeMethod('focus', _id);
  }

  //add isFoucs
  @override
  Future<bool> isFocused() async{
    return await _channel.invokeMethod('isFocused', _id);
  }

  /// Returns `Rect` - The bounds of the window as Object.
  /// add
  double getDevicePixelRatio() {
    // Subsequent version, remove this deprecated member.
    // ignore: deprecated_member_use
    return window.devicePixelRatio;
  }
  // add getBounds
  Future<Rect> getBounds() async {
    final Map<String, dynamic> arguments = {
      'windowId' : _id,
      'devicePixelRatio': getDevicePixelRatio(),
    };
    final Map<dynamic, dynamic> resultData = await _channel.invokeMethod(
      'getBounds',
      arguments,
    );

    return Rect.fromLTWH(
      resultData['x'],
      resultData['y'],
      resultData['width'],
      resultData['height'],
    );
  }

  //add setTitleBarHidden
  @override
  Future<void> setTitleBarHidden() {
    return _channel.invokeMethod('setTitleBarHidden', _id);
  }

  //add setMinimumSize
  Future<void> SetMinimumSize(int width, int height) {
    return _channel.invokeMethod('setMinimumSize', <String, dynamic>{
      'windowId': _id,
      'width': width,
      'height': height,
    });
  }
}
