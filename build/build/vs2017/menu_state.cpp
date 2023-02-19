#include "menu_state.h"

MenuState::MenuState(gef::Platform& platform) :
	platform_(platform)
{
}

void MenuState::Update(float frame_time, State** pCurrentState, State* theInstructionsState, State* theMenuState, State* theInGameState, State* theGameOverState, PrimitiveBuilder* primitive_builder_, gef::InputManager* input_manager_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_)
{
	const gef::Keyboard* keyboard = input_manager_->keyboard();
	//cycle hat player 1
	if ((keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_A)))
	{
		player_.hatSelect--;
		if (player_.hatSelect < 0)
		{
			player_.hatSelect = 11;
		}
	}
	if ((keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_D)))
	{
		player_.hatSelect++;
		if (player_.hatSelect > 11)
		{
			player_.hatSelect = 0;
		}
	}

	//cycle hat player 2
	if ((keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_NUMPAD4)))
	{
		player2_.hatSelect--;
		if (player2_.hatSelect < 0)
		{
			player2_.hatSelect = 11;
		}
	}
	if ((keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_NUMPAD6)))
	{
		player2_.hatSelect++;
		if (player2_.hatSelect > 11)
		{
			player2_.hatSelect = 0;
		}
	}

	//switch to in game state
	if ((keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_SPACE)))
	{
		*pCurrentState = theInGameState;
		onExit(audio_manager_);
		theInGameState->onEnter(primitive_builder_, audio_manager_, player_, player2_);
	}
	//switch to instructions state
	if ((keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_I)))
	{
		*pCurrentState = theInstructionsState;
		onExit(audio_manager_);
		theInstructionsState->onEnter(primitive_builder_, audio_manager_, player_, player2_);
		//play instructions music
		audio_manager_->LoadMusic("Menu.wav", platform_);
		audio_manager_->PlayMusic();
	}
}

void MenuState::Draw(gef::SpriteRenderer* sprite_renderer_, gef::Font* font_, gef::Renderer3D* renderer_3d_, PrimitiveBuilder* primitive_builder_, Player& player_, Player& player2_)
{
	//render textures
	sprite_renderer_->DrawSprite(background);
	sprite_renderer_->DrawSprite(title);
	sprite_renderer_->DrawSprite(start);
	sprite_renderer_->DrawSprite(instructions);
	sprite_renderer_->DrawSprite(player1_sprite_);
	sprite_renderer_->DrawSprite(player2_sprite_);

	// draw players
	renderer_3d_->set_override_material(&primitive_builder_->red_material());
	renderer_3d_->DrawMesh(player_);
	renderer_3d_->set_override_material(&primitive_builder_->blue_material());
	renderer_3d_->DrawMesh(player2_);

	//draw player hats
	renderer_3d_->set_override_material(NULL);
	renderer_3d_->DrawMesh(mesh_instance_[player_.hatSelect]);
	renderer_3d_->DrawMesh(mesh_instance_[player2_.hatSelect + 12]);
}

void MenuState::onEnter(PrimitiveBuilder* primitive_builder_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_)
{
	//play music
	audio_manager_->LoadMusic("sans.wav", platform_);
	audio_manager_->PlayMusic();

	//set player meshes
	mesh = primitive_builder_->CreateBoxMesh(gef::Vector4(0.5, 0.5, 0.5, 0.5), gef::Vector4(0, 0, 0, 0), NULL);
	player_.set_mesh(mesh);
	player2_.set_mesh(mesh);

	//position/scale players
	player_.mesh_Trans_.Scale(gef::Vector4(2, 2, 2, 2));
	player_.mesh_Trans_.SetTranslation(gef::Vector4(-5, 0, 0, 0));
	player2_.mesh_Trans_.Scale(gef::Vector4(2, 2, 2, 2));
	player2_.mesh_Trans_.SetTranslation(gef::Vector4(5, 0, 0, 1));
	player_.set_transform(player_.mesh_Trans_);
	player2_.set_transform(player2_.mesh_Trans_);

	//set model file names
	scene_asset_filename[0] = "pirate_hat.scn";
	scene_asset_filename[1] = "2011_hat.scn";
	scene_asset_filename[2] = "cowboy.scn";
	scene_asset_filename[3] = "party_hat.scn";
	scene_asset_filename[4] = "frog_hat.scn";
	scene_asset_filename[5] = "witch_hat.scn";
	scene_asset_filename[6] = "chef_hat.scn";
	scene_asset_filename[7] = "police_hat.scn";
	scene_asset_filename[8] = "wa_hat.scn";
	scene_asset_filename[9] = "toad_hat.scn";
	scene_asset_filename[10] = "duck_hat.scn";
	scene_asset_filename[11] = "penguin_hat.scn";

	//set loaded hat meshes
	for (int i = 0; i < 12; i++)
	{
		scene_assets_ = LoadSceneAssets(platform_, scene_asset_filename[i]);

		if (scene_assets_)
		{
			loaded_meshes_[i] = GetMeshFromSceneAssets(scene_assets_);
			mesh_instance_[i].set_mesh(loaded_meshes_[i]);
			mesh_instance_[i + 12].set_mesh(loaded_meshes_[i]);
		}
		else
		{
			gef::DebugOut("Scene file %s failed to load\n", scene_asset_filename);
		}
	}

	//repos and scale hats
	float hat_scale_[12];
	hat_scale_[0] = 1.2;
	hat_scale_[1] = 0.5;
	hat_scale_[2] = 1.2;
	hat_scale_[3] = 16.5;
	hat_scale_[4] = 0.35;
	hat_scale_[5] = 1;
	hat_scale_[6] = 0.2;
	hat_scale_[7] = 1.8;
	hat_scale_[8] = 0.035;
	hat_scale_[9] = 1.6;
	hat_scale_[10] = 0.35;
	hat_scale_[11] = 0.035;

	float hat_y_[12];
	hat_y_[0] = 1.5;
	hat_y_[1] = -5;
	hat_y_[2] = 1.44;
	hat_y_[3] = -1.9;
	hat_y_[4] = 0.7;
	hat_y_[5] = 2.2;
	hat_y_[6] = -2.4;
	hat_y_[7] = 1.455;
	hat_y_[8] = -0.31;
	hat_y_[9] = 1.65;
	hat_y_[10] = 0;
	hat_y_[11] = -.4;
	for (int i = 0; i < 12; i++)
	{
		bg_Trans_.Scale(gef::Vector4(hat_scale_[i], hat_scale_[i], hat_scale_[i], hat_scale_[i]));
		bg_Trans_.SetTranslation(gef::Vector4(-5, hat_y_[i], 0, 1));
		mesh_instance_[i].set_transform(bg_Trans_);
	}

	for (int i = 0; i < 12; i++)
	{
		bg_Trans_.Scale(gef::Vector4(hat_scale_[i], hat_scale_[i], hat_scale_[i], hat_scale_[i]));
		bg_Trans_.SetTranslation(gef::Vector4(5, hat_y_[i], 0, 1));
		mesh_instance_[i+12].set_transform(bg_Trans_);
	}

	//setup textures
	background_ = CreateTextureFromPNG("menubg.png", platform_);
	title_text_ = CreateTextureFromPNG("SUPER-SMASH-CUBES.png", platform_);
	start_text_ = CreateTextureFromPNG("PRESS-SPACEBAR-TO-START.png", platform_);
	instructions_text_ = CreateTextureFromPNG("press-i-for-instructions.png", platform_);
	player1_text_ = CreateTextureFromPNG("PLAYER-1.png", platform_);
	player2_text_ = CreateTextureFromPNG("PLAYER-2 (1).png", platform_);

	//setup sprites
	background = SetupSprite(background_, platform_.width() * 0.5f, platform_.height() * 0.5f, platform_.width(), platform_.height());
	title = SetupSprite(title_text_, platform_.width() * 0.5f, platform_.height() * 0.1f, platform_.width() * 0.5, platform_.height() * 0.08);
	start = SetupSprite(start_text_, platform_.width() * 0.5f, platform_.height() * 0.9f, platform_.width() * 0.5, platform_.height() * 0.08);
	instructions = SetupSprite(instructions_text_, platform_.width() * 0.5f, platform_.height() * 0.83f, platform_.width() * 0.38, platform_.height() * 0.07);
	player1_sprite_ = SetupSprite(player1_text_, platform_.width() * 0.26f, platform_.height() * 0.65f, platform_.width() * 0.2, platform_.height() * 0.1);
	player2_sprite_ = SetupSprite(player2_text_, platform_.width() * 0.74f, platform_.height() * 0.65f, platform_.width() * 0.26, platform_.height() * 0.1);

}

void MenuState::onExit(gef::AudioManager* audio_manager_)
{
	//clean up data
	audio_manager_->UnloadMusic();
	delete mesh;
	delete scene_assets_;
	delete background_;
	delete title_text_;
	delete start_text_;
	delete instructions_text_;
	delete player1_text_;
	delete player2_text_;
	for (int i = 0; i < 11; i++)
	{
		delete loaded_meshes_[i];
	}
}

gef::Scene* MenuState::LoadSceneAssets(gef::Platform& platform, const char* filename)
{
	gef::Scene* scene = new gef::Scene();

	if (scene->ReadSceneFromFile(platform, filename))
	{
		// if scene file loads successful
		// create material and mesh resources from the scene data
		scene->CreateMaterials(platform);
		scene->CreateMeshes(platform);
	}
	else
	{
		delete scene;
		scene = NULL;
	}

	return scene;
}

gef::Mesh* MenuState::GetMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}

gef::Texture* MenuState::CreateTextureFromPNG(const char* png_filename, gef::Platform& platform)
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

gef::Sprite MenuState::SetupSprite(gef::Texture* texture, float xpos, float ypos, float width, float height)
{
	gef::Sprite sprite;

	sprite.set_texture(texture);
	sprite.set_position(gef::Vector4(xpos, ypos, 2));
	sprite.set_height(height);
	sprite.set_width(width);
	return sprite;
}