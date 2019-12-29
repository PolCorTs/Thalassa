#include "j1MenuScene.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene1.h"
#include "j1EntityManager.h"
#include "j1Player.h"
#include "j1Bat.h"
#include "j1Platform.h"
#include "j1TransitionsManager.h"
#include "j1Collisions.h"
#include "p2Animation.h"
#include "SDL_mixer/include/SDL_mixer.h"
#include "j1Pathfinding.h"
#include "SDL/include/SDL_mouse.h"
#include "j1Button.h"
#include "j1UIElement.h"
#include "j1Label.h"
#include "j1Box.h"
#include "j1Gui.h"
#include "j1Fonts.h"

#include "Brofiler/Brofiler.h"

j1MenuScene::j1MenuScene()
{
	name.create("menu");

	player.LoadAnimations("idle");
	
}

j1MenuScene::~j1MenuScene() {}

bool j1MenuScene::Awake(pugi::xml_node &)
{
	LOG("Loading Menu");
	bool ret = true;

	return ret;
}

bool j1MenuScene::Start()
{
	if (active)
	{
		// The map is loaded
		App->map->Load("menu.tmx");

		// The audio is played
		//App->audio->PlayMusic("audio/music/menu_music.ogg", 1.0f);

		// Loading textures
		gui_tex = App->tex->Load("gui/buttons.png");
		logo_tex = App->tex->Load("gui/logo.png");
		player_tex = App->tex->Load("textures/character/character.png");

		// Loading fonts
		font = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

		// Creating UI		
		settings_window = App->gui->CreateBox(&menuBoxes, BOX, App->gui->settingsPosition.x, App->gui->settingsPosition.y, { 537, 0, 663, 712 }, gui_tex);
		settings_window->visible = false;

		// We will use it to check if there is a save file
		pugi::xml_document save_game;
		pugi::xml_parse_result result = save_game.load_file("save_game.xml");

		App->gui->CreateBox(&menuBoxes, BOX, App->gui->lastSlider1X, App->gui->slider1Y, { 416, 72, 28, 42 }, gui_tex, (j1UIElement*)settings_window, App->gui->minimum, App->gui->maximum);
		App->gui->CreateBox(&menuBoxes, BOX, App->gui->lastSlider2X, App->gui->slider2Y, { 416, 72, 28, 42 }, gui_tex, (j1UIElement*)settings_window, App->gui->minimum, App->gui->maximum);

		SDL_Rect idle = { 0, 143, 190, 49 };
		SDL_Rect hovered = { 0, 45, 190, 49 };
		SDL_Rect clicked = { 0, 94, 190, 49 };
		App->gui->CreateButton(&menuButtons, BUTTON, 80, 110, idle, hovered, clicked, gui_tex, PLAY_GAME);
		App->gui->CreateButton(&menuButtons, BUTTON, 80, 160, idle, hovered, clicked, gui_tex, OPEN_CREDITS);

		SDL_Rect idle4 = { 0, 699, 190, 49 };

		if (result == NULL)
			App->gui->CreateButton(&menuButtons, BUTTON, 80, 135, idle4, idle4, idle4, gui_tex, NO_FUNCTION);
		else
			App->gui->CreateButton(&menuButtons, BUTTON, 80, 135, idle, hovered, clicked, gui_tex, LOAD_GAME);

		SDL_Rect idle2 = { 28, 201, 49, 49 };
		SDL_Rect hovered2 = { 77, 201, 49, 49 };
		SDL_Rect clicked2 = { 126, 201, 49, 49 };
		App->gui->CreateButton(&menuButtons, BUTTON, 228, 3, idle2, hovered2, clicked2, gui_tex, CLOSE_GAME);
		App->gui->CreateButton(&menuButtons, BUTTON, 64, 135, idle2, hovered2, clicked2, gui_tex, CLOSE_SETTINGS, (j1UIElement*)settings_window);

		SDL_Rect idle3 = { 463, 109, 49, 49 };
		SDL_Rect hovered3 = { 463, 158, 49, 49 };
		SDL_Rect clicked3 = { 463, 207, 49, 49 };
		App->gui->CreateButton(&menuButtons, BUTTON, 3, 3, idle3, hovered3, clicked3, gui_tex, SETTINGS);

		App->gui->CreateLabel(&menuLabels, LABEL, 106, 115, font, "Start", App->gui->beige);
		App->gui->CreateLabel(&menuLabels, LABEL, 98, 165, font, "Credits", App->gui->beige);
		App->gui->CreateLabel(&menuLabels, LABEL, 90, 140, font, "Continue", App->gui->beige);
		App->gui->CreateLabel(&menuLabels, LABEL, 44, 9, font, "Settings", App->gui->brown, (j1UIElement*)settings_window);
		App->gui->CreateLabel(&menuLabels, LABEL, 30, 50, font, "Sound", App->gui->brown, (j1UIElement*)settings_window);
		App->gui->CreateLabel(&menuLabels, LABEL, 30, 89, font, "Music", App->gui->brown, (j1UIElement*)settings_window);

		player_created = false;
	}

	return true;
}

bool j1MenuScene::PreUpdate()
{
	BROFILER_CATEGORY("MenuPreUpdate", Profiler::Color::Orange)
		return true;
}

bool j1MenuScene::Update(float dt)
{
	BROFILER_CATEGORY("MenuUpdate", Profiler::Color::LightSeaGreen)

		// ---------------------------------------------------------------------------------------------------------------------
		// USER INTERFACE MANAGEMENT
		// ---------------------------------------------------------------------------------------------------------------------	

		// Updating the state of the UI
		App->gui->UpdateButtonsState(&menuButtons);
	App->gui->UpdateSliders(&menuBoxes);
	App->gui->UpdateWindow(settings_window, &menuButtons, &menuLabels, &menuBoxes);

	// Button actions
	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
		if (item->data->visible) {
			switch (item->data->state)
			{
			case IDLE:
				item->data->situation = item->data->idle;
				break;

			case HOVERED:
				item->data->situation = item->data->hovered;
				break;

			case RELEASED:
				item->data->situation = item->data->idle;
				if (item->data->bfunction == PLAY_GAME) {
					startGame = true;
		
				}
				else if (item->data->bfunction == LOAD_GAME) {
					loadGame = true;
				
				}
				else if (item->data->bfunction == CLOSE_GAME) {
					continueGame = false;
				}
				else
					if ((item->data->bfunction == SETTINGS && !settings_window->visible)
						|| (item->data->bfunction == CLOSE_SETTINGS && settings_window->visible)) {
						settings_window->visible = !settings_window->visible;
						settings_window->position = App->gui->settingsPosition;

						for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
							if (item->data->parent == settings_window) {
								item->data->visible = !item->data->visible;
								item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
								item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
							}
						}
						for (p2List_item<j1Label*>* item = menuLabels.start; item != nullptr; item = item->next) {
							if (item->data->parent == settings_window) {
								item->data->visible = !item->data->visible;
								item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
								item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
							}
						}
						for (p2List_item<j1Box*>* item = menuBoxes.start; item != nullptr; item = item->next) {
							if (item->data->parent == settings_window) {
								item->data->visible = !item->data->visible;
								item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
								item->data->position.y = settings_window->position.y + item->data->initialPosition.y;

								item->data->minimum = item->data->originalMinimum + settings_window->position.x;
								item->data->maximum = item->data->originalMaximum + settings_window->position.x;
							}
						}
					}
				break;

			case CLICKED:
				item->data->situation = item->data->clicked;
				break;
			}
		}
	}


	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	// Blitting background and animations
	App->map->Draw(0);

	SDL_Rect p = player.GetCurrentFrame(dt);
	App->render->Blit(player_tex, 40, 105, &p, SDL_FLIP_NONE);
	SDL_Rect h = harpy.GetCurrentFrame(dt);
	App->render->Blit(harpy_tex, 205, 35, &h, SDL_FLIP_HORIZONTAL);

	// Blitting the buttons and labels of the menu
	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
		if (item->data->parent != nullptr) continue;
		item->data->Draw(App->gui->buttonsScale);
	}
	for (p2List_item<j1Label*>* item = menuLabels.start; item != nullptr; item = item->next) {
		if (item->data->parent != nullptr) continue;
		if (item->data->visible) item->data->Draw();
	}

	// Blitting the logo
	SDL_Rect logo = { 166, 139, 711, 533 };
	App->render->Blit(logo_tex, 54, 0, &logo, SDL_FLIP_NONE, 1.0f, App->gui->logoScale);

	//Blitting the debug
	if (App->gui->debug) {
		App->render->DrawQuad({ Uint8(3 / 0.25), Uint8(3 / 0.25),
			Uint8(49 / App->gui->buttonsScale), Uint8(49 / App->gui->buttonsScale) }, 255, 0, 0, 255, false, false);
		App->render->DrawQuad({ Uint8(228 / 0.25) * 6 + 47, Uint8(3 / 0.25),
			Uint8(49 / App->gui->buttonsScale), Uint8(49 / App->gui->buttonsScale) }, 255, 0, 0, 255, false, false);
		App->render->DrawQuad({ 320, 440, 380, Uint8(49 / App->gui->buttonsScale) }, 255, 0, 0, 255, false, false);
		App->render->DrawQuad({ 320, 540, 380, Uint8(49 / App->gui->buttonsScale) }, 255, 0, 0, 255, false, false);
		App->render->DrawQuad({ 320, 640, 380, Uint8(49 / App->gui->buttonsScale) }, 255, 0, 0, 255, false, false);
	}

	// Blitting settings window
	if (settings_window != nullptr && settings_window->visible == true)
		settings_window->Draw(App->gui->settingsWindowScale);

	// Blitting the buttons, labels and boxes (sliders) of the window
	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
		if (item->data->parent == nullptr) continue;

		if (item->data->parent->visible == false)
			item->data->visible = false;
		else
			item->data->Draw(App->gui->buttonsScale);

	}
	for (p2List_item<j1Label*>* item = menuLabels.start; item != nullptr; item = item->next) {
		if (item->data->parent == nullptr) continue;

		if (item->data->parent->visible == false)
			item->data->visible = false;
		else {
			if (item->data->text == "Settings")
				item->data->Draw();
			else
				item->data->Draw(App->gui->buttonsScale);
		}
	}
	for (p2List_item<j1Box*>* item = menuBoxes.start; item != nullptr; item = item->next) {
		if (item->data->parent == nullptr) continue;

		if (item->data->parent->visible == false)
			item->data->visible = false;
		else
			item->data->Draw(App->gui->buttonsScale);
	}

	return true;
}

bool j1MenuScene::PostUpdate()
{
	BROFILER_CATEGORY("MenuPostUpdate", Profiler::Color::Yellow)

		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
			continueGame = false;

	return continueGame;
}

bool j1MenuScene::CleanUp()
{
	LOG("Freeing all textures");
	App->tex->UnLoad(logo_tex);
	App->tex->UnLoad(harpy_tex);
	App->tex->UnLoad(player_tex);
	App->tex->UnLoad(gui_tex);

	App->map->CleanUp();
	App->tex->CleanUp();

	for (p2List_item<j1Button*>* item = menuButtons.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		menuButtons.del(item);
	}

	for (p2List_item<j1Label*>* item = menuLabels.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		menuLabels.del(item);
	}

	for (p2List_item<j1Box*>* item = menuBoxes.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		menuBoxes.del(item);
	}

	delete settings_window;
	if (settings_window != nullptr) settings_window = nullptr;

	return true;
}

bool j1MenuScene::Load(pugi::xml_node& node)
{
	pugi::xml_node activated = node.child("activated");

	bool scene_activated = activated.attribute("true").as_bool();

	if ((scene_activated == false) && active)
		ChangeScene(SCENE1);
	else
		ChangeScene(SCENE2);

	return true;
}

void j1MenuScene::ChangeScene(SCENE objectiveScene)
{
	if (!player_created)
	{
		this->active = false;
		startGame = false;
		loadGame = false;
		openCredits = false;

		CleanUp();

	}
	else {
			if (objectiveScene == SCENE::SCENE1) {
				App->scene1->active = true;
				App->scene1->Start();
			}
			App->entity_manager->active = true;
			App->entity_manager->CreateEntity(PLAYER);
			App->entity_manager->Start();
	}
}
