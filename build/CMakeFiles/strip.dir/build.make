# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.21.3_1/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.21.3_1/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/ei3/Desktop/UQ/BcS/Y1S2/CSSE2010/Assessment/A2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/ei3/Desktop/UQ/BcS/Y1S2/CSSE2010/Assessment/A2/build

# Utility rule file for strip.

# Include any custom commands dependencies for this target.
include CMakeFiles/strip.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/strip.dir/progress.make

CMakeFiles/strip: assignment2.elf
	avr-strip assignment2.elf

strip: CMakeFiles/strip
strip: CMakeFiles/strip.dir/build.make
.PHONY : strip

# Rule to build all files generated by this target.
CMakeFiles/strip.dir/build: strip
.PHONY : CMakeFiles/strip.dir/build

CMakeFiles/strip.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/strip.dir/cmake_clean.cmake
.PHONY : CMakeFiles/strip.dir/clean

CMakeFiles/strip.dir/depend:
	cd /Users/ei3/Desktop/UQ/BcS/Y1S2/CSSE2010/Assessment/A2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ei3/Desktop/UQ/BcS/Y1S2/CSSE2010/Assessment/A2 /Users/ei3/Desktop/UQ/BcS/Y1S2/CSSE2010/Assessment/A2 /Users/ei3/Desktop/UQ/BcS/Y1S2/CSSE2010/Assessment/A2/build /Users/ei3/Desktop/UQ/BcS/Y1S2/CSSE2010/Assessment/A2/build /Users/ei3/Desktop/UQ/BcS/Y1S2/CSSE2010/Assessment/A2/build/CMakeFiles/strip.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/strip.dir/depend

