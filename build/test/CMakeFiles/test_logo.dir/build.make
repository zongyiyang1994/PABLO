# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/edoardo/git/PABLO

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/edoardo/git/PABLO/build

# Include any dependencies generated for this target.
include test/CMakeFiles/test_logo.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/test_logo.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/test_logo.dir/flags.make

test/CMakeFiles/test_logo.dir/test_logo.cpp.o: test/CMakeFiles/test_logo.dir/flags.make
test/CMakeFiles/test_logo.dir/test_logo.cpp.o: ../test/test_logo.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/edoardo/git/PABLO/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object test/CMakeFiles/test_logo.dir/test_logo.cpp.o"
	cd /home/edoardo/git/PABLO/build/test && mpic++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/test_logo.dir/test_logo.cpp.o -c /home/edoardo/git/PABLO/test/test_logo.cpp

test/CMakeFiles/test_logo.dir/test_logo.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_logo.dir/test_logo.cpp.i"
	cd /home/edoardo/git/PABLO/build/test && mpic++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/edoardo/git/PABLO/test/test_logo.cpp > CMakeFiles/test_logo.dir/test_logo.cpp.i

test/CMakeFiles/test_logo.dir/test_logo.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_logo.dir/test_logo.cpp.s"
	cd /home/edoardo/git/PABLO/build/test && mpic++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/edoardo/git/PABLO/test/test_logo.cpp -o CMakeFiles/test_logo.dir/test_logo.cpp.s

test/CMakeFiles/test_logo.dir/test_logo.cpp.o.requires:
.PHONY : test/CMakeFiles/test_logo.dir/test_logo.cpp.o.requires

test/CMakeFiles/test_logo.dir/test_logo.cpp.o.provides: test/CMakeFiles/test_logo.dir/test_logo.cpp.o.requires
	$(MAKE) -f test/CMakeFiles/test_logo.dir/build.make test/CMakeFiles/test_logo.dir/test_logo.cpp.o.provides.build
.PHONY : test/CMakeFiles/test_logo.dir/test_logo.cpp.o.provides

test/CMakeFiles/test_logo.dir/test_logo.cpp.o.provides.build: test/CMakeFiles/test_logo.dir/test_logo.cpp.o

# Object files for target test_logo
test_logo_OBJECTS = \
"CMakeFiles/test_logo.dir/test_logo.cpp.o"

# External object files for target test_logo
test_logo_EXTERNAL_OBJECTS =

test/test_logo: test/CMakeFiles/test_logo.dir/test_logo.cpp.o
test/test_logo: test/CMakeFiles/test_logo.dir/build.make
test/test_logo: lib/libpablo.a
test/test_logo: test/CMakeFiles/test_logo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable test_logo"
	cd /home/edoardo/git/PABLO/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_logo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/test_logo.dir/build: test/test_logo
.PHONY : test/CMakeFiles/test_logo.dir/build

test/CMakeFiles/test_logo.dir/requires: test/CMakeFiles/test_logo.dir/test_logo.cpp.o.requires
.PHONY : test/CMakeFiles/test_logo.dir/requires

test/CMakeFiles/test_logo.dir/clean:
	cd /home/edoardo/git/PABLO/build/test && $(CMAKE_COMMAND) -P CMakeFiles/test_logo.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/test_logo.dir/clean

test/CMakeFiles/test_logo.dir/depend:
	cd /home/edoardo/git/PABLO/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/edoardo/git/PABLO /home/edoardo/git/PABLO/test /home/edoardo/git/PABLO/build /home/edoardo/git/PABLO/build/test /home/edoardo/git/PABLO/build/test/CMakeFiles/test_logo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/test_logo.dir/depend
