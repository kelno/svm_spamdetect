#include "CharEquivalences.h"

#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <locale>
#include <codecvt>

bool CharEquivalences::load_character_equivalences(std::string full_path_to_file)
{
	std::wifstream file(full_path_to_file);

    if (file.fail())
        return false;

    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	int i = 0;
	std::wstring line;
	while (std::getline(file, line))
	{
		regex_check.resize(i + 1);
        groups.resize(i + 1);

        groups[i] = line;

		//remove spaces
		line.erase(std::remove_if(line.begin(), line.end(), [](char ch) { return std::isspace<char>(ch, std::locale::classic()); }), line.end());

		std::wstring regex_str = converter.from_bytes("[") + line + converter.from_bytes("]{2}");

		// /!\ This does not support regex character. If regex chars are added to the list, we need to escape them
        //std::wregex test(regex_str);
        regex_check[i] = regex_str;
		i++;
	}

    return true;
}