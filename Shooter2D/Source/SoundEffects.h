#pragma once

#include <SFML/Audio.hpp>

class SoundEffects
{
public:
	~SoundEffects();

	SoundEffects(const SoundEffects&)            = delete;
	SoundEffects& operator=(const SoundEffects&) = delete;

	void hitSound();
	void shotSound();
	void losingSound();

	static SoundEffects& instance();

private:
	SoundEffects();

private:
	sf::SoundBuffer* pBuffers;
	sf::Sound* pSounds;
};

