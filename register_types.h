/* godot-cpp integration testing project.
 *
 * This is free and unencumbered software released into the public domain.
 */

#ifndef BoFGameManager_REGISTER_TYPES_H
#define BoFGameManager_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_BoFGameManager_module(ModuleInitializationLevel p_level);
void uninitialize_BoFGameManager_module(ModuleInitializationLevel p_level);

#endif // EXAMPLE_REGISTER_TYPES_H
