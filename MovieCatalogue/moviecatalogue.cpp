#include <iostream>
#include <cstring>
#include <fstream>

enum class ErrorInCatalog 
{
    catalog_not_open,
    read_from_empty_catalog,
    movie_not_in_catalog,
    no_error_occurred
};

struct SafeAnswer 
{
    int number;
    ErrorInCatalog error;
};

const int MOVIE_NAME_MAX_LENGTH = 128;
struct Movie 
{
    char name[MOVIE_NAME_MAX_LENGTH];
    unsigned int price;
};

void printMovie(const Movie& m)
{
    std::cout << "Movie name: " << m.name << std::endl;
    std::cout << "Movie price: " << m.price << std::endl;
}

bool checkIfCouldNotOpenFile(std::ifstream& file)
{
    if (!file.is_open())
    {      
        return true;
    }

    return false;
}

bool checkIfEmptyFile(std::ifstream& file)
{
    if (file.peek() == std::ifstream::traits_type::eof())
    {
        return true;
    }

    return false;
}

SafeAnswer validatefstream(std::ifstream& fstream)
{
    SafeAnswer sa;

    sa.number = -1;

    if (checkIfCouldNotOpenFile(fstream))
    {
        sa.error = ErrorInCatalog::catalog_not_open;
        return sa;
    }

    if (checkIfEmptyFile(fstream))
    {
        sa.error = ErrorInCatalog::read_from_empty_catalog;
        return sa;
    }

    sa.number = 0;
    sa.error = ErrorInCatalog::no_error_occurred;

    return sa;
}

int getCharsCountInAFile(const char* fileName, const char searchedCh)
{
    std::ifstream file(fileName);

    if (checkIfCouldNotOpenFile(file))
    {
        return -1;
    }

    unsigned count = 0;

    while (true)
    {
        char currentCh = file.get();

        if (file.eof())
        {
            break;
        }

        if (currentCh == searchedCh)
        {
            count++;
        }
    }

    return count;
}

SafeAnswer getNumberOfMovies(const char* catalogName) 
{
    //по описания формат в условието би следвало филмите да са точно толкова, колкото редовете
    //в подадения файл -> броим редовете

    std::ifstream file(catalogName);

    SafeAnswer sa = validatefstream(file);

    if (sa.error != ErrorInCatalog::no_error_occurred)
    {
        return sa;
    }
    
    //ако сме стигнали дотук, би трябвало файлът, от който четем, да 
    //се е отворил успешно и да не е празен, следователно броим редовете
 
    sa.number = getCharsCountInAFile(catalogName, '\n') + 1;

    return sa;
}

SafeAnswer averagePrice(const char* catalogName) 
{
    std::ifstream file(catalogName);

    SafeAnswer sa = validatefstream(file);

    if (sa.error != ErrorInCatalog::no_error_occurred)
    {
        return sa;
    }

    //тук сме сигурни, че не е възникнала грешка

    char movieName[MOVIE_NAME_MAX_LENGTH];
    unsigned moviePrice = 0;
    unsigned sumOfMoviePrices = 0;
    unsigned movies = 0;
    while (file >> movieName >> moviePrice)
    {
        sumOfMoviePrices += moviePrice;
        movies++;
    }

    //не съм много сигурен какво точно означава "закръглено до цяло число"
    //до най-близкото цяло число ли?
    //или просто отрязваме дробната част...
    //ще оставя варианта, който е най-близък до истината (т.е. закръгляме до най-близкото цяло число)
    //std::cout << (double)sumOfMoviePrices / movies << std::endl; -> 16.25 в дадения пример

    sa.number = round(sumOfMoviePrices / (double)movies);

    return sa;
}

SafeAnswer getMoviePrice(const char* catalogName, const char* searchedMovieName) 
{
    std::ifstream file(catalogName);

    SafeAnswer sa = validatefstream(file);

    if (sa.error != ErrorInCatalog::no_error_occurred)
    {
        return sa;
    }

    char movieName[MOVIE_NAME_MAX_LENGTH];
    unsigned moviePrice = 0;

    while (file >> movieName >> moviePrice)
    {
        if (strcmp(movieName, searchedMovieName) == 0)
        {
            sa.number = moviePrice;
            //sa.error = ErrorInCatalog::no_error_occurred; -> този ред е излишен

            return sa;
        }
    }

    //ако сме стигнали дотук, то не сме намерили филм с такова име и трябва да върнем съответната грешка
    sa.error = ErrorInCatalog::movie_not_in_catalog;
    return sa;
}

//в условието не е описано какво точно да прави тази функция, затова
//промених името ѝ (беше readMovie(...)), за да съответства на функция, която аз бих си създал
Movie createMovie(const char movieName[MOVIE_NAME_MAX_LENGTH], unsigned moviePrice)
{
    Movie m;

    strcpy_s(m.name, movieName);
    m.price = moviePrice;

    return m;
}


Movie* saveMoviesInArray(std::ifstream& file, int numberOfMovies) 
{
    // съгласно условието ще считаме, че отвореният поток е валиден и няма да правим съответните
    // валидации (за празен файл и т.н.)

    char movieName[MOVIE_NAME_MAX_LENGTH];
    unsigned moviePrice = 0;

    Movie* movies = new Movie[numberOfMovies];
    int i = 0;

    while (file >> movieName >> moviePrice)
    {
        Movie currMovie = createMovie(movieName, moviePrice);

        //тази проверка е излишна, но компилаторът ревеше с "buffer overrun warning"
        if (i < numberOfMovies) 
        {
            movies[i++] = currMovie;
        }
    }

    return movies;
}

//не съм убеден и за предназначението на тази функция, затова пак ще ѝ променя името
void deleteMovies(Movie * movies, int moviesCount) 
{ 
    delete[moviesCount] movies;
}

//ще направим функцията по-обща, а конкретната сортировка (по възходяща цена) ще направим по-късно
void sortMoviesInArray(
    Movie * movies, 
    int count,
    bool (*isLess)(const Movie& lhs, const Movie& rhs))
{ 
    for (unsigned i = 0; i < count; i++)
    {
        //минимален в смисъл на позицията му в сортировката (а не като стойност)
        unsigned minIndex = i;

        for (unsigned j = i + 1; j < count; j++)
        {
            if (isLess(movies[j], movies[minIndex]))
            {
                minIndex = j;
            }
        }

        if (minIndex != i)
        {
            std::swap(movies[i], movies[minIndex]);
        }
    }
}

void sortMoviesByPriceDescending(Movie* movies, int count)
{
    return sortMoviesInArray(movies, count, [](const Movie& lhs, const Movie& rhs) -> bool
        {
            //std::cout << lhs.price - rhs.price << std::endl;
            return lhs.price > rhs.price;
        });
}

void sortMoviesByPriceAscending(Movie* movies, int count)
{
    return sortMoviesInArray(movies, count, [](const Movie& lhs, const Movie& rhs) -> bool
        {
            //std::cout << lhs.price - rhs.price << std::endl;
            return lhs.price < rhs.price;
        });
}


ErrorInCatalog saveMoviesSorted(const char* catalogName, const char* catalogSortedName) 
{
    std::ifstream file(catalogName);

    SafeAnswer sa = getNumberOfMovies(catalogName);

    if (sa.error != ErrorInCatalog::no_error_occurred)
    {
        return sa.error;
    }

    unsigned numberOfMovies = sa.number;

    Movie* movies = saveMoviesInArray(file, numberOfMovies);
    sortMoviesByPriceAscending(movies, numberOfMovies);

    std::ofstream ofstream(catalogSortedName);

    if (!ofstream.is_open())
    {
        return ErrorInCatalog::catalog_not_open;
    }

    for (unsigned i = 0; i < numberOfMovies; i++)
    {
        ofstream << movies[i].name << " " << movies[i].price;

        if (i < numberOfMovies - 1) //да няма празен ред накрая (не че е съществено)
        {
            ofstream << std::endl; 
        }
    }

    deleteMovies(movies, numberOfMovies);

    return ErrorInCatalog::no_error_occurred;
}

int main() 
{
    SafeAnswer safeNumberOfMovies = getNumberOfMovies("movieCatalog.txt");

    if (safeNumberOfMovies.error == ErrorInCatalog::no_error_occurred) 
    {
        std::cout << "The number of movies is: " << safeNumberOfMovies.number << std::endl;
    }

    SafeAnswer safeAveragePrice = averagePrice("movieCatalog.txt");

    if (safeAveragePrice.error == ErrorInCatalog::no_error_occurred) 
    {
        std::cout << "The average price is: " << safeAveragePrice.number << std::endl;
    }

    SafeAnswer safePrice = getMoviePrice("movieCatalog.txt", "Black-bullet");

    if (safePrice.error == ErrorInCatalog::no_error_occurred) 
    {
        std::cout << "The price for the Black bullet movies is: " << safePrice.number << std::endl;
    }

    //тест за функцията saveMoviesInArray()
    std::ifstream file("movieCatalog.txt");
    Movie * movies = saveMoviesInArray(file, 8);

    sortMoviesByPriceAscending(movies, 8);

    for (unsigned i = 0; i < 8; i++)
    {
        printMovie(movies[i]);
        std::cout << "---------------------" << std::endl;
    }

    delete[8] movies;

    ErrorInCatalog errorSorting = saveMoviesSorted("movieCatalog.txt", "movieCatalogSorted.txt");

    if (errorSorting == ErrorInCatalog::no_error_occurred) 
    {
        std::cout << "Look the content of the movieCatalogSorted.txt file" << std::endl;
    }
}