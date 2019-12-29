#include "j1Hud.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1UIElement.h"
#include "j1Label.h"
#include "j1Scene1.h"
#include "j1Fonts.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Input.h"

j1Hud::j1Hud()
{
	animation = NULL;
	sprites = nullptr;

	idle.LoadAnimations("coinIdle");
}

j1Hud::~j1Hud() {}

bool j1Hud::Start()
{
	text = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

	sprites = App->tex->Load("textures/coin.png");
	lives_tex = App->tex->Load("textures/life.png");

	seconds = App->gui->CreateLabel(&labels_list, LABEL, 540, 0, text, time_text.GetString());
	minutes = App->gui->CreateLabel(&labels_list, LABEL, 450, 0, text, "00:");

	score = { "%i", App->entity_manager->player->score };
	score_points = { "%i", App->entity_manager->player->current_points };


	if (App->scene1->active)
		coins_label = App->gui->CreateLabel(&labels_list, LABEL, 80, 700, text, score.GetString(), { 255, 255, 255, 255 });


	score_label = App->gui->CreateLabel(&labels_list, LABEL, 900, 0, text, score_points.GetString(), { 236, 151, 0, 255 });

	animation = &idle;

	return true;
}

bool j1Hud::Update(float dt)
{

	App->tex->UnLoad(seconds->sprites);
	seconds->sprites = App->font->Print(time_text.GetString(), seconds->color, seconds->font);

	if (seconds->sprites != nullptr)
		seconds->Draw(1.0f, 0, 0, false);
	if (minutes->sprites != nullptr)
		minutes->Draw(1.0f, 0, 0, false);
	if (App->gui->debug)
		App->render->DrawQuad({ 450, 0, 160, 64 }, 255, 0, 0, 255, false, false);

	//COIN
	SDL_Rect r = animation->GetCurrentFrame(dt);
	App->render->Blit(sprites, 3, 700, &r, SDL_FLIP_NONE, 1.0f, 1, 0, INT_MAX, INT_MAX);
	if (App->gui->debug)
		App->render->DrawQuad({ 3, 700, r.w * 4, r.h * 4 }, 255, 0, 0, 255, false, false);

	//PLAYER LIVES
	int space = 0;

	for (int i = App->entity_manager->player->lifes; i >= 0; --i)
	{
		SDL_Rect r = { 0,0,8,11 };
		App->render->Blit(lives_tex, 10 + space, 3, &r, SDL_FLIP_NONE, 1.0f, 1, 0, INT_MAX, INT_MAX);
		if (App->gui->debug)
			App->render->DrawQuad({ 10 + space, 3, r.w * 4, r.h * 4 }, 255, 0, 0, 255, false, false);

		space += 50;
	}

	//LABELS
	score = { "%i", App->entity_manager->player->score };
	if (coins_label != nullptr)
	{
		App->tex->UnLoad(coins_label->sprites);
		coins_label->sprites = App->font->Print(score.GetString(), coins_label->color, coins_label->font);
		if (coins_label->sprites != nullptr)
			coins_label->Draw(1.0f, 0, 0, false);

		if (App->gui->debug)
			App->render->DrawQuad({ 80, 700, 64, 64 }, 255, 0, 0, 255, false, false);
	}

	score_points = { "%i", App->entity_manager->player->current_points };
	if (score_label != nullptr)
	{
		App->tex->UnLoad(score_label->sprites);
		score_label->sprites = App->font->Print(score_points.GetString(), score_label->color, score_label->font);
		if (score_label->sprites != nullptr)
			score_label->Draw(1.0f, 0, 0, false);
		if (App->gui->debug)
			App->render->DrawQuad({ 900, 0, 124, 64 }, 255, 0, 0, 255, false, false);
	}

	return true;
}

bool j1Hud::CleanUp()
{
	for (p2List_item<j1Label*>* item = labels_list.start; item != nullptr; item = item->next)
	{
		labels_list.del(item);
	}

	App->tex->UnLoad(sprites);
	App->tex->UnLoad(lives_tex);

	return true;
}

bool j1Hud::Load(pugi::xml_node & data)
{
	pugi::xml_node hud = data.child("player").child("hud");

	App->entity_manager->player->current_points = hud.child("score").attribute("value").as_uint();
	App->entity_manager->player->score = hud.child("coins_score").attribute("value").as_uint();

	return true;
}

bool j1Hud::Save(pugi::xml_node & data) const
{
	pugi::xml_node hud = data;

	hud.append_child("coins_score").append_attribute("value") = App->entity_manager->player->score;
	//hud.append_child("score").append_attribute("value") = App->entity_manager->player->current_points;

	return true;
}
