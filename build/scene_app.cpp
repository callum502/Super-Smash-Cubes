#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <input/sony_controller_input_manager.h>

SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	input_manager_(NULL),
	audio_manager_(NULL),
	font_(NULL),
	sfx_id_(-1),
	theIntroState(platform),
	theInstructionsState(platform),
	theMenuState(platform),
	theInGameState(platform),
	theGameOverState(platform)
{
}

void SceneApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);

	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);

	// initialise input manager
	input_manager_ = gef::InputManager::Create(platform_);

	// initialise audio manager
	audio_manager_ = gef::AudioManager::Create();

	theIntroState.onEnter(primitive_builder_, audio_manager_, player_, player2_);
	InitFont();
	SetupLights();
}

void SceneApp::CleanUp()
{
	CleanUpFont();

	delete audio_manager_;
	audio_manager_ = NULL;

	delete input_manager_;
	input_manager_ = NULL;

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool SceneApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	input_manager_->Update();

	const gef::Keyboard* keyboard = input_manager_->keyboard();


	//allow player to adjust volume
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_EQUALS)))
	{
		if (volume < 100)
		{
			volume += 60 * frame_time;
		}
	}
	if ((keyboard->IsKeyDown(gef::Keyboard::KeyCode::KC_MINUS)))
	{
		if (volume>0)
		{
			volume -= 60 * frame_time;
		}
	}
	audio_manager_->SetMasterVolume(volume);

	//update current state
	pCurrentState->Update(frame_time, &pCurrentState, &theInstructionsState, &theMenuState,  &theInGameState, &theGameOverState, primitive_builder_, input_manager_, audio_manager_, player_, player2_);

	return true;
}

void SceneApp::Render()
{
	// setup camera

	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 200.0f);
	renderer_3d_->set_projection_matrix(projection_matrix);

	// view
	gef::Vector4 camera_eye(0.0f, 1.0f, 15.0f);
	gef::Vector4 camera_lookat(0.0f, 0.0f, 0.0f);
	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_view_matrix(view_matrix);

	//render current state
	renderer_3d_->Begin();
	sprite_renderer_->Begin();
	pCurrentState->Draw(sprite_renderer_, font_, renderer_3d_, primitive_builder_, player_, player2_);
	DrawHUD();
	sprite_renderer_->End();
	renderer_3d_->End();
}

void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void SceneApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(platform_.width() * 0.94, platform_.height() * 0.95, 0.f), platform_.height() * 0.002f, 0xffffffff, gef::TJ_CENTRE, "FPS: %.1f", fps_);
	}
}

void SceneApp::SetupLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.25f, 0.25f, 0.25f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}


