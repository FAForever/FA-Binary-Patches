#include "Networkstats.h"
#include <cstdlib>
#include <array>

const std::array<uint32_t, 21> colors{
    0xff00FF00, // #00FF00
    0xff1AFF00, // #1AFF00
    0xff33FF00, // #33FF00
    0xff4DFF00, // #4DFF00
    0xff66FF00, // #66FF00
    0xff80FF00, // #80FF00
    0xff99FF00, // #99FF00
    0xffB3FF00, // #B3FF00
    0xffCCFF00, // #CCFF00
    0xffE6FF00, // #E6FF00
    0xffFFFF00, // #FFFF00
    0xffFFE600, // #FFE600
    0xffFFCC00, // #FFCC00
    0xffFFB300, // #FFB300
    0xffFF9900, // #FF9900
    0xffFF8000, // #FF8000
    0xffFF6600, // #FF6600
    0xffFF4D00, // #FF4D00
    0xffFF3300, // #FF3300
    0xffF00000, // #F00000
    0xffE00000, // #E00000
};

int clamp(int value, int min, int max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

SHARED int __thiscall PickColorForConnections(const string *s, int row, int index)
{
    if (row > 0 && (index == 4 || index >= 7))
    {
        const char *cs = s->data();
        int v = atoi(cs);
        return colors[clamp(v, 0, colors.size() - 1)];
    }
    return 0xffffffff;
}