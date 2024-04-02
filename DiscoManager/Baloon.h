#pragma once

enum class Colour
{
	White,
	Green,
	Red,
	Default
};

class Baloon
{
private:
	double price = 0;
	Colour colour = Colour::Default;
public:
	Baloon() = default;
	Baloon(double price, Colour colour);
	double getPrice() const;
	void setPrice(double newPrice);
	Colour getColour() const;
	void setColour(Colour newColour);
};