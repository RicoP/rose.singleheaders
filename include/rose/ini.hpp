#pragma once

#ifdef _WIN32
# define RINI_WIN32CDECL __cdecl
# define RINI_WIN32DLLIMPORT __declspec(dllimport)
# define RINI_LINUXNOEXCEPT
#else
# define RINI_WIN32CDECL
# define RINI_WIN32DLLIMPORT
# define RINI_LINUXNOEXCEPT noexcept
#endif

#if defined(_WIN32) && defined(_MSVC_LANG)
# define RINI_CPLUSPLUS _MSVC_LANG
#else
# define RINI_CPLUSPLUS __cplusplus
#endif

#if RINI_CPLUSPLUS >= 201703
# define RINI_FALLTHROUGH [[fallthrough]]
#else
# define RINI_FALLTHROUGH (void)(0)
#endif

extern "C" RINI_WIN32DLLIMPORT double RINI_WIN32CDECL atof (const char* str) RINI_LINUXNOEXCEPT;
extern "C" RINI_WIN32DLLIMPORT long long int RINI_WIN32CDECL atoll ( const char * str ) RINI_LINUXNOEXCEPT;
extern "C" RINI_WIN32DLLIMPORT int printf ( const char * format, ... );

struct RIniParser {
    typedef unsigned long long hash_t;

    static constexpr char to_lower(char c) {
        if(c >= 'A' && c <= 'Z') c += ('a' - 'A');
        return c;
    }

    static constexpr hash_t shuffle(hash_t a, hash_t b) {
        a ^= a << 13;
        a ^= a >> 8;
        a ^= a << 17;
        a ^= b;
        return a;
    }

    static constexpr hash_t str_hash(const char * str) {
        hash_t h = 0;
        while(char c = *str) {
            h = shuffle(h, to_lower(c));
            str++;
        }
        return h;
    }

    static constexpr bool is_ws(char c) { return c == ' ' || c == '\t' || c == '\r'; }

    char * buffer = nullptr;
	hash_t hashSection = str_hash("GLOBAL");
    int p = 0;

    enum { OK = 0, EOF = -1, UE = -2, UC = -3 };


    static RIniParser FromString(char * string) {
        RIniParser value;
        value.buffer = string;
        return value;
    }

    char peek() const { return buffer[p]; }
    char get()        { return buffer[p++]; }

	void skip_ws() {
		while (is_ws(peek())) {
			++p;
		}
	}

	void skip_line() {
		for(;;) {
            if (buffer[p] == '\n' || buffer[p] == 0) return;
            ++p;
		}
	}

    hash_t next() {
        hash_t hashLabel = 0;
        char c;

        for(;;) {
            skip_ws();

            c = peek();
            if(c == 0) {
                return EOF;
            }

            c = get();
            if(c == '\n') {
                continue;
            }
            if(c == ';') {
                skip_line();
                continue;
            }

            if(c == '[') {
                hashSection = 0;
                skip_ws();
                while((c = get()) != ']') {
                    if(c == 0) return UE;
                    if(is_ws(c)) {
                        skip_ws();
                        if((c = get()) != ']') {
                            return UC;
                        }
                        break;
                    }
                    hashSection = shuffle(hashSection, to_lower(c));
                }
                continue;
            }

            // new Label
            hashLabel = to_lower(c);
            for(;;) {
                c = get();
                if(c == 0)   { --p;       return UE; }
                if(c == '=') { --p;       break;     }
                if(is_ws(c)) { skip_ws(); break;     }
                hashLabel = shuffle(hashLabel, to_lower(c));
            }

            if((c = get()) != '=') return UC;

            // after [WS]*'='[WS]* comes the value
            skip_ws();
            return shuffle(hashSection, hashLabel);
        }
    }

    unsigned read_number(char * buffer, unsigned maxsize) {
        unsigned size = 0;

        for(;;) {
            switch(char c = peek()) {
                default: c = 0; //terminator
                RINI_FALLTHROUGH;
                case '-': case '+': case '.':
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                    buffer[size % maxsize] = c;
                    size++;
                    p++;
                    if(c == 0) return size;
            }
        }
    }

    long long readLong() {
        enum { MAXSIZE = 1024 };
        char buffer[MAXSIZE] = "-1";
        unsigned size = read_number(buffer, MAXSIZE);
        if(MAXSIZE < size) return -1;
        return atoll(buffer);
    }

    double readDouble() {
        enum { MAXSIZE = 1024 };
        char buffer[MAXSIZE] = "-1";
        unsigned size = read_number(buffer, MAXSIZE);
        if(MAXSIZE < size) return -1;
        return atof(buffer);
    }

    const char * readString() {
        int begin = p;
        int end = p;
        while(char c = get()) {
            if(is_ws(c)) continue;
            if(c == '\n') break;
            end = p;
        }
        buffer[end] = 0;
        return buffer + begin;
    }
};

#define INI_BEGIN_TEXT(text) \
    RIniParser INI_STATE = RIniParser::FromString(text); \
    for(;;) { \
    RIniParser::hash_t hash = INI_STATE.next(); \
    switch(hash) {

#define INI_STRING(SECTION, LABEL) \
    break; case RIniParser::shuffle(RIniParser::str_hash(#SECTION),RIniParser::str_hash(#LABEL)): \
    LABEL = INI_STATE.readString();

#define INI_DOUBLE(SECTION, LABEL) \
    break; case RIniParser::shuffle(RIniParser::str_hash(#SECTION),RIniParser::str_hash(#LABEL)): \
    LABEL = INI_STATE.readDouble();

#define INI_LONG(SECTION, LABEL) \
    break; case RIniParser::shuffle(RIniParser::str_hash(#SECTION),RIniParser::str_hash(#LABEL)): \
    LABEL = INI_STATE.readLong();

#define INI_END(...) \
    break; default: INI_STATE.skip_line(); } if(hash != -1) continue; else break; }

#undef RINI_WIN32CDECL
#undef RINI_WIN32DLLIMPORT
#undef RINI_LINUXNOEXCEPT
#undef RINI_CPLUSPLUS
#undef RINI_FALLTHROUGH