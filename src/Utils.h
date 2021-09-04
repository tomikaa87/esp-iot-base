#pragma once

#include <string>

#include <pgmspace.h>

namespace Utils
{
    std::string pgmToStdString(PGM_P str);
    std::string formatFloat(float f);
}