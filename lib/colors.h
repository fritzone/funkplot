#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>
#include <map>
#include <string>
#include <cstring>

namespace Colors
{

struct color
{
    uint32_t c;
    uint8_t r, g, b;
    operator uint32_t() const {return c;}
};

constexpr color RGB_888(uint8_t R, uint8_t G, uint8_t B)
{
    return {((uint32_t)R << 16) | ((uint32_t)G << 8) | (uint32_t)B, R, G, B};
}

constexpr color Pink            = RGB_888(0xFF, 0xC0, 0xCB);
constexpr color LightPink       = RGB_888(0xFF, 0xB6, 0xC1);
constexpr color HotPink         = RGB_888(0xFF, 0x69, 0xB4);
constexpr color DeepPink        = RGB_888(0xFF, 0x14, 0x93);
constexpr color PaleVioletRed   = RGB_888(0xDB, 0x70, 0x93);
constexpr color MediumVioletRed = RGB_888(0xC7, 0x15, 0x85);
constexpr color LightSalmon = RGB_888(0xFF, 0xA0, 0x7A);
constexpr color Salmon      = RGB_888(0xFA, 0x80, 0x72);
constexpr color DarkSalmon  = RGB_888(0xE9, 0x96, 0x7A);
constexpr color LightCoral  = RGB_888(0xF0, 0x80, 0x80);
constexpr color IndianRed   = RGB_888(0xCD, 0x5C, 0x5C);
constexpr color Crimson     = RGB_888(0xDC, 0x14, 0x3C);
constexpr color Firebrick   = RGB_888(0xB2, 0x22, 0x22);
constexpr color DarkRed     = RGB_888(0x8B, 0x00, 0x00);
constexpr color Red         = RGB_888(0xFF, 0x00, 0x00);
constexpr color OrangeRed  = RGB_888(0xFF, 0x45, 0x00);
constexpr color Tomato     = RGB_888(0xFF, 0x63, 0x47);
constexpr color Coral      = RGB_888(0xFF, 0x7F, 0x50);
constexpr color DarkOrange = RGB_888(0xFF, 0x8C, 0x00);
constexpr color Orange     = RGB_888(0xFF, 0xA5, 0x00);
constexpr color Yellow               = RGB_888(0xFF, 0xFF, 0x00);
constexpr color LightYellow          = RGB_888(0xFF, 0xFF, 0xE0);
constexpr color LemonChiffon         = RGB_888(0xFF, 0xFA, 0xCD);
constexpr color LightGoldenrodYellow = RGB_888(0xFA, 0xFA, 0xD2);
constexpr color PapayaWhip           = RGB_888(0xFF, 0xEF, 0xD5);
constexpr color Moccasin             = RGB_888(0xFF, 0xE4, 0xB5);
constexpr color PeachPuff            = RGB_888(0xFF, 0xDA, 0xB9);
constexpr color PaleGoldenrod        = RGB_888(0xEE, 0xE8, 0xAA);
constexpr color Khaki                = RGB_888(0xF0, 0xE6, 0x8C);
constexpr color DarkKhaki            = RGB_888(0xBD, 0xB7, 0x6B);
constexpr color Gold                 = RGB_888(0xFF, 0xD7, 0x00);
constexpr color Cornsilk       = RGB_888(0xFF, 0xF8, 0xDC);
constexpr color BlanchedAlmond = RGB_888(0xFF, 0xEB, 0xCD);
constexpr color Bisque         = RGB_888(0xFF, 0xE4, 0xC4);
constexpr color NavajoWhite    = RGB_888(0xFF, 0xDE, 0xAD);
constexpr color Wheat          = RGB_888(0xF5, 0xDE, 0xB3);
constexpr color Burlywood      = RGB_888(0xDE, 0xB8, 0x87);
constexpr color Tanned         = RGB_888(0xD2, 0xB4, 0x8C);
constexpr color RosyBrown      = RGB_888(0xBC, 0x8F, 0x8F);
constexpr color SandyBrown     = RGB_888(0xF4, 0xA4, 0x60);
constexpr color Goldenrod      = RGB_888(0xDA, 0xA5, 0x20);
constexpr color DarkGoldenrod  = RGB_888(0xB8, 0x86, 0x0B);
constexpr color Peru           = RGB_888(0xCD, 0x85, 0x3F);
constexpr color Chocolate      = RGB_888(0xD2, 0x69, 0x1E);
constexpr color SaddleBrown    = RGB_888(0x8B, 0x45, 0x13);
constexpr color Sienna         = RGB_888(0xA0, 0x52, 0x2D);
constexpr color Brown          = RGB_888(0xA5, 0x2A, 0x2A);
constexpr color Maroon         = RGB_888(0x80, 0x00, 0x00);
constexpr color DarkOliveGreen    = RGB_888(0x55, 0x6B, 0x2F);
constexpr color Olive             = RGB_888(0x80, 0x80, 0x00);
constexpr color OliveDrab         = RGB_888(0x6B, 0x8E, 0x23);
constexpr color YellowGreen       = RGB_888(0x9A, 0xCD, 0x32);
constexpr color LimeGreen         = RGB_888(0x32, 0xCD, 0x32);
constexpr color Lime              = RGB_888(0x00, 0xFF, 0x00);
constexpr color LawnGreen         = RGB_888(0x7C, 0xFC, 0x00);
constexpr color Chartreuse        = RGB_888(0x7F, 0xFF, 0x00);
constexpr color GreenYellow       = RGB_888(0xAD, 0xFF, 0x2F);
constexpr color SpringGreen       = RGB_888(0x00, 0xFF, 0x7F);
constexpr color MediumSpringGreen = RGB_888(0x00, 0xFA, 0x9A);
constexpr color LightGreen        = RGB_888(0x90, 0xEE, 0x90);
constexpr color PaleGreen         = RGB_888(0x98, 0xFB, 0x98);
constexpr color DarkSeaGreen      = RGB_888(0x8F, 0xBC, 0x8F);
constexpr color MediumAquamarine  = RGB_888(0x66, 0xCD, 0xAA);
constexpr color MediumSeaGreen    = RGB_888(0x3C, 0xB3, 0x71);
constexpr color SeaGreen          = RGB_888(0x2E, 0x8B, 0x57);
constexpr color ForestGreen       = RGB_888(0x22, 0x8B, 0x22);
constexpr color Green             = RGB_888(0x00, 0x80, 0x00);
constexpr color DarkGreen         = RGB_888(0x00, 0x64, 0x00);
constexpr color Aqua            = RGB_888(0x00, 0xFF, 0xFF);
constexpr color Cyan            = RGB_888(0x00, 0xFF, 0xFF);
constexpr color LightCyan       = RGB_888(0xE0, 0xFF, 0xFF);
constexpr color PaleTurquoise   = RGB_888(0xAF, 0xEE, 0xEE);
constexpr color Aquamarine      = RGB_888(0x7F, 0xFF, 0xD4);
constexpr color Turquoise       = RGB_888(0x40, 0xE0, 0xD0);
constexpr color MediumTurquoise = RGB_888(0x48, 0xD1, 0xCC);
constexpr color DarkTurquoise   = RGB_888(0x00, 0xCE, 0xD1);
constexpr color LightSeaGreen   = RGB_888(0x20, 0xB2, 0xAA);
constexpr color CadetBlue       = RGB_888(0x5F, 0x9E, 0xA0);
constexpr color DarkCyan        = RGB_888(0x00, 0x8B, 0x8B);
constexpr color Teal            = RGB_888(0x00, 0x80, 0x80);
constexpr color LightSteelBlue = RGB_888(0xB0, 0xC4, 0xDE);
constexpr color PowderBlue     = RGB_888(0xB0, 0xE0, 0xE6);
constexpr color LightBlue      = RGB_888(0xAD, 0xD8, 0xE6);
constexpr color SkyBlue        = RGB_888(0x87, 0xCE, 0xEB);
constexpr color LightSkyBlue   = RGB_888(0x87, 0xCE, 0xFA);
constexpr color DeepSkyBlue    = RGB_888(0x00, 0xBF, 0xFF);
constexpr color DodgerBlue     = RGB_888(0x1E, 0x90, 0xFF);
constexpr color CornflowerBlue = RGB_888(0x64, 0x95, 0xED);
constexpr color SteelBlue      = RGB_888(0x46, 0x82, 0xB4);
constexpr color RoyalBlue      = RGB_888(0x41, 0x69, 0xE1);
constexpr color Blue           = RGB_888(0x00, 0x00, 0xFF);
constexpr color MediumBlue     = RGB_888(0x00, 0x00, 0xCD);
constexpr color DarkBlue       = RGB_888(0x00, 0x00, 0x8B);
constexpr color Navy           = RGB_888(0x00, 0x00, 0x80);
constexpr color MidnightBlue   = RGB_888(0x19, 0x19, 0x70);
constexpr color Lavender        = RGB_888(0xE6, 0xE6, 0xFA);
constexpr color Thistle         = RGB_888(0xD8, 0xBF, 0xD8);
constexpr color Plum            = RGB_888(0xDD, 0xA0, 0xDD);
constexpr color Violet          = RGB_888(0xEE, 0x82, 0xEE);
constexpr color Orchid          = RGB_888(0xDA, 0x70, 0xD6);
constexpr color Fuchsia         = RGB_888(0xFF, 0x00, 0xFF);
constexpr color Magenta         = RGB_888(0xFF, 0x00, 0xFF);
constexpr color MediumOrchid    = RGB_888(0xBA, 0x55, 0xD3);
constexpr color MediumPurple    = RGB_888(0x93, 0x70, 0xDB);
constexpr color BlueViolet      = RGB_888(0x8A, 0x2B, 0xE2);
constexpr color DarkViolet      = RGB_888(0x94, 0x00, 0xD3);
constexpr color DarkOrchid      = RGB_888(0x99, 0x32, 0xCC);
constexpr color DarkMagenta     = RGB_888(0x8B, 0x00, 0x8B);
constexpr color Purple          = RGB_888(0x80, 0x00, 0x80);
constexpr color Indigo          = RGB_888(0x4B, 0x00, 0x82);
constexpr color DarkSlateBlue   = RGB_888(0x48, 0x3D, 0x8B);
constexpr color SlateBlue       = RGB_888(0x6A, 0x5A, 0xCD);
constexpr color MediumSlateBlue = RGB_888(0x7B, 0x68, 0xEE);
constexpr color White         = RGB_888(0xFF, 0xFF, 0xFF);
constexpr color Snow          = RGB_888(0xFF, 0xFA, 0xFA);
constexpr color Honeydew      = RGB_888(0xF0, 0xFF, 0xF0);
constexpr color MintCream     = RGB_888(0xF5, 0xFF, 0xFA);
constexpr color Azure         = RGB_888(0xF0, 0xFF, 0xFF);
constexpr color AliceBlue     = RGB_888(0xF0, 0xF8, 0xFF);
constexpr color GhostWhite    = RGB_888(0xF8, 0xF8, 0xFF);
constexpr color WhiteSmoke    = RGB_888(0xF5, 0xF5, 0xF5);
constexpr color Seashell      = RGB_888(0xFF, 0xF5, 0xEE);
constexpr color Beige         = RGB_888(0xF5, 0xF5, 0xDC);
constexpr color OldLace       = RGB_888(0xFD, 0xF5, 0xE6);
constexpr color FloralWhite   = RGB_888(0xFF, 0xFA, 0xF0);
constexpr color Ivory         = RGB_888(0xFF, 0xFF, 0xF0);
constexpr color AntiqueWhite  = RGB_888(0xFA, 0xEB, 0xD7);
constexpr color Linen         = RGB_888(0xFA, 0xF0, 0xE6);
constexpr color LavenderBlush = RGB_888(0xFF, 0xF0, 0xF5);
constexpr color MistyRose     = RGB_888(0xFF, 0xE4, 0xE1);
constexpr color Gainsboro      = RGB_888(0xDC, 0xDC, 0xDC);
constexpr color LightGray      = RGB_888(0xD3, 0xD3, 0xD3);
constexpr color Silver         = RGB_888(0xC0, 0xC0, 0xC0);
constexpr color DarkGray       = RGB_888(0xA9, 0xA9, 0xA9);
constexpr color Gray           = RGB_888(0x80, 0x80, 0x80);
constexpr color DimGray        = RGB_888(0x69, 0x69, 0x69);
constexpr color LightSlateGray = RGB_888(0x77, 0x88, 0x99);
constexpr color SlateGray      = RGB_888(0x70, 0x80, 0x90);
constexpr color DarkSlateGray  = RGB_888(0x2F, 0x4F, 0x4F);
constexpr color Black          = RGB_888(0x00, 0x00, 0x00);

static const std::map<const std::string, color> colormap = {
    {"pink", Pink}, {"lightpink", LightPink}, {"hotpink", HotPink}, {"deeppink", DeepPink},
    {"palevioletred", PaleVioletRed}, {"mediumvioletred", MediumVioletRed}, {"lightsalmon", LightSalmon},
    {"salmon", Salmon}, {"darksalmon", DarkSalmon}, {"lightcoral", LightCoral}, {"indianred", IndianRed},
    {"crimson", Crimson}, {"firebrick", Firebrick}, {"darkred", DarkRed}, {"red", Red},
    {"orangered", OrangeRed}, {"tomato", Tomato}, {"coral", Coral}, {"darkorange", DarkOrange},
    {"orange", Orange}, {"yellow", Yellow}, {"lightyellow", LightYellow}, {"lemonchiffon", LemonChiffon},
    {"lightgoldenrodyellow", LightGoldenrodYellow}, {"papayawhip", PapayaWhip}, {"moccasin", Moccasin},
    {"peachpuff", PeachPuff}, {"palegoldenrod", PaleGoldenrod}, {"khaki", Khaki}, {"darkkhaki", DarkKhaki},
    {"gold", Gold}, {"cornsilk", Cornsilk}, {"blanchedalmond", BlanchedAlmond}, {"bisque", Bisque},
    {"navajowhite", NavajoWhite}, {"wheat", Wheat}, {"burlywood", Burlywood}, {"tanned", Tanned},
    {"rosybrown", RosyBrown}, {"sandybrown", SandyBrown}, {"goldenrod", Goldenrod},
    {"darkgoldenrod", DarkGoldenrod}, {"peru", Peru}, {"chocolate", Chocolate}, {"saddlebrown", SaddleBrown},
    {"sienna", Sienna}, {"brown", Brown}, {"maroon", Maroon}, {"darkolivegreen", DarkOliveGreen},
    {"olive", Olive}, {"olivedrab", OliveDrab}, {"yellowgreen", YellowGreen}, {"limegreen", LimeGreen},
    {"lime", Lime}, {"lawngreen", LawnGreen}, {"chartreuse", Chartreuse}, {"greenyellow", GreenYellow},
    {"springgreen", SpringGreen}, {"mediumspringgreen", MediumSpringGreen}, {"lightgreen", LightGreen},
    {"palegreen", PaleGreen}, {"darkseagreen", DarkSeaGreen}, {"mediumaquamarine", MediumAquamarine},
    {"mediumseagreen", MediumSeaGreen}, {"seagreen", SeaGreen}, {"forestgreen", ForestGreen},
    {"green", Green}, {"darkgreen", DarkGreen}, {"aqua", Aqua}, {"cyan", Cyan}, {"lightcyan", LightCyan},
    {"paleturquoise", PaleTurquoise}, {"aquamarine", Aquamarine}, {"turquoise", Turquoise},
    {"mediumturquoise", MediumTurquoise}, {"darkturquoise", DarkTurquoise}, {"lightseagreen", LightSeaGreen},
    {"cadetblue", CadetBlue}, {"darkcyan", DarkCyan}, {"teal", Teal}, {"lightsteelblue", LightSteelBlue},
    {"powderblue", PowderBlue}, {"lightblue", LightBlue}, {"skyblue", SkyBlue}, {"lightskyblue", LightSkyBlue},
    {"deepskyblue", DeepSkyBlue}, {"dodgerblue", DodgerBlue}, {"cornflowerblue", CornflowerBlue},
    {"steelblue", SteelBlue}, {"royalblue", RoyalBlue}, {"blue", Blue}, {"mediumblue", MediumBlue},
    {"darkblue", DarkBlue}, {"navy", Navy}, {"midnightblue", MidnightBlue}, {"lavender", Lavender},
    {"thistle", Thistle}, {"plum", Plum}, {"violet", Violet}, {"orchid", Orchid}, {"fuchsia", Fuchsia},
    {"magenta", Magenta}, {"mediumorchid", MediumOrchid}, {"mediumpurple", MediumPurple}, {"blueviolet", BlueViolet},
    {"darkviolet", DarkViolet}, {"darkorchid", DarkOrchid}, {"darkmagenta", DarkMagenta}, {"purple", Purple},
    {"indigo", Indigo}, {"darkslateblue", DarkSlateBlue}, {"slateblue", SlateBlue}, {"mediumslateblue", MediumSlateBlue},
    {"white", White}, {"snow", Snow}, {"honeydew", Honeydew}, {"mintcream", MintCream}, {"azure", Azure},
    {"aliceblue", AliceBlue}, {"ghostwhite", GhostWhite}, {"whitesmoke", WhiteSmoke}, {"seashell", Seashell},
    {"beige", Beige}, {"oldlace", OldLace}, {"floralwhite", FloralWhite}, {"ivory", Ivory}, {"antiquewhite", AntiqueWhite},
    {"linen", Linen}, {"lavenderblush", LavenderBlush}, {"mistyrose", MistyRose}, {"gainsboro", Gainsboro},
    {"lightgray", LightGray}, {"silver", Silver}, {"darkgray", DarkGray}, {"gray", Gray}, {"dimgray", DimGray},
    {"lightslategray", LightSlateGray}, {"slategray", SlateGray}, {"darkslategray", DarkSlateGray},
    {"black", Black}
};

inline int fromHex(uint c) noexcept
{
    return ((c >= '0') && (c <= '9')) ? int(c - '0') :
               ((c >= 'A') && (c <= 'F')) ? int(c - 'A' + 10) :
               ((c >= 'a') && (c <= 'f')) ? int(c - 'a' + 10) :
               -1;
}

static inline int hex2int(const char *s, int n)
{
    if (n < 0)
        return -1;
    int result = 0;
    for (; n > 0; --n) {
        result = result * 16;
        const int h = fromHex(*s++);
        if (h < 0)
            return -1;
        result += h;
    }
    return result;
}

static std::tuple<int, int, int, int>  decodeHexRgb(const char *name)
{
    auto len = strlen(name);
    if (name[0] != '#')
    {
        return {0,0,0,0};
    }

    name++;
    --len;

    int a{0}, r{0}, g{0}, b{0};

    std::string rgb8 = "#00000000";

    if (len == 3) // like: #RGB
    {
        rgb8 = "FF";
        rgb8 += name[0]; rgb8 += name[0];
        rgb8 += name[1]; rgb8 += name[1];
        rgb8 += name[2]; rgb8 += name[2];
    }
    else
    if (len == 6) // like: #RRGGBB
    {
        rgb8 = "FF";
        rgb8 += name;
    }
    else
    if(len == 8)
    {
        rgb8 = name;
    }

    const char* name8 = rgb8.c_str();

    a = hex2int(name8 + 0, 2);
    r = hex2int(name8 + 2, 2);
    g = hex2int(name8 + 4, 2);
    b = hex2int(name8 + 6, 2);

    return {r, g ,b, a};
}


}
#endif
