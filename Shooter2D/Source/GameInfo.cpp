#include "GameResutlLoader.h"
#include "ShootingControl.h"
#include "GameInfo.h"
#include "Factory.h"
#include "Player.h"
#include "IO.h"

GameInfo::GameInfo(IO* io_)
	: pLabelsBox(new Rectangle(LabelsBoxX, GunImagesOffsetY, LabelsBoxWidth, LabelsBoxHeight, sf::Color::Black, BorderSize, sf::Color::Red)),
	  pProgressBar(new Rectangle(ProgressBarX, ProgressBarY, 1, ProgressBarHeight, sf::Color::Green)),
	  pTextBox(new TextBox),
	  pBorder(new Border(BorderSize, sf::Color::Green)),
	  io(io_),
	  pGunImages(new std::vector<sf::Sprite>),
	  enemiesKilled(0),
	  wave(1)
{
	float offset = 0.0f;

	for (int i = 0; i < GunsNumber; i++)
	{
		pGunImages->emplace_back(sf::Sprite(*TextureFactory::getInstance().loadFromFile("Resources/Images/gun" + std::to_string(i + 1) + ".png")));
		(*pGunImages)[i].setPosition(offset + GunImagesOffsetX, GunImagesOffsetY);
		offset += (*pGunImages)[i].getGlobalBounds().width + DistanceBetweenGunImages;
	}

	pBorder->createBorderAround(pGunImages->front());

	initTextBox();
}

GameInfo::~GameInfo()
{
	delete pLabelsBox;
	delete pProgressBar;
	delete pTextBox;
	delete pBorder;
}

void GameInfo::initTextBox()
{
	sf::Font* pFont = FontFactory::getInstance().loadFromFile("Resources/Fonts/arial.ttf");
	
	pTextBox->pWaveLabel = new sf::Text("Wave: 1", *pFont, FontSize);
	pTextBox->pEnemiesKilledLabel = new sf::Text("Enemies killed: 0", *pFont, FontSize);

	constexpr float Margin = (LabelsBoxHeight - FontSize * 2) / 3.0f;
	constexpr float LabelX = LabelsBoxX + Margin;
	constexpr float WaveLabelY = GunImagesOffsetY + Margin;
	constexpr float EnemiesKilledLabelY = WaveLabelY + FontSize + 5.0f;

	initLabel(pTextBox->pWaveLabel, sf::Color::White, LabelX, WaveLabelY);
	initLabel(pTextBox->pEnemiesKilledLabel, sf::Color::White, LabelX, EnemiesKilledLabelY);
}

void GameInfo::initLabel(sf::Text* pLabel, const sf::Color& color, float x, float y)
{
	pLabel->setFillColor(color);
	pLabel->setPosition(x, y);
}

void GameInfo::incEnemiesKilled(float ratio)
{
	pTextBox->pEnemiesKilledLabel->setString("Enemies killed: " + std::to_string(++enemiesKilled));
	pProgressBar->setWidth(static_cast<int>(ProgressBarMaxWidth / ratio));
}

void GameInfo::incWave()
{
	pTextBox->pWaveLabel->setString("Wave: " + std::to_string(++wave));
	pProgressBar->setWidth(1);
}

void GameInfo::draw() const
{
	std::for_each(pGunImages->begin(), pGunImages->end(), [this](const sf::Sprite& sprite) { io->draw(&sprite); });
	
	io->draw(pBorder);
	io->draw(pLabelsBox);
	io->draw(pProgressBar);
	io->draw(pTextBox->pWaveLabel);
	io->draw(pTextBox->pEnemiesKilledLabel);
}

GameInfo::TextBox::~TextBox()
{
	delete pWaveLabel;
	delete pEnemiesKilledLabel;
}

void GameInfo::saveToFile() const
{
	GameResultLoader::storeResult(io->getGameName(), wave, enemiesKilled);
}

void GameInfo::indicateCurrentGun(unsigned char indx, ShootingControl* pShootingControl)
{ 
	pBorder->createBorderAround(pGunImages[0][indx]);
}

TwoPGameInfo::TwoPGameInfo(IO* io)
	: GameInfo(io),
	  pSecondPBorder(new Border(BorderSize, sf::Color::Green)),
	  pFirstPlayerSC(nullptr),
	  pSecondPlayerSC(nullptr)
{ 
	pSecondPBorder->createBorderAround(pGunImages->front());
}

TwoPGameInfo::~TwoPGameInfo()
{
	delete pSecondPBorder;
}

void TwoPGameInfo::draw() const
{
	GameInfo::draw();
	io->draw(pSecondPBorder);
}

void TwoPGameInfo::indicateCurrentGun(unsigned char indx, ShootingControl* pShootingControl)
{
	if (pShootingControl == pFirstPlayerSC || !pFirstPlayerSC)
	{
		pFirstPlayerSC = pShootingControl;
		pBorder->createBorderAround(pGunImages[0][indx]);
	}
	else
	{
		pSecondPlayerSC = pShootingControl;
		pSecondPBorder->createBorderAround(pGunImages[0][indx]);
	}
}



