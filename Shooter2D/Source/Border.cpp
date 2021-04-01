#include "Border.h"

#include <vector>

Border::Border(std::size_t size, sf::Color borderColor)
	: border(),
	  borderSize(size)
{
	for (std::size_t i = 0; i < borderSize; i++)
	{
		border.emplace_back(sf::VertexArray(sf::LineStrip, 5));

		for (std::size_t j = 0; j < 5; j++)
		{
			border[i][j].color = borderColor;
		}
	}
}

void Border::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (auto vao = border.cbegin(); vao != border.cend(); ++vao)
	{
		target.draw(*vao, states);
	}
}

void Border::setBorder(const sf::FloatRect& rectangle)
{
	for (std::size_t i = 0; i < borderSize; i++)
	{
		border[i][0].position = sf::Vector2f(rectangle.left + 1 + i,                   rectangle.top + 1 + i);
		border[i][1].position = sf::Vector2f(rectangle.left + rectangle.width + 1 + i, rectangle.top + 1 + i);
		border[i][2].position = sf::Vector2f(rectangle.left + rectangle.width + 1 + i, rectangle.top + rectangle.height + 1 + i);
		border[i][3].position = sf::Vector2f(rectangle.left + 1 + i,                   rectangle.top + rectangle.height + 1 + i);
		border[i][4].position = sf::Vector2f(rectangle.left + 1 + i,                   rectangle.top + 1 + i);
	}
}