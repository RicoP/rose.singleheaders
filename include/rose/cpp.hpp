#pragma once

// TODO: implement
#define check(X) (void)(X)

struct RCppSpan {
    const char * p;
    int begin;
    int end;

    static RCppSpan Empty() {
        return {"", 0, 0};
    }

    int length() { return end - begin; }

    char get() { return p[begin]; }

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

    char operator*() { return get(); }
    bool operator==(const char * rhs) { return  compare(rhs); }
    bool operator!=(const char * rhs) { return !compare(rhs); }
    operator bool() { return length() != 0; }
};

struct RCppScope {
    enum class Type {
        None = 0,
        Global,
        Struct,
        VariableDeclaration
    } Type = Type::None;
    struct RCppScope * Parent = nullptr;
    union {
        struct RCppGlobal * Global;
        struct RCppStruct * Struct;
        struct RCppVariableDeclaration * VariableDeclaration;
    };
};

typedef int (*RCppParserCallback)(struct RCpp &, struct RCppScope &/*, ...*/);

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
        printf("(i: %d) -> [%.5s]\n", i, p+i);
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

    bool symbol_peek(char symbol) {
        skip_ws();
        return p[i] == symbol;
    }

    bool symbol_skip(char symbol) {
        if(symbol_peek(symbol)) {
            i++;
            return true;
        }
        return false;
    }

    void error(const char * message) {
        //TODO: implement me properly
        printf("ERROR %s \n", message);
        exit(1);
    }

    int parse(RCppParserCallback);
    int parse_body(RCppScope &, RCppParserCallback);
};

struct RCppGlobal {
    void * user = nullptr;
};

struct RCppStruct {
    void * user = nullptr;
    RCppSpan name;
    bool is_class;
};

struct RCppVariableDeclaration {
    void * user = nullptr;
    RCppSpan type;
    RCppSpan name;
};

int RCpp::parse(RCppParserCallback callback) {
    RCppGlobal Global { user };
    RCppScope Scope { RCppScope::Type::Global, nullptr };
    Scope.Global = &Global;
    return parse_body(Scope, callback);
}

int RCpp::parse_body(RCppScope & Scope, RCppParserCallback callback) {
    // TODO: handle errors
    #define STR2(X) #X
    #define STR(X) STR2(X)
    #define assume(X) \
        do { if(X) break; \
            printf("ERROR [" STR(X) "] line %d \n", __LINE__); \
            debug_print_state(); \
            exit(1); \
        } while(0)

    int n = 0;
    state = 0;

    tidy_up_comments();
    for(;;) {
        skip_ws();

        if(p[i] == 0)   return n;
        if(symbol_skip('}')) return n;

        bool is_struct = false;
        bool is_class  = false;
        if((is_struct = keyword_skip("struct")) || (is_class = keyword_skip("class"))) {
            n++;
            check(is_struct != is_class);
            RCppStruct Struct { user };
            Struct.is_class = is_class;
            Struct.name = keyword_read();

            RCppScope StructScope { RCppScope::Type::Struct, &Scope };
            StructScope.Struct = &Struct;
            if(symbol_skip('{')) {
                parse_body(StructScope, callback);
            }
            else {
                callback(*this, StructScope);
            }

            if(RCppSpan name = keyword_read()) {
                RCppSpan type = Struct.name;
                RCppVariableDeclaration VarDecl { user, type, name };
                RCppScope DeclScope { RCppScope::Type::VariableDeclaration, &Scope };
                DeclScope.VariableDeclaration = &VarDecl;
                callback(*this, DeclScope);
            }

            assume(symbol_skip(';'));
            //RCppScopeGlobal global;
            //callback(*this, global);
            continue;
        }
        RCppSpan type = keyword_read();
        do {
            RCppSpan name = keyword_read();
            RCppVariableDeclaration VarDecl { user, type, name };
            RCppScope DeclScope { RCppScope::Type::VariableDeclaration, &Scope };
            DeclScope.VariableDeclaration = &VarDecl;
            callback(*this, DeclScope);
        } while(symbol_skip(','));
        assume(symbol_skip(';'));
    }

    #undef assume
}

#undef check