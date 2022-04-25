#include "./test_screen.hpp"

#include <mm/services/opengl_renderer.hpp>
#include <mm/services/imgui_s.hpp>
#include <mm/services/organizer_scene.hpp>

// tools
#include <mm/services/imgui_menu_bar.hpp>
#include <mm/services/engine_tools.hpp>
#include <mm/services/opengl_renderer_tools.hpp>
#include <mm/services/screen_director_tools.hpp>
#include <mm/services/scene_tools.hpp>
//#include <mm/services/sound_tools.hpp>

#include <mm/opengl/render_tasks/imgui.hpp>

#include <entt/entity/registry.hpp>

#include <mm/logger.hpp>

namespace my_project::Screens {

static void test_screen_start_fn(MM::Engine& engine) {
	SPDLOG_INFO("starting test_screen screen");

	{ // rendering
		auto& rs = engine.getService<MM::Services::OpenGLRenderer>();
		rs.render_tasks.clear();

		rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);
	}
}

void create_test_screen(MM::Engine& engine, MM::Services::ScreenDirector::Screen& screen) {
	// start enable
	//screen.start_enable.push_back(engine.type<MM::Services::SoundService>());
	screen.start_enable.push_back(engine.type<MM::Services::OpenGLRenderer>());
	screen.start_enable.push_back(engine.type<MM::Services::ImGuiService>());
	screen.start_enable.push_back(engine.type<MM::Services::OrganizerSceneService>());

	// tools
	screen.start_enable.push_back(engine.type<MM::Services::ImGuiMenuBar>());
	screen.start_enable.push_back(engine.type<MM::Services::ImGuiEngineTools>());
	screen.start_enable.push_back(engine.type<MM::Services::ImGuiOpenGLRendererTools>());
	screen.start_enable.push_back(engine.type<MM::Services::ImGuiScreenDirectorTools>());
	screen.start_enable.push_back(engine.type<MM::Services::ImGuiSceneToolsService>());
	//screen.start_enable.push_back(engine.type<MM::Services::ImGuiSoundTools>());

	screen.start_provide.push_back({
		engine.type<MM::Services::SceneServiceInterface>(),
		engine.type<MM::Services::OrganizerSceneService>()
	});

	// start disable
	//screen.start_disable.push_back();

	// ####################
	// end disable
	screen.end_disable.push_back(engine.type<MM::Services::OrganizerSceneService>());

	screen.start_fn = test_screen_start_fn;
}

} // my_project::Screens

