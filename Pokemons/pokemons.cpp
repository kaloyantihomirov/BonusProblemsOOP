#include <iostream>
#include <fstream>
#include <cstring>

namespace GlobalConstants
{
    const int MAX_NAME_LENGTH = 50;
    const int MAX_TYPE_LENGTH = 40;
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
};

struct Pokemon
{
    char Name[GlobalConstants::MAX_NAME_LENGTH];

    e_Type Type;

    unsigned Power;
};

void printPokemon(const Pokemon& p)
{
    std::cout << "Pokemon name: " << p.Name << std::endl;
    std::cout << "Pokemon type: " << (int)p.Type << std::endl;
    std::cout << "Pokemon power: " << p.Power << std::endl;
}

Pokemon createPokenom(
    const char * name,
    const e_Type type, 
    unsigned power)
{
    Pokemon p;

    strcpy_s(p.Name, GlobalConstants::MAX_NAME_LENGTH, name);
    p.Type = type;
    p.Power = power;

    return p;
}

Pokemon readPokemon(std::istream& is)
{
    Pokemon p;

    int type;

    is >> p.Name >> type >> p.Power;

    p.Type = (e_Type)type;

    return p;
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
};

PokemonHandler newPokemonHandler(const char* filename)
{
    PokemonHandler ph;
    ph.FileName = "sorted.dat";

    std::ifstream ifs(filename, std::ios::binary | std::ios::in);
    std::ofstream ofs(ph.FileName, std::ios::binary | std::ios::out);   

    Pokemon pokemon;
    Pokemon previousPokemon;

    bool first = true;

    while (ifs)
    {
         pokemon = readPokemonFromBinary(ifs);

         writePokenomToBinary(pokemon, ofs);

         if (!first && pokemon.Power < previousPokemon.Power)
         {
             ofs.seekp(-2 * sizeof(Pokemon), std::ios::cur);
             writePokenomToBinary(previousPokemon, ofs);
             ofs.write((const char*)&pokemon, sizeof(Pokemon));
         }

         previousPokemon = pokemon;
         first = false;
    }

    ifs.close();
    ofs.close();

    return ph;
}

int main()
{
    Pokemon p1 = createPokenom("P1", e_Type::ELECTRIC, 800);
    Pokemon p2 = createPokenom("P2", e_Type::ELECTRIC, 700);
    Pokemon p3 = createPokenom("P3", e_Type::ELECTRIC, 100);
    Pokemon p4 = createPokenom("P4", e_Type::ELECTRIC, 500);

    std::ofstream ofs("pokemons.dat",
        std::ios::binary | std::ios::out);

    writePokenomToBinary(p1, ofs);
    writePokenomToBinary(p2, ofs);
    writePokenomToBinary(p3, ofs);
    writePokenomToBinary(p4, ofs);

    ofs.close();
    PokemonHandler ph = newPokemonHandler("pokemons.dat");

    std::ifstream ifs("sorted.dat", std::ios::binary | std::ios::in);

    Pokemon Pokemons[4];

    ifs.read((char*)Pokemons, 4 * sizeof(Pokemon));

    for (int i = 0; i < 4; i++)
    {
        printPokemon(Pokemons[i]);
    } 

    ifs.close();
}


