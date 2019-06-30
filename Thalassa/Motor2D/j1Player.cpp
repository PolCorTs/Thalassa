#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Player.h"
#include "j1Scene1.h"
#include "j1Collisions.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"


j1Player::j1Player(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::PLAYER) 
{
	animation = NULL;

	idle.LoadAnimations("idle");
	jetpack.LoadAnimations("jetpack");

	//idle.loop = true;
	//idle.speed = 0.008f;
	//idle.PushBack({ 4,36,13,21 });
	//idle.PushBack({ 24,36,13,21 });

	//run.loop = true;
	//run.speed = 0.025f;
	//run.PushBack({ 64,37,13,21 });
	//run.PushBack({ 3,60,14,21 });
	//run.PushBack({ 44,60,13,21 });
	//run.PushBack({ 24,60,13,21 });
	//run.PushBack({ 24,60,13,21 });
	//run.PushBack({ 44,60,13,21 });
	//run.PushBack({ 3,60,14,21 });

	//jetpack.loop = true;
	//jetpack.speed = 0.01f;
	//jetpack.PushBack({ 4,8,14,23 });
	//jetpack.PushBack({ 24,8,14,23 });

	//jump.loop = false;
	//jump.speed = 0.001f;
	////jump.PushBack({ 3,112,15,17 });
	////jump.PushBack({ 23,112,15,17 });
	////jump.PushBack({ 43,112,15,17 });
	////jump.PushBack({ 23,112,15,17 });
	////jump.PushBack({ 3,112,15,17 });
	//jump.PushBack({ 24,87,13,18 });
}

j1Player::~j1Player() {}

bool j1Player::Start() {

	sprites = App->tex->Load("textures/Character_Spritesheet.png");

	position = { 0,0 };
	speed = 0.3f;
	animation = &idle;
	playerCreated = true;

	collider = App->collisions->AddCollider({ (int)position.x + 3, (int)position.y, 12, 20 }, COLLIDER_PLAYER, App->entity_manager);

	return true;
}

bool j1Player::PreUpdate() {

	return true;
}

bool j1Player::Update(float dt) {

	if (playerCreated)
	{
		animation = &idle;

		if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT) {
			position.x += speed;
			animation = &run;
			flip = true;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT) {
			position.x -= speed;
			animation = &run;
			flip = false;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT) {
			position.y -= speed;
			animation = &idle;
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT) {
			position.y += speed;
			animation = &idle;
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == j1KeyState::KEY_REPEAT) {
			//position.y += speed;
			animation = &jump;
		}
	}

	// update collider 
	if (collider != nullptr)
		collider->SetPos(position.x, position.y);

	BlitEntity(animation->GetCurrentFrame(), flip, position.x, position.y);
	LOG("player blitted");

	return true;
}

bool j1Player::PostUpdate() {

	return true;
}

bool j1Player::Load(pugi::xml_node& data) {

	position.x = data.child("position").attribute("x").as_int();
	position.y = data.child("position").attribute("y").as_int();

	return true;
}

bool j1Player::Save(pugi::xml_node& data) const {

	pugi::xml_node player = data.append_child("position");

	player.append_attribute("x") = position.x;
	player.append_attribute("y") = position.y;

	return true;
}

bool j1Player::CleanUp() {
	LOG("Freeing the player");

	return true;
}