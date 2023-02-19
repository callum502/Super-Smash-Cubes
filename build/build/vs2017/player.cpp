#include "player.h"
Player::Player()
{
	set_type(PLAYER);
}

void Player::update(float frame_time, gef::InputManager* input_manager_, b2World* world_, PrimitiveBuilder* primitive_builder_)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	const gef::Keyboard* keyboard = input_manager_->keyboard();
	float speed = 2;

	//left and right movement
	if (right && body_->GetLinearVelocity().x < speed)
	{
		body_->SetLinearVelocity(b2Vec2(body_->GetLinearVelocity().x + (0.1 * frame_time * 60), body_->GetLinearVelocity().y));
	}
	if (left && body_->GetLinearVelocity().x > -speed)
	{
		body_->SetLinearVelocity(b2Vec2(body_->GetLinearVelocity().x - (0.1 * frame_time * 60), body_->GetLinearVelocity().y));
	}

	//jumping
	if (up)
	{
		if (grounded)
		{
			body_->SetLinearVelocity(b2Vec2(body_->GetLinearVelocity().x, 6));
			grounded = false;
			doublejumping = false;
		}
		else if (!doublejumping && body_->GetLinearVelocity().y <= 0)
		{
			body_->SetLinearVelocity(b2Vec2(body_->GetLinearVelocity().x, 5));
			doublejumping = true;
		}	
	}

	//dashing
	if (dash_cooldown > 2)
	{
		if (dash)
		{
			if (right)
			{
				body_->SetLinearVelocity(b2Vec2(body_->GetLinearVelocity().x + 10, body_->GetLinearVelocity().y));
				dash_cooldown = 0;
			}
			else if (left)
			{
				body_->SetLinearVelocity(b2Vec2(body_->GetLinearVelocity().x - 10, body_->GetLinearVelocity().y));
				dash_cooldown = 0;
			}
			else if (body_->GetLinearVelocity().x > 0)
			{
				body_->SetLinearVelocity(b2Vec2(body_->GetLinearVelocity().x + 10, body_->GetLinearVelocity().y));
				dash_cooldown = 0;
			}
			else if (body_->GetLinearVelocity().x < 0)
			{
				body_->SetLinearVelocity(b2Vec2(body_->GetLinearVelocity().x - 10, body_->GetLinearVelocity().y));
				dash_cooldown = 0;
			}
		}
	}
	else
	{
		dash_cooldown += frame_time;
	}

	//blocking
	if (block)
	{
		body_->SetLinearVelocity(b2Vec2(body_->GetLinearVelocity().x, body_->GetLinearVelocity().y - block_force_));
		if (block_force_ > 0)
		{
			block_force_ -= 0.1 * frame_time * 60;
		}
		else
		{
			block_force_ = 0;
		}
	}
	else
	{
		if (block_force_ < 10)
		{
			block_force_ += 0.1 * frame_time * 60;
		}
	}

	//shooting
	if (shoot_timer > 0.5)
	{
		//create new bullet when shoot button pressed
		if (shoot_left || shoot_right)
		{
			Bullet* bullet_ = new Bullet();
			bullet_->init(world_,primitive_builder_, body_->GetPosition().x, body_->GetPosition().y, big,shoot_left, bullet_mesh_, big_bullet_mesh_);
			bullets_.push_back(bullet_);
			shoot_timer = 0;
		}
	}
	else
	{
		shoot_timer += frame_time;
	}

	int bullet_to_del_ = NULL;
	bool delete_bullet_ = false;
	//update bullet and set to delete if not alive
	if (!bullets_.empty())
	{
		int i = 0;
		for (auto bullet : bullets_)
		{
			//if bullet is dead set bullet_to_delete to bullet
			if (!bullet->update(frame_time, world_))
			{
				bullet_to_del_ = i;
				delete_bullet_ = true;
			}
			i++;
		}
	}

	//delete bullet if dead
	if (delete_bullet_)
	{
		world_->DestroyBody(bullets_[bullet_to_del_]->body_);
		delete bullets_[bullet_to_del_];
		bullets_.erase(bullets_.begin()+ bullet_to_del_);
		delete_bullet_ = false;
	}

	//death check
	if (body_->GetPosition().y < -7.5)
	{
		body_->SetTransform(b2Vec2(spawnpoint.x, spawnpoint.y), 0);
		body_->SetLinearVelocity(b2Vec2(0, 0));
		body_->SetAngularVelocity(0);
		lives_--;
		grounded = false;
		death = true;
	}

	resetMoveBools();
}

void Player::render(gef::Renderer3D* renderer_3d_)
{
	renderer_3d_->DrawMesh(*this);

	//draw bullets
	if (!bullets_.empty())
	{
		for (auto bullet : bullets_)
		{
			renderer_3d_->DrawMesh(*bullet);
		}
	}
}

void Player::init(PrimitiveBuilder* primitive_builder_, b2World* world_)
{
	//clear bullet vectors
	bullets_.clear();

	//set meshes
	set_mesh(primitive_builder_->GetDefaultCubeMesh());
	bullet_mesh_ = primitive_builder_->CreateBoxMesh(gef::Vector4(0.25, 0.25, 0.25), gef::Vector4(0, 0, 0));
	big_bullet_mesh_ = primitive_builder_->CreateBoxMesh(gef::Vector4(0.35, 0.35, 0.35), gef::Vector4(0, 0, 0));
	big_mesh_ = primitive_builder_->CreateBoxMesh(gef::Vector4(0.75, 0.75, 0.75, 0.75), gef::Vector4(0, 0, 0, 0), NULL);

	//init variables
	lives_ = 5;
	big = false;
	dash_cooldown = 0;
	grounded = false;
	flip_wait_ = 0;
	rotation = 0;
	maxrotate = 180;
	death = false;
	block_force_ = 5;
	shoot_timer = 0;
	doublejumping = false;
	resetMoveBools();

	// create a physics body for the player
	b2BodyDef player_body_def;
	player_body_def.type = b2_dynamicBody;
	player_body_def.position = b2Vec2(0.0f, 4.0f);
	// create a connection between the rigid body and GameObject
	player_body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

	body_ = world_->CreateBody(&player_body_def);

	// set the shape for the player
	shape_.SetAsBox(0.5f, 0.5f);


	// create the fixture
	b2FixtureDef player_fixture_def;
	player_fixture_def.shape = &shape_;
	player_fixture_def.density = 1.0f;

	// create the fixture on the rigid body
	body_->CreateFixture(&player_fixture_def);

	//set to spawn points
	body_->SetTransform(b2Vec2(spawnpoint), 0);

	// update visuals from simulation data
	UpdateFromSimulation(body_);
}

void Player::resetMoveBools()
{
	//reset input bools every frame
	left = false;
	right = false;
	dash = false;
	up = false;
	shoot_left = false;
	shoot_right = false;
}

void Player::resizeHat()
{
	//set scale and pos variables
	float hat_scale_;
	float y_shift_;
	if (big)
	{
		switch (hatSelect)
		{
		case 0:
			hat_scale_ = 1;
			y_shift_ = 1.2;
			break;
		case 1:
			hat_scale_ = 0.45;
			y_shift_ = -4.75;
			break;
		case 2:
			hat_scale_ = 1.1;
			y_shift_ = 1.2;
			break;
		case 3:
			hat_scale_ = 0.8;
			y_shift_ = 1.75;
			break;
		case 4:
			hat_scale_ = 0.28;
			y_shift_ = 0.5;
			break;
		case 5:
			hat_scale_ = 15;
			y_shift_ = -1.8;
			break;
		case 6:
			hat_scale_ = 0.15;
			y_shift_ = -1.6;
			break;
		case 7:
			hat_scale_ = 1.1;
			y_shift_ = 1.05;
			break;
		case 8:
			hat_scale_ = 0.03;
			y_shift_ = -0.3;
			break;
		case 9:
			hat_scale_ = 1.1;
			y_shift_ = 1.2;
			break;
		case 10:
			hat_scale_ = 0.3;
			y_shift_ = -0.1;
			break;
		case 11:
			hat_scale_ = 0.026;
			y_shift_ = -0.25;
			break;
		default:
			hat_scale_ = 1;
			y_shift_ = 1.2;
		}
	}
	else
	{
		switch (hatSelect)
		{
		case 0:
			hat_scale_ = 0.75;
			y_shift_ = 0.75;
			break;
		case 1:
			hat_scale_ = 0.35;
			y_shift_ = -3.75;
			break;
		case 2:
			hat_scale_ = 0.75;
			y_shift_ = 0.85;
			break;
		case 3:
			hat_scale_ = 0.5;
			y_shift_ = 1.1;
			break;
		case 4:
			hat_scale_ = 0.18;
			y_shift_ = 0.35;
			break;
		case 5:
			hat_scale_ = 10;
			y_shift_ = -1.2;
			break;
		case 6:
			hat_scale_ = 0.1;
			y_shift_ = -1.1;
			break;
		case 7:
			hat_scale_ = 0.9;
			y_shift_ = 0.75;
			break;
		case 8:
			hat_scale_ = 0.021;
			y_shift_ = -0.27;
			break;
		case 9:
			hat_scale_ = 0.75;
			y_shift_ = 0.8;
			break;
		case 10:
			hat_scale_ = 0.2;
			y_shift_ = -0.1;
			break;
		case 11:
			hat_scale_ = 0.018;
			y_shift_ = -0.2;
			break;
		default:
			hat_scale_ = 0.75;
			y_shift_ = 0.75;
		}
	}

	//apply scale and y shift
	hatTrans.Scale(gef::Vector4(hat_scale_, hat_scale_, hat_scale_));
	hatTrans.SetTranslation(gef::Vector4(this->transform().GetTranslation().x(), this->transform().GetTranslation().y() + y_shift_, this->transform().GetTranslation().z(), 0));

	//position shield
	if (block)
	{
		if (big)
		{
			blockTrans.Scale(gef::Vector4(2.25, 2.25, 2.25));
			blockTrans.SetTranslation(gef::Vector4(this->transform().GetTranslation().x(), this->transform().GetTranslation().y() - 0.2, this->transform().GetTranslation().z() + 0.5));
		}
		else
		{
			blockTrans.Scale(gef::Vector4(1.5, 1.5, 1.5));
			blockTrans.SetTranslation(gef::Vector4(this->transform().GetTranslation().x(), this->transform().GetTranslation().y() - 0.2, this->transform().GetTranslation().z() + 0.5));
		}
	}
}

void Player::dance(float frame_time)
{
	//dance animation
	float y = mesh_Trans_.GetTranslation().y();
	if (!falling)
	{
		//move up and flip
		if (mesh_Trans_.GetTranslation().y() < 2.5)
		{
			mesh_Trans_.RotationX(gef::DegToRad(rotation));
			mesh_Trans_.SetTranslation(gef::Vector4(0, y + 0.2 * frame_time * 60, 0, 0));
			rotation += 10 * frame_time * 60;
		}
		else
		{
			falling = true;
		}
	}
	else
	{
		//move down and flip
		if (mesh_Trans_.GetTranslation().y() > 0)
		{
			mesh_Trans_.RotationX(gef::DegToRad(rotation));
			mesh_Trans_.SetTranslation(gef::Vector4(0, y - 0.2 * frame_time * 60, 0, 0));
			//increase rotation until flip complete
			if (rotation < maxrotate)
			{
				rotation += 10 * frame_time * 60;
			}
		}
		else
		{
			//wait between flips
			if (flip_wait_ < 0.2)
			{
				flip_wait_ += frame_time;
			}
			else
			{
				falling = false;
				flip_wait_ = 0;
				maxrotate += 180;
			}
		}
	}
	set_transform(mesh_Trans_);
}

void Player::setBig(PrimitiveBuilder* primitive_builder_, b2World* world_)
{
	//set big bool
	big = true;

	// create the big shape
	b2PolygonShape big_shape_;
	big_shape_.SetAsBox(0.75f, 0.75f);

	// create the big fixture
	b2FixtureDef player_fixture_def;
	player_fixture_def.shape = &big_shape_;
	player_fixture_def.density = 1.0f;

	//set big mesh/fixture
	big_fixture_ = body_->CreateFixture(&player_fixture_def);
	set_mesh(big_mesh_);
	UpdateFromSimulation(body_);
}

void Player::cleanUp()
{
	//clean up data
	delete bullet_mesh_;
	delete big_bullet_mesh_;
	delete big_mesh_;
}

const char* Player::loadHat()
{
	switch (hatSelect)
	{
	case 0:
		return "pirate_hat.scn";
		break;
	case 1:
		return "2011_hat.scn";
		break;
	case 2:
		return "cowboy.scn";
		break;
	case 3:
		return "party_hat.scn";
		break;
	case 4:
		return "frog_hat.scn";
		break;
	case 5:
		return "witch_hat.scn";
		break;
	case 6:
		return "chef_hat.scn";
		break;
	case 7:
		return "police_hat.scn";
		break;
	case 8:
		return "wa_hat.scn";
		break;
	case 9:
		return "toad_hat.scn";
		break;
	case 10:
		return "duck_hat.scn";
		break;
	case 11:
		return "penguin_hat.scn";
		break;
	default:
		return "pirate_hat.scn";
	}
}