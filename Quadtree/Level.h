#pragma once
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include "Actor.h"

/** Stores data for a level. It may be overkill, so chief refactorer may refactor if he wants to. */
class Level
{
public:

	/** Default constructor. */
	Level(): _levelSize(0, 0) {}

    Level(SDL2pp::Point size)
        : _levelSize(size) {}

	/**
	* Gets level  array size.
	*
	* @return	The level array size.
	*/
	SDL2pp::Point GetLevelSize() const;

    void SetLevelSize(SDL2pp::Point newSize);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	///<summary> Adds an actor to the level.</summary>
	///
	///<param name="actor"> The actor to add.</param>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void AddActor(std::shared_ptr<Actor> actor);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	///<summary> Gets the list of all actors.</summary>
	///
	///<returns> The list of actors in the level.</returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<std::shared_ptr<Actor>> & GetActors();

	/**
	 * Updates objects for the level
	 *
	 * @param	deltaTime	Time since last frame.
	 */
	void Update(double deltaTime);
private:
	/** Stores the list of every actor in the level. */
	std::vector<std::shared_ptr<Actor>> _actors;

	/** Stores a list of actors to add at the end of the current update. */
	std::vector<std::shared_ptr<Actor>> _actorsToAdd;

	/** Stores the number of each different type of actor. */
	std::unordered_map<Actor::Type, size_t> _actorCounts;

    SDL2pp::Point _levelSize;
};

