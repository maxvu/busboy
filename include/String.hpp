#ifndef BUSBOY_STRING
#define BUSBOY_STRING

#define BUSBOY_STRING_DEFAULTLEN 10
#define BUSBOY_STRING_GROWTHFACT 1.5

#include <cstddef>
#include <cstring>

namespace busboy {

    class String {

        char * buf;
        size_t len; // the length of the string
        size_t cap; // the capacity of buf, minus one

        String & __resize ( size_t newCap );

        public:

        String ( size_t initCap );
        String ();
        String ( const char * cstr );
        String ( const String & str );
        String ( const String && str );

        ~String ();

        size_t getLength () const;
        size_t getCapacity () const;

        String & operator= ( const String & other );
        String & operator= ( const char * other );

        bool operator== ( const String & other ) const;
        bool operator== ( const char * cstr ) const;

        bool operator!= ( const String & other ) const;
        bool operator!= ( const char * cstr ) const;

        bool operator< ( const String & other ) const;
        bool operator> ( const char * cstr ) const;

        bool operator<= ( const String & other ) const;
        bool operator>= ( const char * cstr ) const;

        String & operator+= ( const String & other );
        String & operator+= ( const char * cstr );

        String operator+ ( const String & other ) const;
        String operator+ ( const char * cstr ) const;

        char & operator[] ( size_t idx );
        const char & operator[] ( size_t idx ) const;

        String & reserve ( size_t newCap );
        String & compact ();

        const char * find ( const char * substr ) const;

        operator const char * () const;

    };

};

#endif
