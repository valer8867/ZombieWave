#include "Enemies.h"
#include "Shot.h"
#include "IO.h"
#include "Factory.h"

Enemy::Enemy(const sf::Texture* const pTexture, const sf::Vector2f& pos, unsigned char health)
	: AnimatedObject(pTexture, pos),
	  hp(health),
	  alive(true),
	  dead(false)
{ }

Enemy::Enemy(const Enemy& rhs)
	: AnimatedObject(rhs),
	  hp(rhs.hp),
	  alive(rhs.alive),
	  dead(rhs.dead)
{ }

Enemy::Enemy(Enemy&& rhs) noexcept
	: AnimatedObject(std::move(rhs)),
	  hp(rhs.hp),
	  alive(rhs.alive),
	  dead(rhs.dead)
{
	rhs.hp = 0;
	rhs.alive = true;
	rhs.dead = false;
}

Enemy& Enemy::operator=(const Enemy& rhs)
{
	if (this != &rhs)
	{
		AnimatedObject::operator=(rhs);
		hp = rhs.hp;
		alive = rhs.alive;
		dead = rhs.dead;
	}

	return *this;
}

Enemy& Enemy::operator=(Enemy&& rhs) noexcept
{
	if (this != &rhs)
	{
		AnimatedObject::operator=(std::move(rhs));
		hp = rhs.hp;
		alive = rhs.alive;
		dead = rhs.dead;

		rhs.hp = 0;
		rhs.alive = true;
		rhs.dead = false;
	}

	return *this;
}

bool Enemy::hit(const Shot& shot)
{
	if (alive && sf::Rect<float>(position.x, position.y, static_cast<float>(size().x), static_cast<float>(size().y)).intersects(
		{ shot.getX(), shot.getY(), static_cast<float>(shot.size().x), static_cast<float>(shot.size().y) } ))
	{
		hp -= shot.getDamage();

		if (hp <= 0)
		{
			currentFrame = maxFrame();
			alive = false;
		}

		return true;
	}

	return false;
}

void Enemy::move(float time)
{
	changeFrame(time);

	if (alive)
	{
		position.x += speedX * time;
		pSprite->setPosition(position);
	}
}

void Enemy::changeFrame(float time)
{
	increaseFrame(time);

	if (currentFrame >= maxFrame())
	{
		if (alive)
		{
			currentFrame = 0.0f;
		}
		else
		{
			currentFrame--;
			dead = true;
		}
	}

	setTextureCoord();
}

void Enemy::init()
{
	pSprite->setTextureRect(sf::IntRect(0, 0, width(), height()));
	speedX = speed();
}

WeakEnemy::WeakEnemy(const sf::Vector2f& pos)
	: Enemy(TextureFactory::getInstance().loadFromFile("Resources/Images/enemie1.png"), pos, 2) 
{
	init();
}

float WeakEnemy::maxFrame() const
{
	if (alive)
	{
		return 4.0f;
	}

	return 10.0f;
}

DiagonalMovingEnemy::DiagonalMovingEnemy(const sf::Vector2f& pos)
	: Enemy(TextureFactory::getInstance().loadFromFile("Resources/Images/enemie2.png"), pos, 3),
	  timer(),
	  speedY(speed())
{
	init();
}

void DiagonalMovingEnemy::move(float time)
{
	changeFrame(time);

	if (alive)
	{
		position.x += speedX * time;
		position.y += speedY * time;

		if (timer.getElapsedTime().asMilliseconds() >= DirectionChangeDelay)
		{
			changeDirection();
		}
		else
		{
			if (position.y + height() >= GameBackground::LowerBound)
			{
				position.y = GameBackground::LowerBound - height();
				changeDirection();
			}
			else
			{
				if (position.y + height() <= GameBackground::UpperBound)
				{
					position.y = GameBackground::UpperBound + height();
					changeDirection();
				}
			}
		}
		
		pSprite->setPosition(position);
	}
}

void DiagonalMovingEnemy::changeDirection()
{
	speedY *= -1;
	timer.restart();
}

float DiagonalMovingEnemy::maxFrame() const
{
	if (alive)
	{
		return 4.0f;
	}

	return 10.0f;
}

JumpingEnemy::JumpingEnemy(const sf::Vector2f& pos)
	: Enemy(TextureFactory::getInstance().loadFromFile("Resources/Images/enemie3.png"), pos, 10),
	  timer(),
	  onGround(false)
{
	init();
}

void JumpingEnemy::changeFrame(float time)
{
	increaseFrame(time);

	if (currentFrame >= maxFrame())
	{
		if (alive)
		{
			currentFrame = 0.0f;
			onGround = true;
			textureCoord.y = Height - HeightIfOnGround;
			position.y += static_cast<float>(textureCoord.y);
			timer.restart();
		}
		else
		{
			currentFrame--;
			dead = true;
		}
	}

	setTextureCoord();
}

void JumpingEnemy::move(float time)
{
	if (!onGround || !alive)
	{		
		Enemy::move(time);
	}
	else 
	{
		if (onGround && timer.getElapsedTime().asMilliseconds() >= DelayBetweenJumps)
		{
			transitionFromOnGround();
			Enemy::move(time);
		}
	}
}

bool JumpingEnemy::hit(const Shot& shot)
{
	if (!onGround)
	{
		return false;
	}

	if (Enemy::hit(shot))
	{
		if (!alive)
		{
			transitionFromOnGround();
			setTextureCoord();
			pSprite->setPosition(position);
		}

		return true;
	}

	return false;
}

void JumpingEnemy::transitionFromOnGround()
{
	onGround = false;
	position.y -= static_cast<float>(textureCoord.y);
	textureCoord.y = 0;
}

int JumpingEnemy::height() const 
{ 
	if (onGround)
	{
		return HeightIfOnGround;
	}

	return Height;
}

float JumpingEnemy::maxFrame() const
{
	if (alive)
	{
		return 9.0f;
	}

	return 16.0f;
}

StrongEnemy::StrongEnemy(const sf::Vector2f& pos)
	: Enemy(TextureFactory::getInstance().loadFromFile("Resources/Images/enemie4.png"), pos, 15)
{
	init();
}

float StrongEnemy::maxFrame() const
{
	if (alive)
	{
		return 10.0f;
	}

	return 22.0f;
}

int WeakEnemy::width()                 const { return 27; }
int WeakEnemy::height()                const { return 42; }
unsigned int WeakEnemy::moneyForHit()  const { return 10; }
float WeakEnemy::speed()               const { return 0.06f; }
float WeakEnemy::frameChangeSpeed()    const { return 0.015f; }

int DiagonalMovingEnemy::width()                 const { return 27; }
int DiagonalMovingEnemy::height()                const { return 42; }
unsigned int DiagonalMovingEnemy::moneyForHit()  const { return 15; }
float DiagonalMovingEnemy::frameChangeSpeed()    const { return 0.015f; }
float DiagonalMovingEnemy::speed()			     const { return 0.055f; }

unsigned int JumpingEnemy::moneyForHit()  const { return 20; }
int JumpingEnemy::width()				  const { return 114; }
float JumpingEnemy::frameChangeSpeed()    const { return 0.02f; }
float JumpingEnemy::speed()			      const { return 0.08f; }

int StrongEnemy::width()                 const { return 90; }
int StrongEnemy::height()                const { return 63; }
unsigned int StrongEnemy::moneyForHit()  const { return 30; }
float StrongEnemy::speed()               const { return 0.05f; }
float StrongEnemy::frameChangeSpeed()    const { return 0.015f; }

