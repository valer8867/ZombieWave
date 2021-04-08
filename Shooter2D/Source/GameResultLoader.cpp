#include "GameResutlLoader.h"

#include <algorithm>
#include <filesystem>
#include <thread>
#include <sstream>

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

bool GameResultLoader::readResult(std::fstream& file, GameResult& result)
{
	const int LEN_MAX = 512;
	char line[LEN_MAX];
	file.getline(line, LEN_MAX, '\n');
	std::string str(line);
	std::stringstream sstr;
	sstr << str;
	sstr >> result.nameLength;
	sstr.get();
	if (str.empty() || result.nameLength > LEN_MAX) return false;	//Simple sanity check
	result.name.resize(result.nameLength);
	sstr.read(result.name.data(), result.nameLength);
	sstr >> result.wave;
	sstr >> result.enemiesKilled;




	//file.read(reinterpret_cast<char*>(&result.nameLength), sizeof(std::size_t));
	//if (file.gcount() != sizeof(std::size_t)) return false;
	//if (result.nameLength > 256) return false;	//Simple sanity check
	//try {
	//	result.name.resize(result.nameLength);
	//} catch(...)
	//{ 
	//	return false;
	//}
	//file.read(reinterpret_cast<char*>(&result.name[0]), result.nameLength * sizeof(char));
	//if (file.gcount() != sizeof(result.nameLength * sizeof(char))) return false;
	//file.read(reinterpret_cast<char*>(&result.wave), sizeof(unsigned short));
	//if (file.gcount() != sizeof(unsigned short)) return false;
	//file.read(reinterpret_cast<char*>(&result.enemiesKilled), sizeof(unsigned short));
	//if (file.gcount() != sizeof(unsigned short)) return false;

	return true;
}

std::fstream& GameResultLoader::writeResult(std::fstream& file, GameResult& result) 
{
	file << result.nameLength << " " << result.name << " " << result.wave << " " << result.enemiesKilled << std::endl;

	//file.write(reinterpret_cast<char*>(&result.nameLength), sizeof(std::size_t));
	//file.write(reinterpret_cast<char*>(&result.name[0]), result.nameLength * sizeof(char));
	//file.write(reinterpret_cast<char*>(&result.wave), sizeof(unsigned short));
	//file.write(reinterpret_cast<char*>(&result.enemiesKilled), sizeof(unsigned short));

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