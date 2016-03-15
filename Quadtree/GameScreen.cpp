#include <limits>
#include <random>
#include "GameScreen.h"
#include "GameManager.h"
#include "SquareActor.h"
#include "Vector2.h"

#define MAX_SPEED 1

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

    // Generate a bunch of squares with random speeds and start positions
    std::shared_ptr<SDL2pp::Texture> square(new SDL2pp::Texture(_mgr->GetRenderer(), "./Assets/Square.png"));
    for (int i = 0; i < 11; ++i)
    {
        std::unordered_map<std::string, std::shared_ptr<SpriteSheet>> sprites;
        sprites["square"] = std::shared_ptr<SpriteSheet>(new SpriteSheet(square, 1, std::numeric_limits<double>::infinity(), false));
        Vector2 speed(std::rand() % MAX_SPEED, std::rand() % MAX_SPEED);
        Vector2 pos(std::rand() % (viewportSize.x - sprites["square"]->GetFrameWidth()), std::rand() % (viewportSize.y - sprites["square"]->GetFrameHeight()));
        std::shared_ptr<Actor> actor(new SquareActor(pos, *_mgr, speed, sprites, "square", SpriteSheet::XAxisDirection::RIGHT, SpriteSheet::YAxisDirection::UP));
        _level->AddActor(actor);
    }

    _quadtree = std::shared_ptr<Quadtree>(new Quadtree(1, SDL2pp::Rect(SDL2pp::Point(0, 0), viewportSize)));

    _font = std::shared_ptr<SDL2pp::Font>(new SDL2pp::Font("./Assets/BEBAS.ttf", 45));

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

    if (_mgr->inputManager->ActionOccurred("TOGGLE_QUAD", Input::Pressed))
    {
        _usingQuadtree = !_usingQuadtree;
    }

	for (auto actor : _level->GetActors())
	{
		// A bit hacky, to be sure
		if (actor->GetType() == Actor::Type::player) continue;
		actor->Update(elapsedSecs);
	}

    if (_usingQuadtree)
        QuadtreeCollisionDetection();
    else
        NaiveCollisionDetection();

	_level->Update(elapsedSecs);
	return SCREEN_CONTINUE;
}

void GameScreen::QuadtreeCollisionDetection()
{
    _quadtree->clear();
    for (auto actor : _level->GetActors())
    {
        _quadtree->insert(actor);
    }
    std::vector<std::shared_ptr<Actor>> tempList;
    tempList.reserve(_level->GetActors().size() / 2);
    for (auto & actor : _level->GetActors())
    {
        _quadtree->retrieve(tempList, actor);
        for (auto & retrieved : tempList)
        {
            if (actor->GetAABB().CheckCollision(retrieved->GetAABB()))
            {
                actor->OnOverlap(*retrieved);
                retrieved->OnOverlap(*actor);
            }
        }
        tempList.clear();
    }
}

void GameScreen::NaiveCollisionDetection()
{
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
}

void GameScreen::Draw()
{
    std::string text = _usingQuadtree ? "Quadtree" : "Naive";
    SDL2pp::Texture quadtreeUsageMsg(_mgr->GetRenderer(), _font->RenderText_Solid(text, SDL_Color{ 0, 0, 0, 255 }));

	SDL2pp::Renderer& rend = _mgr->GetRenderer();
	rend.SetDrawColor(79, 206, 229, 255);
	rend.Clear();

	// Draw all actors
	for (auto actor : _level->GetActors())
	{
		actor->Draw(*_camera);
	}
    _quadtree->draw(_mgr);
    rend.Copy(quadtreeUsageMsg, SDL2pp::NullOpt, SDL2pp::Point(5, 5));
	rend.Present();
}

void GameScreen::Unload()
{
	_paused = false;
	delete _camera;
}
