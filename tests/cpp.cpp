#include "minicatch.hpp"
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <rose/cpp.hpp>

const char ** pcondition;
const char ** pfile;
int * pline;

void check(bool cond) {
    if(cond) return;
    puts("error");
    exit(1);
}

TEST_CASE( "basics1", "[cpp.hpp]" ) {
    char text[] = R"___(
        struct S;
    )___";
    RCpp cpp(text);

    cpp.parse([](RCpp & cpp, RCppScopeGlobal & Global) {
        switch(Global.type) {
            break; case RCppScopeGlobal::Type::Struct: {
                int n = rose_cpp_parse_struct(cpp, +[](RCpp & cpp, RCppScopeStruct & Struct) {
                    check(Struct.name.compare("S"));
                });
                check(n == 1);
            }
            break; default:
                check(0);
        }
    });
}







