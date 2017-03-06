#include "CharEquivalences.h"

#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <locale>
#include <codecvt>

CharEquivalences equi;

bool CharEquivalences::load_character_equivalences(std::string data_dir)
{
	std::wifstream file(data_dir + '/' + equivalence_filename);

    if (file.fail())
        return false;

    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	int i = 0;
	std::wstring line;
	while (std::getline(file, line))
	{
		equi.regex_check.resize(i + 1);
        equi.groups.resize(i + 1);

        equi.groups[i] = line;

		//remove spaces
		line.erase(std::remove_if(line.begin(), line.end(), [](char ch) { return std::isspace<char>(ch, std::locale::classic()); }), line.end());

		std::wstring regex_str = converter.from_bytes("[") + line + converter.from_bytes("]{2}");

		// /!\ This does not support regex character. If regex chars are added to the list, we need to escape them
        //std::wregex test(regex_str);
        equi.regex_check[i] = regex_str;
		i++;
	}

    return true;
}