cmake_minimum_required(VERSION 3.10)

# Project name and language
project(hrry VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define directories
set(SRC_DIR src)
set(INCLUDE_DIR include)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/${INCLUDE_DIR})

# Source files
file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/${SRC_DIR}/*.cpp)

# Executable
add_executable(hrry ${SRC_DIR}/hrry.cpp ${SRCS})

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

