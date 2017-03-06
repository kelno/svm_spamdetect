#ifndef H_EQUIVALENCES
#define H_EQUIVALENCES

#include <vector>
#include <string>
#include <regex>

//init in load_character_equivalences
class CharEquivalences 
{
public:
    std::vector<std::wstring> groups;
    std::vector<std::wregex> regex_check;

    bool load_character_equivalences(std::string full_path_to_file);
};

#endif H_EQUIVALENCES