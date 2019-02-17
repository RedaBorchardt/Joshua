cmake_minimum_required(VERSION 3.0)
project(Joshua)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(Joshua/External/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(Joshua/External/assimp)

option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(Joshua/External/bullet)

add_subdirectory(Joshua/External/gl3w)

set(IRRKLANG_HOME "${CMAKE_CURRENT_SOURCE_DIR}/Joshua/External/irrklang")
set(IRRKLANG_INCLUDE_DIR "${IRRKLANG_HOME}/include")
set(IRRKLANG_LIBRARIES "${IRRKLANG_HOME}/bin/libIrrKlang.so")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(Joshua/Headers/
                    Joshua/External/assimp/include/
                    Joshua/External/bullet/src/
                    Joshua/External/glfw/include/
                    Joshua/External/glm/
                    Joshua/External/stb/
				    Joshua/External/gl3w/include/
					Joshua/External/imgui/
					Joshua/External/irrklang/include/
		    )

file(GLOB EXTERNAL_SOURCES Joshua/External/gl3w/src/gl3w.c
						   Joshua/External/imgui/examples/imgui_impl_opengl3.cpp
						   Joshua/External/imgui/examples/imgui_impl_glfw.cpp
						   Joshua/External/imgui/imgui.cpp
						   Joshua/External/imgui/imgui_draw.cpp
						   Joshua/External/imgui/imgui_widgets.cpp
						   Joshua/External/imgui/imgui_demo.cpp 
		)
file(GLOB PROJECT_HEADERS Joshua/Headers/*.hpp
	)
file(GLOB PROJECT_SOURCES Joshua/Sources/*.cpp
	)
file(GLOB PROJECT_SHADERS Joshua/Resources/Shaders/*.comp
		Joshua/Resources/Shaders/*.frag
		Joshua/Resources/Shaders/*.geom
		Joshua/Resources/Shaders/*.vert)
file(GLOB PROJECT_TEXTURES Joshua/Resources/Textures/*.png
		Joshua/Resources/Textures/*.jpg)
file(GLOB PROJECT_SOUNDS Joshua/Resources/Sounds/*.mp3)
file(GLOB PROJECT_IRRKLANG Joshua/External/irrklang/bin/*.so)	   
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("External" FILES ${EXTERNAL_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
			       ${EXTERNAL_SOURCES} ${PROJECT_TEXTURES})
target_link_libraries(${PROJECT_NAME} assimp glfw
		${GLFW_LIBRARIES} ${GLAD_LIBRARIES} ${IRRKLANG_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

SET( CMAKE_EXPORT_COMPILE_COMMANDS ON )
file(COPY ${PROJECT_SHADERS} DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Resources/Shaders)
file(COPY ${PROJECT_TEXTURES} DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Resources/Textures)
file(COPY ${PROJECT_SOUNDS} DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Resources/Sounds)
file(COPY ${PROJECT_IRRKLANG} DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
