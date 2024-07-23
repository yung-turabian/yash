cmake_minimum_required(VERSION 3.15)
project(hrry 
		VERISON 1.0 
		DESCRIPTION The Emoji shell
		HOMEPAGE_URL https://github.com/yung-turabian/hrry-shell
		LANGUAGES CXX)


# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} src/hrry.cpp)
#[===[


include(cmake/StandardSettings.cmake)

include(cmake/Utils.cmake)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()


# Function to prepend the subdirectory to source files in subdirectories
function(PREPEND var )
   set(listVar "")
   foreach(f ${${var}})
       list(APPEND listVar "${CMAKE_CURRENT_SOURCE_DIR}/${f}")
   endforeach(f)
   set(${var} "${listVar}" PARENT_SCOPE)
endfunction(PREPEND)

# After a normal build, we can specify the location of various outputs of the
# build. We put executables and static libraries outside the build directory in
# bin/ and lib/, respectively.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# Include source code and headers. This runs the CMakeLists.txt in each
# subdirectory. These can define their own libraries, executables, etc. as targets, 
# but here we define all exportable targets in the root CMakeLists.txt.
add_subdirectory(src)
add_subdirectory(include)

# Add the test directory. It is optional and can be disabled during with
#   cmake -DBUILD_TESTING=OFF ..
# To run unit tests produced here, we only need to run:
#   make test
# or
#   ctest 
# In case your tests are printing to console, you can view their output to
# stdout with:
#   ctest -V

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Add the library hrry as a target, with the contents of src/ and include/
# as dependencies.
add_library(hrry STATIC ${hrry_SRC} ${hrry_INCLUDE})


# These variables slightly modify the install location to allow for version
# specific installations.
set(hrry_INCLUDE_DEST "include/hrry-${hrry_VERSION}")
set(hrry_LIB_DEST "lib/hrry-${hrry_VERSION}")

target_include_directories(hrry PUBLIC
   # headers to include when building from source
   $<BUILD_INTERFACE:${hrry_SOURCE_DIR}/include> 
   $<BUILD_INTERFACE:${hrry_BINARY_DIR}/include> 

   # headers to include when installing  
   # (implicitly prefixes with ${CMAKE_INSTALL_PREFIX}).
   $<INSTALL_INTERFACE:include> 
   )

# Install hrry in CMAKE_INSTALL_PREFIX (defaults to /usr/local on linux). 
# To change the install location, run 
#   cmake -DCMAKE_INSTALL_PREFIX=<desired-install-path> ..

install(
    # targets to install
    TARGETS hrry
    # name of the CMake "export group" containing the targets we want to install
    EXPORT hrry_targets
    # Dynamic, static library and include destination locations after running
    # "make install"
    LIBRARY DESTINATION ${hrry_LIB_DEST}
    ARCHIVE DESTINATION ${hrry_LIB_DEST} 
    INCLUDES DESTINATION ${hrry_INCLUDE_DEST}
    )

install(
    # The export we want to save (matches name defined above containing the
    # install targets)
    EXPORT hrry_targets
    # CMake file in which to store the export's information
    FILE  hrry_targets.cmake
    # Namespace prepends all targets in the export (when we import later, we
    # will use hrry::hrry)
    NAMESPACE hrry::
    # where to place the resulting file (here, we're putting it with the library)
    DESTINATION ${hrry_LIB_DEST}
    )

# install(FILES ...) simply puts files in a certain place with certain
# properties. We're just copying include files to the desired include directory
# here.
install(FILES ${hrry_INCLUDE} DESTINATION ${hrry_INCLUDE_DEST})

# Write a "version file" in case someone wants to only load a particular version of
# hrry
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    hrryConfigVersion.cmake
    VERSION ${hrry_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

# Copies the resulting CMake config files to the installed library directory
install(
    FILES 
        "cmake/hrry-config.cmake"
				"${CMAKE_CURRENT_BINARY_DIR}/hrryConfigVersion.cmake"
    DESTINATION ${hrry_LIB_DEST}
    )

# Check for operating system
if(APPLE)
    message(STATUS "Configuring for macOS")

    include_directories(${PROJECT_SOURCE_DIR}/${INCLUDE_DIR}/darwn)

    target_compile_options(hrry PRIVATE "-D_MACOS")

    # You can add macOS specific libraries here
    # target_link_libraries(MyExecutable PRIVATE SomeMacOSLibrary)
    
elseif(UNIX AND NOT APPLE)
    message(STATUS "Configuring for Linux")

    include_directories(${PROJECT_SOURCE_DIR}/${INCLUDE_DIR}/linux)

    target_compile_options(hrry PRIVATE "-DLINUX")
		
		find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)
    set(LIBS ${X11_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

		# You can add Linux specific libraries here
    # target_link_libraries(MyExecutable PRIVATE SomeLinuxLibrary)
    
else()
    message(FATAL_ERROR "Unsupported operating system")
endif()

# Link libraries
target_link_libraries(hrry PRIVATE ${LIBS})

# Define include directories for custom targets
#target_include_directories(Scanner PRIVATE ${INCLUDE_DIR})
#target_include_directories(Tokenizer PRIVATE ${INCLUDE_DIR})
]===]
