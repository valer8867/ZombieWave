#pragma once

#include "AnimatedObject.h"

#include <SFML/System/Clock.hpp>

class Shot;

class Enemy : public AnimatedObject
{
public:
	Enemy(const sf::Texture* const pTexture, const sf::Vector2f& pos, unsigned char health);
	Enemy(const Enemy& rhs);
	Enemy(Enemy&& rhs) noexcept;
	Enemy& operator=(const Enemy& rhs);
	Enemy& operator=(Enemy&& rhs) noexcept;

	virtual bool hit(const Shot& shot);
	virtual void move(float time) override;
	virtual unsigned int moneyForHit() const = 0;
	
	bool isDead()  const { return dead; }
	bool isAlive() const { return alive; }

protected:
	void init();
	virtual void changeFrame(float time) override;

protected:
	short hp;
	bool alive; // if hp <= 0, enemy still moves for the time while on death animation (not implemented yet) is working, but the enemy can't be damaged by the player's shot
	bool dead;  // dead == false untill the death animation ends
};

class WeakEnemy : public Enemy
{
public:
	WeakEnemy(const sf::Vector2f& pos);

	virtual int width() const override;;
	virtual int height() const override;
	virtual unsigned int moneyForHit() const override;

protected:
	virtual float speed() const override;
	virtual float maxFrame() const override;
	virtual float frameChangeSpeed() const override;
};

class DiagonalMovingEnemy : public Enemy
{
public:
	DiagonalMovingEnemy(const sf::Vector2f& pos);

	virtual int width() const override;;
	virtual int height() const override;
	virtual void move(float time) override;
	virtual unsigned int moneyForHit() const override;

protected:
	virtual float speed() const override;
	virtual float maxFrame() const override;
	virtual float frameChangeSpeed() const override;

private:
	void changeDirection();

private:
	sf::Clock timer;
	float speedY;
	constexpr static sf::Int32 DirectionChangeDelay = 2000;
};

class JumpingEnemy : public Enemy
{
public:
	JumpingEnemy(const sf::Vector2f& pos);

	virtual int width() const override;;
	virtual int height() const override;
	virtual void move(float time) override;
	virtual unsigned int moneyForHit() const override;

protected:
	virtual float speed() const override;
	virtual float maxFrame() const override;
	virtual bool hit(const Shot& shot) override;
	virtual void  changeFrame(float time) override;
	virtual float frameChangeSpeed() const override;

private:
	void transitionFromOnGround();

private:
	sf::Clock timer;
	constexpr static sf::Int32 DelayBetweenJumps = 1000;
	constexpr static int Height = 140;
	constexpr static int HeightIfOnGround = 49;
	bool onGround;
};

class StrongEnemy : public Enemy
{
public:
	StrongEnemy(const sf::Vector2f& pos);

	virtual int width() const override;;
	virtual int height() const override;
	virtual unsigned int moneyForHit() const override;

protected:
	virtual float speed() const override;
	virtual float maxFrame() const override;
	virtual float frameChangeSpeed() const override;
};