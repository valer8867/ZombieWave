#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <unordered_map>
#include <algorithm>

template <typename T>
class Factory;

using TextureFactory = Factory<sf::Texture>;
using FontFactory = Factory<sf::Font>;

template <typename T>
class Factory
{
public:
	Factory(const Factory&) = delete;
	Factory& operator=(const Factory&) = delete;
	Factory(Factory&&) = delete;
	Factory& operator=(Factory&&) = delete;

	~Factory()
	{
		std::for_each(cash.begin(), cash.end(), [](std::pair<const std::string, T*>& pair) { delete pair.second; });
	}

	static Factory& getInstance()
	{
		static Factory factory;
		return factory;
	}

	T* loadFromFile(const std::string& filePath)
	{
		auto it = cash.find(filePath);
		if (it != cash.end())
		{
			return it->second;
		}

		T* pObj = new T;
		if (!pObj->loadFromFile(filePath))
		{
			throw std::runtime_error("Can't load object from " + filePath);
		}

		cash.emplace(filePath, pObj);

		return pObj;
	}
	
private:
	Factory() { }
	
private:
	std::unordered_map<std::string, T*> cash;
};