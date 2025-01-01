#pragma once

#include <godot_cpp/classes/node2d.hpp>

using namespace godot;

class Test : public Node2D {
    GDCLASS(Test, Node2D);

    public:
        void my_func(godot::String from);

    protected:
        static void _bind_methods();


};