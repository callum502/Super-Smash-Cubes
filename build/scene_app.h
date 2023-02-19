#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <audio/audio_manager.h>
#include <input/input_manager.h>
#include <box2d/Box2D.h>
#include "audio_3d.h"
#include "game_object.h"
#include "state.h"
#include "menu_state.h"
#include "in_game_state.h"
#include "intro_state.h"
#include "instructions_state.h"
#include "game_over_state.h"

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();
    
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;
	gef::InputManager* input_manager_;
	gef::AudioManager* audio_manager_;

	//audio variables
	int sfx_id_;
	float volume=50;
	Audio3D* audio_3d_;

	//fps
	float fps_;

	//states
	IntroState theIntroState;
	InstructionsState theInstructionsState;
	MenuState theMenuState;
	InGameState theInGameState;
	GameOverState theGameOverState;
	State* pCurrentState = &theIntroState;

	//players 
	Player player_;
	Player player2_;
};

#endif // _SCENE_APP_H
