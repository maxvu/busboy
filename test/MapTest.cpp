#include "catch/catch.hpp"
#include "String.hpp"
#include "Map.hpp"

using busboy::String;
using busboy::Map;

TEST_CASE( "Map access", "[Map]" ) {

    Map<int> ints;
    ints.set( "one", 1 );
    ints.set( "two", 2 );
    ints.set( "three", 3 );
    ints.set( "four", 0 );
    ints[ "four" ] = ints[ "one" ] + ints[ "three" ];

    REQUIRE( ints[ "one" ] == 1 );
    REQUIRE( ints[ "two" ] == 2 );
    REQUIRE( ints[ "three" ] == 3 );

    auto it = ints.begin();
    int sum = 0;
    while ( it ) {
        sum += *it;
        it++;
    }
    REQUIRE( sum == 10 );

    // TODO

}
