cmake_minimum_required(VERSION 3.10)
project(OpenGL)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
set(OpenGL_GL_PREFERENCE GLVND)

find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS})

file(GLOB_RECURSE SRC_ALL
	 LIST_DIRECTORIES true
	 "${CMAKE_SOURCE_DIR}/res/HEADS/*"
	 "${CMAKE_SOURCE_DIR}/main/*"
	 "${CMAKE_SOURCE_DIR}/dep/*"
)
list(APPEND SRC_INCLUDE ${CMAKE_SOURCE_DIR}/main)
list(APPEND SRC_INCLUDE ${CMAKE_SOURCE_DIR}/dep)
list(APPEND SRC_INCLUDE ${CMAKE_SOURCE_DIR}/res/HEADS)

foreach(SRC IN LISTS SRC_ALL)
	if(IS_DIRECTORY ${SRC})
		list(APPEND SRC_INCLUDE ${SRC})
	else()
		list(APPEND SRC_FILES ${SRC})
	endif()
endforeach()

# message("SRC_INCLUDE=${SRC_INCLUDE}")
# message("SRC_FILES=${SRC_FILES}")
# message("GLEW_INCLUDE_DIRS=${GLEW_INCLUDE_DIRS}")

include_directories(${SRC_INCLUDE})
add_executable(OpenGL ${SRC_FILES})
target_link_libraries(OpenGL ${GLEW_LIBRARIES} glfw OpenGL::GL glm::glm)

if(MSVC)
    target_compile_options(OpenGL PRIVATE /std:c++20 /W4)
else()
    target_compile_options(OpenGL PRIVATE -std=c++20 -Wall -Wextra -pedantic)
    target_compile_options(OpenGL PRIVATE -nostartfiles)
endif()
