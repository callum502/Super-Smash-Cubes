#include "instructions_state.h"

InstructionsState::InstructionsState(gef::Platform& platform):
	platform_(platform)
{
}

void InstructionsState::Update(float frame_time, State** pCurrentState, State* theInstructionsState, State* theMenuState, State* theInGameState, State* theGameOverState, PrimitiveBuilder* primitive_builder_, gef::InputManager* input_manager_, gef::AudioManager* audio_manager_,  Player& player_, Player& player2_)
{
	const gef::Keyboard* keyboard = input_manager_->keyboard();

	//allow user to continue to menu state
	if ((keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_SPACE)))
	{
		*pCurrentState = theMenuState;
		onExit(audio_manager_);
		theMenuState->onEnter(primitive_builder_, audio_manager_, player_, player2_);
	}
}

void InstructionsState::Draw(gef::SpriteRenderer* sprite_renderer_, gef::Font* font_, gef::Renderer3D* renderer_3d_, PrimitiveBuilder* primitive_builder_, Player& player_, Player& player2_)
{
	//draw bg
	sprite_renderer_->DrawSprite(bg_sprite_);

	//draw player headings
	sprite_renderer_->DrawSprite(Player1_sprite_);
	sprite_renderer_->DrawSprite(Player2_sprite_);

	//draw player controls
	sprite_renderer_->DrawSprite(Player1_controls1_sprite_);
	sprite_renderer_->DrawSprite(Player1_controls2_sprite_);
	sprite_renderer_->DrawSprite(Player1_controls3_sprite_);
	sprite_renderer_->DrawSprite(Player2_controls1_sprite_);
	sprite_renderer_->DrawSprite(Player2_controls2_sprite_);
	sprite_renderer_->DrawSprite(Player2_controls3_sprite_);

	//draw text
	sprite_renderer_->DrawSprite(spacebar_sprite_);
	sprite_renderer_->DrawSprite(instructions_sprite_);

	//draw volum controls
	sprite_renderer_->DrawSprite(volume_sprite_);


	//draw objective text
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5, platform_.height() * 0.15, -0.99f),
		platform_.height() * 0.0015f,
		0xff000000,
		gef::TJ_CENTRE,
		"The aim of the game is to knock the other player out of the map by shooting or dashing into them"
	);

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5, platform_.height() * 0.19, -0.99f),
		platform_.height() * 0.0015f,
		0xff000000,
		gef::TJ_CENTRE,
		"you can block to reduce impact of these attacks and remember to look out for powerups"
	);

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_.width() * 0.5, platform_.height() * 0.23, -0.99f),
		platform_.height() * 0.0015f,
		0xff000000,
		gef::TJ_CENTRE,
		"the player who runs out of lives first loses!"
	);
}

void InstructionsState::onEnter(PrimitiveBuilder* primitive_builder_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_)
{
	//load player header textures
	Player1_texture_ = CreateTextureFromPNG("PLAYER-1 (1).png", platform_);
	Player2_texture_ = CreateTextureFromPNG("PLAYER-2 (2).png", platform_);

	//load player 1 controls textures
	Player1_controls1_texture_ = CreateTextureFromPNG("W--JUMP.png", platform_);
	Player1_controls2_texture_ = CreateTextureFromPNG("S--BLOCK.png", platform_);
	Player1_controls3_texture_ = CreateTextureFromPNG("SPACEBAR--DASH-FORWARD.png", platform_);

	//load player 2 controls textures
	Player2_controls1_texture_ = CreateTextureFromPNG("NUMPAD-8--JUMP.png", platform_);
	Player2_controls2_texture_ = CreateTextureFromPNG("NUMPAD-5--BLOCK.png", platform_);
	Player2_controls3_texture_ = CreateTextureFromPNG("ENTER--DASH-FORWARD.png", platform_);

	//load spacebar texture
	spacebar_texture_ = CreateTextureFromPNG("PRESS-SPACEBAR-TO-CONTIN.png", platform_);

	//load instructions texture
	instructions_texture_ = CreateTextureFromPNG("INSTRUCTIONS.png", platform_);

	//load volume texture
	volume_texture_ = CreateTextureFromPNG("--INCREASE-VOLUME (1).png", platform_);

	//load background
	bg_texture_ = CreateTextureFromPNG("menubg.png", platform_);


	//setup sprites
	Player1_sprite_ = SetupSprite(Player1_texture_, platform_.width() * 0.25f, platform_.height() * 0.35f, platform_.width() * 0.26, platform_.height() * 0.08);
	Player2_sprite_ = SetupSprite(Player2_texture_, platform_.width() * 0.75f, platform_.height() * 0.35f, platform_.width() * 0.26, platform_.height() * 0.08);

	Player1_controls1_sprite_ = SetupSprite(Player1_controls1_texture_, platform_.width() * 0.25f, platform_.height() * 0.46f, platform_.width() * 0.17, platform_.height() * 0.12);
	Player1_controls2_sprite_ = SetupSprite(Player1_controls2_texture_, platform_.width() * 0.25f, platform_.height() * 0.58f, platform_.width() * 0.17, platform_.height() * 0.12);
	Player1_controls3_sprite_ = SetupSprite(Player1_controls3_texture_, platform_.width() * 0.25f, platform_.height() * 0.665f, platform_.width() * 0.24, platform_.height() * 0.05);

	Player2_controls1_sprite_ = SetupSprite(Player2_controls1_texture_, platform_.width() * 0.75f, platform_.height() * 0.46f, platform_.width() * 0.2, platform_.height() * 0.12);
	Player2_controls2_sprite_ = SetupSprite(Player2_controls2_texture_, platform_.width() * 0.75f, platform_.height() * 0.58f, platform_.width() * 0.2, platform_.height() * 0.12);
	Player2_controls3_sprite_ = SetupSprite(Player2_controls3_texture_, platform_.width() * 0.75f, platform_.height() * 0.67f, platform_.width() * 0.2, platform_.height() * 0.05);

	spacebar_sprite_ = SetupSprite(spacebar_texture_, platform_.width() * 0.5f, platform_.height() * 0.9f, platform_.width() * 0.6, platform_.height() * 0.08);
	instructions_sprite_ = SetupSprite(instructions_texture_, platform_.width() * 0.5f, platform_.height() * 0.1f, platform_.width() * 0.3, platform_.height() * 0.08);
	volume_sprite_ = SetupSprite(volume_texture_, platform_.width() * 0.5f, platform_.height() * 0.8f, platform_.width() * 0.2, platform_.height() * 0.11);

	bg_sprite_ = SetupSprite(bg_texture_, platform_.width() * 0.5f, platform_.height() * 0.5f, platform_.width(), platform_.height());
}

void InstructionsState::onExit(gef::AudioManager* audio_manager_)
{
	//clean up data
	audio_manager_->UnloadMusic();
	delete Player1_texture_;
	delete Player2_texture_;
	delete Player1_controls1_texture_;
	delete Player1_controls2_texture_;
	delete Player1_controls3_texture_;
	delete Player2_controls1_texture_;
	delete Player2_controls2_texture_;
	delete Player2_controls3_texture_;
	delete spacebar_texture_;
	delete instructions_texture_;
	delete volume_texture_;
	delete bg_texture_;
}

gef::Texture* InstructionsState::CreateTextureFromPNG(const char* png_filename, gef::Platform& platform)
{
	gef::PNGLoader png_loader;
	gef::ImageData image_data;
	gef::Texture* texture = NULL;

	// load image data from PNG file 
	png_loader.Load(png_filename, platform, image_data);

	// if the image data is valid, then create a texture from it
	if (image_data.image() != NULL)
		texture = gef::Texture::Create(platform, image_data);

	return texture;
}

gef::Sprite InstructionsState::SetupSprite(gef::Texture* texture, float xpos, float ypos, float width, float height)
{
	gef::Sprite sprite;

	sprite.set_texture(texture);
	sprite.set_position(gef::Vector4(xpos, ypos, 2));
	sprite.set_height(height);
	sprite.set_width(width);
	return sprite;
}
