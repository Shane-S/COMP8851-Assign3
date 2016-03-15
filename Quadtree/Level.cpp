#include "Level.h"
#include "GameScreen.h"
#include "Actor.h"

using std::shared_ptr;

SDL2pp::Point Level::GetLevelSize() const
{
	return _levelSize;
}
void Level::SetLevelSize(SDL2pp::Point newSize)
{
    _levelSize = newSize;
}

void Level::AddActor(std::shared_ptr<Actor> actor)
{
	_actorCounts[actor->GetType()]++;
	_actorsToAdd.push_back(actor);
}

std::vector<std::shared_ptr<Actor>>& Level::GetActors()
{
	return _actors;
}

void Level::Update(double deltaTime)
{
	// Add any actors added during the update
	_actors.insert(_actors.end(), _actorsToAdd.begin(), _actorsToAdd.end());
	_actorsToAdd.clear();

	if (_actors.empty()) return;

	// Clean up all destroyed actors
	auto destroyedFinder = [](shared_ptr<Actor> a) {return a->IsDestroyed(); };
	_actors.erase(std::remove_if(_actors.begin(), _actors.end(), destroyedFinder), _actors.end());
}
