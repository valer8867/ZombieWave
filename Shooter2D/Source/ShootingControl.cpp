#include "ShootingControl.h"
#include "GameInfo.h"
#include "Factory.h"

ShootingControl::ShootingControl(const sf::Vector2f& position, std::shared_ptr<GameInfo> pGi)
	: pGameInfo(pGi),
	  guns(),
	  timer(),
	  pGunLabel(new sf::Text),
	  pMoneyLabel(new sf::Text),
	  pAim(new sf::Sprite(*TextureFactory::getInstance().loadFromFile("Resources/Images/aim.png"))),
	  playerMoney(0),
	  currentGun(0)
{
	initGuns();

	sf::Font* pFont = FontFactory::getInstance().loadFromFile("Resources/Fonts/arial.ttf");

	initText(pGunLabel, pFont, sf::Color::Red, "0%", GunLabelFontSize);
	initText(pMoneyLabel, pFont, GreenColor, "0$", MoneyLabelFontSize);
	SET_POSITION(pGunLabel, position, GUN_LABEL_OFFSET_X, GUN_LABEL_OFFSET_Y);
	SET_POSITION(pMoneyLabel, position, MONEY_LABEL_OFFSET_X, MONEY_LABEL_OFFSET_Y);
	SET_POSITION(pAim, position, AIM_OFFSET_X, AIM_OFFSET_Y);

	timer.restart();
}

void ShootingControl::initGuns()
{
	sf::Int32 reloadingTime = 2000;
	sf::Int32 delay = 1000;
	unsigned int price = 500; 
	unsigned char numberOfShots = 8;
	unsigned char damage = 1;

	for (int i = 0; i < GunsNumber; i++)
	{
		guns.emplace_back(reloadingTime, delay, price, numberOfShots, damage);
		reloadingTime += 500;
		price +=500;
		numberOfShots += 4;
		delay -= 100;
		damage++;
	}

	guns[0].price = 0;
}

void ShootingControl::initText(sf::Text* pText, const sf::Font* pFont, const sf::Color& color, const std::string& string, unsigned int characterSize)
{
	pText->setFont(*pFont);
	pText->setCharacterSize(characterSize);
	pText->setFillColor(color);
	pText->setString(string);
}

ShootingControl::~ShootingControl()
{
	delete pGunLabel;
	delete pMoneyLabel;
	delete pAim;
}

bool ShootingControl::isDelayOver()
{
	if (!guns[currentGun].shots || timer.getElapsedTime().asMilliseconds() < guns[currentGun].Delay)
	{
		return false;
	}
		
	pGunLabel->setFillColor(sf::Color::Red);

	if (!(--guns[currentGun].shots))
	{
		pGunLabel->setString("0%");
	}
	else
	{
		pGunLabel->setString(std::to_string(guns[currentGun].shots));
	}

	timer.restart();

	return true;
}

void ShootingControl::update(const sf::Vector2f& position)
{
	SET_POSITION(pGunLabel, position, GUN_LABEL_OFFSET_X, GUN_LABEL_OFFSET_Y);
	SET_POSITION(pMoneyLabel, position, MONEY_LABEL_OFFSET_X, MONEY_LABEL_OFFSET_Y);
	SET_POSITION(pAim, position, AIM_OFFSET_X, AIM_OFFSET_Y);

	float time = static_cast<float>(timer.getElapsedTime().asMilliseconds());

	if (!guns[currentGun].shots) // on reloading
	{
		int reload = static_cast<int>(time * 100.0f / static_cast<float>(guns[currentGun].ReloadingTime));

		if (reload >= 100)
		{
			pGunLabel->setFillColor(GreenColor);
			guns[currentGun].shots = guns[currentGun].NumberOfShots;
			pGunLabel->setString(std::to_string(guns[currentGun].shots));
		}
		else
		{
			pGunLabel->setString(std::to_string(reload) + "%");
		}
	}
	else
	{
		time >= guns[currentGun].Delay ? pGunLabel->setFillColor(GreenColor) : pGunLabel->setFillColor(sf::Color::Red);
	}
}

void ShootingControl::reload()
{
	if (!guns[currentGun].shots)
	{
		return;
	}

	guns[currentGun].shots = 0;
	pGunLabel->setFillColor(sf::Color::Red);
	pGunLabel->setString("0%");
	timer.restart();
}

bool ShootingControl::setNewGun(unsigned char indx)
{
	if (playerMoney >= guns[indx].price)
	{
		if (indx == currentGun) return false;

		playerMoney -= guns[indx].price;
		guns[indx].price = 0;
		currentGun = indx;
		pGameInfo->indicateCurrentGun(indx, this);
		pMoneyLabel->setString(std::to_string(playerMoney) + "$");

		if (!guns[currentGun].shots)
		{
			pGunLabel->setFillColor(sf::Color::Red);
		}
		else
		{
			pGunLabel->setString(std::to_string(guns[currentGun].shots));
		}

		timer.restart();

		return true;
	}

	return false;
}

void ShootingControl::increaseMoney(unsigned int value)
{
	playerMoney += value;
	pMoneyLabel->setString(std::to_string(playerMoney) + "$");
}

ShootingControl::Gun::Gun(sf::Int32 reloadingTime, sf::Int32 delay, unsigned int price_, unsigned char numberOfShots, unsigned char damage)
	: ReloadingTime(reloadingTime),
	  Delay(delay),
	  price(price_),
	  NumberOfShots(numberOfShots),
	  Damage(damage),
	  shots(0)
{ }