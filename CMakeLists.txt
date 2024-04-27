cmake_minimum_required(VERSION 3.27)

project(gles2_minesweeper)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer)
find_package(glm REQUIRED)
include(FindOpenGL)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

file(COPY res/ DESTINATION res/)

if(WIN32)
	add_link_options("-mwindows")
	add_executable(${PROJECT_NAME} ${SRC_FILES} "res/icon/icon.rc")
	target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++ ${SDL2_STATIC_LIBRARIES} SDL2_mixer glm::glm EGL GLESv2)
else()
	add_executable(${PROJECT_NAME} ${SRC_FILES})
	target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDL2_mixer ${OPENGL_gles2_LIBRARY} glm::glm)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS} ${SDL2_mixer_INCLUDE_DIRS})