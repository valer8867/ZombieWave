#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

constexpr unsigned char NumberOfResults = 10;
const std::string GameResultsFile = "results";

class GameResultLoader
{
public:
	struct GameResult
	{
		std::size_t nameLength;
		sf::String name;
		unsigned short wave;
		unsigned short enemiesKilled;

		bool operator<(const GameResult& rhs) const;
		bool operator>(const GameResult& rhs) const;
		bool operator==(const GameResult& rhs) const;
		bool operator!=(const GameResult& rhs) const;
	};

	struct ResultDataStruct		//To store in file
	{
		int nameLen;
		sf::Uint32 nameUnicode[64];
		unsigned short wave;
		unsigned short enemiesKilled;
	};


	static void storeResult(const sf::String& gameName, unsigned short wave, unsigned short enemiesKilled);
	static std::vector<GameResult> loadResults();

	static bool wasChanged() { return changed; }

private:
	static void storeResultImpl(const sf::String& gameName, unsigned short wave, unsigned short enemiesKilled);
	static bool readResult(std::fstream& file, GameResult& result);
	static std::fstream& writeResult(std::fstream& file, GameResult& result);
	static void createIfUnexisting();

private:
	static bool changed;
};