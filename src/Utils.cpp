#include "Utils.h"

#include <stdio.h>

namespace Utils
{
    std::string pgmToStdString(PGM_P str)
    {
        const auto len = strlen_P(str);
        std::string ss(len, 0);
        memcpy_P(&ss[0], str, len);
        return ss;
    }

    std::string formatFloat(const float f)
    {
        std::string s(10, 0);
        snprintf(&s[0], s.size() - 1, "%.1f", f);
        return s;
    }
}