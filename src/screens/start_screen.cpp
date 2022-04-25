#include "./start_screen.hpp"

#include <mm/services/opengl_renderer.hpp>
#include <mm/services/imgui_s.hpp>
#include <mm/services/organizer_scene.hpp>

// tools
#include <mm/services/imgui_menu_bar.hpp>
#include <mm/services/engine_tools.hpp>
#include <mm/services/opengl_renderer_tools.hpp>
#include <mm/services/screen_director_tools.hpp>
#include <mm/services/scene_tools.hpp>

#include <mm/resource_manager.hpp>
#include <mm/opengl/texture_loader.hpp>
#include <mm/opengl/lite_particles2d_type_loader.hpp>

#include <mm/opengl/fbo_builder.hpp>

#include <mm/opengl/render_tasks/clear.hpp>
#include <mm/opengl/render_tasks/lite_particles2d.hpp>
#include <mm/opengl/bloom.hpp>
#include <mm/opengl/render_tasks/composition.hpp>
#include <mm/opengl/render_tasks/imgui.hpp>

#include "../setup_scene.hpp"

#include <entt/entity/registry.hpp>

#include <mm/logger.hpp>

namespace LiteParticles2DDemo::Screens {

using namespace entt::literals;

static void setup_particles_types(MM::Engine&) {
	auto& lpt_rm = MM::ResourceManager<MM::OpenGL::LiteParticles2DType>::ref();

	lpt_rm.reload<MM::OpenGL::LiteParticles2DTypeLoaderJson>("MM::fade_demo",
R"({
  "compute": {
    "age_delta": 0.02,
    "force_vec": { "x": 0.0, "y": 0.9 },
    "turbulence": 4.0,
    "turbulence_individuality": 0.03,
    "turbulence_noise_scale": 0.05,
    "turbulence_time_scale": 0.6,
    "dampening": 1.0
  },
  "render": {
    "color_start": {
      "x": 1.8,
      "y": 3.0,
      "z": 7.0,
      "w": 1.0
    },
    "color_end": {
      "x": 6.3,
      "y": 0.4,
      "z": 3.5,
      "w": 1.0
    },
    "size_start": 0.1,
    "size_end": 0.02
  }
})"_json);

}

static void start_screen_start_fn(MM::Engine& engine) {
	SPDLOG_INFO("entering start_screen");

	setup_particles_types(engine);

	{ // rendering
		auto& rs = engine.getService<MM::Services::OpenGLRenderer>();
		rs.render_tasks.clear();

		auto& rm_t = MM::ResourceManager<MM::OpenGL::Texture>::ref();
		auto [w, h] = engine.getService<MM::Services::SDLService>().getWindowSize();

		// depth
#ifdef MM_OPENGL_3_GLES
		rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
			"depth",
			GL_DEPTH_COMPONENT24, // d16 is the onlyone for gles 2 (TODO: test for 3)
			w, h,
			GL_DEPTH_COMPONENT, GL_UNSIGNED_INT
		);
#else
		rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
			"depth",
			GL_DEPTH_COMPONENT32F, // TODO: stencil?
			w, h,
			GL_DEPTH_COMPONENT, GL_FLOAT
		);
#endif

		const float render_scale = 1.f;

		// hdr color gles3 / webgl2
		rm_t.reload<MM::OpenGL::TextureLoaderEmpty>(
			"hdr_color",
			GL_RGBA16F,
			w * render_scale, h * render_scale,
			GL_RGBA,
			GL_HALF_FLOAT
		);
		{ // filter
			rm_t.get("hdr_color"_hs)->bind(0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		rs.targets["game_view"] = MM::OpenGL::FBOBuilder::start()
			.attachTexture(rm_t.get("hdr_color"_hs), GL_COLOR_ATTACHMENT0)
			.attachTexture(rm_t.get("depth"_hs), GL_DEPTH_ATTACHMENT)
			.setResizeFactors(render_scale, render_scale)
			.setResize(true)
			.finish();
		assert(rs.targets["game_view"]);

		// clear
		auto& clear_opaque = rs.addRenderTask<MM::OpenGL::RenderTasks::Clear>(engine);
		clear_opaque.target_fbo = "game_view";
		// clears all color attachments
		clear_opaque.r = 0.f;
		clear_opaque.g = 0.f;
		clear_opaque.b = 0.f;
		clear_opaque.a = 1.f;
		clear_opaque.mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

		{ // render to hdr
			auto& lprt = rs.addRenderTask<MM::OpenGL::RenderTasks::LiteParticles2D>(engine);
			lprt.target_fbo = "game_view";
		}

		// rn does rt too
		MM::OpenGL::setup_bloom(engine, "hdr_color", 9, 1.f);

		// not part of setup_bloom
		auto& comp = rs.addRenderTask<MM::OpenGL::RenderTasks::Composition>(engine);
		comp.color_tex = "hdr_color";
		comp.bloom_tex = "blur_tmp1";
		comp.target_fbo = "display";

		rs.addRenderTask<MM::OpenGL::RenderTasks::ImGuiRT>(engine);
	}

	engine.getService<MM::Services::SceneServiceInterface>().changeScene(create_scene(engine));
}

void create_start_screen(MM::Engine& engine, MM::Services::ScreenDirector::Screen& screen) {
	// start enable
	screen.start_enable.push_back(engine.type<MM::Services::OpenGLRenderer>());
	screen.start_enable.push_back(engine.type<MM::Services::ImGuiService>());
	screen.start_enable.push_back(engine.type<MM::Services::OrganizerSceneService>());

	// tools
	screen.start_enable.push_back(engine.type<MM::Services::ImGuiMenuBar>());
	screen.start_enable.push_back(engine.type<MM::Services::ImGuiEngineTools>());
	screen.start_enable.push_back(engine.type<MM::Services::ImGuiOpenGLRendererTools>());
	screen.start_enable.push_back(engine.type<MM::Services::ImGuiScreenDirectorTools>());
	screen.start_enable.push_back(engine.type<MM::Services::ImGuiSceneToolsService>());

	screen.start_provide.push_back({
		engine.type<MM::Services::SceneServiceInterface>(),
		engine.type<MM::Services::OrganizerSceneService>()
	});

	// start disable
	//screen.start_disable.push_back();

	// ####################
	// end disable
	screen.end_disable.push_back(engine.type<MM::Services::OrganizerSceneService>());

	screen.start_fn = start_screen_start_fn;
}

} // LiteParticles2DDemo::Screens

