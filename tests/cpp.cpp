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

TEST_CASE( "parse simpel struct", "[cpp.hpp]" ) {
    char text[] = R"___(
        struct S;
    )___";
    RCpp cpp(text);

    cpp.parse([](RCpp & cpp, RCppScope & Scope) {
        switch(Scope.Type) {
            break; case RCppScope::Type::Struct: {
                RCppStruct * Struct = Scope.Struct;
                check(Struct->name == "S");
            }
            break; case RCppScope::Type::Global: {
            }
            break; default:
                printf("You shouldn't be here \n");
                check(0);
        }
        return 0;
    });
}

TEST_CASE( "parse simpel struct {}", "[cpp.hpp]" ) {
    char text[] = R"___(
        struct S {};
    )___";
    RCpp cpp(text);

    cpp.parse([](RCpp & cpp, RCppScope & Scope) {
        switch(Scope.Type) {
            break; case RCppScope::Type::Struct: {
                RCppStruct * Struct = Scope.Struct;
                check(Struct->name == "S");
            }
            break; case RCppScope::Type::Global: {
            }
            break; default:
                printf("You shouldn't be here \n");
                check(0);
        }
        return 0;
    });
}
