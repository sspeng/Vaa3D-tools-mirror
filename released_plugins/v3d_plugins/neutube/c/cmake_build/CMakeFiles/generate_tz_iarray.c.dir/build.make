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
CMAKE_COMMAND = "/Applications/CMake 2.8-9.app/Contents/bin/cmake"

# The command to remove a file.
RM = "/Applications/CMake 2.8-9.app/Contents/bin/cmake" -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = "/Applications/CMake 2.8-9.app/Contents/bin/ccmake"

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/zhaot/Work/neutube/neurolabi/c

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/zhaot/Work/neutube/neurolabi/c/cmake_build

# Utility rule file for generate_tz_iarray.c.

# Include the progress variables for this target.
include CMakeFiles/generate_tz_iarray.c.dir/progress.make

CMakeFiles/generate_tz_iarray.c: ../tz_iarray.c

../tz_iarray.c: ../tz_array.c.t
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/zhaot/Work/neutube/neurolabi/c/cmake_build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating /Users/zhaot/Work/neutube/neurolabi/c/tz_iarray.c"
	cd /Users/zhaot/Work/neutube/neurolabi/c && [ ! -f /Users/zhaot/Work/neutube/neurolabi/c/tz_iarray.c ] || chmod u+w /Users/zhaot/Work/neutube/neurolabi/c/tz_iarray.c
	cd /Users/zhaot/Work/neutube/neurolabi/c && perl ../shell/parsetmpl.pl /Users/zhaot/Work/neutube/neurolabi/c/tz_array.c.t /Users/zhaot/Work/neutube/neurolabi/c/tz_iarray.c int IARRAY
	cd /Users/zhaot/Work/neutube/neurolabi/c && chmod a-w /Users/zhaot/Work/neutube/neurolabi/c/tz_iarray.c

CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.o: 
CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.o: CMakeFiles/generate_tz_iarray.c
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/zhaot/Work/neutube/neurolabi/c/cmake_build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.o   -c /Users/zhaot/Work/neutube/neurolabi/c/cmake_build/CMakeFiles/generate_tz_iarray.c

CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /Users/zhaot/Work/neutube/neurolabi/c/cmake_build/CMakeFiles/generate_tz_iarray.c > CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.i

CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /Users/zhaot/Work/neutube/neurolabi/c/cmake_build/CMakeFiles/generate_tz_iarray.c -o CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.s

CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.o.requires:
.PHONY : CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.o.requires

CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.o.provides: CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.o.requires
	$(MAKE) -f CMakeFiles/generate_tz_iarray.c.dir/build.make CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.o.provides.build
.PHONY : CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.o.provides

CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.o.provides.build: CMakeFiles/generate_tz_iarray.c.dir/CMakeFiles/generate_tz_iarray.c.o

generate_tz_iarray.c: CMakeFiles/generate_tz_iarray.c
generate_tz_iarray.c: ../tz_iarray.c
generate_tz_iarray.c: CMakeFiles/generate_tz_iarray.c.dir/build.make
.PHONY : generate_tz_iarray.c

# Rule to build all files generated by this target.
CMakeFiles/generate_tz_iarray.c.dir/build: generate_tz_iarray.c
.PHONY : CMakeFiles/generate_tz_iarray.c.dir/build

CMakeFiles/generate_tz_iarray.c.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/generate_tz_iarray.c.dir/cmake_clean.cmake
.PHONY : CMakeFiles/generate_tz_iarray.c.dir/clean

CMakeFiles/generate_tz_iarray.c.dir/depend:
	cd /Users/zhaot/Work/neutube/neurolabi/c/cmake_build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/zhaot/Work/neutube/neurolabi/c /Users/zhaot/Work/neutube/neurolabi/c /Users/zhaot/Work/neutube/neurolabi/c/cmake_build /Users/zhaot/Work/neutube/neurolabi/c/cmake_build /Users/zhaot/Work/neutube/neurolabi/c/cmake_build/CMakeFiles/generate_tz_iarray.c.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/generate_tz_iarray.c.dir/depend

