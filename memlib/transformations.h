#include <iostream>
#include <vector>
#include <cstring>
//
// Created by aliaksej on 07.05.23.
//

#ifndef MEM_TRANSFORMATIONS_H
#define MEM_TRANSFORMATIONS_H

/// \name
/// string_split(const std::string& str, const std::string& delimiter)
/// \details
/// Splits string to a vector of words by delimiter
/// \returns
/// Vector of words
inline std::vector<std::string> string_split(const std::string& str, const std::string& delimiter)
{
    // Result vector
    std::vector<std::string> words;

    // Save str argument to temporary variable
    char* line = strdup(str.c_str());

    char* word = strtok(line, delimiter.c_str());

    while(word)
    {
        if (strcmp(word, " ") != 0)
            words.emplace_back(word);

        word = strtok(nullptr, delimiter.c_str());
    }
    free(word);
    return words;
}

#endif //MEM_TRANSFORMATIONS_H
