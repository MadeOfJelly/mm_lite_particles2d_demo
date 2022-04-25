cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(my_project CXX C)

add_executable(my_project
	./setup_services.hpp
	./setup_services.cpp

	./screens/test_screen.hpp
	./screens/test_screen.cpp

	./main.cpp
)

target_link_libraries(my_project PUBLIC
	imgui_service
	screen_director

	opengl_renderer_s
	imgui_render_task

	#sound_service
	filesystem_soloud_file

	imgui_tools
	imgui_sound
	imgui_opengl_renderer_tools

	organizer_scene

	#random

	transform_system
)

if(EMSCRIPTEN)
	set_target_properties(my_project PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")

	set_target_properties(my_project PROPERTIES LINK_FLAGS "-flto -s USE_SDL=2 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s FULL_ES2=1 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1")
endif()

