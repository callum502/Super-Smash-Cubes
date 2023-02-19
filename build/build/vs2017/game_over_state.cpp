#include "game_over_state.h"

GameOverState::GameOverState(gef::Platform& platform):
platform_(platform)
{
}

void GameOverState::Update(float frame_time, State** pCurrentState, State* theInstructionsState, State* theMenuState, State* theInGameState, State* theGameOverState, PrimitiveBuilder* primitive_builder_, gef::InputManager* input_manager_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_)
{
	const gef::Keyboard* keyboard = input_manager_->keyboard();
	
	//move confetti
	if (confetti_Trans_.GetTranslation().y() > -20)
	{
		confetti_Trans_.SetTranslation(gef::Vector4(confetti_Trans_.GetTranslation().x(), confetti_Trans_.GetTranslation().y() - 0.1 * frame_time * 60, 0, 1));
		mesh_instance_[0].set_transform(confetti_Trans_);
	}
	else
	{
		confetti_Trans_.SetTranslation(gef::Vector4(confetti_Trans_.GetTranslation().x(), 5, 0, 1));
		mesh_instance_[0].set_transform(confetti_Trans_);
	}

	//resize hats
	player_.resizeHat();
	mesh_instance_[1].set_transform(player_.hatTrans);

	player2_.resizeHat();
	mesh_instance_[2].set_transform(player2_.hatTrans);

	//winner dance
	if (player_.lives_)
	{
		player_.dance(frame_time);
	}
	else 
	{
		player2_.dance(frame_time);
	}

	//switch back to menu state
	if ((keyboard->IsKeyPressed(gef::Keyboard::KeyCode::KC_SPACE)))
	{
		*pCurrentState = theMenuState;
		theMenuState->onEnter(primitive_builder_, audio_manager_, player_, player2_);
	}
}

void GameOverState::Draw(gef::SpriteRenderer* sprite_renderer_, gef::Font* font_, gef::Renderer3D* renderer_3d_, PrimitiveBuilder* primitive_builder_, Player& player_, Player& player2_)
{
	// draw winner text
	if (player_.lives_)
	{
		sprite_renderer_->DrawSprite(player1_sprite_);
	}
	else
	{
		sprite_renderer_->DrawSprite(player2_sprite_);
	}

	//draw play again text
	sprite_renderer_->DrawSprite(spacebar_sprite_);

	//draw players
	renderer_3d_->set_override_material(&primitive_builder_->red_material());
	renderer_3d_->DrawMesh(player_);
	renderer_3d_->set_override_material(&primitive_builder_->blue_material());
	renderer_3d_->DrawMesh(player2_);

	//draw confetti and hats
	renderer_3d_->set_override_material(NULL);
	renderer_3d_->DrawMesh(mesh_instance_[0]);
	renderer_3d_->DrawMesh(mesh_instance_[1]);
	renderer_3d_->DrawMesh(mesh_instance_[2]);
}

void GameOverState::onEnter(PrimitiveBuilder* primitive_builder_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_)
{
	//music
	audio_manager_->LoadMusic("win.wav", platform_);
	audio_manager_->PlayMusic();

	//set player meshes
	mesh_ = primitive_builder_->CreateBoxMesh(gef::Vector4(0.75, 0.75, 0.75, 0.75), gef::Vector4(0, 0, 0, 0), NULL);
	player_.set_mesh(mesh_);
	player2_.set_mesh(mesh_);

	//for hats to be correct size
	player_.big = true;
	player2_.big = true;

	//for dance function
	player_.falling = false;
	player2_.falling = false;

	//position/scale players
	player_.mesh_Trans_.Scale(gef::Vector4(1, 1, 1, 1));
	player2_.mesh_Trans_.Scale(gef::Vector4(1, 1, 1, 1));
	if (player_.lives_)
	{
		player_.mesh_Trans_.SetTranslation(gef::Vector4(0, 0, 0, 1));
		player2_.mesh_Trans_.SetTranslation(gef::Vector4(5, -1, -10, 1));
	}
	else
	{
		player_.mesh_Trans_.SetTranslation(gef::Vector4(-5, -1, -10, 1));
		player2_.mesh_Trans_.SetTranslation(gef::Vector4(0, 0, 0, 1));
	}
	player_.set_transform(player_.mesh_Trans_);
	player2_.set_transform(player2_.mesh_Trans_);

	//set model file names
	const char* scene_asset_filename[3];
	scene_asset_filename[0] = "confetti.scn";

	//load player hats
	scene_asset_filename[1] = player_.loadHat();
	scene_asset_filename[2] = player2_.loadHat();

	//set loaded model meshes
	for (int i = 0; i < 3; i++)
	{
		scene_assets_ = LoadSceneAssets(platform_, scene_asset_filename[i]);

		if (scene_assets_)
		{
			loaded_meshes_[i] = GetMeshFromSceneAssets(scene_assets_);
			mesh_instance_[i].set_mesh(loaded_meshes_[i]);
		}
		else
		{
			gef::DebugOut("Scene file %s failed to load\n", scene_asset_filename);
		}
	}

	//setup textures
	spacebar_texture_ = CreateTextureFromPNG("PRESS-SPACEBAR-TO-PLAY-A.png", platform_);
	player1_texture_ = CreateTextureFromPNG("PLAYER-1-WINS.png", platform_);
	player2_texture_ = CreateTextureFromPNG("PLAYER-2-WINS.png", platform_);

	//setup sprite
	spacebar_sprite_ = SetupSprite(spacebar_texture_, platform_.width() * 0.5f, platform_.height() * 0.9f, platform_.width() * 0.53, platform_.height() * 0.07);
	player1_sprite_ = SetupSprite(player1_texture_, platform_.width() * 0.5f, platform_.height() * 0.1f, platform_.width() * 0.49, platform_.height() * 0.1);
	player2_sprite_ = SetupSprite(player2_texture_, platform_.width() * 0.5f, platform_.height() * 0.1f, platform_.width() * 0.49, platform_.height() * 0.1);

	//position confetti
	confetti_Trans_.Scale(gef::Vector4(0.8, 0.8, 0.8, 0.8));
	confetti_Trans_.SetTranslation(gef::Vector4(0, 0, 0, 1));
}

void GameOverState::onExit(gef::AudioManager* audio_manager_)
{
	//clean up data
	audio_manager_->UnloadMusic();

	delete spacebar_texture_;
	delete player1_texture_;
	delete player2_texture_;

	delete mesh_;

	for (int i = 0; i < 3; i++)
	{
		delete loaded_meshes_[i];
	}
}

gef::Scene* GameOverState::LoadSceneAssets(gef::Platform& platform, const char* filename)
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

gef::Mesh* GameOverState::GetMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}

gef::Texture* GameOverState::CreateTextureFromPNG(const char* png_filename, gef::Platform& platform)
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

gef::Sprite GameOverState::SetupSprite(gef::Texture* texture, float xpos, float ypos, float width, float height)
{
	gef::Sprite sprite;

	sprite.set_texture(texture);
	sprite.set_position(gef::Vector4(xpos, ypos, 2));
	sprite.set_height(height);
	sprite.set_width(width);
	return sprite;
}