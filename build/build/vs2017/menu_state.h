#pragma once
#include "State.h"
#include <list>

class MenuState : public State
{
public:
	MenuState(gef::Platform& platform);
	void Update(float frame_time, State** pCurrentState, State* theInstructionsState, State* theMenuState, State* theInGameState, State* theGameOverState, PrimitiveBuilder* primitive_builder_, gef::InputManager* input_manager_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_);
	void Draw(gef::SpriteRenderer* sprite_renderer_, gef::Font* font_, gef::Renderer3D* renderer_3d_, PrimitiveBuilder* primitive_builder_, Player& player_, Player& player2_);
	void onEnter(PrimitiveBuilder* primitive_builder_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_);
	void onExit(gef::AudioManager* audio_manager_);
private:
	gef::Scene* LoadSceneAssets(gef::Platform& platform, const char* filename);
	gef::Mesh* GetMeshFromSceneAssets(gef::Scene* scene);
	gef::Texture* CreateTextureFromPNG(const char* png_filename, gef::Platform& platform);
	gef::Sprite MenuState::SetupSprite(gef::Texture* texture, float xpos, float ypos, float width, float height);
	gef::Platform& platform_;

	//player mesh
	gef::Mesh* mesh;

	//loaded assets
	const char* scene_asset_filename[12];
	gef::Mesh* loaded_meshes_[12];
	gef::MeshInstance mesh_instance_[24];
	gef::Scene* scene_assets_;


	//background variables
	float state_time = 0;
	gef::Matrix44 bg_Trans_;

	//textures and sprites
	gef::Sprite background;
	gef::Texture* background_;

	gef::Sprite title;
	gef::Texture* title_text_;

	gef::Sprite start;
	gef::Texture* start_text_;

	gef::Sprite instructions;
	gef::Texture* instructions_text_;

	gef::Sprite player1_sprite_;
	gef::Texture* player1_text_;

	gef::Sprite player2_sprite_;
	gef::Texture* player2_text_;
};

