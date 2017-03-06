#ifndef H_EQUIVALENCES
#define H_EQUIVALENCES

#include <vector>
#include <string>
#include <regex>

const std::string equivalence_filename = "characters_equivalences.txt";

//init in load_character_equivalences
class CharEquivalences 
{
public:
    std::vector<std::wstring> groups;
    std::vector<std::wregex> regex_check;

    bool load_character_equivalences(std::string data_dir);

};

#endif H_EQUIVALENCES