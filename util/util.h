#pragma once

#include <string>
#include <vector>
#include <sstream>

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delim)) {
        if (!token.empty()) {  // 跳过空字符串
            tokens.push_back(token);
        }
    }
    return tokens;
}