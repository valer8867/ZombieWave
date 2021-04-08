#include "IO.h"
#include "Game.h"
#include "Factory.h"
#include "GameResutlLoader.h"

#include <SFML/Graphics.hpp>
#include <functional>
#include <utility>
#include <mutex>

IO::IO(const std::string& title)
	: pWindow(new sf::RenderWindow(sf::VideoMode(static_cast<unsigned>(GameBackground::RightBound), static_cast<unsigned>(GameBackground::LowerBound)), title)),
	  pGameBackground(new GameBackground),
	  scenes(5, nullptr),
	  pGame(nullptr),
	  gameThread(),
	  currentScene(Scenes::MAIN_SCENE),
	  gameOnPause(false), exit(false)
{ 
	createMainScene();
}

IO::~IO()
{
	delete pWindow;
	delete pGameBackground;
	
	std::for_each(scenes.begin(), scenes.end(), [](Menu* pScene) { delete pScene; });
}

void IO::start()
{
	pWindow->setFramerateLimit(60);
	while (pWindow->isOpen() && !exit)
	{
		sf::Event event;

		while (pWindow->pollEvent(event))
		{
			if (event.type == sf::Event::Closed) 
			{
				if (pGame) stopGame(); 
				return;
			}
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Key::Escape)
				{
					if (pGame)
					{
						if (!pGame->isGameFinished()) pauseGame();
						else						  saveFinishedGame(false);
					}
					else
					{
						currentScene = Scenes::MAIN_SCENE;
					}
				}
				else if (!pGame || gameOnPause || pGame->isGameFinished())
				{
					if (event.key.code == sf::Keyboard::Key::Down)       CURRENT_SCENE_PTR(scenes, currentScene)->down();
					else if (event.key.code == sf::Keyboard::Key::Up)    CURRENT_SCENE_PTR(scenes, currentScene)->up();
					else if (event.key.code == sf::Keyboard::Key::Enter) CURRENT_SCENE_PTR(scenes, currentScene)->onClick();
				}
			}
			else if (event.type == sf::Event::TextEntered && (!pGame || pGame->isGameFinished()))
			{
				if(event.text.unicode >= 0x20 || event.text.unicode == '\b')
					CURRENT_SCENE_PTR(scenes, currentScene)->textEntered(event.text.unicode);
			}
			if (event.type == sf::Event::LostFocus)
			{
				if (pGame && !gameOnPause) pauseGame();
			}
		}

		if (!pGame || gameOnPause)
		{
			drawCurrentScene();
		}
		else
		{
			if (pGame->isGameFinished())
			{			
				createGameSavingScene();
				setActiveContext(true);
				currentScene = Scenes::SAVE_GAME_SCENE;
				drawCurrentScene();
			}
		}
	}
}

void IO::killGameThread(std::thread&& gameThread)
{
	gameThread.join();
	delete pGame;
	pGame = nullptr;
	gameOnPause = false;
	pWindow->setActive(true);
}

void IO::startOnePGame()
{
	pGame = new Game(this);
	gameThread = pGame->startOnePGame();
}

void IO::startTwoPGame()
{
	pGame = new Game(this);
	gameThread = pGame->startTwoPGame();
}

GameBackground::GameBackground()
	: pSprite(new sf::Sprite(*TextureFactory::getInstance().loadFromFile("Resources/Images/background.png")))
{ }

GameBackground::~GameBackground()
{
	delete pSprite;
}

void IO::pauseGame()
{
	gameOnPause = !gameOnPause;

	if (!gameOnPause) pWindow->setActive(false);

	pGame->setPause(gameOnPause);

	if (gameOnPause)
	{
		currentScene = Scenes::ON_PAUSE_SCENE;
		createOnPauseScene();

		/* The game thread should invoke setActiveContetx(false) and lock mutex before that thread will invoke pWindow->setActive(true) and will
		start drawing scene
		*/
		while (pGame->activeContext)
			;

		pWindow->setActive(true);
	}
}

void IO::stopGame()
{
	pGame->finishGame();
	killGameThread(std::move(gameThread));
}

sf::String IO::getGameName() const
{
	return SAVE_GAME_SCENE_PTR(scenes) ? SAVE_GAME_SCENE_PTR(scenes)->getTextLineStr(0) : "";
}

void IO::saveFinishedGame(bool save)
{
	pGame->setNeedToSave(save);
	killGameThread(std::move(gameThread));
	currentScene = Scenes::MAIN_SCENE;
	SAVE_GAME_SCENE_PTR(scenes)->setTextLineStr(0, "Enter game name for saving");
}

void IO::createMainScene()
{
	createFullWindowSizeScene(&MAIN_SCENE_PTR(scenes));

	MAIN_SCENE_PTR(scenes)->createButtons(5);
	MAIN_SCENE_PTR(scenes)->initButton("One Player", std::bind(&IO::startOnePGame, this), 0);
	MAIN_SCENE_PTR(scenes)->initButton("Two Players", std::bind(&IO::startTwoPGame, this), 1);
	MAIN_SCENE_PTR(scenes)->initButton("Stats", [this] { createStatsScene(); currentScene = Scenes::STATS_SCENE; }, 2);
	MAIN_SCENE_PTR(scenes)->initButton("Help", [this] { createHelpScene(); currentScene = Scenes::HELP_SCENE; }, 3);
	MAIN_SCENE_PTR(scenes)->initButton("Exit", [this] { exit = true; }, 4);
}

void IO::createOnPauseScene()
{
	if (!ON_PAUSE_SCENE_PTR(scenes))
	{
		createFullWindowSizeScene(&ON_PAUSE_SCENE_PTR(scenes));

		ON_PAUSE_SCENE_PTR(scenes)->createButtons(2);
		ON_PAUSE_SCENE_PTR(scenes)->initButton("Continue", std::bind(&IO::pauseGame, this), 0);
		ON_PAUSE_SCENE_PTR(scenes)->initButton("Main menu", [this]() { stopGame(); currentScene = Scenes::MAIN_SCENE; }, 1);
	}
}

void IO::createGameSavingScene()
{
	if (!SAVE_GAME_SCENE_PTR(scenes))
	{
		createFullWindowSizeScene(&SAVE_GAME_SCENE_PTR(scenes));

		SAVE_GAME_SCENE_PTR(scenes)->createTextLines(1);
		SAVE_GAME_SCENE_PTR(scenes)->createButtons(2);
		SAVE_GAME_SCENE_PTR(scenes)->initButton("Enter game name for saving", [] {}, 0);

		

		SAVE_GAME_SCENE_PTR(scenes)->initButton("Save", std::bind(&IO::saveFinishedGame, this, true), 1);
		SAVE_GAME_SCENE_PTR(scenes)->initButton("Do not save", std::bind(&IO::saveFinishedGame, this, false), 2);

		SAVE_GAME_SCENE_PTR(scenes)->setButtonWidth(0, 400);
		SAVE_GAME_SCENE_PTR(scenes)->removeButtonBorder(0);
	}
}

void IO::createStatsScene()
{
	if (!STATS_SCENE_PTR(scenes) || GameResultLoader::wasChanged())
	{
		delete STATS_SCENE_PTR(scenes);
		createFullWindowSizeScene(&STATS_SCENE_PTR(scenes));

		auto gameResults = GameResultLoader::loadResults();

		STATS_SCENE_PTR(scenes)->setButtonsSize(600, 35);
		STATS_SCENE_PTR(scenes)->setFontSize(20);
		STATS_SCENE_PTR(scenes)->createConstTextLines(gameResults.size());
		STATS_SCENE_PTR(scenes)->createButtons(1);

		auto size = gameResults.size();
		for (std::size_t i = 0; i < size; i++)
		{
			GameResultLoader::GameResult& result = gameResults[i];

			STATS_SCENE_PTR(scenes)->initButton(std::to_string(i + 1) + "." + result.name +		//Might convert name to regular string - need to check!
				" Wave: " + std::to_string(result.wave) + " Enemies Killed: " + std::to_string(result.enemiesKilled), [] {}, i);
			STATS_SCENE_PTR(scenes)->removeButtonBorder(i);
		}

		STATS_SCENE_PTR(scenes)->initButton("Main menu", [this] { currentScene = Scenes::MAIN_SCENE; }, size);
		STATS_SCENE_PTR(scenes)->setButtonWidth(size, 300);
	}
}

void IO::createHelpScene()
{
	if (!HELP_SCENE_PTR(scenes))
	{
		createFullWindowSizeScene(&HELP_SCENE_PTR(scenes));

		const int buttonWidth = 300;
		const int buttonHeight = 30;
		const int buttonGap = 3;
		const int offsetFirstCol = pWindow->getSize().x / 2 - buttonWidth - buttonGap;
		const int offsetSecondCol = pWindow->getSize().x / 2 + buttonGap;
		const int rectTotal = (NumbOfPlayerControls + 1) * 2;

		HELP_SCENE_PTR(scenes)->setButtonsSize(buttonWidth, buttonHeight);
		HELP_SCENE_PTR(scenes)->setFontSize(20);
		HELP_SCENE_PTR(scenes)->createConstTextLines(rectTotal);

		HELP_SCENE_PTR(scenes)->initButton("Player 1:", [] {}, 0);
		HELP_SCENE_PTR(scenes)->setButtonPos(0, offsetFirstCol, 0);
		HELP_SCENE_PTR(scenes)->initButton("Player 2:", [] {}, 1);
		HELP_SCENE_PTR(scenes)->setButtonPos(1, offsetSecondCol, 0);

		for (size_t i = 2; i < NumbOfPlayerControls + 2; i++)
		{
			HELP_SCENE_PTR(scenes)->initButton(FirstPlayerKeysStr[i - 2], [] {}, i);
			HELP_SCENE_PTR(scenes)->removeButtonBorder(i);
			HELP_SCENE_PTR(scenes)->setButtonPos(i, offsetFirstCol, (buttonHeight + buttonGap) * (i - 1));

			HELP_SCENE_PTR(scenes)->initButton(SecondPlayerKeysStr[i - 2], [] {}, i + NumbOfPlayerControls);
			HELP_SCENE_PTR(scenes)->removeButtonBorder(i + NumbOfPlayerControls);
			HELP_SCENE_PTR(scenes)->setButtonPos(i + NumbOfPlayerControls, offsetSecondCol, (buttonHeight + buttonGap) * (i - 1));
		}
	

		//HELP_SCENE_PTR(scenes)->setButtonPos(1, 20, 30);
		//HELP_SCENE_PTR(scenes)->setButtonWidth(1, 200);
		//HELP_SCENE_PTR(scenes)->setTextOffsetInButton(1, 5, 5);

						//end of test section
		



		//HELP_SCENE_PTR(scenes)->initButton("Pause/Back: ESCAPE", [] {}, rectTotal - 1);
		//HELP_SCENE_PTR(scenes)->removeButtonBorder(rectTotal - 1);
		//HELP_SCENE_PTR(scenes)->initButton("Main menu", [this] { currentScene = Scenes::MAIN_SCENE; }, rectTotal - 1);
		//HELP_SCENE_PTR(scenes)->setButtonWidth(rectTotal, 300);
	}
}

void IO::createFullWindowSizeScene(Menu** ppScene)
{
	*ppScene = new Menu(static_cast<int>(GameBackground::RightBound),
		static_cast<int>(GameBackground::LowerBound),
		{ 0.0f, 0.0f },
		10.0f,
		"Resources/Images/menuBackground.png",
		"Resources/Fonts/arial.ttf");

	(*ppScene)->setButtonsSize(300, 50);
	(*ppScene)->setButtonsColor(sf::Color::Black);
	(*ppScene)->setButtonsTextColor(sf::Color::White);
	(*ppScene)->setFontSize(30);
}
