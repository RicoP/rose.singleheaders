#include "minicatch.hpp"
#include <rose/ini.hpp>

#include <string>

TEST_CASE( "basics", "[ini.hpp]" ) {
    // text must be a non-const zero terminated char array.
    char text[] = R"___(
        date = 26th of March 2017

        [ImageData]
        image = splat1.dds
        unused = hello world
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

    REQUIRE( date     == "26th of March 2017" );
    REQUIRE( image    == "splat1.dds" );
    REQUIRE( width    == 1024 );
    REQUIRE( height   == 1024 );
    REQUIRE( pixelsum == 269479.5 );
}

TEST_CASE( "arrays", "[ini.hpp]" ) {
    // text must be a non-const zero terminated char array.
    char text[] = R"___(
        [Map]
        number=16
        tile_size_width = 128
        tile_size_height = 128

        [Splat]
        original = splat1.png
        x=0
        y=0

        [Splat]
        original = splat2.png
        x=128
        y=0
    )___";

    int map_number=0;
    int map_tile_size_width=0;
    int map_tile_size_height=0;

    std::vector<const char *> splats_original;
    std::vector<int> splats_x;
    std::vector<int> splats_y;
    int n = 0;

    // Example INI parsing logic
    INI_BEGIN_TEXT(text)
        INI_LONG(Map, number)           map_number = value;
        INI_LONG(Map, tile_size_width)  map_tile_size_width = value;
        INI_LONG(Map, tile_size_height) map_tile_size_height = value;
        INI_SECTION(Splat) {
            n = value;
            splats_original.push_back({});
            splats_x.push_back({});
            splats_y.push_back({});
        }
        INI_STRING(Splat, original) splats_original[n] = value;
        INI_LONG(Splat, x) splats_x[n] = value;
        INI_LONG(Splat, y) splats_y[n] = value;
    INI_END();

    REQUIRE( map_number = 16 );
    REQUIRE( map_tile_size_width = 128 );
    REQUIRE( map_tile_size_height = 128 );
    REQUIRE( n == 1 );
    REQUIRE( n + 1 == splats_original.size() );
    REQUIRE( n + 1 == splats_x.size() );
    REQUIRE( n + 1 == splats_y.size() );
    REQUIRE( splats_x[0] == 0 );
    REQUIRE( splats_x[0] == 0 );
    REQUIRE( splats_x[1] == 128 );
    REQUIRE( splats_y[1] == 0 );
}

