#include "GameResutlLoader.h"

#include <algorithm>
#include <filesystem>
#include <thread>

bool GameResultLoader::changed = false;

bool GameResultLoader::GameResult::operator<(const GameResultLoader::GameResult& rhs) const
{
	return wave < rhs.wave || enemiesKilled < rhs.enemiesKilled;
}

bool GameResultLoader::GameResult::operator>(const GameResultLoader::GameResult& rhs) const
{
	return rhs < *this;
}

bool GameResultLoader::GameResult::operator==(const GameResultLoader::GameResult& rhs) const
{
	return wave == rhs.wave && enemiesKilled == rhs.enemiesKilled && name == rhs.name;
}

bool GameResultLoader::GameResult::operator!=(const GameResultLoader::GameResult& rhs) const
{
	return !(*this == rhs);
}

void GameResultLoader::storeResult(const std::string& gameName, unsigned short wave, unsigned short enemiesKilled)
{
	std::string copy(gameName);

	std::thread thread(&GameResultLoader::storeResultImpl, copy, wave, enemiesKilled);
	thread.detach();
}

void GameResultLoader::storeResultImpl(const std::string& gameName, unsigned short wave, unsigned short enemiesKilled)
{
	GameResult newResult;
	newResult.nameLength = gameName.size();
	newResult.name = gameName;
	newResult.wave = wave;
	newResult.enemiesKilled = enemiesKilled;

	auto results = loadResults();
	auto lessThanNew = std::find_if(results.begin(), results.end(), [&newResult](const GameResult& result) { return result < newResult; });

	if (lessThanNew != results.end() || results.size() < NumberOfResults)
	{
		results.insert(lessThanNew, newResult);

		createIfUnexisting();
		std::fstream file(GameResultsFile, std::ios::in | std::ios::out);

		for (unsigned char i = 0; i < NumberOfResults && i < results.size(); i++)
		{
			writeResult(file, results[i]);
		}

		auto fileSize = file.tellp();
		file.close();
		std::filesystem::resize_file(GameResultsFile, fileSize);

		changed = true;
	}
}

std::vector<GameResultLoader::GameResult> GameResultLoader::loadResults()
{
	std::fstream file(GameResultsFile, std::ios::in);
	if (!file.is_open())
	{
		return { };
	}

	std::vector<GameResult> results;
	GameResult result;

	for (; !file.eof(); )
	{
		if (!readResult(file, result))
		{
			break;
		}

		results.emplace_back(std::move(result));
	}

	file.close();

	return results;
}

std::fstream& GameResultLoader::readResult(std::fstream& file, GameResult& result)
{
	file.read(reinterpret_cast<char*>(&result.nameLength), sizeof(std::size_t));
	result.name.resize(result.nameLength);
	file.read(reinterpret_cast<char*>(&result.name[0]), result.nameLength * sizeof(char));
	file.read(reinterpret_cast<char*>(&result.wave), sizeof(unsigned short));
	file.read(reinterpret_cast<char*>(&result.enemiesKilled), sizeof(unsigned short));

	return file;
}

std::fstream& GameResultLoader::writeResult(std::fstream& file, GameResult& result) 
{
	file.write(reinterpret_cast<char*>(&result.nameLength), sizeof(std::size_t));
	file.write(reinterpret_cast<char*>(&result.name[0]), result.nameLength * sizeof(char));
	file.write(reinterpret_cast<char*>(&result.wave), sizeof(unsigned short));
	file.write(reinterpret_cast<char*>(&result.enemiesKilled), sizeof(unsigned short));

	return file;
}

void GameResultLoader::createIfUnexisting()
{
	if (!std::filesystem::exists(GameResultsFile))
	{
		std::fstream file(GameResultsFile, std::ios::in | std::ios::out | std::ios::trunc);
		file.close();
	}
}