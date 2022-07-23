#include "Bruteforce.h"
#include "Dictionary.h"

void PrintUsage(const char* agr_0)
{
    std::cout << "Usage: " << agr_0 << " [path_to_chipher_aes_file]\n"
        << "  options:  -d ... use dictionary\n";
}

int main(int argc, char** argv)
{
#if defined DEBUG

    Bruteforce::CrackFile ("chipher_text_brute_force");
    Dictionary::CrackFile ("chipher_text_brute_force");

#else    
    if (argc == 2)
    {
        Bruteforce::CrackFile(argv[1]);
        system("pause");
    }
    else if (argc == 3 && !strcmp(argv[2], "-d"))
    {   
        Dictionary::CrackFile(argv[1]);
        system("pause");
    }
    else
    {
        PrintUsage(argv[0]);
    }
#endif
    return 0;
}