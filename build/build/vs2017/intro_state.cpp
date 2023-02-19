#include "intro_state.h"

IntroState::IntroState(gef::Platform& platform) :
	platform_(platform)
{
}

void IntroState::Update(float frame_time, State** pCurrentState, State* theInstructionsState, State* theMenuState, State* theInGameState, State* theGameOverState, PrimitiveBuilder* primitive_builder_, gef::InputManager* input_manager_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_)
{
	const gef::Keyboard* keyboard = input_manager_->keyboard();

	if (graphics == cubes)
	{
		//move players into each other
		if (player_pos.x < -1)
		{
			player_pos.x += 0.45 * frame_time * 60;
			if (player_pos.y < 0)
			{
				player_pos.y += 0.1 * frame_time * 60;
			}
		}
		else
		{
			graphics = flash;
			audio_manager_->PlaySample(sfx_id_, false);
		}
		player_.mesh_Trans_.SetTranslation(gef::Vector4(player_pos.x, player_pos.y, 0, 0));
		player_.set_transform(player_.mesh_Trans_);
		player2_.mesh_Trans_.SetTranslation(gef::Vector4(-player_pos.x, player_pos.y, 0, 0));
		player2_.set_transform(player2_.mesh_Trans_);
	}
	else if (graphics == flash)
	{
		//increase size of white flash sprite
		if (flash_size < 1)
		{
			flash_size += 0.15 * frame_time * 60;
		}
		else
		{
			graphics = text;
		}
		flash_sprite.set_height(platform_.height() * flash_size);
		flash_sprite.set_width(platform_.width() * flash_size);
	}
	else if (graphics == text)
	{
		//title text scrolls down
		if (textpos_ < 0.50)
		{
			textpos_ += 0.01 * frame_time * 60;
			intro.set_position(gef::Vector4(platform_.width() * 0.5, platform_.height() * textpos_, 2));
		}
		else
		{
			audio_manager_->PlayMusic();
			graphics = done;
			start_visible = true;
		}
	}
	else if (graphics == done)
	{
		//start text appears
		text_timer += frame_time;
		if (text_timer > 0.75)
		{
			start_visible = !start_visible;
			text_timer = 0;
		}

		//allow user to continue to instructions state
		if ((keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_SPACE)))
		{
			*pCurrentState = theInstructionsState;
			onExit(audio_manager_);
			theInstructionsState->onEnter(primitive_builder_, audio_manager_, player_, player2_);
		}
	}
}

void IntroState::Draw(gef::SpriteRenderer* sprite_renderer_, gef::Font* font_, gef::Renderer3D* renderer_3d_, PrimitiveBuilder* primitive_builder_, Player& player_, Player& player2_)
{
	if (graphics == cubes)
	{
		// draw players
		renderer_3d_->set_override_material(&primitive_builder_->red_material());
		renderer_3d_->DrawMesh(player_);
		renderer_3d_->set_override_material(&primitive_builder_->blue_material());
		renderer_3d_->DrawMesh(player2_);
	}
	else if (graphics == flash)
	{
		//draw flash
		sprite_renderer_->DrawSprite(flash_sprite);
	}
	else if (graphics == text || graphics == done)
	{
		//draw text
		sprite_renderer_->DrawSprite(intro);
		if (start_visible)
		{
			sprite_renderer_->DrawSprite(start);
		}
	}
}

void IntroState::onEnter(PrimitiveBuilder* primitive_builder_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_)
{
	//load music
	audio_manager_->LoadMusic("Menu.wav", platform_);
	//load samples
	sfx_id_ = audio_manager_->LoadSample("bang.wav", platform_);

	//set player meshes
	player_mesh_ = primitive_builder_->CreateBoxMesh(gef::Vector4(0.5, 0.5, 0.5, 0.5), gef::Vector4(0, 0, 0, 0), NULL);
	player_.set_mesh(player_mesh_);
	player2_.set_mesh(player_mesh_);

	//position/scale players
	player_.mesh_Trans_.Scale(gef::Vector4(2, 2, 2, 2));
	player_.mesh_Trans_.SetTranslation(gef::Vector4(-15, -3, 0, 0));
	player2_.mesh_Trans_.Scale(gef::Vector4(2, 2, 2, 2));
	player2_.mesh_Trans_.SetTranslation(gef::Vector4(15, -3, 0, 1));
	player_.set_transform(player_.mesh_Trans_);
	player2_.set_transform(player2_.mesh_Trans_);
	player_pos.x = -15;
	player_pos.y = -3;

	//init text
	intro_text_ = CreateTextureFromPNG("SUPER-SMASH.png", platform_);
	intro = SetupSprite(intro_text_, 0, 0, platform_.width() * 0.42, platform_.height() * 0.26);

	//init white flash
	flash_tex = CreateTextureFromPNG("white.png", platform_);
	flash_sprite = SetupSprite(flash_tex, platform_.width() * 0.5, platform_.height() * 0.5, 1, 1);

	//init start text
	start_text_ = CreateTextureFromPNG("PRESS-SPACEBAR-TO-START.png", platform_);
	start = SetupSprite(start_text_, platform_.width() * 0.5f, platform_.height() * 0.9f, platform_.width() * 0.5, platform_.height() * 0.09);
}

void IntroState::onExit(gef::AudioManager* audio_manager_)
{
	//clean up data
	audio_manager_->UnloadAllSamples();
	delete player_mesh_;

	delete intro_text_;
	delete flash_tex;
	delete start_text_;
}

gef::Texture* IntroState::CreateTextureFromPNG(const char* png_filename, gef::Platform& platform)
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

gef::Sprite IntroState::SetupSprite(gef::Texture* texture, float xpos, float ypos, float width, float height)
{
	gef::Sprite sprite;
	sprite.set_texture(texture);
	sprite.set_position(gef::Vector4(xpos, ypos, 2));
	sprite.set_height(height);
	sprite.set_width(width);
	return sprite;
}