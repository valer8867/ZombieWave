#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

constexpr std::size_t ParticlesCount = 1000;
constexpr float ParticleMaxLifeTime = 200.0f;
constexpr float ParticleMinLifeTime = 100.0f;
constexpr float K = 3.14f / 180.0f;

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:
	ParticleSystem() = default;
	ParticleSystem(const sf::Vector2f& position, std::size_t particlesCount);
	ParticleSystem(const ParticleSystem& rhs);
	ParticleSystem(ParticleSystem&& rhs) noexcept;
	ParticleSystem& operator=(const ParticleSystem& rhs);
	ParticleSystem& operator=(ParticleSystem&& rhs) noexcept;

	bool update(float time);

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	struct Particle
	{
		sf::Vector2f velocity;
		float lifeTime;
	};

	sf::VertexArray vertices;
	std::vector<Particle> particles;
};
