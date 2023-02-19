#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>
#include <input/keyboard.h>
#include <primitive_builder.h>
#include <list>

namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

enum OBJECT_TYPE
{
	PLAYER,
	ENEMY,
	BULLET,
	POWERUP,
	GROUND
};

class GameObject : public gef::MeshInstance
{
public:
	void UpdateFromSimulation(const b2Body* body);
	void MyCollisionResponse();

	inline void set_type(OBJECT_TYPE type) { type_ = type; }
	inline OBJECT_TYPE type() { return type_; }
	float timer;
	bool delete_ = false;
private:
	OBJECT_TYPE type_;
};

#endif // _GAME_OBJECT_H