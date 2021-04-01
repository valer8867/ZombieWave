#include "Game.h"
#include "Level.h"
#include "IO.h"
#include "Player.h"
#include "GameInfo.h"

#include <SFML/Graphics.hpp>
#include <thread>
#include <unordered_map>

const std::array<sf::Keyboard::Key, NumbOfPlayerControls> FirstPlayerKeys = 
{
	sf::Keyboard::Key::W,
	sf::Keyboard::Key::S,
	sf::Keyboard::Key::A,
	sf::Keyboard::Key::D,
	sf::Keyboard::Key::LShift,
	sf::Keyboard::Key::R,
	sf::Keyboard::Key::Num1,
	sf::Keyboard::Key::Num2,
	sf::Keyboard::Key::Num3,
	sf::Keyboard::Key::Num4,
	sf::Keyboard::Key::Num5
};

const std::array<sf::Keyboard::Key, NumbOfPlayerControls> SecondPlayerKeys =
{
	sf::Keyboard::Key::Up,
	sf::Keyboard::Key::Down,
	sf::Keyboard::Key::Left,
	sf::Keyboard::Key::Right,
	sf::Keyboard::Key::RControl,
	sf::Keyboard::Key::P,
	sf::Keyboard::Key::Num6,
	sf::Keyboard::Key::Num7,
	sf::Keyboard::Key::Num8,
	sf::Keyboard::Key::Num9,
	sf::Keyboard::Key::Num0
};

std::shared_ptr<MappingKeysToControls> pFirstPlayerMapping = std::make_shared<MappingKeysToControls>(FirstPlayerKeys);
std::shared_ptr<MappingKeysToControls> pSecondPlayerMapping = std::make_shared<MappingKeysToControls>(SecondPlayerKeys);

const std::string FirstPlayerKeysStr[NumbOfPlayerControls] = 
{
	"Move Up: W", "Move Down: S", "Move Left: A", "Move Right: D", "Shoot: Left Shift", "Reload: R", "First Gun: 1", "Second gun: 2", "Third Gun: 3", "Fourth Gun: 4", "Fifth Gun: 5"
};

const std::string SecondPlayerKeysStr[NumbOfPlayerControls] =
{
	"Move Up: : UP", "Move Down: DOWN", "Move Left: LEFT", "Move Right : RIGHT", "Shoot: Right Control", "Reload: P", "First Gun: 6", "Second gun: 7", "Third Gun: 8", "Fourth Gun: 9", "Fifth Gun: 0"
};

Game::Game(IO* io_)
	: state(),
	  mutex(),
	  io(io_),
	  activeContext(false),
	  gameOver(false), pause(false), finished(false), shouldSave(false), decisionToSave(false)
{ }

std::thread Game::startOnePGame()
{
	auto pGameInfo = std::make_shared<GameInfo>(io);
	auto pPlayer   = std::make_shared<Player>(io, pGameInfo, sf::Vector2f(500.0f, 500.0f), pFirstPlayerMapping, "Resources/Images/player1.png");
	auto pLevel    = std::make_shared<Level>(io, pGameInfo);

	io->setActiveContext(false);
	std::thread game(&Game::onePGameLoop, this, pPlayer, pLevel, pGameInfo);
	
	return game;
}

std::thread Game::startTwoPGame()
{
	auto pGameInfo     = std::make_shared<TwoPGameInfo>(io);
	auto pFirstPlayer  = std::make_shared<Player>(io, pGameInfo, sf::Vector2f(500.0f, 500.0f), pFirstPlayerMapping, "Resources/Images/player1.png");
	auto pSecondPlayer = std::make_shared<Player>(io, pGameInfo, sf::Vector2f(500.0f, 300.0f), pSecondPlayerMapping, "Resources/Images/player2.png");
	auto pLevel        = std::make_shared<Level>(io, pGameInfo);

	io->setActiveContext(false);
	std::thread game(&Game::twoPGameLoop, this, pFirstPlayer, pSecondPlayer, pLevel, pGameInfo);

	return game;
}

void Game::onePGameLoop(std::shared_ptr<Player> pPlayer, std::shared_ptr<Level> pLevel, std::shared_ptr<GameInfo> pGameInfo)
{
	io->setActiveContext(true);
	activeContext = true;

	sf::Clock clock;
	gameOver = false;
	pause = false;

	while (!gameOver)
	{
		float elapsedTime = static_cast<float>(clock.restart().asMilliseconds()); 

		checkForPause(clock); // if the game was on pause, the clock will be restarted in that function
		if (finished)
		{
			break;
		}

		pPlayer->move(elapsedTime);

		if (pPlayer->fire())
		{
			pLevel->addShot(pPlayer->getShotPosition(), pPlayer.get(), pPlayer->damage());
		}

		if (!pLevel->update(elapsedTime))
		{
			break;
		}

		io->clearWindow();
		io->drawGameBackground();

		pGameInfo->draw();
		pPlayer->draw();
		pLevel->draw();

		io->display();
	}

	{
		std::scoped_lock lock(mutex);

		io->setActiveContext(false);
		activeContext = false;
		finished = true;
	}

	saveGame(pGameInfo);
}

void Game::twoPGameLoop(std::shared_ptr<Player> pFirstPlayer, std::shared_ptr<Player> pSecondPlayer, std::shared_ptr<Level> pLevel, std::shared_ptr<GameInfo> pGameInfo)
{
	io->setActiveContext(true);
	activeContext = true;

	sf::Clock clock;
	gameOver = false;
	pause = false;

	while (!gameOver)
	{
		float elapsedTime = static_cast<float>(clock.restart().asMilliseconds());

		checkForPause(clock); // if the game was on pause, the clock will be restarted in that function
		if (finished)
		{
			break;
		}

		pFirstPlayer->move(elapsedTime);
		pSecondPlayer->move(elapsedTime);

		if (pFirstPlayer->fire())
		{
			pLevel->addShot(pFirstPlayer->getShotPosition(), pFirstPlayer.get(), pFirstPlayer->damage());
		}

		if (pSecondPlayer->fire())
		{
			pLevel->addShot(pSecondPlayer->getShotPosition(), pSecondPlayer.get(), pSecondPlayer->damage());
		}

		if (!pLevel->update(elapsedTime))
		{
			break;
		}

		io->clearWindow();
		io->drawGameBackground();

		pGameInfo->draw();
		pFirstPlayer->draw();
		pSecondPlayer->draw();
		pLevel->draw();

		io->display();
	}

	{
		std::scoped_lock lock(mutex);

		io->setActiveContext(false);
		activeContext = false;
		finished = true;
	}

	saveGame(pGameInfo);
}

void Game::checkForPause(sf::Clock& clock)
{
	std::unique_lock lock(mutex);

	if (pause)
	{	
		io->setActiveContext(false);
		activeContext = false;
		state.wait(lock, [this]() { return !pause || finished; } );

		if (finished)
		{
			gameOver = true;
		}
		else
		{
			clock.restart();
			io->setActiveContext(true);
			activeContext = true;
		}
	}
}

void Game::saveGame(std::shared_ptr<GameInfo> pGameInfo)
{
	std::unique_lock lock(mutex);

	if (!decisionToSave)
	{
		state.wait(lock, [this]() { return decisionToSave || shouldSave; });

		if (shouldSave)
		{
			pGameInfo->saveToFile();
		}
	}
}

void Game::setNeedToSave(bool toSave /* = true */)
{
	std::scoped_lock lock(mutex);

	shouldSave = toSave;
	decisionToSave = true;

	state.notify_one();
}

void Game::setPause(bool onPause /* = true */)
{
	std::scoped_lock lock(mutex);

	pause = onPause;
	if (!pause)
	{
		state.notify_one();
	}
}

void Game::finishGame()
{
	std::scoped_lock lock(mutex);

	pause = true;
	finished = true;
	decisionToSave = true;

	state.notify_one();
}

bool Game::isGameFinished()
{
	std::scoped_lock lock(mutex);

	return finished;
}

MappingKeysToControls::MappingKeysToControls(std::array<sf::Keyboard::Key, NumbOfPlayerControls> keys)
	: mapping()
{
	mapping.emplace(PlayerControls::UP,     (keys[0]));
	mapping.emplace(PlayerControls::DOWN,   (keys[1]));
	mapping.emplace(PlayerControls::LEFT,   (keys[2]));
	mapping.emplace(PlayerControls::RIGHT,  (keys[3]));
	mapping.emplace(PlayerControls::SHOOT,  (keys[4]));
	mapping.emplace(PlayerControls::RELOAD, (keys[5]));
	mapping.emplace(PlayerControls::GUN1,   (keys[6]));
	mapping.emplace(PlayerControls::GUN2,   (keys[7]));
	mapping.emplace(PlayerControls::GUN3,   (keys[8]));
	mapping.emplace(PlayerControls::GUN4,   (keys[9]));
	mapping.emplace(PlayerControls::GUN5,   (keys[10]));
}

void MappingKeysToControls::change(PlayerControls control, sf::Keyboard::Key newKey)
{
	mapping[control] = newKey;
}
