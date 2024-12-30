# A collection of header only libraries

__________________________________________
# <rose/ini.hpp>

`rose/ini.hpp` is a lightweight, header-only library for parsing and processing `.ini` configuration files. It allows easy extraction and assignment of values from `ini` files into variables in your code, enabling quick integration into projects with minimal overhead.

Using `rose/ini.hpp`, you can parse an `ini` file and extract its contents into variables. Here's an example of how to use the library:

### Code Example

```cpp
#include <rose/ini.hpp>

// text must be a non-const zero terminated char array.
char text[] = R"___(
    [ImageData]
    image = splat1.dds
    height = 1024
    width = 1024
    pixelsum = 269479.6901960784
)___";

// Variables to store extracted data
const char *image = nullptr;
long long width = 0;
long long height = 0;
double pixelsum = 0;

// Example INI parsing logic
INI_BEGIN_TEXT(text)
    INI_STRING(ImageData, image) {
        if (char *last_slash = strrchr(pathBuffer, '/')) {
            // Replace after the last slash
            strcpy(last_slash + 1, image);
        } else {
            // No slash, replace the whole string
            strcpy(pathBuffer, image);
        }
        puts(pathBuffer);
        splat.texture = LoadTexture(pathBuffer);
    }
    INI_LONG(ImageData, width);
    INI_LONG(ImageData, height);
    INI_DOUBLE(ImageData, pixelsum);
INI_END();
```

### Explanation
* INI_BEGIN_TEXT and INI_END define the scope of parsing.
* INI_STRING, INI_LONG, and INI_DOUBLE are macros to extract specific keys (image, width, height, and pixelsum) under a section (ImageData) in the .ini file.
* The macro can end with a semicolon indicating the end ofr be wollowed up with a scope section `{...}` where the new value is handled in a specific way 


### Globals
When a ini file starts with values with no section they default in the `GLOBAL` section. 

```cpp
// text must be a non-const zero terminated char array.
char text[] = R"___(
    date = 26th of march 2017
    [ImageData]
    height = 1024
    width = 1024
)___";

// Variables to store extracted data
const char *date = nullptr;
long long width = 0;
long long height = 0;

// Example INI parsing logic
INI_BEGIN_TEXT(text)
    INI_STRING(GLOBAL, date);
    INI_LONG(ImageData, width);
    INI_LONG(ImageData, height);
INI_END();
```

## Important 
Note that all `const char *` are pointer into the passed in `text` variable and their lifetime is bound to the lifetime of `text`.
