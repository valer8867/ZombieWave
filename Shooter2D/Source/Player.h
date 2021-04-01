#pragma once

#include "AnimatedObject.h"
#include "ShootingControl.h"

#include <SFML/Graphics.hpp>

class IO;
class GameInfo;
struct MappingKeysToControls;

enum class PlayerControls
{
	UP,
	DOWN,
	LEFT, 
	RIGHT,
	SHOOT,
	RELOAD,
	GUN1,
	GUN2,
	GUN3,
	GUN4,
	GUN5,
};

class Player : public AnimatedObject
{
public:
	Player(IO* io_, std::shared_ptr<GameInfo> pGameInfo, const sf::Vector2f& pos, std::shared_ptr<MappingKeysToControls> pMappingKeysToControls, const std::string& texturePath);

	virtual int width()  const override;
	virtual int height() const override;
	virtual void move(float time) override;

	bool fire();
	void draw() const;

	void increaseMoney(unsigned int value) { pShootingControl->increaseMoney(value); }
	unsigned char damage() const           { return pShootingControl->getCurrentGunDamage(); }
	auto getShotPosition() const           { return pShootingControl->getShotPosition(position); }

protected:
	virtual float speed() const override;
	virtual float maxFrame() const override;
	virtual void  changeFrame(float time) override;
	virtual float frameChangeSpeed() const override;

private:
	void update(float time);
	void setNewGun(unsigned char newGun);

private:
	std::unique_ptr<ShootingControl> pShootingControl;
	std::shared_ptr<MappingKeysToControls> pMapping;
	IO* io;
	float speedY;
	bool shot;
};