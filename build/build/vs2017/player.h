#include "bullet.h"
#include "game_object.h"
#include <system/debug_log.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include<vector>
class Player : public GameObject
{
public:
	Player();
	void update(float frame_time, gef::InputManager* input_manager_, b2World* world_, PrimitiveBuilder* primitive_builder_);
	void render(gef::Renderer3D* renderer_3d_);
	void init(PrimitiveBuilder* primitive_builder_, b2World* world_);
	void resetMoveBools();
	void resizeHat();
	void dance(float frame_time);
	void setBig(PrimitiveBuilder* primitive_builder_, b2World* world_);
	void cleanUp();
	const char* loadHat();
	//body
	b2Body* body_;
	//tansformation matrix
	gef::Matrix44 mesh_Trans_;
	//shape
	b2PolygonShape shape_;
	//spawnpoint
	b2Vec2 spawnpoint;

	//meshes
	gef::Mesh* bullet_mesh_;
	gef::Mesh* big_bullet_mesh_;
	gef::Mesh* big_mesh_;

	//bullet variables
	std::vector<Bullet*> bullets_;
	float shoot_timer;

	//move bools
	bool left;
	bool right;
	bool dash;
	bool up;
	bool shoot_right;
	bool shoot_left;

	//block variables
	bool block;
	float block_force_;
	gef::Matrix44 blockTrans;

	//jump bool
	bool grounded;
	bool doublejumping;

	//dash timer
	float dash_cooldown;

	//death bool
	bool death;

	//lives remaining
	int lives_;

	//powerup variables
	bool big;
	b2Fixture* big_fixture_;

	//current hat
	int hatSelect = 0;

	//hat translation
	gef::Matrix44 hatTrans;

	//dance variables
	float rotation;
	float maxrotate;
	float flip_wait_; //timer
	bool falling;

private:
};