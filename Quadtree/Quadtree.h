#pragma once
#include <memory>
#include "Actor.h"
#include "GameManager.h"

// Based on http://gamedevelopment.tutsplus.com/tutorials/quick-tip-use-quadtrees-to-detect-likely-collisions-in-2d-space--gamedev-374
class Quadtree
{
public:
    Quadtree(int level, SDL2pp::Rect bounds, Quadtree* parent);

    // Insert an actor into the quadtree (obviously)
    void insert(std::shared_ptr<Actor>& actor);

    void update();

    // Clear this quadtree and its children recursively
    void clear();

    // Split this quadtree such that it has four children
    void split();

    // Get all objects that might be colliding with this tree's children
    void retrieve(std::vector<std::shared_ptr<Actor>*> & list, std::shared_ptr<Actor>& actor);

    // Determine the index of the child node in which the given rectangle belongs (-1 if it doesn't fit in any)
    std::uint32_t getChildIndices(const SDL2pp::Rect& rect) const;

    // Determine the index of the child node in which the given actor (-1 if it doesn't fit in any)
    std::uint32_t getChildIndices(const std::shared_ptr<Actor>& a) const;

    // If only a single index is indicated, returns that; otherwise, returns -1
    int getChildIndex(std::uint32_t indices) const;

    void draw(GameManager *mgr);

private:
    int level_;
    std::vector<std::shared_ptr<Actor>> actors_;
    std::vector < std::shared_ptr<Actor>*> removed_;
    std::vector<std::shared_ptr<Actor>> reinserted_;
    SDL2pp::Rect bounds_;
    std::vector<std::shared_ptr<Quadtree>> children_;
    Quadtree* parent_;

    void reinsert(std::shared_ptr<Actor>& actor);

    static const int MAX_LEVELS;
    static const int DIVIDE_THRESHOLD;
};