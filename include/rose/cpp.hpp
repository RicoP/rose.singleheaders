#pragma once

// TODO: implement
#define check(X) (void)(X)

typedef void (*RCppParserCallback)(struct RCpp &, struct RCppScopeGlobal &/*, ...*/);
typedef void (*RCppParserStructCallback)(struct RCpp &, struct RCppScopeStruct &/*, ...*/);
typedef void (*RCppParserStructBodyCallback)(struct RCpp &, struct RCppScopeStructBody &/*, ...*/);

struct RCppSpan {
    const char * p;
    int begin;
    int end;

    static RCppSpan Empty() {
        return {"", 0, 0};
    }

    int length() { return end - begin; }

    char get() { return p[begin]; }

    char operator*() { return get(); }

    bool compare(const char * rhs) {
        const char * lhs = p + begin;
        const char * lhs_end = p + end;
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
    // variables
    void * user = nullptr;
    char * p = nullptr;
    int i = 0;
    int state = 0;
    // cstr/dstr
    RCpp() = default;
    RCpp(char * text) : p(text) {};
    RCpp(const RCpp &) = delete;


    void debug_print_state() {
        printf("(i: %d) -> %.5s\n", i, p+i);
    }


private:
    bool is_number(char c)     { return c >= '0' && c <= '9'; }
    bool is_upper(char c)      { return c >= 'A' && c <= 'Z'; }
    bool is_lower(char c)      { return c >= 'a' && c <= 'z'; }
    bool is_alpha(char c)      { return is_upper(c) || is_lower(c); }
    bool is_alpha_num(char c)  { return is_alpha(c) || is_number(c); }
    bool is_whitespace(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }

    void skip_ws() { while(is_whitespace(p[i])) { i++; } }

    RCppSpan keyword_peek() {
        skip_ws();

        RCppSpan span { p, i, i };
        if(is_number(*span)) return span;
        if(!is_alpha(*span)) return span;
        while(is_alpha_num(p[span.end])) { span.end++; }
        return span;
    }

public:

    void tidy_up_comments() {}

    bool keyword_skip(const char * keyword) {
        RCppSpan span = keyword_peek();
        if(span.compare(keyword)) {
            i += span.length();
            return true;
        }
        return false;
    }

    RCppSpan keyword_read() {
        RCppSpan keyword = keyword_peek();
        i+=keyword.length();
        return keyword;
    }

    bool symbol_skip(char symbol) {
        skip_ws();
        if(p[i] == symbol) {
            i++;
            return true;
        }
        return false;
    }

    void parse(RCppParserCallback callback);
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
    bool is_class;
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

void RCpp::parse(RCppParserCallback callback) {
    // TODO: handle errors
    #define assume(X) (X)

    int n = 0;
    state = 0;

    tidy_up_comments();
    for(;;) {
        skip_ws();

        if(p[i] == 0) return;

        bool is_struct = false;
        bool is_class  = false;
        if((is_struct = keyword_skip("struct")) || (is_class = keyword_skip("class"))) {
            check(is_struct != is_class);
            RCppScopeStruct Struct { user };
            Struct.is_class = is_class;
            Struct.name = keyword_read();

            assume(symbol_skip(';'));

            //RCppScopeGlobal global;
            //callback(*this, global);
        }
    }

    #undef assume
}
int  rose_cpp_parse_struct(RCpp &, RCppParserStructCallback) { return 1; }
int  rose_cpp_parse_struct_body(RCpp &, RCppParserStructBodyCallback) { return 1; }

#undef check