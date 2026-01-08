import 'package:flutter/material.dart';

void main() {
  runApp(const AuraGlowApp());
}

class AuraGlowApp extends StatelessWidget {
  const AuraGlowApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Aura Glow (Scaffold)',
      theme: ThemeData.dark(useMaterial3: true),
      home: const Scaffold(
        body: Center(
          child: Text(
            'Aura Glow scaffold is installed.\nNext: wire camera + FFI + shaders.',
            textAlign: TextAlign.center,
          ),
        ),
      ),
    );
  }
}
