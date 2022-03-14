#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>
#include <map>
#include <string>
#include <QColor>

namespace Colors
{
constexpr uint32_t RGB_888(uint8_t R, uint8_t G, uint8_t B)
{
    return ((uint32_t)R << 16) | ((uint32_t)G << 8) | (uint32_t)B;
}

constexpr uint32_t Pink            = RGB_888(0xFF, 0xC0, 0xCB);
constexpr uint32_t LightPink       = RGB_888(0xFF, 0xB6, 0xC1);
constexpr uint32_t HotPink         = RGB_888(0xFF, 0x69, 0xB4);
constexpr uint32_t DeepPink        = RGB_888(0xFF, 0x14, 0x93);
constexpr uint32_t PaleVioletRed   = RGB_888(0xDB, 0x70, 0x93);
constexpr uint32_t MediumVioletRed = RGB_888(0xC7, 0x15, 0x85);
constexpr uint32_t LightSalmon = RGB_888(0xFF, 0xA0, 0x7A);
constexpr uint32_t Salmon      = RGB_888(0xFA, 0x80, 0x72);
constexpr uint32_t DarkSalmon  = RGB_888(0xE9, 0x96, 0x7A);
constexpr uint32_t LightCoral  = RGB_888(0xF0, 0x80, 0x80);
constexpr uint32_t IndianRed   = RGB_888(0xCD, 0x5C, 0x5C);
constexpr uint32_t Crimson     = RGB_888(0xDC, 0x14, 0x3C);
constexpr uint32_t Firebrick   = RGB_888(0xB2, 0x22, 0x22);
constexpr uint32_t DarkRed     = RGB_888(0x8B, 0x00, 0x00);
constexpr uint32_t Red         = RGB_888(0xFF, 0x00, 0x00);
constexpr uint32_t OrangeRed  = RGB_888(0xFF, 0x45, 0x00);
constexpr uint32_t Tomato     = RGB_888(0xFF, 0x63, 0x47);
constexpr uint32_t Coral      = RGB_888(0xFF, 0x7F, 0x50);
constexpr uint32_t DarkOrange = RGB_888(0xFF, 0x8C, 0x00);
constexpr uint32_t Orange     = RGB_888(0xFF, 0xA5, 0x00);
constexpr uint32_t Yellow               = RGB_888(0xFF, 0xFF, 0x00);
constexpr uint32_t LightYellow          = RGB_888(0xFF, 0xFF, 0xE0);
constexpr uint32_t LemonChiffon         = RGB_888(0xFF, 0xFA, 0xCD);
constexpr uint32_t LightGoldenrodYellow = RGB_888(0xFA, 0xFA, 0xD2);
constexpr uint32_t PapayaWhip           = RGB_888(0xFF, 0xEF, 0xD5);
constexpr uint32_t Moccasin             = RGB_888(0xFF, 0xE4, 0xB5);
constexpr uint32_t PeachPuff            = RGB_888(0xFF, 0xDA, 0xB9);
constexpr uint32_t PaleGoldenrod        = RGB_888(0xEE, 0xE8, 0xAA);
constexpr uint32_t Khaki                = RGB_888(0xF0, 0xE6, 0x8C);
constexpr uint32_t DarkKhaki            = RGB_888(0xBD, 0xB7, 0x6B);
constexpr uint32_t Gold                 = RGB_888(0xFF, 0xD7, 0x00);
constexpr uint32_t Cornsilk       = RGB_888(0xFF, 0xF8, 0xDC);
constexpr uint32_t BlanchedAlmond = RGB_888(0xFF, 0xEB, 0xCD);
constexpr uint32_t Bisque         = RGB_888(0xFF, 0xE4, 0xC4);
constexpr uint32_t NavajoWhite    = RGB_888(0xFF, 0xDE, 0xAD);
constexpr uint32_t Wheat          = RGB_888(0xF5, 0xDE, 0xB3);
constexpr uint32_t Burlywood      = RGB_888(0xDE, 0xB8, 0x87);
constexpr uint32_t Tan            = RGB_888(0xD2, 0xB4, 0x8C);
constexpr uint32_t RosyBrown      = RGB_888(0xBC, 0x8F, 0x8F);
constexpr uint32_t SandyBrown     = RGB_888(0xF4, 0xA4, 0x60);
constexpr uint32_t Goldenrod      = RGB_888(0xDA, 0xA5, 0x20);
constexpr uint32_t DarkGoldenrod  = RGB_888(0xB8, 0x86, 0x0B);
constexpr uint32_t Peru           = RGB_888(0xCD, 0x85, 0x3F);
constexpr uint32_t Chocolate      = RGB_888(0xD2, 0x69, 0x1E);
constexpr uint32_t SaddleBrown    = RGB_888(0x8B, 0x45, 0x13);
constexpr uint32_t Sienna         = RGB_888(0xA0, 0x52, 0x2D);
constexpr uint32_t Brown          = RGB_888(0xA5, 0x2A, 0x2A);
constexpr uint32_t Maroon         = RGB_888(0x80, 0x00, 0x00);
constexpr uint32_t DarkOliveGreen    = RGB_888(0x55, 0x6B, 0x2F);
constexpr uint32_t Olive             = RGB_888(0x80, 0x80, 0x00);
constexpr uint32_t OliveDrab         = RGB_888(0x6B, 0x8E, 0x23);
constexpr uint32_t YellowGreen       = RGB_888(0x9A, 0xCD, 0x32);
constexpr uint32_t LimeGreen         = RGB_888(0x32, 0xCD, 0x32);
constexpr uint32_t Lime              = RGB_888(0x00, 0xFF, 0x00);
constexpr uint32_t LawnGreen         = RGB_888(0x7C, 0xFC, 0x00);
constexpr uint32_t Chartreuse        = RGB_888(0x7F, 0xFF, 0x00);
constexpr uint32_t GreenYellow       = RGB_888(0xAD, 0xFF, 0x2F);
constexpr uint32_t SpringGreen       = RGB_888(0x00, 0xFF, 0x7F);
constexpr uint32_t MediumSpringGreen = RGB_888(0x00, 0xFA, 0x9A);
constexpr uint32_t LightGreen        = RGB_888(0x90, 0xEE, 0x90);
constexpr uint32_t PaleGreen         = RGB_888(0x98, 0xFB, 0x98);
constexpr uint32_t DarkSeaGreen      = RGB_888(0x8F, 0xBC, 0x8F);
constexpr uint32_t MediumAquamarine  = RGB_888(0x66, 0xCD, 0xAA);
constexpr uint32_t MediumSeaGreen    = RGB_888(0x3C, 0xB3, 0x71);
constexpr uint32_t SeaGreen          = RGB_888(0x2E, 0x8B, 0x57);
constexpr uint32_t ForestGreen       = RGB_888(0x22, 0x8B, 0x22);
constexpr uint32_t Green             = RGB_888(0x00, 0x80, 0x00);
constexpr uint32_t DarkGreen         = RGB_888(0x00, 0x64, 0x00);
constexpr uint32_t Aqua            = RGB_888(0x00, 0xFF, 0xFF);
constexpr uint32_t Cyan            = RGB_888(0x00, 0xFF, 0xFF);
constexpr uint32_t LightCyan       = RGB_888(0xE0, 0xFF, 0xFF);
constexpr uint32_t PaleTurquoise   = RGB_888(0xAF, 0xEE, 0xEE);
constexpr uint32_t Aquamarine      = RGB_888(0x7F, 0xFF, 0xD4);
constexpr uint32_t Turquoise       = RGB_888(0x40, 0xE0, 0xD0);
constexpr uint32_t MediumTurquoise = RGB_888(0x48, 0xD1, 0xCC);
constexpr uint32_t DarkTurquoise   = RGB_888(0x00, 0xCE, 0xD1);
constexpr uint32_t LightSeaGreen   = RGB_888(0x20, 0xB2, 0xAA);
constexpr uint32_t CadetBlue       = RGB_888(0x5F, 0x9E, 0xA0);
constexpr uint32_t DarkCyan        = RGB_888(0x00, 0x8B, 0x8B);
constexpr uint32_t Teal            = RGB_888(0x00, 0x80, 0x80);
constexpr uint32_t LightSteelBlue = RGB_888(0xB0, 0xC4, 0xDE);
constexpr uint32_t PowderBlue     = RGB_888(0xB0, 0xE0, 0xE6);
constexpr uint32_t LightBlue      = RGB_888(0xAD, 0xD8, 0xE6);
constexpr uint32_t SkyBlue        = RGB_888(0x87, 0xCE, 0xEB);
constexpr uint32_t LightSkyBlue   = RGB_888(0x87, 0xCE, 0xFA);
constexpr uint32_t DeepSkyBlue    = RGB_888(0x00, 0xBF, 0xFF);
constexpr uint32_t DodgerBlue     = RGB_888(0x1E, 0x90, 0xFF);
constexpr uint32_t CornflowerBlue = RGB_888(0x64, 0x95, 0xED);
constexpr uint32_t SteelBlue      = RGB_888(0x46, 0x82, 0xB4);
constexpr uint32_t RoyalBlue      = RGB_888(0x41, 0x69, 0xE1);
constexpr uint32_t Blue           = RGB_888(0x00, 0x00, 0xFF);
constexpr uint32_t MediumBlue     = RGB_888(0x00, 0x00, 0xCD);
constexpr uint32_t DarkBlue       = RGB_888(0x00, 0x00, 0x8B);
constexpr uint32_t Navy           = RGB_888(0x00, 0x00, 0x80);
constexpr uint32_t MidnightBlue   = RGB_888(0x19, 0x19, 0x70);
constexpr uint32_t Lavender        = RGB_888(0xE6, 0xE6, 0xFA);
constexpr uint32_t Thistle         = RGB_888(0xD8, 0xBF, 0xD8);
constexpr uint32_t Plum            = RGB_888(0xDD, 0xA0, 0xDD);
constexpr uint32_t Violet          = RGB_888(0xEE, 0x82, 0xEE);
constexpr uint32_t Orchid          = RGB_888(0xDA, 0x70, 0xD6);
constexpr uint32_t Fuchsia         = RGB_888(0xFF, 0x00, 0xFF);
constexpr uint32_t Magenta         = RGB_888(0xFF, 0x00, 0xFF);
constexpr uint32_t MediumOrchid    = RGB_888(0xBA, 0x55, 0xD3);
constexpr uint32_t MediumPurple    = RGB_888(0x93, 0x70, 0xDB);
constexpr uint32_t BlueViolet      = RGB_888(0x8A, 0x2B, 0xE2);
constexpr uint32_t DarkViolet      = RGB_888(0x94, 0x00, 0xD3);
constexpr uint32_t DarkOrchid      = RGB_888(0x99, 0x32, 0xCC);
constexpr uint32_t DarkMagenta     = RGB_888(0x8B, 0x00, 0x8B);
constexpr uint32_t Purple          = RGB_888(0x80, 0x00, 0x80);
constexpr uint32_t Indigo          = RGB_888(0x4B, 0x00, 0x82);
constexpr uint32_t DarkSlateBlue   = RGB_888(0x48, 0x3D, 0x8B);
constexpr uint32_t SlateBlue       = RGB_888(0x6A, 0x5A, 0xCD);
constexpr uint32_t MediumSlateBlue = RGB_888(0x7B, 0x68, 0xEE);
constexpr uint32_t White         = RGB_888(0xFF, 0xFF, 0xFF);
constexpr uint32_t Snow          = RGB_888(0xFF, 0xFA, 0xFA);
constexpr uint32_t Honeydew      = RGB_888(0xF0, 0xFF, 0xF0);
constexpr uint32_t MintCream     = RGB_888(0xF5, 0xFF, 0xFA);
constexpr uint32_t Azure         = RGB_888(0xF0, 0xFF, 0xFF);
constexpr uint32_t AliceBlue     = RGB_888(0xF0, 0xF8, 0xFF);
constexpr uint32_t GhostWhite    = RGB_888(0xF8, 0xF8, 0xFF);
constexpr uint32_t WhiteSmoke    = RGB_888(0xF5, 0xF5, 0xF5);
constexpr uint32_t Seashell      = RGB_888(0xFF, 0xF5, 0xEE);
constexpr uint32_t Beige         = RGB_888(0xF5, 0xF5, 0xDC);
constexpr uint32_t OldLace       = RGB_888(0xFD, 0xF5, 0xE6);
constexpr uint32_t FloralWhite   = RGB_888(0xFF, 0xFA, 0xF0);
constexpr uint32_t Ivory         = RGB_888(0xFF, 0xFF, 0xF0);
constexpr uint32_t AntiqueWhite  = RGB_888(0xFA, 0xEB, 0xD7);
constexpr uint32_t Linen         = RGB_888(0xFA, 0xF0, 0xE6);
constexpr uint32_t LavenderBlush = RGB_888(0xFF, 0xF0, 0xF5);
constexpr uint32_t MistyRose     = RGB_888(0xFF, 0xE4, 0xE1);
constexpr uint32_t Gainsboro      = RGB_888(0xDC, 0xDC, 0xDC);
constexpr uint32_t LightGray      = RGB_888(0xD3, 0xD3, 0xD3);
constexpr uint32_t Silver         = RGB_888(0xC0, 0xC0, 0xC0);
constexpr uint32_t DarkGray       = RGB_888(0xA9, 0xA9, 0xA9);
constexpr uint32_t Gray           = RGB_888(0x80, 0x80, 0x80);
constexpr uint32_t DimGray        = RGB_888(0x69, 0x69, 0x69);
constexpr uint32_t LightSlateGray = RGB_888(0x77, 0x88, 0x99);
constexpr uint32_t SlateGray      = RGB_888(0x70, 0x80, 0x90);
constexpr uint32_t DarkSlateGray  = RGB_888(0x2F, 0x4F, 0x4F);
constexpr uint32_t Black          = RGB_888(0x00, 0x00, 0x00);

static const std::map<const std::string, uint32_t> colormap = {
    {"pink", Pink}, {"lightpink", LightPink}, {"hotpink", HotPink}, {"deeppink", DeepPink}, {"palevioletred", PaleVioletRed}, {"mediumvioletred", MediumVioletRed}, {"lightsalmon", LightSalmon}, {"salmon", Salmon}, {"darksalmon", DarkSalmon}, {"lightcoral", LightCoral}, {"indianred", IndianRed}, {"crimson", Crimson}, {"firebrick", Firebrick}, {"darkred", DarkRed}, {"red", Red}, {"orangered", OrangeRed}, {"tomato", Tomato}, {"coral", Coral}, {"darkorange", DarkOrange}, {"orange", Orange}, {"yellow", Yellow}, {"lightyellow", LightYellow}, {"lemonchiffon", LemonChiffon}, {"lightgoldenrodyellow", LightGoldenrodYellow}, {"papayawhip", PapayaWhip}, {"moccasin", Moccasin}, {"peachpuff", PeachPuff}, {"palegoldenrod", PaleGoldenrod}, {"khaki", Khaki}, {"darkkhaki", DarkKhaki}, {"gold", Gold}, {"cornsilk", Cornsilk}, {"blanchedalmond", BlanchedAlmond}, {"bisque", Bisque}, {"navajowhite", NavajoWhite}, {"wheat", Wheat}, {"burlywood", Burlywood}, {"tan", Tan}, {"rosybrown", RosyBrown}, {"sandybrown", SandyBrown}, {"goldenrod", Goldenrod}, {"darkgoldenrod", DarkGoldenrod}, {"peru", Peru}, {"chocolate", Chocolate}, {"saddlebrown", SaddleBrown}, {"sienna", Sienna}, {"brown", Brown}, {"maroon", Maroon}, {"darkolivegreen", DarkOliveGreen}, {"olive", Olive}, {"olivedrab", OliveDrab}, {"yellowgreen", YellowGreen}, {"limegreen", LimeGreen}, {"lime", Lime}, {"lawngreen", LawnGreen}, {"chartreuse", Chartreuse}, {"greenyellow", GreenYellow}, {"springgreen", SpringGreen}, {"mediumspringgreen", MediumSpringGreen}, {"lightgreen", LightGreen}, {"palegreen", PaleGreen}, {"darkseagreen", DarkSeaGreen}, {"mediumaquamarine", MediumAquamarine}, {"mediumseagreen", MediumSeaGreen}, {"seagreen", SeaGreen}, {"forestgreen", ForestGreen}, {"green", Green}, {"darkgreen", DarkGreen}, {"aqua", Aqua}, {"cyan", Cyan}, {"lightcyan", LightCyan}, {"paleturquoise", PaleTurquoise}, {"aquamarine", Aquamarine}, {"turquoise", Turquoise}, {"mediumturquoise", MediumTurquoise}, {"darkturquoise", DarkTurquoise}, {"lightseagreen", LightSeaGreen}, {"cadetblue", CadetBlue}, {"darkcyan", DarkCyan}, {"teal", Teal}, {"lightsteelblue", LightSteelBlue}, {"powderblue", PowderBlue}, {"lightblue", LightBlue}, {"skyblue", SkyBlue}, {"lightskyblue", LightSkyBlue}, {"deepskyblue", DeepSkyBlue}, {"dodgerblue", DodgerBlue}, {"cornflowerblue", CornflowerBlue}, {"steelblue", SteelBlue}, {"royalblue", RoyalBlue}, {"blue", Blue}, {"mediumblue", MediumBlue}, {"darkblue", DarkBlue}, {"navy", Navy}, {"midnightblue", MidnightBlue}, {"lavender", Lavender}, {"thistle", Thistle}, {"plum", Plum}, {"violet", Violet}, {"orchid", Orchid}, {"fuchsia", Fuchsia}, {"magenta", Magenta}, {"mediumorchid", MediumOrchid}, {"mediumpurple", MediumPurple}, {"blueviolet", BlueViolet}, {"darkviolet", DarkViolet}, {"darkorchid", DarkOrchid}, {"darkmagenta", DarkMagenta}, {"purple", Purple}, {"indigo", Indigo}, {"darkslateblue", DarkSlateBlue}, {"slateblue", SlateBlue}, {"mediumslateblue", MediumSlateBlue}, {"white", White}, {"snow", Snow}, {"honeydew", Honeydew}, {"mintcream", MintCream}, {"azure", Azure}, {"aliceblue", AliceBlue}, {"ghostwhite", GhostWhite}, {"whitesmoke", WhiteSmoke}, {"seashell", Seashell}, {"beige", Beige}, {"oldlace", OldLace}, {"floralwhite", FloralWhite}, {"ivory", Ivory}, {"antiquewhite", AntiqueWhite}, {"linen", Linen}, {"lavenderblush", LavenderBlush}, {"mistyrose", MistyRose}, {"gainsboro", Gainsboro}, {"lightgray", LightGray}, {"silver", Silver}, {"darkgray", DarkGray}, {"gray", Gray}, {"dimgray", DimGray}, {"lightslategray", LightSlateGray}, {"slategray", SlateGray}, {"darkslategray", DarkSlateGray}, {"black", Black}
};

static QColor QColorFromUint32(uint32_t colour)
{
    QColor result((colour >> 16) & 0xff, (colour >> 8) & 0xff, colour  & 0xff);
    return result;
}

}
#endif
