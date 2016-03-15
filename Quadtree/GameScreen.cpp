#include <limits>
#include <random>
#include "GameScreen.h"
#include "GameManager.h"
#include "SquareActor.h"
#include "Vector2.h"

#define MAX_SPEED 400

std::shared_ptr<Level> GameScreen::GetLevel() const
{
	return _level;
}

int GameScreen::Load()
{
	_mgr = GameManager::GetInstance();

	SDL2pp::Point viewportSize = _mgr->GetWindow().GetSize();

    // Load level one in order to render
    _level = std::shared_ptr<Level>(new Level(viewportSize));
    SDL2pp::Point levelSize = _level->GetLevelSize();

	_camera = new Camera(SDL2pp::Point(0, 0), viewportSize, _level->GetLevelSize());

    std::shared_ptr<SDL2pp::Texture> square(new SDL2pp::Texture(_mgr->GetRenderer(), "./Assets/Square.png"));
    for (int i = 0; i < 100; ++i)
    {
        std::unordered_map<std::string, std::shared_ptr<SpriteSheet>> sprites;
        sprites["square"] = std::shared_ptr<SpriteSheet>(new SpriteSheet(square, 1, std::numeric_limits<double>::infinity(), false));
        Vector2 speed(std::rand() % MAX_SPEED, std::rand() % MAX_SPEED);
        Vector2 pos(std::rand() % (viewportSize.x - sprites["square"]->GetFrameWidth()), std::rand() % (viewportSize.y - sprites["square"]->GetFrameHeight()));
        std::shared_ptr<Actor> actor(new SquareActor(pos, *_mgr, speed, sprites, "square", SpriteSheet::XAxisDirection::RIGHT, SpriteSheet::YAxisDirection::UP));
        _level->AddActor(actor);
    }

    // Inialize the quadtree
    //_levelQuadtree = std::make_shared<Quadtree>();
    //_levelQuadtree->Initialize(4, levelSize.GetX(), levelSize.GetY());

	return SCREEN_LOAD_SUCCESS;
}

int GameScreen::Update(double elapsedSecs)
{
	SDL_PumpEvents();
	_mgr->inputManager->UpdateKeyboardState();

    if(_paused) return SCREEN_CONTINUE;
    
	// Check if the player pauses the game and handle the pause
	if (_mgr->inputManager->ActionOccurred("QUIT", Input::Pressed))
    {
        return SCREEN_FINISH;
    }

	for (auto actor : _level->GetActors())
	{
		// A bit hacky, to be sure
		if (actor->GetType() == Actor::Type::player) continue;
		actor->Update(elapsedSecs);
	}

    // Naive collision detection for now
    for (auto it = _level->GetActors().begin(); it != _level->GetActors().end(); it++)
    {
        for (auto it2 = it + 1; it2 != _level->GetActors().end(); it2++)
        {
            if ((*it)->GetAABB().CheckCollision((*it2)->GetAABB()))
            {
                (*it)->OnOverlap(**it2);
                (*it2)->OnOverlap(**it);
            }
        }
    }

	_level->Update(elapsedSecs);
	return SCREEN_CONTINUE;
}

void GameScreen::Draw()
{
	SDL2pp::Renderer& rend = _mgr->GetRenderer();
	rend.SetDrawColor(100, 100, 100, 255);
	rend.Clear();

	// Draw all actors
	for (auto actor : _level->GetActors())
	{
		actor->Draw(*_camera);
	}

	rend.Present();
}

void GameScreen::Unload()
{
	_paused = false;
	delete _camera;
}
