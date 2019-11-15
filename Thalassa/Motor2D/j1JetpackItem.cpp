#include "j1JetpackItem.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Collisions.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"
#include "j1Map.h"
#include "j1Scene1.h"

j1JetpackItem::j1JetpackItem(int x, int y, ENTITY_TYPE type) : j1Entity(x, y, ENTITY_TYPE::JETPACK_ITEM)
{
	idleAnim.LoadAnimations("bubbleDestroy");
	destroyAnim.LoadAnimations("bubbleIdle");
}

j1JetpackItem::~j1JetpackItem()
{
}

bool j1JetpackItem::Start()
{
	sprites = App->tex->Load("textures/Particles/bubble.png");

	/*position = { 50,150 };*/

	animation = &idleAnim;

	collider = App->collisions->AddCollider({ (int)position.x + 2, (int)position.y + 2, 12, 12 }, COLLIDER_ITEM, App->entity_manager);

	return true;
}

bool j1JetpackItem::Update(float dt)
{

	BlitEntity(animation->GetCurrentFrame(dt), false);

	return true;
}

bool j1JetpackItem::PostUpdate()
{
	return true;
}

bool j1JetpackItem::CleanUp()
{
	return true;
}

void j1JetpackItem::OnCollision(Collider * c1, Collider * c2)
{
	if (c1->type == COLLIDER_PLAYER) {
		if (c2->type == COLLIDER_ITEM)
		{
			animation = &destroyAnim;
			c2->to_delete = true;
		}
	}
}
