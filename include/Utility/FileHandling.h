#pragma once

#include <string>

namespace remi
{
    /**
     * Loads the file as a string.
     *
     * @param path The path to the file.
     *
     * @returns The file contents as a string.
     */
    std::string loadFile(std::string path);
}