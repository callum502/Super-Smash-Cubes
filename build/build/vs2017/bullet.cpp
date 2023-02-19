#include "bullet.h"

void Bullet::init(b2World* world_, PrimitiveBuilder* primitive_builder_, float playerx, float playery, bool big, bool shoot_left, gef::Mesh* mesh, gef::Mesh* big_mesh)
{
	//init
	timer = 0;
	set_type(BULLET);

	//setup mesh
	if (big)
	{
		set_mesh(big_mesh);
	}
	else
	{
		set_mesh(mesh);
	}
	// create a physics body for the bullet
	b2BodyDef bullet_body_def;
	bullet_body_def.type = b2_dynamicBody;
	float x = 0.8;
	if (big)
	{
		x = x * 1.25;
	}
	if (shoot_left)
	{
		x = x * -1;
	}

	bullet_body_def.position = b2Vec2(playerx + x, playery);

	// create a connection between the rigid body and GameObject
	bullet_body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

	body_ = world_->CreateBody(&bullet_body_def);

	// set the shape for the bullet
	if (big)
	{
		shape_.SetAsBox(0.35f, 0.35f);
	}
	else
	{
		shape_.SetAsBox(0.25f, 0.25f);
	}

	// create the fixture
	b2FixtureDef bullet_fixture_def;
	bullet_fixture_def.shape = &shape_;
	bullet_fixture_def.density = 1.0f;

	// create the fixture on the rigid body
	body_->CreateFixture(&bullet_fixture_def);

	// update visuals from simulation data
	UpdateFromSimulation(body_);

	//set speed/rotate
	body_->SetAngularVelocity(50);
	if (shoot_left)
	{
		body_->SetLinearVelocity(b2Vec2(-20, 0));
	}
	else
	{
		body_->SetLinearVelocity(b2Vec2(20, 0));
	}

	body_->SetGravityScale(0);
}

bool Bullet::update(float frame_time, b2World* world_)
{
	UpdateFromSimulation(body_);
	if (timer > 2)
	{
		return false; //return player dead
	}
	else
	{
		timer += frame_time;
	}

	if (fabs(body_->GetLinearVelocity().x) != 20 && timer>0.1)//collision check that works for static ground objects
	{
		return false; //return player dead
	}

	return true; //player still alive
}
