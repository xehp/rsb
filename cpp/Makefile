# Makefile
#
# History
# Created 2015-07-26 by Henrik Bjorkman www.eit.se using other habl Makefile as template
# Improved to use all .cpp files in src without needing to list them here.
# 2016-01-18 Cleanup so it can be used for HParser.
# Adapted to be used for P8c Code. /Henrik 2016-01-19
# Backported to drekkar/RSB/yukigassen. /Henrik 2016-01-19






# The file we want (and where we want it)
PROJ_NAME=../web/hybrid/hello_world_sdl2.js


# Where are the source files relative to the Makefile
# give give "." here if Makefile is in project root (the best place for it).
PROJ_ROOT=.



# List here all folders with source files
# If there are both .c and .cpp files then those are listed separately here.
# For each folder here a rule (aka recepie) is needed also, if not there will be a "No rule to make target" error.
CPP_FILES :=
CPP_FILES += $(wildcard $(PROJ_ROOT)/src/*.cpp)



# Object and dependecy files needed to make the executable file. 
# This will automatically add one for each C/C++ file.
OBJ_FILES    := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
OBJ_FILES    += $(addprefix obj/,$(notdir $(C_FILES:.c=.o)))
DEP          := $(addprefix dep/, $(SRC_FILES:.cpp=.d))


# Which compiler to use, see readme.txt on how to install it.

# If building for Win32 on a Linux use i686-w64-mingw32-, leave CROSS_PREFIX blank for native build
##CROSS_PREFIX   = i686-w64-mingw32-
#CROSS_PREFIX   = 
#CC             = ${CROSS_PREFIX}gcc
#CXX            = ${CROSS_PREFIX}g++
#GCOV           = ${CROSS_PREFIX}gcov
#STDC_LIB_PATH  = ${CROSS_PREFIX}/distribution/lib
#LD             = ${CROSS_PREFIX}ld
#AR             = ${CROSS_PREFIX}ar
#RANLIB         = ${CROSS_PREFIX}ranlib
#STRIP          = ${CROSS_PREFIX}strip

# In this case we cross compile for web
CXX=/home/$(USER)/git/emscripten/em++


# compiler options
# -O2 is optimization level
# -s ALLOW_MEMORY_GROWTH=1 is needed or else we run out of memory in the emscripten generated code.
# -s EXPORTED_FUNCTIONS is special for emscripten, we will call those functions from java script.
CC_FLAGS= -O2 -s ALLOW_MEMORY_GROWTH=1 
LD_FLAGS= -O2 -s EXPORTED_FUNCTIONS="['_main', '_mainHandleNumericEvent', '_mainHandleStringEvent']" -s ALLOW_MEMORY_GROWTH=1



# Turn on as many warnings as we can
CC_FLAGS += -Werror -Wall -Wextra 

# Our code quality is not that great. These warnings can perhaps be allowed.
# Really wish we did not need to allow these warnings but code will not compile as it is otherwise.
CC_FLAGS += -Wno-warn-absolute-paths
CC_FLAGS += -Wno-error=unused-parameter
CC_FLAGS += -Wno-error=ignored-qualifiers
CC_FLAGS += -Wno-error=unused-function
CC_FLAGS += -Wno-error=sign-compare
CC_FLAGS += -Wno-error=unused-variable


# This flag makes it so compiler stops at first error, it helps making the error visible.
CC_FLAGS += -Wfatal-errors

# Using -w can help us see the errors among all warnings. Use it only temporarily.
#CC_FLAGS += -w







# Some own proprietary compiler flags
# GLM_FORCE_RADIANS is needed for source compatibility with our native client.
CC_FLAGS += -DGLM_FORCE_RADIANS


# Include paths where to look for header files, all folder with header files need to be listed here or else there will be a "No such file or directory" error when building.
# -I... is include paths (tells where the header files are)
CC_FLAGS += -I/home/$(USER)/git/SDL-emscripten/include
CC_FLAGS += -I/usr/include/glm
CC_FLAGS += -I$(PROJ_ROOT)/inc
CC_FLAGS += -I$(PROJ_ROOT)/src


# We need to link some library files also, list those here.
# -l... is to include some library files in the build
LD_FLAGS += /home/$(USER)/git/SDL-emscripten/build/.libs/libSDL2.a
LD_FLAGS += -lGL
LD_FLAGS += -lGLU
LD_FLAGS += -lglut


# In this application we need to pack some images needed into the output .js file. List those needed here.
# --preload-file is special for emscripten to include some files needed by the client.
LD_FLAGS += --preload-file townhall.png
LD_FLAGS += --preload-file courier.png
LD_FLAGS += --preload-file snowscreen.png



#-------------------

# If user type just "make" on command line this is what is built
all: $(PROJ_NAME)

# If user give command "make help" this is what to do
help: 
	@echo "Usage: make [all|clean]"
	@echo ""
	@echo "On windows use mingw32-make instead of just make"
	@echo ""
	@echo "If compiler is not installed see: readme.txt"
	@echo "If compilation takes to long, try adding: -j3"
	@echo ""


# This is how to link the objects files into the executable file.
$(PROJ_NAME) : $(OBJ_FILES) 
	@echo ""
	$(CXX) $(OBJ_FILES) D3Matrix4x4.o D3Vector.o D3ObjWithPos.o D3Int.o D3PosAndOrientation.o $(LD_FLAGS) -o $(PROJ_NAME)
	@echo ""



# This is how to make the object files.
#
# http://stackoverflow.com/questions/231229/how-to-generate-a-makefile-with-source-in-sub-directories-using-just-one-makefil
# use "canned command sequences" in order to avoid repeating the actual compiler build rule:
# http://www.gnu.org/software/make/manual/make.html#Canned-Recipes
#
# @mkdir -p obj is to make sure obj directory exist
# The @ at the beginning of command is to tell make not to print the command being executed. 
# $@ is the full name of the file to make (that is the first word in the recepie).
# $< expands to the path where make located the first dependency (sort of a short for the first word after ':' in the recepie).
define cxx-command
@echo ""
@mkdir -p obj
$(CXX) $(CC_FLAGS) -c -o $@ $<
@echo ""
endef

define cc-command
@echo ""
@mkdir -p obj
$(CC) $(CC_FLAGS) -c -o $@ $<
@echo ""
endef




# One recepie (aka rule) is needed for every folder in CPP_FILES & C_FILES, otherwise there will be a "*** No rule to make target" error when doing make.

#obj/%.o: src/%.c
#	$(cc-command)


obj/%.o: src/%.cpp
	$(cxx-command)







# If user give command "make clean" this is what to do
# Well we could use "rm -rf obj" but just in case user put something there that he/she wants to keep.
clean:
	rm -f obj/*.o
	rm -f obj/*.d
	rmdir obj






# Automatic dependency graph generation
# -MMD means that for each .o file a .d file shall also be generated.
# -include means we include the dependency file for each object file to make into this Makefile.
CC_FLAGS += -MMD
-include $(OBJ_FILES:.o=.d)




