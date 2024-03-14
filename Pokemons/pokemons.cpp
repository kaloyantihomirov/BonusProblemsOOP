#include <iostream>
#include <fstream>
#include <cstring>

namespace GlobalConstants
{
	const int MAX_NAME_LENGTH = 50;
	const int DEFAULT_POWER_VALUE = 500;
}

enum class e_Type
{
	NORMAL,
	FIRE,
	WATER,
	GRASS,
	ELECTRIC,
	GHOST,
	FLYING,
	s_number_of_values
};

struct Pokemon
{
	char Name[GlobalConstants::MAX_NAME_LENGTH];

	e_Type Type;

	unsigned Power;
};

void writePokemon(const Pokemon& p, std::ostream& is)
{
	is << p.Name << " " << (int)p.Type << " " << p.Power << std::endl;
}

Pokemon createPokenom(
	const char* name,
	const e_Type type,
	int power)
{
	Pokemon p;

	if (strlen(name) > GlobalConstants::MAX_NAME_LENGTH)
	{
		std::cerr << "The name will be truncated, as we couldn't store all of the given symbols" << std::endl;
	}
	strcpy_s(p.Name, GlobalConstants::MAX_NAME_LENGTH, name);

	p.Type = type;

	if (power < 10 || power > 1000)
	{
		power = GlobalConstants::DEFAULT_POWER_VALUE;
		std::cerr << "The given power value was not in the accepted range, so a default value of " << GlobalConstants::DEFAULT_POWER_VALUE << " was used." << std::endl;
	}

	p.Power = power;

	return p;
}

Pokemon readPokemon(std::istream& is)
{
	char name[GlobalConstants::MAX_NAME_LENGTH];
	is.getline(name, GlobalConstants::MAX_NAME_LENGTH, ' ');

	int type, power;
	is >> type >> power;

	e_Type typeAsEnum;

	if (type < 0 || type >= (int)e_Type::s_number_of_values)
	{
		std::cerr << "The provided type was not correct, so the default 0 was used." << std::endl;
		typeAsEnum = (e_Type)0;
	}
	else
	{
		typeAsEnum = (e_Type)type;
	}

	return createPokenom(name, typeAsEnum, power);
}

void writePokenomToBinary(const Pokemon& p, std::ostream& of)
{
	of.write((const char*)&p, sizeof(p));
}

Pokemon readPokemonFromBinary(std::istream& is)
{
	Pokemon p;

	is.read((char*)&p, sizeof(Pokemon));

	return p;
}

struct PokemonHandler
{
	const char* FileName;

	//it is a good idea to cache the size
    //and increment it each time we insert a pokemon
    //as we would use it quite often
	int FileSize = 0;
};

PokemonHandler newPokemonHandler(const char* filename)
{
	PokemonHandler ph;

	ph.FileName = filename;

	ph.FileSize = 0;

	return ph;
}

int size(PokemonHandler& ph)
{
	std::ifstream ifs(ph.FileName, std::ios::binary);

	if (!ifs.is_open())
	{
		return -1;
	}

	ifs.seekg(0, std::ios::end);

	int size = ifs.tellg() / sizeof(Pokemon);

	ph.FileSize = size;

	return size;
}

void setAt(int pos, const Pokemon& pok, std::ostream& os)
{
	if (pos < 0)
	{
		return;
	}

	os.seekp(pos * sizeof(Pokemon), std::ios::beg);

	os.write((const char*)&pok, sizeof(Pokemon));
}

void readPokemonAt(int pos, Pokemon& pok, std::istream& is)
{
	is.seekg(pos * sizeof(Pokemon), std::ios::beg);

	is.read((char*)&pok, sizeof(Pokemon));
}

Pokemon at(const PokemonHandler& ph, int i)
{
	std::ifstream ifs(ph.FileName, std::ios::binary | std::ios::in);

	if (!ifs.is_open() || i < 0 || i >= ph.FileSize)
	{
		std::cerr << "An error occurred! " << std::endl;
		return {};
	}

	Pokemon soughtPokemon;

	readPokemonAt(i, soughtPokemon, ifs);

	ifs.close();

	return soughtPokemon;
}

void swap(const PokemonHandler& ph, int i, int j)
{
	if (i < 0 || i >= ph.FileSize || j < 0 || j >= ph.FileSize)
	{
		std::cerr << "An error occurred!" << std::endl;
		return;
	}

	if (i == j)
	{
		return;
	}

	Pokemon pi = at(ph, i);
	Pokemon pj = at(ph, j);

	std::ofstream ofs(ph.FileName, std::ios::binary);

	if (!ofs.is_open())
	{
		std::cerr << "An error occurred!" << std::endl;
		return;
	}

	setAt(i, pj, ofs);
	
	setAt(j, pi, ofs);

	ofs.close();
}

void textify(const PokemonHandler& ph, const char* filename)
{
	//std::cout << ph.FileSize << std::endl;
	std::ifstream ifs(ph.FileName, std::ios::binary | std::ios::in);

	if (!ifs.is_open())
	{
		return;
	}

	std::ofstream ofs(filename);

	if (!ofs.is_open())
	{
		return;
	}

	int count = 0;
	while (count < ph.FileSize)
	{
		Pokemon p = readPokemonFromBinary(ifs);

		writePokemon(p, std::cout);

		//this does not work
		if (!ifs)
		{
			break;
		}

		writePokemon(p, ofs);
		count++;
	}
}

void findPokemonPos(int initialSize, 
	const Pokemon& pokemon, 
	const PokemonHandler& ph, 
	std::ostream& ofs)
{
	int i = initialSize - 1;

	while (i > 0 && pokemon.Power < at(ph, i).Power)
	{
		ofs.seekp((i + 1) * sizeof(Pokemon), std::ios::beg);

		Pokemon temp = at(ph, i);
		ofs.write((const char*)&temp, sizeof(Pokemon));

		i--;
	}

	setAt(i + 1, pokemon, ofs);
}

void insert(PokemonHandler& ph, const Pokemon& pokemon)
{
	//first, we add the pokemon to the end of the file and then 
	//find its place
	//so the file is still sorted (insertion sort would work fine)

	int initialSize = ph.FileSize;

	std::ofstream ofs(ph.FileName, std::ios::binary | std::ios::in);

	if (!ofs.is_open())
	{
		std::cerr << "An error occurred!" << std::endl;
	}

	setAt(initialSize * sizeof(Pokemon), pokemon, ofs);

	ph.FileSize++;
	
	findPokemonPos(initialSize, pokemon, ph, ofs);

	ofs.close();
}

void untextify(PokemonHandler& ph, const char* filename)
{
	std::ifstream ifs(filename);

	if (!ifs.is_open())
	{
		return;
	}

	ph.FileSize = 0;
	std::ofstream ofs(ph.FileName);
	ofs.close();

	while (true)
	{
		Pokemon p = readPokemon(ifs);

		if (ifs.eof())
		{
			break;
		}

		insert(ph, p);
	}

	return;
}


int main()
{
	PokemonHandler ph = newPokemonHandler("ph.dat");

	Pokemon p1 = createPokenom("P1", e_Type::GHOST, 400);
	Pokemon p2 = createPokenom("P2", e_Type::FIRE, 600);
	Pokemon p3 = createPokenom("P3", e_Type::GHOST, 620);
	Pokemon p4 = createPokenom("P4", e_Type::GHOST, 700);

	insert(ph, p1);
	insert(ph, p2);
	insert(ph, p3);
	insert(ph, p4);

	Pokemon p5 = createPokenom("P5", e_Type::GHOST, 444);	

	insert(ph, p5);

	textify(ph, "pokemons.txt");
}