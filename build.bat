@ECHO OFF
cl /Zi /DDEBUG /DSDL_MAIN_HANDLED /EHsc src/*.cpp src/system/*.cpp src/game/*.cpp src/graphics/*.cpp src/math/*.cpp /Iinclude/ lib/*.lib OpenGL32.lib /Foobj/ /Febin/Ge1120.exe	