#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>

#include "Alcohol.h"
#include "Baloon.h"
#include "Narga.h"
#include "NightClub.h"
#include "Client.h"

#pragma warning(disable : 4996)

double Alcohol::getPrice() const
{
	return price;
}

void Alcohol::setPrice(double price) 
{
	if (price < 0)
	{
		price = 0;
		return;
	}

	this->price = price;
}

const char* Alcohol::getName() const
{
	return name;
}

void Alcohol::setName(const char* newName)
{
	if (strlen(newName) > 128)
	{
		strcpy(name, "Default name");
		return;
	}

	strcpy(name, newName);
}

Alcohol::Alcohol(const char* newName, double price)
{
	setName(newName);
	setPrice(price);
}

double Baloon::getPrice() const
{
	return price;
}

void Baloon::setPrice(double newPrice)
{
	if (price < 0)
	{
		price = 0;
		return;
	}

	price = newPrice;
}

Colour Baloon::getColour() const
{
	return colour;
}

void Baloon::setColour(Colour newColour)
{
	colour = newColour;
}

Baloon::Baloon(double price, Colour colour)
{
	setPrice(price);
	setColour(colour);
}

double Narga::getPrice() const
{
	return price;
}

void Narga::setPrice(double newPrice) 
{
	if (newPrice < 0)
	{
		price = 0;
		return;
	}

	price = newPrice;
}

const char* Narga::getBrand() const
{
	return brand;
}

void Narga::setBrand(const char* newBrand)
{
	if (strlen(brand) > 1024)
	{
		strcpy_s(brand, "Default brand");
		return;
	}

	strcpy_s(brand, newBrand);
}

Flavour Narga::getFlavour() const
{
	return flavour;
}

void Narga::setFlavour(Flavour newFlavour)
{
	flavour = newFlavour;
}

Narga::Narga(double price, const char* brand, Flavour flavour)
{
	setPrice(price);
	setBrand(brand);
	setFlavour(flavour);
}

bool Narga::checkIfNargaSatisfiesACondition(
	bool (*predicate)(const Narga& narga)) const
{
	return (predicate(*this));
}

bool NightClub::addAlcohol(Alcohol alcohol)
{
	if (currAlcoholsCount == 512)
	{
		return false;
	}

	alcohols[currAlcoholsCount++] = alcohol;
	return true;
}

bool NightClub::addBaloon(Baloon baloon)
{
	if (currBaloonsCount == 1024)
	{
		return false;
	}

	baloons[currBaloonsCount++] = baloon;
	return true;
}

bool NightClub::addNarga(Narga narga)
{
	if (currBaloonsCount == 256)
	{
		return false;
	}

	nargas[currNargasCount++] = narga;
	return true;
}

int NightClub::findBaloonByColour(Colour colour) const
{
	for (int i = 0; i < currBaloonsCount; i++)
	{
		if (baloons[i].getColour() == colour)
		{
			return i;
		}
	}

	return -1;
}

bool NightClub::removeBaloon(Colour colour)
{
	int baloonIndex = findBaloonByColour(colour);

	if (baloonIndex == -1)
	{
		//no such baloon exists (with the given colour)
		return false;
	}

	std::swap(baloons[baloonIndex], baloons[currBaloonsCount - 1]);
	currBaloonsCount--;
	return true;
}

//int NightClub::findNargaByBrandAndFlavour(const char* brand, Flavour flavour) const
//{
//	for (int i = 0; i < currNargasCount; i++)
//	{
//		if (strcmp(nargas[i].getBrand(), brand) == 0 &&
//			nargas[i].getFlavour() == flavour)
//		{
//			return i;
//		}
//	}
//
//	return -1;
//}

int NightClub::findNarga(const SearchBy& criteria, 
	Flavour flavour, 
	const char * brand) const
{
	for (int i = 0; i < currNargasCount; i++)
	{
		bool areFlavoursSame = criteria.searchByFlavour
			? nargas[i].getFlavour() == flavour 
			: true;
		bool areBrandsSame = criteria.searchByBrand 
			? strcmp(nargas[i].getBrand(), brand) == 0
			: true;

		if (areFlavoursSame && areBrandsSame)
		{
			return i;
		}
	}

	return -1;
}

bool NightClub::removeNarga(const char* brand, Flavour searchedFlav)
{
	//int nargaIndex = findNargaByBrandAndFlavour(brand, searchedFlav);

	/* 
	A more generic approach
	*/ 
	
	SearchBy criteria;
	criteria.searchByBrand = true;
	criteria.searchByFlavour = true;

	int nargaIndex = findNarga(criteria, searchedFlav, brand);
	
	if (nargaIndex == -1)
	{
		return false;
	}

	std::swap(nargas[nargaIndex], nargas[currNargasCount - 1]);
	currNargasCount--;
	return true;
}

int NightClub::findAlcoholByName(const char* name) const
{
	for (int i = 0; i < currAlcoholsCount; i++)
	{
		if (strcmp(alcohols[i].getName(), name) == 0)
		{
			return i;
		}
	}

	return -1;
}

bool NightClub::removeAlcohol(const char* name)
{
	int alcoholIndex = findAlcoholByName(name);

	if (alcoholIndex == -1)
	{
		return false;
	}

	std::swap(alcohols[alcoholIndex], alcohols[currAlcoholsCount - 1]);
	currAlcoholsCount--;
	return true;
}

Client::Client(const char* _name, const char* _billFilename)
{
	name = new char[strlen(_name) + 1];
	strcpy(name, _name);

	billFilename = new char[strlen(_billFilename) + 1];
	strcpy(billFilename, _billFilename);
}

Client::~Client()
{
	delete[] name;
	delete[] billFilename;
}

const char * Client::getBillFilename() const
{
	return billFilename;
}

const char* Client::getName() const
{
	return name;
}

void NightClub::addAlcoholToBill(double& total, const char* line) const
{
	std::stringstream ss(line);
	char alcoholName[128 + 1];

	ss.getline(alcoholName, 128 + 1, '-');

	int quantity;
	ss >> quantity;

	int alcoholIndex = findAlcoholByName(alcoholName);

	double alcoholPrice = alcoholIndex == -1
		? 0
		: alcohols[alcoholIndex].getPrice();

	total += quantity * alcoholPrice;
}

void NightClub::addBaloonToBill(double& total, const char* line) const
{
	std::stringstream ss(line);
	char baloonColourAsStr[20];

	ss.getline(baloonColourAsStr, 20, '-');

	Colour colourAsEnum = strToColour(baloonColourAsStr);

	int quantity;
	ss >> quantity;

	int baloonIndex = findBaloonByColour(colourAsEnum);

	double baloonPrice = baloonIndex == -1
		? 0
		: baloons[baloonIndex].getPrice();

	total += quantity * baloonPrice;
}

void NightClub::addNargaToBill(double& total, const char* line) const
{
	std::stringstream ss(line);
	char nargaBrand[1024];

	ss.getline(nargaBrand, 1024, '-');

	char nargaFlavour[32];
	ss.getline(nargaFlavour, 32, '-');

	Flavour flavourAsEnum = strToFlavour(nargaFlavour);

	int quantity;
	ss >> quantity;

	SearchBy criteria;
	criteria.searchByBrand = true;
	criteria.searchByFlavour = true;
	int nargaIndex = findNarga(criteria, flavourAsEnum, nargaBrand);

	double nargaPrice = nargaIndex == -1
		? 0
		: nargas[nargaIndex].getPrice();

	total += quantity * nargaPrice;
}

double NightClub::calculateClientTotal(const Client& client) const
{
	std::ifstream in(client.getBillFilename());

	if (!in.is_open())
	{
		return -1;
	}

	double total = 0;

	char line[2048]; 
	char lastMetCategory[256] = "";

	while (in.getline(line, 2048))
	{
		if (strcmp(line, "Alcohols") == 0 ||
			strcmp(line, "Baloons") == 0  ||
		    strcmp(line, "Narga") == 0)
		{
			strcpy(lastMetCategory, line);
		}
		else
		{
			if (strcmp(lastMetCategory, "Alcohols") == 0)
			{
				addAlcoholToBill(total, line);
			}
			else if (strcmp(lastMetCategory, "Baloons") == 0)
			{
				addBaloonToBill(total, line);
			}
			else if (strcmp(lastMetCategory, "Narga") == 0)
			{
				addNargaToBill(total, line);
			}
		}
	}

	return total;
}

double NightClub::dailyTotal(Client* clients, int clientsCount) const
{
	double dailyTotal = 0;

	std::ofstream out("money.dat", std::ios::binary);

	if (!out.is_open())
	{
		return -1;
	}

	for (int i = 0; i < clientsCount; i++)
	{
		double clientTotal = calculateClientTotal(clients[i]);
		const char* clientName = clients[i].getName();

		out.write(clientName, strlen(clientName));
		out.write((const char*)&clientTotal, sizeof(clientTotal));

		dailyTotal += clientTotal;
	}

	out.write((const char*)&dailyTotal, sizeof(dailyTotal));

	return dailyTotal;
}

Colour strToColour(const char* str)
{
	if (strcmp(str, "green"))
	{
		return Colour::Green;
	}
	else if (strcmp(str, "white"))
	{
		return Colour::White;
	}
	else if (strcmp(str, "red"))
	{
		return Colour::Red;
	}
	else
	{
		return Colour::Default;
	}
}

Flavour strToFlavour(const char* str)
{
	if (strcmp(str, "berry"))
	{
		return Flavour::Berry;
	}
	else if (strcmp(str, "queen"))
	{
		return Flavour::Queen;
	}
	else if (strcmp(str, "mango"))
	{
		return Flavour::Mango;
	}
	else if (strcmp(str, "mint"))
	{
		return Flavour::Mint;
	}
	else if (strcmp(str, "strawberry"))
	{
		return Flavour::Strawberry;
	}
	else
	{
		return Flavour::DefaultFlavour;
	}
}

bool readBaloon(Baloon& baloon, std::istream& in)
{
	char baloonColour[20];

	in.getline(baloonColour, 20, '-');

	Colour colourAsEnum = strToColour(baloonColour);

	double price;

	in >> price;

	if (!in)
	{
		return false;
	}

	baloon.setPrice(price);
	baloon.setColour(colourAsEnum);
	return true;
}

bool readAlcohol(Alcohol& alcohol, std::istream& in)
{
	char alcoholName[128 + 1];

	in.getline(alcoholName, 128 + 1, '-');

	double price;

	in >> price;

	if (!in)
	{
		return false;
	}

	alcohol.setPrice(price);
	alcohol.setName(alcoholName);
	return true;
}

bool readNarga(Narga& narga, std::istream& in)
{
	char brand[1024 + 1];

	in.getline(brand, 1024 + 1, '-');

	char flavourAsStr[32];

	in.getline(flavourAsStr, 32, '-');

	Flavour flavourAsEnum = strToFlavour(flavourAsStr);

	double price;

	in >> price;

	if (!in)
	{
		return false;
	}

	narga.setPrice(price);
	narga.setBrand(brand);
	narga.setFlavour(flavourAsEnum);
	return true;
}

int main()
{
	return 0;
}

