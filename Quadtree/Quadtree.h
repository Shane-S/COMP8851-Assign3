#pragma once
#include <memory>
#include "Actor.h"

// Based on http://gamedevelopment.tutsplus.com/tutorials/quick-tip-use-quadtrees-to-detect-likely-collisions-in-2d-space--gamedev-374
class Quadtree
{
public:
    Quadtree(int level, SDL2pp::Rect bounds) :
        level_(level), bounds_(bounds), children_(), actors_()
    {
        children_.reserve(4);
        for (int i = 0; i < 4; i++) children_.push_back(nullptr);
    }

    // Insert an actor into the quadtree (obviously)
    void insert(std::shared_ptr<Actor> actor);

    // Clear this quadtree and its children recursively
    void clear();

    // Split this quadtree such that it has four children
    void split();

    // Get all objects that might be colliding with this tree's children
    void retrieve(std::vector<std::shared_ptr<Actor>> & list, std::shared_ptr<Actor>& actor) const;

    // Determine the index of the child node in which the given rectangle belongs (-1 if it doesn't fit in any)
    std::uint32_t getChildIndices(const SDL2pp::Rect& rect) const;

    // Determine the index of the child node in which the given actor (-1 if it doesn't fit in any)
    std::uint32_t getChildIndices(const std::shared_ptr<Actor>& a) const;

    // If only a single index is indicated, returns that; otherwise, returns -1
    int getChildIndex(std::uint32_t indices) const;

private:
    int level_;
    std::vector<std::shared_ptr<Actor>> actors_;
    SDL2pp::Rect bounds_;
    std::vector<std::shared_ptr<Quadtree>> children_;

    static const int MAX_LEVELS;
    static const int DIVIDE_THRESHOLD;
};