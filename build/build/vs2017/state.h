#pragma once
#include <input/touch_input_manager.h>	
#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <input/input_manager.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include <audio/audio_manager.h>
#include "graphics/scene.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <input/keyboard.h>
#include <graphics/sprite.h>
#include <audio/audio_manager.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/texture.h>
#include <cstdlib>
#include "player.h"

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

class State
{
public:
	virtual void Update(float frame_time, State** pCurrentState, State* theInstructionsState, State* theMenuState, State* theInGameState, State* theGameOverState, PrimitiveBuilder* primitive_builder_, gef::InputManager* input_manager_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_);
	virtual void Draw(gef::SpriteRenderer* sprite_renderer_, gef::Font* font_, gef::Renderer3D* renderer_3d_, PrimitiveBuilder* primitive_builder_, Player& player_, Player& player2_);
	virtual void onEnter(PrimitiveBuilder* primitive_builder_, gef::AudioManager* audio_manager_, Player &player_, Player &player2_);
	virtual void onExit();
};