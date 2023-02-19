#pragma once
#include "State.h"
#include <thread>
#include <vector>
using std::thread;

class InGameState :
    public State
{
public:
    InGameState(gef::Platform& platform);
    void Update(float frame_time, State** pCurrentState, State* theInstructionsState, State* theMenuState, State* theInGameState, State* theGameOverState, PrimitiveBuilder* primitive_builder_, gef::InputManager* input_manager_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_);
    void Draw(gef::SpriteRenderer* sprite_renderer_, gef::Font* font_, gef::Renderer3D* renderer_3d_, PrimitiveBuilder* primitive_builder_, Player& player_, Player& player2_);
    void onEnter(PrimitiveBuilder* primitive_builder_, gef::AudioManager* audio_manager_, Player &player_, Player &player2_);
    void onExit(Player& player_, Player& player2_, gef::AudioManager* audio_manager_);
private:
    gef::Scene* LoadSceneAssets(gef::Platform& platform, const char* filename);
    gef::Mesh* GetMeshFromSceneAssets(gef::Scene* scene);
    gef::Texture* CreateTextureFromPNG(const char* png_filename, gef::Platform& platform);
    void InitGround(GameObject& ground, b2Body* body, float xpos, float ypos);

    // create the physics world
    b2World* world_;
    gef::Platform& platform_;

    //player lives ui objects
    GameObject player1_lives;
    GameObject player2_lives;

    // ground variables
    gef::Mesh* ground_mesh_;
    GameObject ground_;
    b2Body* ground_body_;

    // ground Middle variables
    GameObject m_ground_;
    b2Body* m_ground_body_;

    // ground Right variables
    GameObject tr_ground_;
    b2Body* tr_ground_body_;

    // ground top Right variables
    GameObject tl_ground_;
    b2Body* tl_ground_body_;

    // ground top left variables
    GameObject r_ground_;
    b2Body* r_ground_body_;

    //powerup variables
    GameObject powerup_;
    b2Body* powerup_body_;

    enum powerup_state
    {
        cooldown,
        alive,
        active
    };
    powerup_state powerup_state_;
    gef::Matrix44 powerupTrans;
    float powerup_y_;
    bool hover_up_;

    //timers
    float powerup_active_timer_;
    float powerup_cooldown_timer_;
    float zoom_timer_;

    //loaded assets
    gef::Mesh* loaded_meshes_[6];
    gef::MeshInstance mesh_instance_[6];
    gef::Scene* scene_assets_;


    //background variables
    gef::Matrix44 bg_;
    bool zoomed_;
    float distance;

    //sound effects
    int sfx_id_[4];
    bool bonk_playing_;
    bool player_collision_;
    bool seperated_;

    //shield textures
    gef::Texture* shield_green_;
    gef::Texture* shield_yellow_;

    //shield materials
    gef::Material* mat;
    gef::Material* mat2;
};