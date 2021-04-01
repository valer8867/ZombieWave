#pragma once

#include <fstream>
#include <vector>
#include <string>

constexpr unsigned char NumberOfResults = 10;
const std::string GameResultsFile = "results";

class GameResultLoader
{
public:
	struct GameResult
	{
		std::size_t nameLength;
		std::string name;
		unsigned short wave;
		unsigned short enemiesKilled;

		bool operator<(const GameResult& rhs) const;
		bool operator>(const GameResult& rhs) const;
		bool operator==(const GameResult& rhs) const;
		bool operator!=(const GameResult& rhs) const;
	};

	static void storeResult(const std::string& gameName, unsigned short wave, unsigned short enemiesKilled);
	static std::vector<GameResult> loadResults();

	static bool wasChanged() { return changed; }

private:
	static void storeResultImpl(const std::string& gameName, unsigned short wave, unsigned short enemiesKilled);
	static std::fstream& readResult(std::fstream& file, GameResult& result);
	static std::fstream& writeResult(std::fstream& file, GameResult& result);
	static void createIfUnexisting();

private:
	static bool changed;
};