#include "Actor.h"
#include "GameScreen.h"

Actor::Actor(Vector2 position, GameManager & manager, Vector2 spd, std::unordered_map<std::string, std::shared_ptr<SpriteSheet>>& sprites,
			const std::string&& startSprite, SpriteSheet::XAxisDirection startXDirection, SpriteSheet::YAxisDirection startYDirection)
	:_curKinematic{ position, spd },
	_prevKinematic{ position, spd },
    _health(100),
    _mgr(&manager),
	_gameScreen(std::dynamic_pointer_cast<GameScreen>(manager.GetCurrentScreen())),
	_isVisible(true),
	_isDestroyed(false),
	_startXDir(startXDirection),
	_startYDir(startYDirection),
	_spriteXDir(startXDirection),
	_spriteYDir(startYDirection),
    _colourMod{255, 255, 255, 255}
{
	// Copy the list of sprites
	_sprites = sprites;
	_currentSpriteSheet = startSprite;

	_aabb = AABB(_sprites[startSprite]->GetFrameWidth(), _sprites[startSprite]->GetFrameHeight(), *this);
	_sprites[_currentSpriteSheet]->SetXAxisDirection(_spriteXDir);
	_sprites[_currentSpriteSheet]->SetYAxisDirection(_spriteYDir);
	_sprites[_currentSpriteSheet]->Start();
}

Actor::~Actor()
{
}

AABB Actor::GetAABB()
{
	return _aabb;
}

std::shared_ptr<SpriteSheet> Actor::GetTexture()
{
	return _sprites[_currentSpriteSheet];
}

void Actor::OnOverlap(Actor& other)
{

}

Vector2 Actor::GetPosition()
{
	return _curKinematic.position;
}

Vector2 Actor::GetSpeed()
{
	return _curKinematic.velocity;
}

size_t Actor::GetHealth()
{
	return _health;
}

void Actor::SetSpeed(Vector2 spd)
{
	_curKinematic.velocity = spd;
}

void Actor::SetHealth(size_t health)
{
	_health = health;
}

void Actor::SetPosition(Vector2 pos)
{
	_curKinematic.position = pos;
}

SpriteSheet::XAxisDirection Actor::GetActorXDirection() const
{
	return _spriteXDir;
}

SpriteSheet::YAxisDirection Actor::GetActorYDirection() const
{
	return _spriteYDir;
}

void Actor::SetActorXDirection(SpriteSheet::XAxisDirection dir)
{
	_spriteXDir = dir;
}

void Actor::SetActorYDirection(SpriteSheet::YAxisDirection dir)
{
	_spriteYDir = dir;
}

void Actor::SetVisibility(bool isVisible)
{
	_isVisible = isVisible;
}

bool Actor::IsVisible() const
{
	return _isVisible;
}

bool Actor::IsDestroyed() const
{
	return _isDestroyed;
}

void Actor::Destroy()
{
	_isVisible = false;
	_isDestroyed = true;
}

void Actor::Update(double elapsedSecs)
{
	if (_isDestroyed) return;

	_sprites[_currentSpriteSheet]->Update(elapsedSecs);
}

void Actor::UpdatePosition(double elapsedSecs)
{
}

void Actor::Draw(Camera& camera)
{
	if (_isDestroyed || !_isVisible) return;

	const SDL2pp::Rect& cameraViewport = camera.GetViewport();
	SDL2pp::Renderer& rend = _mgr->GetRenderer();

	std::shared_ptr<SpriteSheet> spriteSheet = _sprites[_currentSpriteSheet];
    SDL2pp::Point tempPoint = SDL2pp::Point((int)_curKinematic.position.GetX(), (int)_curKinematic.position.GetY());

	spriteSheet->SetColourMod(_colourMod);
	spriteSheet->Draw(tempPoint + SDL2pp::Point(-cameraViewport.x, -cameraViewport.y));
}

void Actor::Reset(Vector2 pos)
{
	SetPosition(pos);
	SetHealth(100);
	SetActorXDirection(_startXDir);
	SetActorYDirection(_startYDir);
	_aabb.UpdatePosition(*this);
}