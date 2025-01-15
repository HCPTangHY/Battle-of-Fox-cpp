#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <string>
#include <random>
#include <stdexcept>

#include "../include/nlohmann/json.hpp"

namespace BoF {

inline double rn(double v, int d = 0) {
    const double m = std::pow(10.0, d);
    return std::round(v * m) / m;
}

inline std::vector<std::string> split(const std::string& str, char delim) {
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

inline double range_rand(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

inline int get_number_in_range(const std::string& r) {
    try {
        // If input is just a number
        if (r.find_first_not_of("-0123456789.") == std::string::npos) {
            return static_cast<int>(std::stod(r));
        }

        // Handle sign
        int sign = 1;
        std::string str = r;
        if (!r.empty() && r[0] == '-') {
            sign = -1;
            str = r.substr(1);
        }

        // Split the range
        std::vector<std::string> range = split(str, '-');
        if (range.empty()) {
            throw std::invalid_argument("Cannot parse the number. Check the format");
        }

        // Convert range values and generate random number
        int min = std::stoi(range[0]) * sign;
        int max = std::stoi(range[1]);

        if (min < 0) {
            throw std::invalid_argument("Cannot parse number. Check the format");
        }

        return range_rand(min, max);
    }
    catch (const std::exception&) {
        return 0;
    }
}

inline double get_point_in_range(std::string range, int length) {
    std::vector<std::string> range_parts = split(range, '-');
    double min = range_parts.size() > 0? std::atoi(range_parts[0].c_str()) / 100 : 0;
    double max = range_parts.size() > 1? std::atoi(range_parts[1].c_str()) / 100 : 1;
    return range_rand(min*length, max*length);
}

template<typename T>
inline T minmax(T value, T min, T max) {
    return std::min(std::max(value, min), max);
}

template<typename T>
inline T lim(T v) {
    return minmax(v, T(0), T(100));
}

}