#include "ParticleSystem.h"

#include <random>
#include <vector>

ParticleSystem::ParticleSystem(const sf::Vector2f& position, std::size_t particlesCount)
	: sf::Drawable(), sf::Transformable(),
	  vertices(sf::Points, particlesCount),
	  particles(particlesCount)
{
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<float> speedYDist(-45.0f, 45.0f);
	std::uniform_real_distribution<float> speedXDist(0.1f, 0.2f);
	std::uniform_real_distribution<float> lifeTimeDist(ParticleMinLifeTime, ParticleMaxLifeTime);

	for (std::size_t i = 0; i < particlesCount; i++)
	{
		float speed = speedXDist(generator);
		vertices[i].position = position;
		particles[i].velocity = sf::Vector2f(-speed, speedYDist(generator) * speed * K);
		particles[i].lifeTime = lifeTimeDist(generator);
	}
}

ParticleSystem::ParticleSystem(const ParticleSystem& rhs)
	: vertices(rhs.vertices),
	  particles(rhs.particles)
{ }

ParticleSystem::ParticleSystem(ParticleSystem&& rhs) noexcept
	: vertices(std::move(rhs.vertices)),
	  particles(std::move(rhs.particles))
{ }

ParticleSystem& ParticleSystem::operator=(const ParticleSystem& rhs)
{
	if (this != &rhs)
	{
		vertices = rhs.vertices;
		particles = rhs.particles;
	}

	return *this;
}

ParticleSystem& ParticleSystem::operator=(ParticleSystem&& rhs) noexcept
{
	if (this != &rhs)
	{
		vertices = std::move(rhs.vertices);
		particles = std::move(rhs.particles);
	}

	return *this;
}

bool ParticleSystem::update(float time)
{
	bool aliveParticles = false;

	for (std::size_t i = 0; i < particles.size(); i++)
	{
		Particle& particle = particles[i];
		sf::Vertex& vertex = vertices[i];

		particle.lifeTime -= time;

		if (particle.lifeTime > 0.0f) aliveParticles = true;
		else						  continue;

		float ratio = ParticleMaxLifeTime / particle.lifeTime;

		vertex.position += particle.velocity * time;
		vertex.color = sf::Color(static_cast<sf::Uint8>(ratio * 255.0f), 0, 0);
	}

	return aliveParticles;
}

void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(vertices, states);
}