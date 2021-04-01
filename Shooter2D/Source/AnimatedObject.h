#pragma once

#include "MovingObject.h"

class AnimatedObject : public MovingObject
{
public:
	AnimatedObject(const sf::Texture* const pTexture, const sf::Vector2f& pos);
	AnimatedObject(const AnimatedObject& rhs);
	AnimatedObject(AnimatedObject&& rhs) noexcept;
	AnimatedObject& operator=(const AnimatedObject& rhs);
	AnimatedObject& operator=(AnimatedObject&& rhs) noexcept;

protected:
	virtual float maxFrame() const = 0;
	virtual void  changeFrame(float time);
	virtual float frameChangeSpeed() const = 0;
	
	void setTextureCoord();
	void increaseFrame(float time) { currentFrame += frameChangeSpeed() * time; }

protected:
	sf::Vector2i textureCoord;
	float currentFrame;
};
