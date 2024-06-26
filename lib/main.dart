import 'dart:async';
import 'dart:convert';

import 'package:desktop_drop/desktop_drop.dart';
import 'package:desktop_multi_window/desktop_multi_window.dart';
import 'package:drag_and_drop_windows/drag_and_drop_windows.dart';
import 'package:dragdropwindows/dragdropwindows.dart';
import 'package:flutter/material.dart';

void main(List<String> args) {
  if (args.firstOrNull == 'multi_window') {
    runApp(const SubApp());
  } else {
    runApp(const MainApp());
  }
}

class MainApp extends StatelessWidget {
  const MainApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        body: DropTarget(
          onDragDone: (detail) {
            print('onDragDone: $detail');
          },
          onDragEntered: (detail) {
            print('onDragEntered: $detail');
          },
          onDragExited: (detail) {
            print('onDragExited: $detail');
          },
          child: Center(
            child: TextButton(
                onPressed: () async {
                  final window =
                      await DesktopMultiWindow.createWindow(jsonEncode({
                    'args1': 'multi_window',
                    'args2': 100,
                    'args3': true,
                    'bussiness': 'bussiness_test',
                  }));
                  window
                    ..setFrame(const Offset(0, 0) & const Size(1280, 720))
                    ..center()
                    ..setTitle('Another window')
                    ..show();
                },
                child: const Text('Show Sub window')),
          ),
        ),
      ),
    );
  }
}

class SubApp extends StatelessWidget {
  const SubApp({super.key});

  @override
  Widget build(BuildContext context) {
    StreamSubscription subscription = dropEventStream.listen((paths) {
      print('dropEventStream: $paths');
    });

    try {
      Dragdropwindows.start((DropType type, List<String> results) {
        print("Dragdropwindows type=$type results=$results");
      });
    } catch (e) {}

    return MaterialApp(
      home: Scaffold(
        body: Center(
          child: DropTarget(
            onDragDone: (detail) {
              print('onDragDone: $detail');
            },
            onDragEntered: (detail) {
              print('onDragEntered: $detail');
            },
            onDragExited: (detail) {
              print('onDragExited: $detail');
            },
            child: TextButton(
                onPressed: () async {
                  print('Sub app');
                },
                child: const Text('Sub window')),
          ),
        ),
      ),
    );
  }
}
