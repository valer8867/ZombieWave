#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Clock.hpp>
#include <condition_variable>
#include <atomic>
#include <array>

enum class PlayerControls;
class IO;
class Player;
class Level;
class GameInfo;

constexpr std::size_t NumbOfPlayerControls = 11;

extern const std::string FirstPlayerKeysStr[NumbOfPlayerControls];
extern const std::string SecondPlayerKeysStr[NumbOfPlayerControls];

struct MappingKeysToControls
{
	MappingKeysToControls(std::array<sf::Keyboard::Key, NumbOfPlayerControls> keys);

	void change(PlayerControls control, sf::Keyboard::Key newKey);

	std::unordered_map<PlayerControls, sf::Keyboard::Key> mapping;
};

class Game
{
public:
	Game(IO* io_);

	void finishGame();
	std::thread startOnePGame();
	std::thread startTwoPGame();
	bool isGameFinished();
	void setPause(bool onPause = true);
	void setNeedToSave(bool toSave = true);

private:
	void checkForPause(sf::Clock& clock);
	void saveGame(std::shared_ptr<GameInfo> pGameInfo);
	void onePGameLoop(std::shared_ptr<Player> pPlayer, std::shared_ptr<Level> pLevel, std::shared_ptr<GameInfo> pGameInfo);
	void twoPGameLoop(std::shared_ptr<Player> pFirstPlayer, std::shared_ptr<Player> pSecondPlayer, std::shared_ptr<Level> pLevel, std::shared_ptr<GameInfo> pGameInfo);

private:
	std::condition_variable state;
	std::mutex mutex;
	IO* io;

public:
	std::atomic_bool activeContext;

private:
	bool gameOver;
	bool pause;
	bool finished;
	bool shouldSave;
	bool decisionToSave;
};
