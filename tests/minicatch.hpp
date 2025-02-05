#pragma once

#include <vector>
#include <stdio.h> // printf

#define MC_MAKE_UNIQUE_NAME_CONCAT_IMPL(prefix, suffix) prefix##suffix
#define MC_MAKE_UNIQUE_NAME_CONCAT(prefix, suffix) MC_MAKE_UNIQUE_NAME_CONCAT_IMPL(prefix, suffix)
#define MC_MAKE_UNIQUE_NAME(prefix) MC_MAKE_UNIQUE_NAME_CONCAT(prefix##_, __LINE__)


struct MiniCatchCase {
    typedef void (*FPTestCase)(const char **, const char **, int *);

    const char * label = nullptr;
    const char * section = nullptr;
    FPTestCase testcase = nullptr;
};

struct MiniCatch {
    std::vector<MiniCatchCase> cases;

    void reg(const char * label, const char * section, MiniCatchCase::FPTestCase f) {
        MiniCatchCase testcase;
        testcase.label = label;
        testcase.section = section;
        testcase.testcase = f;
        cases.push_back(testcase);
    }
};

MiniCatch & minicatch() {
    static MiniCatch instance;
    return instance;
}

#define TEST_CASE(LABEL, SECTION) \
struct MC_MAKE_UNIQUE_NAME(TEST_CASE_) { \
    static void runner(const char **, const char **, int *); \
    MC_MAKE_UNIQUE_NAME(TEST_CASE_) () { \
        minicatch().reg(LABEL, SECTION, MC_MAKE_UNIQUE_NAME(TEST_CASE_) ::runner); \
    } \
} MC_MAKE_UNIQUE_NAME(TEST_CASE_VAR_); \
void MC_MAKE_UNIQUE_NAME(TEST_CASE_) ::runner(const char ** pcondition, const char ** pfile, int * pline)

int main() {
    int returnvalue = 0;
    for(auto & testcase : minicatch().cases) {
        printf(
            "Testing section [%s], case [%s]: "
            , testcase.section, testcase.label
        );

        const char * condition = "";
        const char * file = "";
        int line = 0;
        testcase.testcase(&condition, &file, &line);
        if(line) {
            returnvalue = 1;
            printf(
                "Fail \n"
                "> File: %s:%d \n"
                "> Condition: %s \n\n"
                , file, line
                , condition
            );
            continue;
        }
        printf("OK \n");
    }
    if(returnvalue == 0) { }
    else                 { printf("\n\n<Errors>\n\n"); }

    return returnvalue;
}

#define REQUIRE( COND ) do { \
        *pline = 0; \
        if(COND) break; \
        *pcondition = (#COND); \
        *pfile = __FILE__; \
        *pline = __LINE__; \
        return; \
    } while(0)


#undef MC_WIN32CDECL
#undef MC_WIN32DLLIMPORT
#undef MC_LINUXNOEXCEPT
#undef MC_TOKENPASTE
#undef MC_TOKENPASTEX
