# Untitled C Game

Awesome game that I'm currently working on it's written from scratch using only C99 and OpenGL.

> Version 0.2.9

## Changelog

- `v0.2.0`: Changed underlying implementation for `VGFX_Window`, it's no longer just a typedef for `GLFWwindow`.
- `v0.2.0`: General code refactoring.
- `v0.2.1`: Implemented camera movement to make debugging the scene easier.
- `v0.2.2`: Created temporary `VGFX_Camera3D` type, to make traversing 3D Scenes easier.
- `v0.2.3`: Changed how `GLEW` is initialized.
- `v0.2.3`: Implemented `VGFX_Input`.
- `v0.2.3`: Implemented `VGFX_Window` based input handling system.
- `v0.2.4`: Fixed some bugs related to `VGFX_Input` and `VGFX_Window`.
- `v0.2.4`: Added more input functions to `VGFX_Input`.
- `v0.2.4`: Refactored `VGFX_Camera3D` to utilize the new input system.
- `v0.2.5`: Deleted `VGFX_Input` and `vgfx/input.c`.
- `v0.2.5`: Completely rewritten the input system to be event based.
- `v0.2.5`: Refactored `VGFX_Camera3D` to utilize the new input system, yet again.
- `v0.2.5`: Button input support for `VGFX_WindowEvent`.
- `v0.2.6`: Added new event types to `VGFX_WindowEvent`.
- `v0.2.6`: Changed how `VGFX_Window` window size and framebuffer size is handled.
- `v0.2.6`: Changed how `VGFX_Window` context is set as current, and changed how framebuffer size is set.
- `v0.2.7`: Small refactoring for `VGFX_WindowDescriptor`.
- `v0.2.7`: Refactored code to utilize new version of `vstd`.
- `v0.2.7`: Small refactoring to comments and code structure.
- `v0.2.8`: Created `VGFX_Font` and `VGFX_Glyph`.
- `v0.2.8`: Implemented SDF font rendring.
- `v0.2.9`: Changed to `VGFX_Texture2D` and how textures are bound.
- `v0.2.9`: Small refactoring. 
