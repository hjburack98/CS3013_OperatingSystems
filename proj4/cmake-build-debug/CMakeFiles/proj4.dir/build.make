# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/proj4.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/proj4.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/proj4.dir/flags.make

CMakeFiles/proj4.dir/proj4.c.o: CMakeFiles/proj4.dir/flags.make
CMakeFiles/proj4.dir/proj4.c.o: ../proj4.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/proj4.dir/proj4.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/proj4.dir/proj4.c.o   -c /Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4/proj4.c

CMakeFiles/proj4.dir/proj4.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/proj4.dir/proj4.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4/proj4.c > CMakeFiles/proj4.dir/proj4.c.i

CMakeFiles/proj4.dir/proj4.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/proj4.dir/proj4.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4/proj4.c -o CMakeFiles/proj4.dir/proj4.c.s

# Object files for target proj4
proj4_OBJECTS = \
"CMakeFiles/proj4.dir/proj4.c.o"

# External object files for target proj4
proj4_EXTERNAL_OBJECTS =

proj4: CMakeFiles/proj4.dir/proj4.c.o
proj4: CMakeFiles/proj4.dir/build.make
proj4: CMakeFiles/proj4.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable proj4"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/proj4.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/proj4.dir/build: proj4

.PHONY : CMakeFiles/proj4.dir/build

CMakeFiles/proj4.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/proj4.dir/cmake_clean.cmake
.PHONY : CMakeFiles/proj4.dir/clean

CMakeFiles/proj4.dir/depend:
	cd /Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4 /Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4 /Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4/cmake-build-debug /Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4/cmake-build-debug /Users/harrisonburack/Documents/@WPI/2019-2020/A-Term/CS3013_OperatingSystems/proj4/cmake-build-debug/CMakeFiles/proj4.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/proj4.dir/depend

