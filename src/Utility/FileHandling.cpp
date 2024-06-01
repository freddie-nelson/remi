#include "../../include/Utility/FileHandling.h"

#include <fstream>
#include <sstream>

std::string remi::loadFile(std::string path)
{
    try
    {
        std::ifstream f(path);

        std::stringstream buffer;
        buffer << f.rdbuf();

        f.close();

        return buffer.str();
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("Failed to load file: '" + path + "'. Reason: " + e.what());
    }
}