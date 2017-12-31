#include "catch/catch.hpp"
#include "String.hpp"

using busboy::String;

TEST_CASE( "String access", "[String]" ) {

    SECTION( "Construction, equality" ) {

        String one = "one";
        String two = "two";
        CHECK( one.getLength() == 3 );
        CHECK( one.getCapacity() >= 3 );

        CHECK( one.operator==( "one" ) );
        CHECK( one.operator==( String( one ) ) );
        CHECK( !one.operator!=( "one" ) );
        CHECK( !one.operator==( "two" ) );
        CHECK( one.operator!=( two ) );

        CHECK( one.operator<( two ) );
        CHECK( one.operator<= ( two ) );
        CHECK( two.operator>( one ) );
        CHECK( two.operator>=( one ) );

        String anotherOne( one );
        CHECK( one.operator==( anotherOne ) );

    }

    SECTION( "Concatenation" ) {

        CHECK(
            ( String( "Hello" ) + String( ", World!" ) ).operator==(
                "Hello, World!"
            )
        );

        CHECK(
            ( String( "Hello" ) += String( ", World!" ) ).operator==(
                "Hello, World!"
            )
        );

        CHECK(
            ( String( "Hello" ) + ", World!" ).operator==(
                "Hello, World!"
            )
        );

        CHECK(
            ( String( "Hello" ) += ", World!" ).operator==(
                "Hello, World!"
            )
        );

    }

    SECTION( "Assignment, reservation, compaction" ) {

        String msg;
        msg = "Hello, World!";
        CHECK( msg.getLength() == 13 );

        msg.compact();
        CHECK( msg.getLength() == 13 );

        msg.reserve( 23 );
        CHECK( msg.getCapacity() >= 23 );

        msg.compact();
        CHECK( msg.getLength() == 13 );
        CHECK( msg.getCapacity() == 13 );

        CHECK( msg.operator==( "Hello, World!" ) );

    }

    SECTION( "Subscript access" ) {

        String msg( "Hello, World!" );
        CHECK( msg[ 0 ] == 'H' );
        CHECK( msg[ 1 ] == 'e' );
        CHECK( msg[ 2 ] == 'l' );
        CHECK( msg[ 3 ] == 'l' );
        CHECK( msg[ 4 ] == 'o' );
        CHECK( msg[ 12 ] == '!' );
        CHECK_THROWS( msg[ 13 ] );

    }

}
