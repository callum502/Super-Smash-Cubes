#pragma once
#include "State.h"

class IntroState : public State
{
public:
	IntroState(gef::Platform& platform);
	void Update(float frame_time, State** pCurrentState, State* theInstructionsState, State* theMenuState, State* theInGameState, State* theGameOverState, PrimitiveBuilder* primitive_builder_, gef::InputManager* input_manager_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_);
	void Draw(gef::SpriteRenderer* sprite_renderer_, gef::Font* font_, gef::Renderer3D* renderer_3d_, PrimitiveBuilder* primitive_builder_, Player& player_, Player& player2_);
	void onEnter(PrimitiveBuilder* primitive_builder_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_);
	void onExit(gef::AudioManager* audio_manager_);
private:
	gef::Texture* CreateTextureFromPNG(const char* png_filename, gef::Platform& platform);
	gef::Sprite IntroState::SetupSprite(gef::Texture* texture, float xpos, float ypos, float width, float height);

	//player mesh
	gef::Mesh* player_mesh_;

	//platform
	gef::Platform& platform_;

	//textures/sprites
	gef::Texture* intro_text_;
	gef::Sprite intro;
	gef::Texture* flash_tex;
	gef::Sprite flash_sprite;
	gef::Texture* start_text_;
	gef::Sprite start;

	//position/size variables
	float flash_size=0;
	float textpos_ =0;
	b2Vec2 player_pos;

	//start text bool/timer
	float text_timer = 0;
	bool start_visible = false;

	//state of intro
	enum graphics
	{
		cubes,
		flash,
		text,
		done
	};
	graphics graphics = cubes;

	//audio 
	int sfx_id_;

};

