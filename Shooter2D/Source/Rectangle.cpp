#include "Rectangle.h"
#include "Border.h"

#include <SFML/System.hpp>

Rectangle::Rectangle(float left, float top, int width_, int height_, const sf::Color& color, std::size_t borderSize, const sf::Color& borderColor)
	: position(left, top),
	  width(width_),
	  height(height_),
	  pBorder(new Border(borderSize, borderColor)),
	  pRectangle(new sf::VertexArray(sf::Quads, 4))
{
	setPosition(left, top);

	for (int i = 0; i < 4; i++)
	{
		pRectangle[0][i].color = color;
	}

	pBorder->createBorderAround(*pRectangle);
}

Rectangle::Rectangle(const sf::Vector2f& position_, const sf::Vector2i& size, const sf::Color& color, std::size_t borderSize, const sf::Color& borderColor)
	: Rectangle(position_.x, position_.y, size.x, size.y, color, borderSize, borderColor)
{ }

Rectangle::~Rectangle()
{
	delete pRectangle;
	delete pBorder;
}

void Rectangle::setPosition(float left, float top)
{
	position = { left, top };

	pRectangle[0][0].position = sf::Vector2f(left, top);
	pRectangle[0][1].position = sf::Vector2f(left + width, top);
	pRectangle[0][2].position = sf::Vector2f(left + width, top + height);
	pRectangle[0][3].position = sf::Vector2f(left, top + height);
	
	if (pBorder)
	{
		pBorder->createBorderAround(*pRectangle);
	}
}

void Rectangle::resize(int width_, int height_)
{
	width = width_;
	height = height_;

	setPosition(position.x, position.y);
}

void Rectangle::setWidth(int width_)
{
	width = width_;

	pRectangle[0][1].position = sf::Vector2f(position.x + width, position.y);
	pRectangle[0][2].position = sf::Vector2f(position.x + width, position.y + height);

	if (pBorder)
	{
		pBorder->createBorderAround(*pRectangle);
	}
}

void Rectangle::setHeight(int height_)
{
	height = height_;

	pRectangle[0][2].position = sf::Vector2f(position.x + width, position.y + height);
	pRectangle[0][3].position = sf::Vector2f(position.x, position.y + height);

	if (pBorder)
	{
		pBorder->createBorderAround(*pRectangle);
	}
}			

void Rectangle::setColor(const sf::Color& color)
{
	for (int i = 0; i < 4; i++)
	{
		pRectangle[0][i].color = color;
	}
}

void Rectangle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(*pRectangle, states);

	if (pBorder)
	{
		target.draw(*pBorder, states);
	}
}

TextureRectangle::TextureRectangle(float left, float top, int width, int height, sf::Texture* pTex, std::size_t borderSize, const sf::Color& borderColor)
	: Rectangle(left, top, width, height, sf::Color::White, borderSize, borderColor),
	  pTexture(pTex)
{
	setTextureCoord(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
}

TextureRectangle::TextureRectangle(const sf::Vector2f& position_, const sf::Vector2i& size, sf::Texture* pTex, std::size_t borderSize, const sf::Color& borderColor)
	: Rectangle(position_, size, sf::Color::White, borderSize, borderColor),
	  pTexture(pTex)
{
	setTextureCoord(0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y));
}

void TextureRectangle::setTextureCoord(float x, float y, float width, float height)
{
	pRectangle[0][0].texCoords = sf::Vector2f(x, y);
	pRectangle[0][1].texCoords = sf::Vector2f(x + width, y);
	pRectangle[0][2].texCoords = sf::Vector2f(x + width, y + height);
	pRectangle[0][3].texCoords = sf::Vector2f(x, y + height);
}

void TextureRectangle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (pBorder)
	{
		target.draw(*pBorder, states);
	}

	states.texture = pTexture;
	target.draw(*pRectangle, states);
}

void Rectangle::removeBorder()
{
	delete pBorder;
	pBorder = nullptr;
}
