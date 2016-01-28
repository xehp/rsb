
How to install tools needed to use Emscripten with SDL2 and OpenGL.


Copyright (C) 2015 Henrik Björkman




History:
Written in Mars 2015 by Henrik Bjorkman www.eit.se




In folder RsbClient is a demo program, it can be used to test that all tools are properly installed.


Tools for native build
======================

Mars 2015

remove old versions of emscripten (if any, it was needed when this was written in Mars 2015,  some year from now probably not needed):
sudo apt-get remove emscripten

First install the tools (gcc assumed to be installed already)
sudo apt-get install cmake  nodejs git default-jre libsdl2-dev eclipse-cdt freeglut3-dev libsdl2-image-dev libsdl2-ttf-dev libjpeg-dev libglm-dev libglew-dev glew-utils


If something is still missing try this:
sudo apt-get install build-essential clang llvm python2.7 libcgal-dev libsdl2-gfx-dev libsdl2-image-2.0-0   libjpeg-dev libsoil-dev





 

To compile this to native code use eclipse.


Or from command line (this is NOT updated, some cpp files may be missing here): 
g++ D3Matrix4x4.cpp D3PosAndOrientation.cpp D3Vector.cpp hello_world_sdl2.cpp -lm $(pkg-config --cflags --libs sdl2 gl glu)

To run it do (it does not crash but colors are not smooth as intended, and its not 100% full screen):
./a.out


To compile it with llvm clang do (this is NOT updated, some cpp files may be missing here): 
clang++ D3Matrix4x4.cpp D3PosAndOrientation.cpp D3Vector.cpp hello_world_sdl2.cpp -I /usr/include/SDL2 -lSDL2 -lSDL2 -lSOIL -lGL -lGLU -lm -o test

To run the compiled code do  (it does not crash but colors are not smooth as intended, and its not 100% full screen):
./test


Project files for eclipse are included.
But turn off code analyzer if you use eclipse since it will not work well with glm.










Installing emscripten Juni 2015
===============================

sudo apt-get install cmake nodejs libglm-dev


http://kripken.github.io/emscripten-site/docs/building_from_source/building_fastcomp_manually_from_source.html#building-fastcomp-from-source

cd ~/git

git clone https://github.com/kripken/emscripten-fastcomp

cd emscripten-fastcomp
git clone https://github.com/kripken/emscripten-fastcomp-clang tools/clang

mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD="X86;JSBackend" -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_TESTS=OFF -DCLANG_INCLUDE_EXAMPLES=OFF -DCLANG_INCLUDE_TESTS=OFF

make -j2








cd ~/git
git clone https://github.com/kripken/emscripten.git
cd emscripten
./emcc

update the ~/.emscripten file, specifying the location of fastcomp in the EMSCRIPTEN_ROOT and LLVM_ROOT variables.
EMSCRIPTEN_ROOT = '/home/henrik/git/emscripten'
LLVM_ROOT='/home/henrik/git/emscripten-fastcomp/build/bin'

./emcc


If there is a problem perhaps in ~/.emscripten change also:
NODE_JS = '/usr/bin/nodejs'



export PATH="/home/henrik/git/emscripten:$PATH"

cd ~/git
git clone https://github.com/gsathya/SDL-emscripten.git
cd SDL-emscripten/
emconfigure ./configure --host=asmjs-unknown-emscripten --disable-assembly --disable-threads --enable-cpuinfo=false CFLAGS="-O2"
emmake make









Compiling on legolas June 2015
==============================

cd ~/git/yukigassen/cpp
make help
make -j3




