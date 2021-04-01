#include "AnimatedObject.h"

AnimatedObject::AnimatedObject(const sf::Texture* const pTexture, const sf::Vector2f& pos)
	: MovingObject(pTexture, pos),
	  textureCoord(0, 0),
	  currentFrame(0.0f)
{ }

AnimatedObject::AnimatedObject(const AnimatedObject& rhs)
	: MovingObject(rhs),
	  textureCoord(rhs.textureCoord),
	  currentFrame(rhs.currentFrame)
{ }

AnimatedObject::AnimatedObject(AnimatedObject&& rhs) noexcept
	: MovingObject(std::move(rhs)),
	  textureCoord(std::move(rhs.textureCoord)),
	  currentFrame(rhs.currentFrame)
{
	rhs.currentFrame = 0.0f;
}

AnimatedObject& AnimatedObject::operator=(const AnimatedObject& rhs)
{
	if (this != &rhs)
	{
		MovingObject::operator=(rhs);
		textureCoord = rhs.textureCoord;
		currentFrame = rhs.currentFrame;
	}

	return *this;
}

AnimatedObject& AnimatedObject::operator=(AnimatedObject&& rhs) noexcept
{
	if (this != &rhs)
	{
		MovingObject::operator=(std::move(rhs));
		textureCoord = std::move(rhs.textureCoord);
		currentFrame = rhs.currentFrame;
		rhs.currentFrame = 0.0f;
	}

	return *this;
}

void AnimatedObject::changeFrame(float time)
{
	increaseFrame(time);

	if (currentFrame >= maxFrame())
	{
		currentFrame = 0.0f;
	}

	setTextureCoord();
}

void AnimatedObject::setTextureCoord()
{
	textureCoord.x = static_cast<int>(currentFrame) * width();
	pSprite->setTextureRect({ textureCoord.x, textureCoord.y, width(), height() });
}
