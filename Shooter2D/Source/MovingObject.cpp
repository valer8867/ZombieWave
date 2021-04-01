#include "MovingObject.h"

MovingObject::MovingObject(const sf::Texture* const pTexture, const sf::Vector2f& pos)
	: position(pos),
	  pSprite(std::make_shared<sf::Sprite>(*pTexture)),
	  speedX(0.0f)
{ 
	pSprite->setPosition(position);
}

MovingObject::MovingObject(const MovingObject& rhs)
	: position(rhs.position),
	  pSprite(std::make_shared<sf::Sprite>(*rhs.pSprite)),
	  speedX(rhs.speedX)
{ }

MovingObject::MovingObject(MovingObject&& rhs) noexcept
	: position(std::move(rhs.position)),
	  pSprite(std::move(rhs.pSprite)),
	  speedX(rhs.speedX)
{
	rhs.speedX = 0.0f;
}

MovingObject& MovingObject::operator=(const MovingObject& rhs)
{
	if (this != &rhs)
	{
		position = rhs.position;
		pSprite = std::make_shared<sf::Sprite>(*rhs.pSprite);
		speedX = rhs.speedX;
	}

	return *this;
}

MovingObject& MovingObject::operator=(MovingObject&& rhs) noexcept
{
	if (this != &rhs)
	{
		position = std::move(rhs.position);
		pSprite = std::move(rhs.pSprite);
		speedX = rhs.speedX;
		rhs.speedX = 0.0f;
	}

	return *this;
}

MovingObject::~MovingObject() = default;