#pragma once
#include "Actor.h"

class SquareActor : public Actor
{
public:
    SquareActor(Vector2 position,
                GameManager & manager,
                Vector2 spd,
                std::unordered_map<std::string, std::shared_ptr<SpriteSheet>>& sprites,
                const std::string&& startSprite,
                SpriteSheet::XAxisDirection startXDirection,
                SpriteSheet::YAxisDirection startYDirection)
        : Actor(position, manager, spd, sprites, std::move(startSprite), startXDirection, startYDirection),
          collidedLastFrame(false) {}

    virtual void OnOverlap(Actor& other) override;
    virtual Type GetType() const override { return Type::object; }
    virtual void UpdatePosition(double deltaSecs) override;
    virtual void Update(double deltaSecs) override;

    bool collidedLastFrame;
};