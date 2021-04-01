#pragma once

#include "Shot.h"
#include "Enemies.h"
#include "ParticleSystem.h"

#include <array>
#include <list>

class IO;
class GameInfo;

class Level
{
public:
	Level(IO* io_, std::shared_ptr<GameInfo> pGi);

	void addShot(const sf::Vector2f& position, Player* pPlayer, unsigned char damage);
	bool update(float time);
	void draw() const;

private:
	void generateEnemies();
	void removeKilledEnemies();
	void updateShots(float time);
	bool updateEnemies(float time);
	void createHitEffect(Shot* pShot);

	static void enemiesDeleter(std::list<Enemy*>* pEnemies);

private:
	class EnemiesFactory
	{
	public:
		EnemiesFactory();

		Enemy* createEnemy();
		void increseAllowedEnemiesTypes();

	private:
		float getRandomYCoordinate(unsigned char enemyType);
		unsigned char getRandomEnemyType();

	private:
		static constexpr unsigned char MaxEnemiesTypes = 3;
		std::array<std::unique_ptr<Enemy>, MaxEnemiesTypes + 1> enemiesSamples;
		unsigned char enemiesTypesAllowed;
	};

	struct Wave
	{
		sf::Clock clock;
		const sf::Int32 GenerationDelayDec = 100;
		const sf::Int32 GenerationDelayLimit = 2000;
		sf::Int32 generationDelay = 5000;
		unsigned short wave = 0;
		unsigned short enemiesKilled = 0;
		unsigned short minEnemiesCount = 4;
		unsigned short currentEnemiesCount = 0;
		unsigned short waveOverallEnemiesCount = 8;
	};

	struct HitEffects
	{
		// If two players simultaneously hit enemies, both particle systems should be updated and rendered, else only one or none
		std::pair<ParticleSystem, bool> firstHitEffect; // firstHitEffect.second == true if that particle system should be updated and rendered
		std::pair<ParticleSystem, bool> secondHitEffect; // secondHitEffect.second == true if that particle system should be updated and rendered
	};

private:
	std::unique_ptr<std::list<Enemy*>, decltype(&Level::enemiesDeleter)> pEnemies;
	std::unique_ptr<EnemiesFactory> pEnemiesFactory;
	std::unique_ptr<std::list<Shot>> pShots;
	std::unique_ptr<Wave> pWave;
	std::unique_ptr<HitEffects> pHitEffects; 
	std::shared_ptr<GameInfo> pGameInfo;
	IO* io;
};
