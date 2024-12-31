#include <rose/ini.hpp>

#include <string>

#define REQUIRE( COND ) do { if(COND) break; puts("ERROR: " #COND); return -1; } while(0)

int main() {
    // text must be a non-const zero terminated char array.
    char text[] = R"___(
        date = 26th of march 2017

        [ImageData]
        image = splat1.dds
        height = 1024
        width = 1024
        pixelsum = 269479.5
    )___";

    // Variables to store extracted data
    std::string date;
    std::string image;
    long long width = 0;
    long long height = 0;
    double pixelsum = 0;

    // Example INI parsing logic
    INI_BEGIN_TEXT(text)
        INI_STRING(GLOBAL, date) date = value;
        INI_STRING(ImageData, image) image = value;
        INI_LONG(ImageData, width) width = value;
        INI_LONG(ImageData, height) height = value;
        INI_DOUBLE(ImageData, pixelsum) pixelsum = value;
    INI_END();

    REQUIRE( date     == "26th of march 2017" );
    REQUIRE( image    == "splat1.dds" );
    REQUIRE( width    == 1024 );
    REQUIRE( height   == 1024 );
    REQUIRE( pixelsum == 269479.5 );

    puts("<ini.hpp test OK>");
    return 0;
}
