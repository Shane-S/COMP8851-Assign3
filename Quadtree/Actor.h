#ifndef ACTOR_H
#define ACTOR_H

#include <SDL2pp\SDL2pp.hh>
#include <string>
#include <memory>
#include <unordered_map>
#include "Vector2.h"
#include "AABB.h"
#include "GameManager.h"
#include "Camera.h"
#include "SpriteSheet.h"

class GameScreen;
class Level;

class Actor
{
public:

   /**
	* Contains the actor's current kinematic properties (position, velocity, rotation and angular speed in radians).
	*/
	struct KinematicState
	{
		KinematicState()
			: position(0, 0), velocity(0, 0), rotation(0), angular(0) {}

		KinematicState(Vector2 startPos, Vector2 startVel, float startRot = 0.f, float startAng = 0.f)
			: position(startPos), velocity(startVel), rotation(startRot), angular(startAng) {}

        Vector2 position;
		Vector2 velocity;

		float rotation;
		float angular;
	};

	enum Edge {
		TOP,
		BOTTOM,
		RIGHT,
		LEFT,
		NONE
	};

	enum Type {
		npc,
		player,
		boss,
		door,
		object,
		projectile,
		turret,
		toggle,
		enemy
	};

	/**
	 * Creates a new Actor, which has a position and the potential to be drawn in the game world.
	 *
	 * @param	position		starting position of the actor.
	 * @param [in,out]	manager A reference to the game manager.
	 * @param	spd				The actor's starting velocity.
	 * @param	sprites			A map of names to sprite sheets.
	 * @param	startSprite		The name of the sprite to show by default.
	 * @param	startXDirection	The direction in the x axis which the actor will face at the start.
	 * @param	startYDirection	The direction in the y axis which the actor will face at the start.
	 */
	Actor(Vector2 position, GameManager & manager, Vector2 spd, std::unordered_map<std::string, std::shared_ptr<SpriteSheet>>& sprites, const std::string&& startSprite,
			SpriteSheet::XAxisDirection startXDirection, SpriteSheet::YAxisDirection startYDirection);

	/** Destructor. */
	~Actor();

   /**
	* Return AABB of the agent
	*
	* @return _aabb
	*/
	AABB GetAABB();

	/**
	 * Returns in the textures needed to render the AI.
	 *
	 * @return	null if it fails, else the texture.
	 */
	std::shared_ptr<SpriteSheet> GetTexture();

	// Returns position of the agent.
    Vector2 GetPosition();

	/**
	 * Gets the speed.
	 *
	 * @return	The speed.
	 */
	Vector2 GetSpeed();

	/**
	 * Gets the health.
	 *
	 * @return	The health.
	 */
	size_t GetHealth();

	/**
	 * Sets a speed.
	 *
	 * @param	spd	The speed.
	 */
	void SetSpeed(Vector2 spd);

	/**
	 * Sets a health.
	 *
	 * @param	heath	The health.
	 */
	void SetHealth(size_t health);

	/**
	 * Sets a position.
	 *
	 * @param	pos	The position.
	 */
	void SetPosition(Vector2 pos);

	/**
	 * Gets actor direction.
	 *
	 * @return	The actor direction.
	 */
	SpriteSheet::XAxisDirection GetActorXDirection() const;

	/**
	 * @brief	Gets actor y coordinate direction.
	 *
	 * @author	Shane
	 * @date	2/7/2016
	 *
	 * @return	The actor y coordinate direction.
	 */
	SpriteSheet::YAxisDirection GetActorYDirection() const;

	/**
	 * Sets actor direction.
	 *
	 * @param	dir	The direction.
	 */
	void SetActorXDirection(SpriteSheet::XAxisDirection dir);

	/**
	* Sets actor direction.
	*
	* @param	dir	The direction.
	*/
	void SetActorYDirection(SpriteSheet::YAxisDirection dir);

	/**
	 * @brief	Sets whether this actor is visible.
	 *
	 * @author	Shane
	 * @date	2/8/2016
	 *
	 * @param	isVisible	true if this object is visible.
	 */
	void SetVisibility(bool isVisible);

	/**
	 * @brief	Query if this object is visible.
	 *
	 * @author	Shane
	 * @date	2/8/2016
	 *
	 * @return	true if visible, false if not.
	 */
	bool IsVisible() const;

   /**
	* @brief	Query if this object is destroyed.
	*
 	* @return	true if destroyed, false if not.
	*/
	bool IsDestroyed() const;

	/**
	 * @brief	Destroy this object.
	 */
	void Destroy();

	// All the state changing stuff happens in here. 
	virtual void Update(double elapsedSecs);

	// Updates position of the agent by adding _speed to it.
	virtual void UpdatePosition(double elapsedSecs);

	virtual Type GetType() const = 0;

	/**
	 * Draws.
	 *
	 * @param [in,out]	camera	ref to the camera.
	 * 
	 * Draws this object.
	 */
	virtual void Draw(Camera& camera);

	/**
	* Resets the actor.
	*
	* @param [in,out]	Position to reset at.
	*
	*/
	virtual void Reset(Vector2 pos);
    
    /**
     * Called when this actor overlaps with another.
     * @param other The actor with which this actor overlapped.
     */
    virtual void OnOverlap(Actor& other);

protected:

	// TEMPORARY struct to hold actor bounds while I figure out something more elegant (it's probably going to stay forever though)
	struct Bounds {
		double rightBound, leftBound, topBound, bottomBound;
		int rightCol, leftCol, topRow, bottomRow;
	};

	/** The health. */
	size_t _health;

	KinematicState _curKinematic;
	KinematicState _prevKinematic;

    SDL_Color _colourMod;

	AABB _aabb;

	std::string _currentSpriteSheet;	
	std::unordered_map<std::string, std::shared_ptr<SpriteSheet>> _sprites;

	/** The game manager which drives the game loop. */
	GameManager* _mgr;
	
	/** @brief	A pointer to the current game screen. */
	std::shared_ptr<GameScreen> _gameScreen;

	SpriteSheet::XAxisDirection _spriteXDir;
	SpriteSheet::YAxisDirection _spriteYDir;

	SpriteSheet::XAxisDirection _startXDir;
	SpriteSheet::YAxisDirection _startYDir;

	/** @brief	true if this object is visible. */
	bool _isVisible;

	bool _isDestroyed;
};

#endif