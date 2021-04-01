#pragma once

#include <SFML/Graphics.hpp>

class Border : public sf::Drawable
{
public:
	Border(std::size_t size, sf::Color borderColor);

	void createBorderAround(const sf::VertexArray& vao) { setBorder(vao.getBounds()); }
	void createBorderAround(const sf::Sprite& sprite) { setBorder(sprite.getGlobalBounds()); }

private:
	void setBorder(const sf::FloatRect& rectangle);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	std::vector<sf::VertexArray> border;
	std::size_t borderSize;
};
