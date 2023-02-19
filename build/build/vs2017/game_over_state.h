#pragma once
#include "State.h"

class GameOverState : public State
{
public:
	GameOverState(gef::Platform& platform);
	void Update(float frame_time, State** pCurrentState, State* theInstructionsState, State* theMenuState, State* theInGameState, State* theGameOverState, PrimitiveBuilder* primitive_builder_, gef::InputManager* input_manager_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_);
	void Draw(gef::SpriteRenderer* sprite_renderer_, gef::Font* font_, gef::Renderer3D* renderer_3d_, PrimitiveBuilder* primitive_builder_, Player& player_, Player& player2_);
	void onEnter(PrimitiveBuilder* primitive_builder_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_);
	void onExit(gef::AudioManager* audio_manager_);
private:
	gef::Scene* LoadSceneAssets(gef::Platform& platform, const char* filename);
	gef::Mesh* GetMeshFromSceneAssets(gef::Scene* scene);
	gef::Texture* CreateTextureFromPNG(const char* png_filename, gef::Platform& platform);
	gef::Sprite GameOverState::SetupSprite(gef::Texture* texture, float xpos, float ypos, float width, float height);

	//platform
	gef::Platform& platform_;

	//player mesh
	gef::Mesh* mesh_;

	//loaded assets
	gef::Mesh* loaded_meshes_[3];
	gef::MeshInstance mesh_instance_[3];
	gef::Scene* scene_assets_;

	//tranform confetti
	gef::Matrix44 confetti_Trans_;

	//textures and sprites
	gef::Sprite spacebar_sprite_;
	gef::Texture* spacebar_texture_;

	gef::Sprite player1_sprite_;
	gef::Texture* player1_texture_;

	gef::Sprite player2_sprite_;
	gef::Texture* player2_texture_;
};

