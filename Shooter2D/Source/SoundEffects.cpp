#include "SoundEffects.h"
#include <array>

constexpr uint8_t NumberOfSounds = 3;

static const std::array<std::string, NumberOfSounds> PathesToSounds =
{
	"Resources/Sound/hitSound.wav",
	"Resources/Sound/shotSound.ogg",
	"Resources/Sound/losingSound.wav"
};

SoundEffects& SoundEffects::instance()
{
	static SoundEffects soundEffects;
	return soundEffects;
}

SoundEffects::SoundEffects()
	: pBuffers(new sf::SoundBuffer[NumberOfSounds]),
	  pSounds(new sf::Sound[NumberOfSounds])
{
	for (int i = 0; i < NumberOfSounds; i++)
	{
		pBuffers[i].loadFromFile(PathesToSounds[i]);
		pSounds[i].setBuffer(pBuffers[i]);
	}
}

SoundEffects::~SoundEffects()
{
	delete[] pBuffers;
	delete[] pSounds;
}

void SoundEffects::hitSound()
{
	pSounds[0].play();
}

void SoundEffects::shotSound()
{
	pSounds[1].play();
}

void SoundEffects::losingSound()
{
	pSounds[2].play();
}
