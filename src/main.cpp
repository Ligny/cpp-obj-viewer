#include <iostream>
#include "App.hpp"

int main(int argc, char **argv)
{
    const char *objPath;
    const char *mtlPath;
    bool ok;

    if (argc < 2) {
        std::cerr << "Usage: ./viewer model.obj [material.mtl]"
                  << std::endl;
        return 84;
    }
    objPath = argv[1];
    mtlPath = (argc >= 3) ? argv[2] : nullptr;
    ok = false;
    {
        App app(objPath, mtlPath, ok);
        if (!ok)
            return 84;
        app.run();
    }
    return 0;
}
