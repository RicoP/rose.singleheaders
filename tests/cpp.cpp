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
        struct S {
            int x;
        };
    )___";
    RCpp cpp(text);

    rose_cpp_parse(cpp, +[](RCpp & cpp, RCppScopeGlobal & Global) {
        switch(Global.type) {
            break; case RCppScopeGlobal::Type::Struct: {
                int n = rose_cpp_parse_struct(cpp, +[](RCpp & cpp, RCppScopeStruct & Struct) {
                    check(Struct.name.compare("S"));
                    int n = rose_cpp_parse_struct_body(cpp, +[](RCpp & cpp, RCppScopeStructBody & StructBody) {
                        switch(StructBody.type) {
                            break; case RCppScopeStructBody::Type::Variable:
                            check(StructBody.Variable.type.compare("int"));
                            check(StructBody.Variable.name.compare("x"));
                        }
                    });
                    check(n == 1);
                });
                check(n == 1);
            }
            break; default:
                check(0);
        }
    });
}







