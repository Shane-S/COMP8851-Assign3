#ifndef GAME_SCREEN_H
#define GAME_SCREEN_H

#include <cstring>
#include <string>
#include <cstdint>
#include "Screen.h"
#include "Level.h"
#include "SpriteSheet.h"
#include "Actor.h"
#include "Quadtree.h"

#define NUM_MENU_ITEMS 3

class GameScreen :
	public Screen
{
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	///<summary> Creates a new player actor with the given parameters.</summary>
	///
	///<param name="backgroundPath"> Path to the file containing the background image.</param>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	GameScreen()
        : _paused(false),  _usingQuadtree(false), _builtTree(false) {}

	virtual int Load();
	virtual int Update(double elapasedSecs) override;
	virtual void Draw() override;
	virtual void Unload() override;

	/**
	 * @brief	Gets the level associated with this GameScreen.
	 *
	 * @author	Shane
	 * @date	2/5/2016
	 *
	 * @return	The level associated with this GameScreen.
	 */
	std::shared_ptr<Level> GetLevel() const;
    
protected:
    void NaiveCollisionDetection();
    void QuadtreeCollisionDetection();

    std::shared_ptr<SDL2pp::Font> _font;

	bool _paused;
	Camera* _camera;
	GameManager* _mgr;
	std::shared_ptr<Level> _level;
	std::shared_ptr<Quadtree> _quadtree;
    std::vector<std::shared_ptr<Actor>*> candidates_;
    bool _usingQuadtree;
    bool _builtTree;
};

#endif
