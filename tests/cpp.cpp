#include "minicatch.hpp"
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <rose/cpp.hpp>

const char ** pcondition;
const char ** pfile;
int * pline;

void _check(bool cond, const char * str) {
    if(cond) return;
    printf("\nERROR: [%s]\n", str);
    exit(1);
}

#define check(X) _check(X, STR(X))

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
        }
        return 0;
    });
}


TEST_CASE( "parse simpel struct with vars", "[cpp.hpp]" ) {
    char text[] = R"___(
        struct S { int x,y; };
    )___";
    int count = 0;
    RCpp cpp(text);
    cpp.user = &count;

    cpp.parse([](RCpp & cpp, RCppScope & Scope) {
        int * count = (int*)cpp.user;
        switch(Scope.Type) {
            break; case RCppScope::Type::VariableDeclaration: {
                check(Scope.Parent->Type == RCppScope::Type::Struct);
                check(Scope.Parent->Struct->name == "S");

                RCppVariableDeclaration * VarDecl = Scope.VariableDeclaration;
                check(VarDecl->type == "int");
                check(VarDecl->name == "x" || VarDecl->name == "y");
                (*count)++;
            }
        }
        return 0;
    });

    REQUIRE(count == 2);
}


TEST_CASE( "parse simpel struct with vars and instance", "[cpp.hpp]" ) {
    char text[] = R"___(
        struct S { int x,y; } s;
    )___";
    int count = 0;
    RCpp cpp(text);
    cpp.user = &count;

    cpp.parse([](RCpp & cpp, RCppScope & Scope) {
        int * count = (int*)cpp.user;
        switch(Scope.Type) {
            break; case RCppScope::Type::VariableDeclaration: {
                if(Scope.Parent->Type == RCppScope::Type::Struct) {
                    check(Scope.Parent->Struct->name == "S");

                    RCppVariableDeclaration * VarDecl = Scope.VariableDeclaration;
                    check(VarDecl->type != "int" || (VarDecl->name == "x" || VarDecl->name == "y"));
                }
                else if(Scope.Parent->Type == RCppScope::Type::Global) {
                    RCppVariableDeclaration * VarDecl = Scope.VariableDeclaration;
                    check(VarDecl->type != "S" || (VarDecl->name == "s"));
                } else {
                    check(0);
                }
                (*count)++;
            }
        }
        return 0;
    });

    REQUIRE(count == 3);
}



TEST_CASE( "Global variables", "[cpp.hpp]" ) {
    char text[] = R"___(
        int x,y;
    )___";
    int count = 0;
    RCpp cpp(text);
    cpp.user = &count;

    cpp.parse([](RCpp & cpp, RCppScope & Scope) {
        int * count = (int*)cpp.user;
        switch(Scope.Type) {
            break; case RCppScope::Type::VariableDeclaration: {
                if(Scope.Parent->Type == RCppScope::Type::Global) {
                    RCppVariableDeclaration * VarDecl = Scope.VariableDeclaration;
                    check(VarDecl->type == "int");
                    check((VarDecl->name == "x" || VarDecl->name == "y"));
                } else {
                    check(0);
                }
                (*count)++;
            }
        }
        return 0;
    });

    REQUIRE(count == 2);
}
