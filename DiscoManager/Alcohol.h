#pragma once	

class Alcohol
{
private:
	char name[128 + 1] = "default name";
	double price = 0;
public:
	Alcohol() = default;
	Alcohol(const char*, double price);
	double getPrice() const;
	void setPrice(double price);
	const char* getName() const;
	void setName(const char*);
};
