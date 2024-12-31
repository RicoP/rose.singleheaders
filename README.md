# A collection of header only libraries

__________________________________________
# <rose/ini.hpp>

`rose/ini.hpp` is a lightweight, header-only library for parsing and processing `.ini` configuration files. It allows easy extraction and assignment of values from `ini` files into variables in your code, enabling quick integration into projects with minimal overhead.

Using `rose/ini.hpp`, you can parse an `ini` file and extract its contents into variables. Here's an example of how to use the library:

### Ini file structure
A ini file is made out of sections enclosed by `[SECTION NAME]` followed by labels followed by `=` followed by a value. When a label comes before any other action it defaults to the `[GLOBAL]` section.
the values of a label can be strings or numbers and always end with the end of the line. A line that starts with a `;` counbts as a comment and will be skipped.

```ini
; My .ini file
[section]
label = value
```


### Code Example

```cpp
#include <rose/ini.hpp>

// text must be a non-const zero terminated char array.
char text[] = R"___(
    date = 26th of march 2017

    [ImageData]
    image = splat1.dds
    height = 1024
    width = 1024
    pixelsum = 269479.6901960784
)___";

// Variables to store extracted data
const char *date = nullptr;
const char *image = nullptr;
long long width = 0;
long long height = 0;
double pixelsum = 0;

// Example INI parsing logic
INI_BEGIN_TEXT(text)
    INI_STRING(GLOBAL, date) date = value;
    INI_STRING(ImageData, image)
        image = value;
        if (char *last_slash = strrchr(pathBuffer, '/')) {
            // Replace after the last slash
            strcpy(last_slash + 1, image);
        } else {
            // No slash, replace the whole string
            strcpy(pathBuffer, image);
        }
        puts(pathBuffer);
    INI_LONG(ImageData, width) width = value;
    INI_LONG(ImageData, height) height = value;
    INI_DOUBLE(ImageData, pixelsum) pixelsum = value;
INI_END();
```

### Explanation
* INI_BEGIN_TEXT and INI_END must enclose all other macros.
* INI_STRING, INI_LONG, and INI_DOUBLE are macros to extract specific keys (image, width, height, and pixelsum) under a section (ImageData) in the .ini file.
    * `INI_LONG` will read a whole number.
    * `INI_DOUBLE` will read a floating point number with double precission.
    * `INI_STRING` will read the entire line until it sees a return character or the end of the file. It will skip over leading or trailing whiespaces.
* The macro can end with a semicolon indicating the end ofr be wollowed up with a scope section `{...}` where the new value is handled in a specific way
* When a ini file starts with values with no section they default in the `GLOBAL` section.

## Important
Note that `INI_STRING` will yield a `const char *` pointer pointing into the passed in `text` variable and it's lifetime is bound to the lifetime of `text`.

## Error handling

Errors while parsing can be handled as follows

```cpp
INI_BEGIN_TEXT(text)
    INI_LONG(ImageData, width);
    INI_ERROR(code, line) {
        printf("Error in ini file. Line %d \n", line);
        if(code == INI_ERROR_CODE_UNEXPECTED_CHARACTER) {
            printf("unexpected character \n");
        }
        if(code == INI_ERROR_CODE_UNEXPECTED_END) {
            printf("premature end while parsing \n");
        }
        if(code == INI_ERROR_CODE_TYPE) {
            printf("unexpected type \n");
        }
    }
INI_END();
```