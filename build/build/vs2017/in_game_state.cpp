#include "in_game_state.h"

InGameState::InGameState(gef::Platform& platform) :
	platform_(platform)
{
}
void InGameState::Update(float frame_time, State** pCurrentState, State* theInstructionsState, State* theMenuState, State* theInGameState, State* theGameOverState, PrimitiveBuilder* primitive_builder_, gef::InputManager* input_manager_, gef::AudioManager* audio_manager_, Player& player_, Player& player2_)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	const gef::Keyboard* keyboard = input_manager_->keyboard();

	//set player 1 input keys
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_SPACE)))
	{
		player_.dash = true;
	}
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_A)))
	{
		player_.left = true;
	}
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_D)))
	{
		player_.right = true;
	}
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_W)))
	{
		player_.up = true;
	}
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_Q)))
	{
		player_.shoot_left = true;
	}
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_E)))
	{
		player_.shoot_right = true;
	}
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_S)))
	{
		if (!player_.block)
		{
			audio_manager_->PlaySample(sfx_id_[3]);
		}
		player_.block = true;
	}
	else
	{
		player_.block = false;
	}

	//set player 2 input keys
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_RETURN)))
	{
		player2_.dash = true;
	}
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_NUMPAD4)))
	{
		player2_.left = true;
	}
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_NUMPAD6)))
	{
		player2_.right = true;
	}
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_NUMPAD8)))
	{
		player2_.up = true;
	}
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_NUMPAD7)))
	{
		player2_.shoot_left = true;
	}
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_NUMPAD9)))
	{
		player2_.shoot_right = true;
	}
	
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_NUMPAD5)))
	{
		if (!player2_.block)
		{
			//play block sound once
			audio_manager_->PlaySample(sfx_id_[3]);
		}
		player2_.block = true;
	}
	else
	{

		player2_.block = false;
	}

	//update players
	player_.update(frame_time, input_manager_, world_, primitive_builder_);
	player2_.update(frame_time, input_manager_, world_, primitive_builder_);

	//play death sound
	if (player_.death || player2_.death)
	{
		audio_manager_->PlaySample(sfx_id_[2], false);
		player_.death = false;
		player2_.death = false;
	}

	// update physics world
	float timeStep = frame_time;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world_->Step(timeStep, velocityIterations, positionIterations);

	// update object visuals from simulation data
	player_.UpdateFromSimulation(player_.body_);
	player2_.UpdateFromSimulation(player2_.body_);

	// collision detection
	// get the head of the contact list
	b2Contact* contact = world_->GetContactList();
	// get contact count
	int contact_count = world_->GetContactCount();

	for (int contact_num = 0; contact_num < contact_count; ++contact_num)
	{
		if (contact->IsTouching())
		{
			// get the colliding bodies
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

			// DO COLLISION RESPONSE HERE
			Player* player = NULL;

			GameObject* gameObjectA = NULL;
			GameObject* gameObjectB = NULL;

			gameObjectA = reinterpret_cast<GameObject*>(bodyA->GetUserData().pointer);
			gameObjectB = reinterpret_cast<GameObject*>(bodyB->GetUserData().pointer);


			if (gameObjectA && gameObjectB)
			{
				if (gameObjectA->type() == PLAYER && gameObjectB->type() == POWERUP)
				{
					//set player to big
					player = reinterpret_cast<Player*>(bodyA->GetUserData().pointer);
					player->setBig(primitive_builder_, world_);

					//set powerup to active
					powerup_state_ = active;

					//remove powerup
					world_->DestroyBody(powerup_body_);
					powerup_.UpdateFromSimulation(powerup_body_);

					//play sound
					audio_manager_->PlaySample(sfx_id_[1], false);
				}

				if (gameObjectA->type() == PLAYER && gameObjectB->type() == PLAYER)
				{
					//play collsion sound once
					if (seperated_)
					{
						audio_manager_->PlaySample(sfx_id_[0], false);
						seperated_ = false;
					}
					player_collision_ = true;
				}

				//set player to grounded when colliding with side/top of ground
				if (gameObjectA->type() == PLAYER && gameObjectB->type() == GROUND)
				{
					player = reinterpret_cast<Player*>(bodyA->GetUserData().pointer);
					if (player->body_->GetPosition().y >= bodyB->GetPosition().y)
					{
						player->grounded = true;
					}
				}

				//set player to grounded when colliding with side/top of ground
				if (gameObjectA->type() == GROUND && gameObjectB->type() == PLAYER)
				{
					player = reinterpret_cast<Player*>(bodyB->GetUserData().pointer);
					if (player->body_->GetPosition().y >= bodyA->GetPosition().y)
					{
						player->grounded = true;
					}
				}
			}
		}

		// Get next contact point
		contact = contact->GetNext();
	}

	//check if players have stopped colliding
	if (!player_collision_)
	{
		seperated_ = true;
	}
	else
	{
		player_collision_ = false;
	}

	if (powerup_state_ == cooldown)
	{
		//spawn powerup after 10 seconds
		powerup_cooldown_timer_ += frame_time;
		if (powerup_cooldown_timer_ > 10)
		{
			powerup_cooldown_timer_ = 0;
			powerup_state_ = alive;

			// setup the mesh for the powerup
			powerup_.set_mesh(primitive_builder_->GetDefaultSphereMesh());

			// create a physics body
			b2BodyDef powerup_body_def;
			powerup_body_def.type = b2_dynamicBody;
			powerup_body_def.position = b2Vec2(0.0f, 2.5f);

			// create a connection between the rigid body and GameObject
			powerup_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&powerup_);

			powerup_body_ = world_->CreateBody(&powerup_body_def);

			// create the shape
			b2PolygonShape powerup_shape_;
			powerup_shape_.SetAsBox(0.5f, 0.5f);

			// create the fixture
			b2FixtureDef powerup_fixture_def;
			powerup_fixture_def.shape = &powerup_shape_;
			powerup_fixture_def.isSensor = true;

			// create the fixture on the rigid body
			powerup_body_->CreateFixture(&powerup_fixture_def);

			powerup_.UpdateFromSimulation(powerup_body_);
			powerup_.set_type(POWERUP);
		}
	}
	else if (powerup_state_ == alive)
	{
		//hover animation
		if (hover_up_)
		{
			if (powerup_body_->GetPosition().y < 2.7)
			{
				powerup_body_->SetLinearVelocity(b2Vec2(0, 0.25));
			}
			else
			{
				hover_up_ = false;
			}
		}
		else
		{
			if (powerup_body_->GetPosition().y > 2.3)
			{
				powerup_body_->SetLinearVelocity(b2Vec2(0, -0.25));
			}
			else
			{
				hover_up_ = true;
			}
		}
		powerup_.UpdateFromSimulation(powerup_body_);
	}
	else if (powerup_state_ == active)
	{
		//after 10 seconds make player small again
		powerup_active_timer_ += frame_time;
		if (powerup_active_timer_ > 10)
		{
			powerup_active_timer_ = 0;
			powerup_state_ = cooldown;

			b2PolygonShape shape_;
			shape_.SetAsBox(0.5f, 0.5f);

			// destroy big fixture
			if (player_.big)
			{
				player_.body_->DestroyFixture(player_.big_fixture_);
				player_.set_mesh(primitive_builder_->GetDefaultCubeMesh());
				player_.big = false;
			}
			else if (player2_.big)
			{
				player2_.body_->DestroyFixture(player2_.big_fixture_);
				player2_.set_mesh(primitive_builder_->GetDefaultCubeMesh());
				player2_.big = false;
			}
		}
	}

	//resize hats and shield
	player_.resizeHat();
	player2_.resizeHat();

	mesh_instance_[1].set_transform(player_.hatTrans);
	mesh_instance_[2].set_transform(player2_.hatTrans);

	mesh_instance_[3].set_transform(player_.blockTrans);
	mesh_instance_[4].set_transform(player2_.blockTrans);

	//check for game over
	if (!player_.lives_ || !player2_.lives_)
	{
		//switch to game over state
		*pCurrentState = theGameOverState;
		onExit(player_, player2_, audio_manager_);
		theGameOverState->onEnter(primitive_builder_, audio_manager_, player_, player2_);
		platform_.set_render_target_clear_colour(gef::Colour(0.4f, 0.525f, 0.7f, 1.0f));
	}
}

void InGameState::Draw(gef::SpriteRenderer* sprite_renderer_, gef::Font* font_, gef::Renderer3D* renderer_3d_, PrimitiveBuilder* primitive_builder_, Player& player_, Player& player2_)
{
	//draw loaded models
	renderer_3d_->set_override_material(NULL);
	renderer_3d_->DrawMesh(mesh_instance_[0]);
	renderer_3d_->DrawMesh(mesh_instance_[1]);
	renderer_3d_->DrawMesh(mesh_instance_[2]);

	//draw shield with correct colour
	if (player_.block)
	{
		if (player_.block_force_>6.66)
		{
			mat->set_texture(shield_green_);
			renderer_3d_->set_override_material(mat);
			renderer_3d_->DrawMesh(mesh_instance_[3]);
		}
		else if (player_.block_force_ > 3.33)
		{
			mat->set_texture(shield_yellow_);
			renderer_3d_->set_override_material(mat);
			renderer_3d_->DrawMesh(mesh_instance_[3]);
		}
		else if (player_.block_force_>0.1)
		{
			renderer_3d_->set_override_material(NULL);
			renderer_3d_->DrawMesh(mesh_instance_[3]);
		}
	}

	//draw player 2 shield with correct colour
	if (player2_.block)
	{
		if (player2_.block_force_ > 6.66)
		{
			mat2->set_texture(shield_green_);
			renderer_3d_->set_override_material(mat2);
			renderer_3d_->DrawMesh(mesh_instance_[4]);
		}
		else if (player2_.block_force_ > 3.33)
		{
			mat2->set_texture(shield_yellow_);
			renderer_3d_->set_override_material(mat2);
			renderer_3d_->DrawMesh(mesh_instance_[4]);
		}
		else if (player2_.block_force_ > 0.1)
		{
			renderer_3d_->set_override_material(NULL);
			renderer_3d_->DrawMesh(mesh_instance_[4]);
		}
	}

	//draw platforms
	renderer_3d_->set_override_material(NULL);
	renderer_3d_->DrawMesh(ground_);
	renderer_3d_->DrawMesh(m_ground_);
	renderer_3d_->DrawMesh(r_ground_);
	renderer_3d_->DrawMesh(tl_ground_);
	renderer_3d_->DrawMesh(tr_ground_);

	// draw players
	renderer_3d_->set_override_material(&primitive_builder_->red_material());
	player_.render(renderer_3d_);

	renderer_3d_->set_override_material(&primitive_builder_->blue_material());
	player2_.render(renderer_3d_);

	//draw player 1 lives
	renderer_3d_->set_override_material(&primitive_builder_->red_material());
	for (int i = 0; i < player_.lives_; i++)
	{
		player_.mesh_Trans_.SetIdentity();
		player_.mesh_Trans_.RotationY(gef::DegToRad(15));
		player_.mesh_Trans_.SetTranslation(gef::Vector4(i * 1 - 9.5, 5, -i * 0.5, 1));
		player1_lives.set_transform(player_.mesh_Trans_);
		renderer_3d_->DrawMesh(player1_lives);
	}

	//draw player 2 lives
	renderer_3d_->set_override_material(&primitive_builder_->blue_material());
	for (int i = player2_.lives_; i > 0; i--)
	{
		player2_.mesh_Trans_.SetIdentity();
		player2_.mesh_Trans_.RotationY(gef::DegToRad(75));
		player2_.mesh_Trans_.SetTranslation(gef::Vector4(i * -1 + 11, 5, i * -0.5, 1));
		player2_lives.set_transform(player2_.mesh_Trans_);
		renderer_3d_->DrawMesh(player2_lives);
	}

	//draw powerup
	if (powerup_state_ == alive)
	{
		renderer_3d_->set_override_material(&primitive_builder_->green_material());
		renderer_3d_->DrawMesh(powerup_);
	}
}

void InGameState::onEnter(PrimitiveBuilder* primitive_builder_, gef::AudioManager* audio_manager_, Player &player_, Player &player2_ )
{
	//set filenames for loaded models
	const char* scene_asset_filename[6];
	scene_asset_filename[0] = "bg2.scn";
	scene_asset_filename[3] = "shield_red.scn";
	scene_asset_filename[4] = "shield_red.scn";
	scene_asset_filename[5] = "platform.scn";

	//play music
	audio_manager_->LoadMusic("Megalovania.wav", platform_);
	audio_manager_->PlayMusic();

	//load sound effects
	sfx_id_[0] = audio_manager_->LoadSample("Bonk.wav", platform_);
	sfx_id_[1] = audio_manager_->LoadSample("box_collected.wav", platform_);
	sfx_id_[2] = audio_manager_->LoadSample("death.wav", platform_);
	sfx_id_[3] = audio_manager_->LoadSample("shield.wav", platform_);

	//initialise live objects
	player1_lives.set_mesh(primitive_builder_->GetDefaultCubeMesh());
	player2_lives.set_mesh(primitive_builder_->GetDefaultCubeMesh());

	// initialise the physics world
	b2Vec2 gravity(0.0f, -9.81f);
	world_ = new b2World(gravity);

	//set spawn
	player_.spawnpoint = b2Vec2(-7.5, 2);
	player2_.spawnpoint = b2Vec2(7.5, 2);
	
	//init players
	player_.init(primitive_builder_, world_);
	player2_.init(primitive_builder_, world_);

	//init variables
	powerup_state_ = cooldown;
	powerup_active_timer_ = 0;
	powerup_cooldown_timer_ = 0;
	zoom_timer_ = 0;
	zoomed_ = false;
	distance = 70;
	powerupTrans.SetIdentity();
	powerup_y_ = 2.5;

	//init shield materials
	mat = new gef::Material();
	mat2 = new gef::Material();

	//load player hats
	scene_asset_filename[1] = player_.loadHat();
	scene_asset_filename[2] = player2_.loadHat();

	//set loaded model meshes
	for (int i = 0; i < 6; i++)
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

	//init all grounds in parallel threads
	std::vector<thread*> threads;
	threads.push_back(new thread(&InGameState::InitGround, this, std::ref(ground_), ground_body_, -3, -3));
	threads.push_back(new thread(&InGameState::InitGround, this, std::ref(r_ground_), r_ground_body_, 3, -3));
	threads.push_back(new thread(&InGameState::InitGround, this, std::ref(m_ground_), m_ground_body_, 0, 1));
	threads.push_back(new thread(&InGameState::InitGround, this, std::ref(tl_ground_), tl_ground_body_, -7.5, -0.5));
	threads.push_back(new thread(&InGameState::InitGround, this, std::ref(tr_ground_), tr_ground_body_, 7.5, -0.5));
	
	//join threads
	for (auto thread : threads)
	{
		thread->join();
	}

	//setup matrices for transforming ground meshes
	gef::Matrix44 ground_rotation_;
	ground_rotation_.SetIdentity();
	ground_rotation_.RotationX(gef::DegToRad(-5));
	gef::Matrix44 ground_transform;
	ground_transform.SetIdentity();
	ground_transform.Scale(gef::Vector4(0.35, 0.75, 1));
	ground_transform.SetTranslation(gef::Vector4(5, 0, 0));

	//transform ground meshes
	ground_transform.SetTranslation(gef::Vector4(0, 0, 1.5));
	m_ground_.set_transform(m_ground_.transform()* ground_transform * ground_rotation_);

	ground_transform.SetTranslation(gef::Vector4(-2.05, -1.25, 1.5));
	ground_.set_transform(ground_.transform()* ground_transform* ground_rotation_);

	ground_transform.SetTranslation(gef::Vector4(2.05, -1.25, 1.5));
	r_ground_.set_transform(r_ground_.transform()* ground_transform* ground_rotation_);

	ground_transform.SetTranslation(gef::Vector4(-4.9, -0.45, 1.5));
	tl_ground_.set_transform(tl_ground_.transform()* ground_transform* ground_rotation_);

	ground_transform.SetTranslation(gef::Vector4(4.9, -0.45, 1.5));
	tr_ground_.set_transform(tr_ground_.transform()* ground_transform* ground_rotation_);

	//position background
	gef::Matrix44 bg_rotation_;
	bg_rotation_.RotationY(gef::DegToRad(90));
	bg_.Scale(gef::Vector4(0.1, 0.1, 0.1, 0.1));
	bg_.SetTranslation(gef::Vector4(-5, -10, 0, 0));
	mesh_instance_[0].set_transform(bg_* bg_rotation_);

	//set bg colour
	platform_.set_render_target_clear_colour(gef::Colour(1.0f, 0.787f, 0.435f, 1.0f));

	//load shield textures
	shield_green_ = CreateTextureFromPNG("shield_green.png", platform_);
	shield_yellow_ = CreateTextureFromPNG("shield_yellow.png", platform_);
}

void InGameState::onExit(Player& player_, Player& player2_, gef::AudioManager* audio_manager_)
{
	//clean up data
	player_.cleanUp();
	player2_.cleanUp();

	audio_manager_->UnloadMusic();
	audio_manager_->UnloadAllSamples();

	delete shield_green_;
	delete shield_yellow_;

	delete mat;
	delete mat2;

	delete world_;

	for (int i = 0; i < 6; i++)
	{
		delete loaded_meshes_[i];
	}
}

gef::Scene* InGameState::LoadSceneAssets(gef::Platform& platform, const char* filename)
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

gef::Mesh* InGameState::GetMeshFromSceneAssets(gef::Scene* scene)
{
	gef::Mesh* mesh = NULL;

	// if the scene data contains at least one mesh
	// return the first mesh
	if (scene && scene->meshes.size() > 0)
		mesh = scene->meshes.front();

	return mesh;
}

gef::Texture* InGameState::CreateTextureFromPNG(const char* png_filename, gef::Platform& platform)
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

void InGameState::InitGround(GameObject& ground, b2Body* body, float xpos, float ypos)
{
	gef::Vector4 ground_half_dimensions(2.0f, 0.25f, 0.5f);

	//set the mesh for the ground
	ground.set_mesh(mesh_instance_[5].mesh());

	// create a physics body
	b2BodyDef body_def;
	body_def.type = b2_staticBody;
	body_def.position = b2Vec2(xpos, ypos);

	body_def.userData.pointer = reinterpret_cast<uintptr_t>(&ground);

	body = world_->CreateBody(&body_def);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(ground_half_dimensions.x(), ground_half_dimensions.y());

	// create the fixture
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;

	// create the fixture on the rigid body
	body->CreateFixture(&fixture_def);

	//set type
	ground.set_type(GROUND);

	// update visuals from simulation data
	ground.UpdateFromSimulation(body);
}
