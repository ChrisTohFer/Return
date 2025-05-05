#include "return_engine/main_loop.h"

int main()
{
    //majority of functionality is handled by the engine itself hence calling straight into the engine main loop here
    //to customize the functionality of the engine for the game we use callbacks, virtual functions, and data
    const re::EngineConfig config;
    re::main_loop(config);
}