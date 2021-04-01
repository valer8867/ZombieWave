#pragma once

#include "IO.h"
#include "Border.h"
#include "Rectangle.h"

#include <SFML/Graphics.hpp>

class IO;
class ShootingControl;

constexpr float GunImagesOffsetX = 2.0f;
constexpr float GunImagesOffsetY = 3.0f;
constexpr float DistanceBetweenGunImages = 5.0f;
constexpr std::size_t BorderSize = 2;
constexpr unsigned int FontSize = 20;
constexpr int LabelsBoxWidth = 200;
constexpr int LabelsBoxHeight = 75;
constexpr float LabelsBoxX = GameBackground::RightBound - static_cast<float>(LabelsBoxWidth + BorderSize + 1);
constexpr int ProgressBarHeight = 3;
constexpr float ProgressBarX = LabelsBoxX + BorderSize;
constexpr float ProgressBarY = LabelsBoxHeight + GunImagesOffsetY - BorderSize - ProgressBarHeight - 3.0f;
constexpr int ProgressBarMaxWidth = LabelsBoxWidth - (BorderSize * 2);

class GameInfo
{
public:
	GameInfo(IO* io_);
	virtual ~GameInfo();

	void incWave();
	void saveToFile() const;
	void incEnemiesKilled(float ratio);

	virtual void draw() const;
	virtual void indicateCurrentGun(unsigned char indx, ShootingControl* pShootingControl);

private:
	void initTextBox();
	void initLabel(sf::Text* pLabel, const sf::Color& color, float x, float y);

private:
	struct TextBox
	{
		sf::Text* pWaveLabel = nullptr;
		sf::Text* pEnemiesKilledLabel = nullptr;

		~TextBox();
	};

private:
	Rectangle* pLabelsBox;
	Rectangle* pProgressBar;
	TextBox* pTextBox;

protected:
	Border* pBorder;
	IO* io;
	std::vector<sf::Sprite>* pGunImages;

private:
	unsigned short enemiesKilled;
	unsigned short wave;
};

class TwoPGameInfo : public GameInfo
{
public:
	TwoPGameInfo(IO* io);
	~TwoPGameInfo();

	virtual void draw() const override;
	virtual void indicateCurrentGun(unsigned char indx, ShootingControl* pShootingControl) override;

private:
	Border* pSecondPBorder;
	ShootingControl* pFirstPlayerSC;
	ShootingControl* pSecondPlayerSC;
};
