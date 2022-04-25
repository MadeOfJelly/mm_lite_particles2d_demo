cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(LiteParticles2DDemo CXX C)

add_executable(LiteParticles2DDemo
	./setup_services.hpp
	./setup_services.cpp

	./setup_scene.hpp
	./setup_scene.cpp

	./screens/start_screen.hpp
	./screens/start_screen.cpp

	./main.cpp
)

target_link_libraries(LiteParticles2DDemo PUBLIC
	imgui_service
	screen_director

	opengl_renderer_s
	clear_render_task
	lite_particles2d # not only rendertask
	bloom
	composition_render_task
	imgui_render_task

	filesystem_soloud_file

	imgui_tools
	imgui_opengl_renderer_tools

	organizer_scene

	random
)

if(EMSCRIPTEN)
	set_target_properties(LiteParticles2DDemo PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")

	set_target_properties(LiteParticles2DDemo PROPERTIES LINK_FLAGS "-flto -s USE_SDL=2 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s FULL_ES2=1 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1")
endif()

