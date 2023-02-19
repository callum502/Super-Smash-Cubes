#pragma once
#include "game_object.h"
class Bullet : public GameObject
{
public:
	void init(b2World* world_, PrimitiveBuilder* primitive_builder_, float playerx, float playery, bool big, bool shoot_left, gef::Mesh* mesh, gef::Mesh* big_mesh);
	bool update(float frame_time, b2World* world_);

	//body
	b2Body* body_;
	//shape
	b2PolygonShape shape_;
	//delete bool
	bool delete_bullet_;
};

