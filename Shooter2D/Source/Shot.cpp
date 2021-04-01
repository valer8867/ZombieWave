#include "Shot.h"
#include "Factory.h"

Shot::Shot(const sf::Vector2f& pos, Player* pPlayer_, unsigned char damage_)
	: MovingObject(TextureFactory::getInstance().loadFromFile("Resources/Images/shot.png"), pos),
	  pPlayer(pPlayer_),
	  damage(damage_)
{ 
	speedX = speed();
}

Shot::Shot(const Shot& rhs)
	: MovingObject(rhs),
	  pPlayer(rhs.pPlayer),
	  damage(rhs.damage)
{ }

Shot::Shot(Shot&& rhs) noexcept
	: MovingObject(std::move(rhs)),
	  pPlayer(std::move(rhs.pPlayer)),
	  damage(rhs.damage)
{
	rhs.pPlayer = nullptr;
	rhs.damage = 0;
}

Shot& Shot::operator=(const Shot& rhs)
{
	if (this != &rhs)
	{
		MovingObject::operator=(rhs);
		pPlayer = rhs.pPlayer;
		damage = rhs.damage;
	}

	return *this;
}

Shot& Shot::operator=(Shot&& rhs) noexcept
{
	if (this != &rhs)
	{
		MovingObject::operator=(std::move(rhs));
		pPlayer = std::move(rhs.pPlayer);
		damage = rhs.damage;
		rhs.damage = 0;
		rhs.pPlayer = nullptr;
	}

	return *this;
}

void Shot::move(float time)
{
	position.x += time * speed();

	pSprite->setPosition(position);
}

int Shot::width() const
{
	return 10;
}

int Shot::height() const
{
	return 5;
}

float Shot::speed() const
{
	return -1.0f;
}




