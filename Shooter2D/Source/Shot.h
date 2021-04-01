#pragma once

#include "MovingObject.h"

class Player;

class Shot : public MovingObject
{
public:
	Shot(const sf::Vector2f& pos, Player* pPlayer_, unsigned char damage_);
	Shot(const Shot& rhs);
	Shot(Shot&& rhs) noexcept;
	Shot& operator=(const Shot& rhs);
	Shot& operator=(Shot&& rhs) noexcept;

	virtual int width() const override;
	virtual int height() const override;
	virtual void move(float time) override;

	Player* getPlayer() const { return pPlayer; }
	unsigned char getDamage() const { return damage; }

protected:
	virtual float speed() const override;

private:
	Player* pPlayer;
	unsigned char damage;
};
