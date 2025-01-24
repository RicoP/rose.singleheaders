#pragma once

struct RCppSpan {
    const char * p;
    int begin;
    int count;

    bool compare(const char * rhs) {
        const char * lhs = p + begin;
        const char * lhs_end = p + begin + count;
        for(;;) {
            if(lhs == lhs_end) return *rhs == 0;
            if(*lhs != *rhs) return false;
            if(*lhs == 0) return true;
            lhs++;
            rhs++;
        }
    }
};

struct RCpp {
    // cstr/dstr
    RCpp() = default;
    RCpp(char * text) : p(text) {};
    RCpp(const RCpp &) = delete;
    // variables
    void * user = nullptr;
    char * p = nullptr;
    int state = 0;

};

struct RCppScopeGlobal {
    void * user = nullptr;
    enum class Type {
        NONE = 0,
        Struct
    } type;
};

struct RCppScopeStruct {
    void * user = nullptr;
    RCppSpan name;
};

struct RCppScopeVariable {
    RCppSpan type;
    RCppSpan name;
};

struct RCppScopeStructBody {
    void * user = nullptr;
    enum class Type {
        NONE = 0,
        Variable
    } type;

    union {
        RCppScopeVariable Variable;
    };
};

typedef void (*RCppParserCallback)(RCpp &, RCppScopeGlobal &/*, ...*/);
typedef void (*RCppParserStructCallback)(RCpp &, RCppScopeStruct &/*, ...*/);
typedef void (*RCppParserStructBodyCallback)(RCpp &, RCppScopeStructBody &/*, ...*/);

void rose_cpp_parse(RCpp &, RCppParserCallback) {}
int  rose_cpp_parse_struct(RCpp &, RCppParserStructCallback) { return 1; }
int  rose_cpp_parse_struct_body(RCpp &, RCppParserStructBodyCallback) { return 1; }
