#include "application.h"

int main(int argc, char** argv)
{
#ifdef DEBUG
    message("GE1120 DEBUG build\n");
#else
    message("GE1120 RELEASE build\n");
#endif

    size_t allocationSize = MB(1);
    if (g_memory.init(allocationSize))
    {
        message("Allocated %d bytes of memory\n", allocationSize);
        stringBuffer = (char*)g_memory.reserve(STRING_BUFFER_SIZE);
        Application app;
        if (!init(&app))
        {
            err("Game initialization failed\n");
        }
        else
        {
            run(&app);
            destroy(&app);
        }
    }
    else
    {
        err("Memory allocation failed!");
    }
    
    message("Exiting program.\n");    
    return 0;
}
