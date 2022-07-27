#include "Bruteforce.h"
#include "Dictionary.h"

#define DEBUG

void PrintUsage(char** argv)
{
    std::cout << "Usage: " << argv[0] << " [path_to_chipher_aes_file]\n"
        << "  options:  -d ... use dictionary\n";
}

int main(int argc, char** argv)
{
#if defined DEBUG

    Bruteforce bruteforceCracker;
    bruteforceCracker.CrackFile("chipher_text_brute_force");

    Dictionary dictionaryCracker; 
    dictionaryCracker.CrackFile("chipher_text_brute_force");

#else    
    if (argc == 2)
    {
        Bruteforce bruteforceCracker;
        bruteforceCracker.CrackFile(argv[1]);
        system("pause");
    }
    else if (argc == 3 && !strcmp(argv[2], "-d"))
    {   
        Dictionary dictionaryCracker;
        dictionaryCracker.CrackFile(argv[1]);
        system("pause");
    }
    else
    {
        PrintUsage(argv);
    }
#endif
    return 0;
}