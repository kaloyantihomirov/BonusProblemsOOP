#include "Baloon.h"
#include "Alcohol.h"
#include "Narga.h"
#include "Client.h"

#pragma once

class NightClub
{
private:
	Baloon baloons[1024];
	Alcohol alcohols[512];
	Narga nargas[256];

	int currBaloonsCount = 0;
	int currAlcoholsCount = 0;
	int currNargasCount = 0;

	void addAlcoholToBill(double& total, const char* line) const;
	void addBaloonToBill(double& total, const char* line) const;
	void addNargaToBill(double& total, const char* line) const;
public:
	NightClub() = default;

	//returns the index of the first occurrence 
	//of a baloon with the given colour
	//if such baloon does not exist, 
	//the result is -1
	int findBaloonByColour(Colour colour) const;

	//int findNargaByBrandAndFlavour(const char * brand, Flavour flavour) const;

	int findNarga(const SearchBy& criteria,
		Flavour flavour,
		const char* brand = "Default brand") const;
	int findAlcoholByName(const char* name) const;
	bool addBaloon(Baloon baloon);
	bool addNarga(Narga narga);
	bool addAlcohol(Alcohol alcohol);
	bool removeBaloon(Colour colour);
	bool removeNarga(const char* brand, Flavour searchedFlav);
	bool removeAlcohol(const char* name);
	double calculateClientTotal(const Client& client) const;
	double dailyTotal(Client* clients, int clientsCount) const;

	//double calculateClientTotal(const Client& client);
};