#include "catch/catch.hpp"
#include "List.hpp"

using busboy::List;

TEST_CASE( "List access", "[List]" ) {

    SECTION ( "Empty list" ) {

        {
            List<int> a;
            const List<int> b;

            CHECK( a.getLength() == 0 );
            CHECK( b.getLength() == 0 );
            CHECK_THROWS( a.peekHead() );
            CHECK_THROWS( b.peekHead() );
            CHECK_THROWS( a.peekTail() );
            CHECK_THROWS( b.peekTail() );
            CHECK( a.reverse() == b );
        }

        {
            List<int> a, b, c, d;
            a.push( 1 );
            b.unshift( 1 );
            CHECK( a == b );
        }

        {
            List<int> a;
            CHECK( a.begin() == a.end() );
        }

        {
            List<int> a;
            CHECK( !a.begin() );
            CHECK( !a.end() );
        }

        {
            List<int> a;
            a.concat( List<int>({ 1 }) );
            CHECK( a.operator==( List<int>({ 1 }) ) );
        }

        {
            List<int> a;
            CHECK( a.isEmpty() );
        }

        {
            List<int> a;
            List<int> b = a;
            b.reverse();
            CHECK( a.operator==( b ) );
        }

    }

    SECTION ( "List, size one" ) {

        {
            List<int> a = { 1 };
            a.push( 2 );
            CHECK( a.operator==( List<int>({ 1, 2 }) ) );
        }

        {
            List<int> a = { 1 };
            CHECK( a.pop() == 1 );
            CHECK( a.operator==( List<int>() ) );
        }

        {
            List<int> a = { 2 };
            a.unshift( 1 );
            CHECK( a.operator==( List<int>({ 1, 2 }) ) );
        }

        {
            List<int> a = { 1 };
            CHECK( a.shift() == 1 );
            CHECK( a.operator==( List<int>() ) );
        }

        {
            List<int> a = { 2 };
            CHECK( a.pop() == 2 );
        }

        {
            List<int> a = { 2 };
            CHECK( a.shift() == 2 );
        }

        {
            List<int> a = { 1 };
            a.peekHead() = 2;
            CHECK( a.peekTail() == 2 );
        }

        {
            List<int> a = { 1 };
            a.concat( List<int>({ 2 }) );
            CHECK( a.getLength() == 2 );
            CHECK( a.operator==( List<int>({ 1, 2 }) ) );
        }

        {
            CHECK(
                (List<int>({ 1 })).reverse().operator==( List<int>({ 1 }) )
            );
        }

        {
            List<int> a = { 1 };
            CHECK( a.clear().getLength() == 0 );
        }

    }

    SECTION( "List, size two" ) {
        // TODO
    }

    SECTION( "List, size > two" ) {
        // TODO
    }

    SECTION( "Iterator access" ) {
        // TODO
    }

}
