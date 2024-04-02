#pragma once

struct SearchBy
{
	bool searchByBrand = false;
	bool searchByFlavour = true;
};

enum class Flavour
{
	Berry,
	Queen,
	Mango,
	Mint,
	Strawberry,
	DefaultFlavour
};

class Narga
{
private:
	double price = 0;
	char brand[1024] = "Default brand";
	Flavour flavour = Flavour::DefaultFlavour;
public:
	Narga() = default;;
	Narga(double price, const char* brand, Flavour flavour);
	double getPrice() const;
	void setPrice(double price);
	const char* getBrand() const;
	void setBrand(const char* newBrand);
	Flavour getFlavour() const;
	void setFlavour(Flavour newFlavour);
	bool checkIfNargaSatisfiesACondition(bool(*predicate)(const Narga& narga)) const;
};