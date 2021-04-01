#pragma once

#include <SFML/Graphics.hpp>

class Border;

class Rectangle : public sf::Drawable
{
public:
	Rectangle(float left, float top, int width, int height, const sf::Color& color, std::size_t borderSize = 0, const sf::Color& borderColor = sf::Color::White);
	Rectangle(const sf::Vector2f& position_, const sf::Vector2i& size, const sf::Color& color, std::size_t borderSize = 0, const sf::Color& borderColor = sf::Color::White);
	virtual ~Rectangle();
	
	void removeBorder();
	void setWidth(int width);
	void setHeight(int height);
	void resize(int width, int height);
	void setColor(const sf::Color& color);
	void setPosition(float left, float top);

	int getWidth() const { return width; }
	int getHeight() const { return height; }
	float getY() const { return position.y; }
	float getX() const { return position.x; }
	sf::Vector2f getPosition() const { return position; }
	sf::Vector2i getSize() const { return { width, height }; }

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Vector2f position;
	int width;
	int height;

protected:
	Border* pBorder;
	sf::VertexArray* pRectangle;
};

class TextureRectangle : public Rectangle
{
public:
	TextureRectangle(float left, float top, int width, int height, sf::Texture* pTexture, std::size_t borderSize = 0,  const sf::Color& borderColor = sf::Color::White);
	TextureRectangle(const sf::Vector2f& position_, const sf::Vector2i& size, sf::Texture* pTexture, std::size_t borderSize = 0, const sf::Color& borderColor = sf::Color::White);

	void setTextureCoord(float x, float y, float width, float height);

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Texture* pTexture;
};
