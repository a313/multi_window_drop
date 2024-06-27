import 'dart:convert';
import 'dart:developer';

import 'package:desktop_multi_window/desktop_multi_window.dart';
// import 'package:dragdropwindows/dragdropwindows.dart';
import 'package:flutter/material.dart';

void main(List<String> args) {
  if (args.isEmpty) {
    runApp(const MainApp());
  } else {
    runApp(const SubApp());
  }
}

class MainApp extends StatelessWidget {
  const MainApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        body: Center(
          child: TextButton(
              onPressed: () async {
                final window = await DesktopMultiWindow.createWindow(
                    jsonEncode({'args1': 'multi_window'}));
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

class SubApp extends StatelessWidget {
  const SubApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        body: Center(
          child: TextButton(
              onPressed: () async {
                log('Sub app');
              },
              child: const Text('Sub window')),
        ),
      ),
    );
  }
}
