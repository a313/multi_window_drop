import 'dart:convert';

import 'package:desktop_multi_window/desktop_multi_window.dart';
import 'package:drag_and_drop_windows/drag_and_drop_windows.dart';
// import 'package:dragdropwindows/dragdropwindows.dart';
import 'package:flutter/material.dart';

void main(List<String> args) {
  if (args.isEmpty) {
    runApp(const MainApp());
  } else {
    runApp(const SubApp());
  }
}

class MainApp extends StatefulWidget {
  const MainApp({super.key});

  @override
  State<MainApp> createState() => _MainAppState();
}

class _MainAppState extends State<MainApp> {
  @override
  void initState() {
    super.initState();
    dropEventStream.listen((paths) {
      print('MainApp dropEventStream: $paths');
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        body: Center(
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
                  ..setFrame(const Offset(0, 0) & const Size(400, 400))
                  ..center()
                  ..setTitle('Another window')
                  ..show();
              },
              child: const Text('Show Sub window')),
        ),
      ),
    );
  }
}

class SubApp extends StatefulWidget {
  const SubApp({super.key});

  @override
  State<SubApp> createState() => _SubAppState();
}

class _SubAppState extends State<SubApp> {
  @override
  void initState() {
    super.initState();
    dropEventStream.listen((paths) {
      print('SubApp dropEventStream: $paths');
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        body: Center(
          child: TextButton(
              onPressed: () async {
                print('Sub app');
              },
              child: const Text('Sub window')),
        ),
      ),
    );
  }
}
