#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Clock.hpp>
#include <string>
#include <vector>

class GameInfo;

const sf::Color GreenColor = sf::Color(3, 32, 16);
constexpr unsigned char GunsNumber = 5;
constexpr unsigned char GunLabelFontSize = 13;
constexpr unsigned char MoneyLabelFontSize = 13;

#define GUN_LABEL_OFFSET_X   -38
#define GUN_LABEL_OFFSET_Y   -9
#define MONEY_LABEL_OFFSET_X  20
#define MONEY_LABEL_OFFSET_Y -13
#define AIM_OFFSET_X         -40
#define AIM_OFFSET_Y          10
#define SET_POSITION(pDrawable, position, offsetX, offsetY) pDrawable->setPosition(position.x + (offsetX), position.y + (offsetY));

class ShootingControl
{
public:
	ShootingControl(const sf::Vector2f& position, std::shared_ptr<GameInfo> pGi);
	~ShootingControl();

	bool isDelayOver();
	void reload();
	bool setNewGun(unsigned char indx);
	void increaseMoney(unsigned int value);
	void update(const sf::Vector2f& position);

	const auto getAim() const { return pAim; }
	const auto getGunLabel() const { return pGunLabel; }
	const auto getMoneyLabel() const { return pMoneyLabel;  }
	auto getCurrentGunDamage() const { return guns[currentGun].Damage; }
	auto getShotPosition(const sf::Vector2f& playerPosition) const { return sf::Vector2f(playerPosition.x - 10.0f, playerPosition.y + AIM_OFFSET_Y); }

private:
	void initGuns();
	void initText(sf::Text* pText, const sf::Font* pFont, const sf::Color& color, const std::string& string, unsigned int characterSize);

private:
	struct Gun
	{
		Gun(sf::Int32 reloadingTime, sf::Int32 delay, unsigned int price_, unsigned char numberOfShots, unsigned char damage);

		const sf::Int32 ReloadingTime;
		const sf::Int32 Delay;
		unsigned int price;
		const unsigned char NumberOfShots;
		const unsigned char Damage;
		unsigned char shots;
	};

private:
	std::shared_ptr<GameInfo> pGameInfo;
	std::vector<Gun> guns;
	sf::Clock timer;
	sf::Text* pGunLabel;
	sf::Text* pMoneyLabel;
	sf::Sprite* pAim;
	unsigned int playerMoney;
	unsigned char currentGun;
};