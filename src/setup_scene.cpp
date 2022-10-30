#include "./setup_scene.hpp"

#include <mm/engine.hpp>

#include <entt/entity/registry.hpp>
#include <entt/entity/organizer.hpp>

// ctx
#include <mm/opengl/components/lite_particles2d.hpp>
#include <mm/components/time_delta.hpp>
#include <mm/opengl/camera_3d.hpp>

// components
#include <mm/components/position2d.hpp>

#include <mm/random/srng.hpp>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

#include <mm/scalar_range2.hpp>

#include <memory>

namespace LiteParticles2DDemo {

using namespace entt::literals;

namespace Components {

// or what ever
struct LiteParticles2DEmitter {
	float age {0.f}; // this changes each tick, if >=1, spawn and reset to 0
	float age_delta {1.f}; // times per sec

	MM::ScalarRange2<uint16_t> particle_count {1u, 1u};

	// particles data

	std::string p_type {};

	MM::ScalarRange2<float> p_pos_x {0.f, 0.f};
	MM::ScalarRange2<float> p_pos_y {0.f, 0.f};

	MM::ScalarRange2<float> p_dir {0.f, 0.f}; // 0-1, not rad
	MM::ScalarRange2<float> p_dir_force {0.f, 0.f};

	MM::ScalarRange2<float> p_age {0.f, 0.f};
};

} // Components

namespace Systems {

void lite_particles2d_emit(
	entt::view<entt::get_t<
		const MM::Components::Position2D,
		Components::LiteParticles2DEmitter
	>> view,
	const MM::Components::TimeDelta& td,
	MM::Components::LiteParticles2DUploadQueue& lp_uq,
	MM::Random::SRNG& rng
) {
	view.each([&lp_uq, &td, &rng](const auto& pos, Components::LiteParticles2DEmitter& lpe) {
		lpe.age += lpe.age_delta * td.tickDelta;

		for (; lpe.age >= 1.f; lpe.age -= 1.f) {
			const auto type = entt::hashed_string::value(lpe.p_type.c_str());

			const size_t count = rng.range(lpe.particle_count);
			for (size_t i = 0; i < count; i++) {
				float dir = rng.range(lpe.p_dir) * glm::two_pi<float>();
				lp_uq.queue.push(MM::Components::LiteParticle2D{
					type, // type

					//{rng.negOneToOne() * 30.f, rng.negOneToOne() * 30.f}, // pos
					pos.pos + /*lpe.pos_offset +*/ glm::vec2{rng.range(lpe.p_pos_x), rng.range(lpe.p_pos_y)}, // pos
					glm::vec2{glm::cos(dir), glm::sin(dir)} * rng.range(lpe.p_dir_force), // vel

					rng.range(lpe.p_age) // age
				});
			}
		}
	});
}

} // Systems

std::unique_ptr<MM::Scene> create_scene(MM::Engine&) {
	auto scene_mem = std::make_unique<MM::Scene>();
	MM::Scene& scene = *scene_mem;

	auto& cam = scene.ctx().emplace<MM::OpenGL::Camera3D>();
	cam.horizontalViewPortSize = 100.f;
	cam.setOrthographic();
	cam.updateView();

	scene.ctx().emplace<MM::Components::LiteParticles2DUploadQueue>();
	scene.ctx().emplace<MM::Random::SRNG>(42u);

	// setup v system
	auto& org = scene.ctx().emplace<entt::organizer>();
	org.emplace<Systems::lite_particles2d_emit>("lite_particles2d_emit");

	{ // default
		auto e = scene.create();
		auto& p = scene.emplace<MM::Components::Position2D>(e);
		p.pos.y = -20.f;

		auto& lpe = scene.emplace<Components::LiteParticles2DEmitter>(e);
		lpe.age_delta = 60.f;
		lpe.particle_count = {1, 2};
		lpe.p_type = "MM::fade_demo";
		lpe.p_dir = {0.f, 1.f};
		lpe.p_dir_force = {0.f, 2.f};
		lpe.p_age = {0.f, 0.2f};
	}

	return scene_mem;
}

} // LiteParticles2DDemo

