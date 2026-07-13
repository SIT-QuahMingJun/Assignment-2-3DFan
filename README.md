<!--
/*!***************************************************************************
\file       README.md
\author     m.quah
\par        m.quah@digipen.edu
\date       13/07/2026
\brief
    Provides a brief overview and build instructions for the Fan3D project.
***************************************************************************/
-->

# Assignment 2 - 3D Fan

A shared C++ OpenGL ES 3.0 project that renders an interactive 3D fan on Windows, Android, and WebGL. The same `Scene/` renderer, model, transform, and shader code is used by all three platforms.

## Controls

- Tap or left-click to switch the fan on or off.
- Drag to change the blade speed.
- Release after a drag to let the extra speed decay gradually.
- Press `Esc` to close the Desktop version.

## Desktop (Windows)

Requirements: Visual Studio 2022, CMake, Git, and an internet connection for the first dependency download.

```bat
script_build_and_run.bat
```

CMake fetches GLM, GLFW, and GLEW automatically.

## Android

Open the `android/` directory in Android Studio, allow Gradle and CMake to sync, and run the `app` configuration on an emulator or device. The native target fetches GLM automatically.

## Web

Activate the Emscripten SDK, then run:

```bat
build_web.bat
cd web
python -m http.server 8080
```

Open <http://localhost:8080/>.

On Linux or macOS:

```sh
./build_web.sh
cd web
python3 -m http.server 8080
```

The Web scripts download GLM into `external/` when it is not already available.

## Continuous integration

GitHub Actions builds the Desktop version on Windows and Linux, builds and uploads the Android debug APK, and builds the WebGL version. Pushes to `main` also deploy the contents of `web/` to GitHub Pages.

## Project layout

```text
Scene/                         Shared C++ renderer, fan model and transforms
android/                       Android Studio project and JNI entry point
android/app/src/main/assets/   Shared GLSL shader assets
cmake/                         CMake dependency setup
.github/workflows/             Desktop, Android and Web/Pages automation
build_web.bat / build_web.sh   Local Emscripten builds
```

## Clean generated files

Close Android Studio and any running build, then run from the project root:

```bat
cmake -P clean_all.cmake
```

This removes generated Desktop, Android, Web, downloaded dependency directories, IDE caches, and local machine configuration while preserving source files.
