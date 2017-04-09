
How to install tools needed to use Emscripten with SDL2 and OpenGL.


Copyright (C) 2015 Henrik Björkman




History:
Written in Mars 2015 by Henrik Bjorkman www.eit.se




In folder RsbClient is a demo program, it can be used to test that all tools are properly installed.


Tools for native build
======================


First install the tools:
sudo apt-get install cmake  nodejs git default-jre libsdl2-dev eclipse-cdt freeglut3-dev libsdl2-image-dev libsdl2-ttf-dev libjpeg-dev libglm-dev libglew-dev glew-utils gcc g++ libsdl2-mixer-dev libsdl2-dev libsdl2-mixer-dev


Currently this is also needed:
sudo apt-get install  default-jdk

If something is still missing try some of these (although I think these are not needed): 
sudo apt-get install build-essential clang llvm python2.7 libcgal-dev libsdl2-gfx-dev libsdl2-image-2.0-0   libjpeg-dev libsoil-dev

Old libraries installing them probably only confuse things (but I had them installed so they may be needed anyway):
sudo apt-get install libsdl-image1.2-dev



 

To compile this to native code use eclipse.








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

update the ~/.emscripten file, specifying the location of fastcomp in the EMSCRIPTEN_ROOT and LLVM_ROOT variables (remember to replace henrik with you username here and elsewhere in this instruction):
EMSCRIPTEN_ROOT = '/home/henrik/git/emscripten'
LLVM_ROOT='/home/henrik/git/emscripten-fastcomp/build/bin'


Try emcc again:
./emcc

Result should look something like this:
INFO:root:(Emscripten: Running sanity checks)
WARNING:root:no input files




If there is a problem perhaps in ~/.emscripten change also:
NODE_JS = '/usr/bin/nodejs'



export PATH="/home/henrik/git/emscripten:$PATH"
#export PATH="~/git/emscripten:$PATH"

cd ~/git
git clone https://github.com/gsathya/SDL-emscripten.git
cd SDL-emscripten/
emconfigure ./configure --host=asmjs-unknown-emscripten --disable-assembly --disable-threads --enable-cpuinfo=false CFLAGS="-O2"
emmake make









Compiling on legolas June 2015
==============================

cd ~/git/yukigassen/RsbClient
make help
make -j3




Compiling on Mac 2017
=====================

brew cask install eclipse-java eclipse-ide eclipse-cpp


That did not help

tried to download from
http://www.eclipse.org/cdt/

That worked better



brew install cmake glew SDL2 GLM SDL2_image freeglut glm SDL2_mixer

It compiles but does not link
ld: library not found for -lGL
http://stackoverflow.com/questions/3026478/help-solving-a-error-in-opengl
-framework OpenGL
Now it can run, but no debugger.

In order to compile on Linux again I had to remove "-framework OpenGL" and put -lGL & -lGLU back. GL and GLU need to be after SDL in the list of libs.
Dont know how to make an eclipse setting that supports both ways so next tim compiling on MAC the settings must be changed again.






