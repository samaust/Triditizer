# README #

### What is this repository for? ###

Triditizer adds 3D monitors/tvs support to DirectX 9 games.

* Current features

Stereo modes : overunder

Basic laser sight

### How do I get set up? ###

* Building

Compiles on Visual Studio 2015.

* Dependencies

Currently depends on Microsoft Detours Version 1.5 (Build 46) to hook the DirectX functions.

* How to use

Inject into the game using a Windows dll injector. Xenos is recommended (https://github.com/DarthTon/Xenos). It is necessary to inject into the game early in its loading so that the vertex shader creation functions are hooked. This works with Xenos on Windows 10.

### Credits ###

Reading of ini settings file is done using a third-party library from CodeProject (http://www.codeproject.com/Articles/5401/CIni)

The menu was developped source code found on an online forum. See files headers for details.

### Who do I talk to? ###

* Repo owner