# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_SOURCE_DIR = /Users/lijie/CLionProjects/program2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/lijie/CLionProjects/program2/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/program2.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/program2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/program2.dir/flags.make

CMakeFiles/program2.dir/driver.cpp.o: CMakeFiles/program2.dir/flags.make
CMakeFiles/program2.dir/driver.cpp.o: ../driver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/lijie/CLionProjects/program2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/program2.dir/driver.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/program2.dir/driver.cpp.o -c /Users/lijie/CLionProjects/program2/driver.cpp

CMakeFiles/program2.dir/driver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/program2.dir/driver.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/lijie/CLionProjects/program2/driver.cpp > CMakeFiles/program2.dir/driver.cpp.i

CMakeFiles/program2.dir/driver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/program2.dir/driver.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/lijie/CLionProjects/program2/driver.cpp -o CMakeFiles/program2.dir/driver.cpp.s

CMakeFiles/program2.dir/test_queue.cpp.o: CMakeFiles/program2.dir/flags.make
CMakeFiles/program2.dir/test_queue.cpp.o: ../test_queue.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/lijie/CLionProjects/program2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/program2.dir/test_queue.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/program2.dir/test_queue.cpp.o -c /Users/lijie/CLionProjects/program2/test_queue.cpp

CMakeFiles/program2.dir/test_queue.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/program2.dir/test_queue.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/lijie/CLionProjects/program2/test_queue.cpp > CMakeFiles/program2.dir/test_queue.cpp.i

CMakeFiles/program2.dir/test_queue.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/program2.dir/test_queue.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/lijie/CLionProjects/program2/test_queue.cpp -o CMakeFiles/program2.dir/test_queue.cpp.s

CMakeFiles/program2.dir/test_priority_queue.cpp.o: CMakeFiles/program2.dir/flags.make
CMakeFiles/program2.dir/test_priority_queue.cpp.o: ../test_priority_queue.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/lijie/CLionProjects/program2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/program2.dir/test_priority_queue.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/program2.dir/test_priority_queue.cpp.o -c /Users/lijie/CLionProjects/program2/test_priority_queue.cpp

CMakeFiles/program2.dir/test_priority_queue.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/program2.dir/test_priority_queue.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/lijie/CLionProjects/program2/test_priority_queue.cpp > CMakeFiles/program2.dir/test_priority_queue.cpp.i

CMakeFiles/program2.dir/test_priority_queue.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/program2.dir/test_priority_queue.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/lijie/CLionProjects/program2/test_priority_queue.cpp -o CMakeFiles/program2.dir/test_priority_queue.cpp.s

CMakeFiles/program2.dir/test_set.cpp.o: CMakeFiles/program2.dir/flags.make
CMakeFiles/program2.dir/test_set.cpp.o: ../test_set.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/lijie/CLionProjects/program2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/program2.dir/test_set.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/program2.dir/test_set.cpp.o -c /Users/lijie/CLionProjects/program2/test_set.cpp

CMakeFiles/program2.dir/test_set.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/program2.dir/test_set.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/lijie/CLionProjects/program2/test_set.cpp > CMakeFiles/program2.dir/test_set.cpp.i

CMakeFiles/program2.dir/test_set.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/program2.dir/test_set.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/lijie/CLionProjects/program2/test_set.cpp -o CMakeFiles/program2.dir/test_set.cpp.s

# Object files for target program2
program2_OBJECTS = \
"CMakeFiles/program2.dir/driver.cpp.o" \
"CMakeFiles/program2.dir/test_queue.cpp.o" \
"CMakeFiles/program2.dir/test_priority_queue.cpp.o" \
"CMakeFiles/program2.dir/test_set.cpp.o"

# External object files for target program2
program2_EXTERNAL_OBJECTS =

program2: CMakeFiles/program2.dir/driver.cpp.o
program2: CMakeFiles/program2.dir/test_queue.cpp.o
program2: CMakeFiles/program2.dir/test_priority_queue.cpp.o
program2: CMakeFiles/program2.dir/test_set.cpp.o
program2: CMakeFiles/program2.dir/build.make
program2: CMakeFiles/program2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/lijie/CLionProjects/program2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable program2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/program2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/program2.dir/build: program2

.PHONY : CMakeFiles/program2.dir/build

CMakeFiles/program2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/program2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/program2.dir/clean

CMakeFiles/program2.dir/depend:
	cd /Users/lijie/CLionProjects/program2/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/lijie/CLionProjects/program2 /Users/lijie/CLionProjects/program2 /Users/lijie/CLionProjects/program2/cmake-build-debug /Users/lijie/CLionProjects/program2/cmake-build-debug /Users/lijie/CLionProjects/program2/cmake-build-debug/CMakeFiles/program2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/program2.dir/depend
