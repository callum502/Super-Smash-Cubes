#pragma once
#include "State.h"

class InstructionsState : public State
{
public:
	InstructionsState(gef::Platform& platform);
	void Update(float frame_time, State** pCurrentState, State* theInstructionsState, State* theMenuState, State* theInGameState, State* theGameOverState, PrimitiveBuilder* primitive_builder_, gef::InputManager* input_manager_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_);
	void Draw(gef::SpriteRenderer* sprite_renderer_, gef::Font* font_, gef::Renderer3D* renderer_3d_, PrimitiveBuilder* primitive_builder_, Player& player_, Player& player2_);
	void onEnter(PrimitiveBuilder* primitive_builder_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_);
	void onExit(gef::AudioManager* audio_manager_);
private:
	gef::Texture* CreateTextureFromPNG(const char* png_filename, gef::Platform& platform);
	gef::Sprite SetupSprite(gef::Texture* texture, float xpos, float ypos, float width, float height);
	gef::Platform& platform_;

	//textures and sprites
	//player headers
	gef::Sprite Player1_sprite_;
	gef::Texture* Player1_texture_;

	gef::Sprite Player2_sprite_;
	gef::Texture* Player2_texture_;

	//player 1 controls
	gef::Sprite Player1_controls1_sprite_;
	gef::Texture* Player1_controls1_texture_;

	gef::Sprite Player1_controls2_sprite_;
	gef::Texture* Player1_controls2_texture_;

	gef::Sprite Player1_controls3_sprite_;
	gef::Texture* Player1_controls3_texture_;

	//player 2 controls
	gef::Sprite Player2_controls1_sprite_;
	gef::Texture* Player2_controls1_texture_;

	gef::Sprite Player2_controls2_sprite_;
	gef::Texture* Player2_controls2_texture_;

	gef::Sprite Player2_controls3_sprite_;
	gef::Texture* Player2_controls3_texture_;

	//spacebar
	gef::Sprite spacebar_sprite_;
	gef::Texture* spacebar_texture_;

	//Instructions
	gef::Sprite instructions_sprite_;
	gef::Texture* instructions_texture_;

	//volume
	gef::Sprite volume_sprite_;
	gef::Texture* volume_texture_;

	//background
	gef::Sprite bg_sprite_;
	gef::Texture* bg_texture_;
};

