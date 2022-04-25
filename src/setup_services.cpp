#include "./setup_services.hpp"

//#include <mm/screens/mm_logo_screen.hpp>
#include "./screens/test_screen.hpp"

template<>
bool setup_service<MM::Services::FilesystemService>(MM::Engine& engine, [[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	engine.addService<MM::Services::FilesystemService>(argv[0], "mm_template", true, true
#ifndef NDEBUG
		// hack for local machine
		//,std::vector<std::tuple<std::string, std::string, bool>>{{"../res/", "/", true}}
#endif
	);

	// enable by default
	if (!engine.enableService<MM::Services::FilesystemService>()) {
		SPDLOG_ERROR("failed to enable service {}", engine.getService<MM::Services::FilesystemService>().name());
		return false;
	}

	return true;
}

template<>
bool setup_service<MM::Services::SDLService>(MM::Engine& engine, [[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	auto& sdl = engine.addService<MM::Services::SDLService>(
		SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER
	);

	// enable by default
	if (!engine.enableService<MM::Services::SDLService>()) {
		SPDLOG_ERROR("failed to enable service {}", sdl.name());
		return false;
	}

	sdl.createGLWindow(argv[0],
		1280, 720, // default res
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);

	// after window creation
	SDL_GL_SetSwapInterval(1); // 0 == vsync off

	return true;
}

template<>
bool setup_service<MM::Services::ScreenDirector>(MM::Engine& engine, [[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	auto& sd = engine.addService<MM::Services::ScreenDirector>();

	// add screens before enable
	//MM::Screens::create_mm_logo(
		//engine,
		//sd.screens["MM::Screens::mm_logo"],
		//"my_project::Screens::main_menu"
	//);
	my_project::Screens::create_test_screen(engine, sd.screens["my_project::Screens::test_screen"]);

	// queue the inital screen before enable
	sd.queueChangeScreenTo("my_project::Screens::test_screen");

	// enable by default, starts first screen
	if (!engine.enableService<MM::Services::ScreenDirector>()) {
		SPDLOG_ERROR("failed to enable service {}", sd.name());
		return false;
	}

	return true;
}


