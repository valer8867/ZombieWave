#include "Factory.h"
#include "Player.h"
#include "IO.h"
#include "Game.h"

#include <string>

Player::Player(IO* io_, std::shared_ptr<GameInfo> pGameInfo, const sf::Vector2f& pos, std::shared_ptr<MappingKeysToControls> pMappingKeysToControls, const std::string& texturePath)
	: AnimatedObject(TextureFactory::getInstance().loadFromFile(texturePath), pos),
	  pShootingControl(std::make_unique<ShootingControl>(pos, pGameInfo)),
	  pMapping(pMappingKeysToControls),
	  io(io_),
	  speedY(0.0f),
	  shot(false)
{ 
	pSprite->setTextureRect(sf::IntRect(0, 0, width(), height()));
}

void Player::move(float time)
{
	update(time);

	if (speedX || speedY)
	{
		position.x += time * speedX;
		position.y += time * speedY;

		if (position.x < GameBackground::LeftBound)                  position.x = GameBackground::LeftBound;
		else if (position.x + width() > GameBackground::RightBound)  position.x = GameBackground::RightBound- width();
		if (position.y + height() < GameBackground::UpperBound)      position.y = GameBackground::UpperBound - height();
		else if (position.y + height() > GameBackground::LowerBound) position.y = GameBackground::LowerBound - height();

		pSprite->setPosition(position);
		speedX = speedY = 0.0f;
	}
}

void Player::update(float time)
{
	if (io->isPressed(pMapping->mapping[PlayerControls::UP]))
	{
		speedY = -speed();
	}
	else if (io->isPressed(pMapping->mapping[PlayerControls::DOWN]))
	{
		speedY = speed();
	}
	if (io->isPressed(pMapping->mapping[PlayerControls::LEFT]))
	{
		speedX = -speed();
	}
	else if (io->isPressed(pMapping->mapping[PlayerControls::RIGHT]))
	{
		speedX = speed();
	}
	if (io->isPressed(pMapping->mapping[PlayerControls::SHOOT]))
	{
		shot = pShootingControl->isDelayOver();
	}
	if (io->isPressed(pMapping->mapping[PlayerControls::RELOAD]))
	{
		pShootingControl->reload();
	}
	else if (io->isPressed(pMapping->mapping[PlayerControls::GUN1]))
	{
		setNewGun(0);
	}
	else if (io->isPressed(pMapping->mapping[PlayerControls::GUN2]))
	{
		setNewGun(1);

	}
	else if (io->isPressed(pMapping->mapping[PlayerControls::GUN3]))
	{
		setNewGun(2);

	}
	else if (io->isPressed(pMapping->mapping[PlayerControls::GUN4]))
	{
		setNewGun(3);

	}
	else if (io->isPressed(pMapping->mapping[PlayerControls::GUN5]))
	{
		setNewGun(4);
	}

	changeFrame(time);
	pShootingControl->update(position);
}

void Player::setNewGun(unsigned char newGun)
{
	if (pShootingControl->setNewGun(newGun))
	{
		textureCoord.y = newGun * height();
	}
}

bool Player::fire()
{
	if (shot)
	{
		shot = false;
		return true;
	}

	return false;
}

void Player::changeFrame(float time)
{
	if (speedX || speedY)
	{
		AnimatedObject::changeFrame(time);
	}
	else
	{
		currentFrame = 0.0f;
		setTextureCoord();
	}
}

void Player::draw() const
{
	io->draw(pSprite.get());
	io->draw(pShootingControl->getMoneyLabel());
	io->draw(pShootingControl->getGunLabel());
	io->draw(pShootingControl->getAim());
}

int Player::width() const
{
	return 40;
}

int Player::height() const
{
	return 40;
}

float Player::speed() const
{
	return 0.08f;
}

float Player::frameChangeSpeed() const
{
	return 0.012f;
}

float Player::maxFrame() const
{
	return 6.0f;
}