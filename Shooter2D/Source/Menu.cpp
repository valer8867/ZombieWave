#include "Menu.h"
#include "Factory.h"

Menu::Menu(int width, int height, const sf::Vector2f& position, float indicatorRadius, const std::string& textureFilePath, const std::string& fontFilePath)
	: buttons(),
	  constTextlinesIndexes(),
	  pRect(new TextureRectangle(position.x, position.y, width, height, TextureFactory::getInstance().loadFromFile(textureFilePath))),
	  pFont(FontFactory::getInstance().loadFromFile(fontFilePath)),
	  pButtonsColor(new sf::Color),
	  pButtonsTextColor(new sf::Color(sf::Color::White)),
	  pCurrButtonIndicator(new sf::CircleShape(indicatorRadius)),
	  indicator(0),
	  buttonWidth(DefaultButtonWidth),
	  buttonHeight(DefaultButtonHeight),
	  fontSize(ButtontFontSize)
{ }

Menu::~Menu()
{
	delete pRect;
	delete pButtonsColor;
	delete pButtonsTextColor;
	delete pCurrButtonIndicator;

	std::for_each(buttons.begin(), buttons.end(), [](Button* pButton) { delete pButton; });
}

void Menu::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(*pRect, states);

	std::for_each(buttons.cbegin(), buttons.cend(), [&target, &states](Button* pButton) 
	{ 
		target.draw(*pButton->pRectnagle, states); 
		target.draw(*pButton->pText, states);
	});

	if (buttons.size() != constTextlinesIndexes.size() && !isIndicatorOnConstTextLine())
	{
		target.draw(*pCurrButtonIndicator, states);
	}
}

void Menu::down()
{
	if (buttons.size() != constTextlinesIndexes.size())
	{
		do
		{
			if (++indicator >= buttons.size())
			{
				indicator = 0;
			}
		} while (isIndicatorOnConstTextLine());

		moveIndicator();
	}
}

void Menu::up()
{
	if (buttons.size() != constTextlinesIndexes.size())
	{
		do
		{
			indicator ? indicator-- : indicator = buttons.size() - 1;
		} while (isIndicatorOnConstTextLine());

		moveIndicator();
	}
}

void Menu::onClick()
{
	buttons[indicator]->onClickFunction();
}

void Menu::setPosition(float x, float y)
{
	pRect->setPosition(x, y);
}

void Menu::setFontSize(unsigned char size)
{
	fontSize = size;
	
	for (auto& button : buttons)
	{
		button->pText->setCharacterSize(fontSize);
		setButtonTextPosition(button);
	}
}

void Menu::setButtonsSize(std::size_t width, std::size_t height)
{
	buttonWidth = width;
	buttonHeight = height;

	if (buttons.empty()) return;

	float distanceBetweenButtons, buttonRectX, buttonRectY;
	calcButtonsParams(distanceBetweenButtons, buttonRectX, buttonRectY);
	
	for (auto& button : buttons)
	{
		button->pRectnagle->setPosition(buttonRectX, buttonRectY);
		setButtonTextPosition(button);

		buttonRectY += (distanceBetweenButtons + buttonHeight);
	}

	moveIndicator();
}

void Menu::setButtonWidth(std::size_t indx, std::size_t width)
{
	Button* pButton = buttons[indx];

	pButton->pRectnagle->setWidth(width);
	pButton->pRectnagle->setPosition(static_cast<float>(pRect->getWidth() - pButton->pRectnagle->getWidth()) / 2.0f + pRect->getX(),
									 pButton->pRectnagle->getY());
	setButtonTextPosition(pButton);

	if (indicator == indx)
	{
		moveIndicator();
	}
}

void Menu::removeButtonBorder(std::size_t buttonIndx)
{
	buttons[buttonIndx]->pRectnagle->removeBorder();
}

void Menu::initButton(const std::string& text, std::function<void(void)> function, std::size_t indx)
{
	Button* pButton = buttons[indx];

	pButton->pText->setString(text);
	pButton->onClickFunction = function;
	setButtonTextPosition(pButton);
}

void Menu::setButtonTextPosition(Button* pButton)
{
	const auto& textSize = pButton->pText->getLocalBounds();
	const auto& buttonRectPos = pButton->pRectnagle->getPosition();

	pButton->pText->setPosition(buttonRectPos.x + static_cast<float>(pButton->pRectnagle->getWidth() - textSize.width) / 2.0f,
		                        buttonRectPos.y + static_cast<float>(buttonHeight - fontSize) / 2.0f);
}

void Menu::calcButtonsParams(float& distance, float& buttonsX, float& firstButtonY)
{
	distance = static_cast<float>(pRect->getHeight() - (buttons.size() * buttonHeight)) / static_cast<float>(buttons.size() + 1);
	buttonsX = static_cast<float>(pRect->getWidth() - buttonWidth) / 2.0f + pRect->getX();
	firstButtonY = distance + pRect->getY();
}

// T is either TextLine or Button
template <typename T>
void Menu::createMenuObjectsImpl(std::size_t count)
{
	auto prevSize = buttons.size();
	buttons.insert(buttons.cend(), count, nullptr);

	float distanceBetweenButtons, buttonRectX, buttonRectY;
	calcButtonsParams(distanceBetweenButtons, buttonRectX, buttonRectY);

	for (std::size_t i = 0; i < buttons.size(); i++, buttonRectY += (buttonHeight + distanceBetweenButtons))
	{
		Button*& pButton = buttons[i];

		if (i >= prevSize)
		{
			pButton = new T; // T is either TextLine or Button
			pButton->pRectnagle = new Rectangle(buttonRectX, buttonRectY, buttonWidth, buttonHeight, *pButtonsColor, ButtonBorderSize, sf::Color::White/*buttonBorderColor*/);
			pButton->pText = new sf::Text("", *pFont, fontSize);
			pButton->pText->setFillColor(*pButtonsTextColor);
		}
		else
		{
			pButton->pRectnagle->setPosition(static_cast<float>(pRect->getWidth() - pButton->pRectnagle->getWidth()) / 2.0f + pRect->getX(), 
				                             buttonRectY);
		}

		setButtonTextPosition(pButton);
	}

	moveIndicator();
}

template <typename T> // T is either Button or TextLine
void Menu::createMenuObjects(std::size_t count)
{
	if (count)
	{
		createMenuObjectsImpl<T>(count);

		if (buttons.size() != constTextlinesIndexes.size() && isIndicatorOnConstTextLine())
		{
			down();
		}
	}
}

void Menu::createButtons(std::size_t buttonsNumber)
{
	createMenuObjects<Button>(buttonsNumber);
}

void Menu::setButtonsColor(const sf::Color& color)
{
	*pButtonsColor = color;
	std::for_each(buttons.begin(), buttons.end(), [&color](Button* pButton) { pButton->pRectnagle->setColor(color); });
}

void Menu::setButtonsTextColor(const sf::Color& color)
{
	*pButtonsTextColor = color;
	std::for_each(buttons.begin(), buttons.end(), [&color](Button* pButton) { pButton->pText->setFillColor(color); });
}

void Menu::moveIndicator()
{
	if (buttons.empty())
	{
		return;
	}

	const auto& curButtonPos = buttons[indicator]->pRectnagle->getPosition();
	float radius = pCurrButtonIndicator->getRadius();

	pCurrButtonIndicator->setPosition(curButtonPos.x - 3.0f * radius, curButtonPos.y + static_cast<float>(buttonHeight) * 0.5f - radius);
}

Menu::Button::~Button()
{
	delete pText;
	delete pRectnagle;
}

void Menu::TextLine::addSymbol(char c)
{
	std::string text = pText->getString();

	if (c == '\b' && !text.empty())
	{
		text.erase(--text.end());
	}
	else
	{
		text += c;
	}

	pText->setString(text);

	while (pText->getLocalBounds().width >= pRectnagle->getWidth() && pText->getCharacterSize())
	{
		pText->setCharacterSize(pText->getCharacterSize() - 1);
	}
}

void Menu::createTextLines(std::size_t linesNumber)
{
	createMenuObjects<TextLine>(linesNumber);
}

void Menu::textEntered(char c)
{
	if (buttons.empty()) return;

	TextLine* pTextLine = dynamic_cast<TextLine*>(buttons[indicator]);
	auto indx = indicator;

	if (pTextLine && !isIndicatorOnConstTextLine())
	{
		pTextLine->addSymbol(c);
		setButtonTextPosition(pTextLine);
	}
}

void Menu::setTextureCoord(float x, float y)
{
	pRect->setTextureCoord(x, y, static_cast<float>(pRect->getWidth()), static_cast<float>(pRect->getHeight()));
}

void Menu::setTextLineStr(std::size_t indx, const std::string& text)
{
	if (buttons.empty()) return;

	TextLine* pTextLine = dynamic_cast<TextLine*>(buttons[indx]);
	if (pTextLine && !isIndicatorOnConstTextLine())
	{
		pTextLine->pText->setString(text);
		pTextLine->pText->setCharacterSize(fontSize);
		setButtonTextPosition(pTextLine);
	}
}

std::string Menu::getTextLineStr(std::size_t indx)
{
	if (buttons.empty()) return "";

	TextLine* pTextLine = dynamic_cast<TextLine*>(buttons[indx]);
	if (pTextLine)
	{
		return pTextLine->pText->getString();
	}

	return "";
}

void Menu::createConstTextLines(std::size_t linesNumber)
{
	if (linesNumber)
	{
		auto size = buttons.size();
		for (std::size_t i = 0; i < linesNumber; i++)
		{
			constTextlinesIndexes.emplace_back(size + i);
		}

		createMenuObjectsImpl<TextLine>(linesNumber);
	}
}