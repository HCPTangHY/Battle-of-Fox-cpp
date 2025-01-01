#include "test.h"

#include <godot_cpp/variant/utility_functions.hpp>

void Test::my_func(godot::String from) {
    UtilityFunctions::print("Hello, world! form BoF");
    UtilityFunctions::print(from);
}

void Test::_bind_methods() {
    ClassDB::bind_method(D_METHOD("my_func", "from"), &Test::my_func);
}