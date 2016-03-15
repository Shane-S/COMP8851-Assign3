#include "SquareActor.h"
#include "GameScreen.h"

void SquareActor::OnOverlap(Actor & other)
{
    _colourMod = SDL_Color{ 255, 0, 0, 255 };
}

void SquareActor::UpdatePosition(double deltaSecs)
{
    _prevKinematic = _curKinematic;

    float deltaX = _curKinematic.velocity.GetX() * deltaSecs;
    float deltaY = _curKinematic.velocity.GetY() * deltaSecs;

    float posX = _curKinematic.position.GetX() + deltaX;
    float posY = _curKinematic.position.GetY() + deltaY;
    SDL2pp::Point levelSize = _gameScreen->GetLevel()->GetLevelSize();

    if (posX + _aabb.GetWidth() > levelSize.x)
    {
        posX = levelSize.x - _aabb.GetWidth();
        _curKinematic.velocity.SetX(_curKinematic.velocity.GetX() * -1);
    }
    else if (posX < 0)
    {
        posX = 0;
        _curKinematic.velocity.SetX(_curKinematic.velocity.GetX() * -1);
    }

    if (posY + _aabb.GetHeight() > levelSize.y)
    {
        posY = levelSize.y - _aabb.GetHeight();
        _curKinematic.velocity.SetY(_curKinematic.velocity.GetY() * -1);
    }
    else if (posY + deltaY < 0)
    {
        posY = 0;
        _curKinematic.velocity.SetY(_curKinematic.velocity.GetY() * -1);
    }

    _curKinematic.position.SetX(posX);
    _curKinematic.position.SetY(posY);
}

void SquareActor::Update(double deltaSecs)
{
    Actor::Update(deltaSecs);
    UpdatePosition(deltaSecs);
    _aabb.UpdatePosition(*this);
    _colourMod = SDL_Color{ 255, 255, 255, 255 };
}
