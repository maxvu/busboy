#include "catch/catch.hpp"
#include "Ptr.hpp"
#include "String.hpp"

using busboy::Ptr;
using busboy::String;

TEST_CASE( "Ptr access", "[Ptr]" ) {

    {
        Ptr<int> one = 1;
        CHECK( *one == 1 );
    }

    {
        Ptr<int> two;
        *two = 2;
        CHECK( *two == 2 );
        CHECK( two.isUnique() );
    }

    {
        Ptr<int> three = 3;
        Ptr<int> threeCopy = three;
        CHECK( !three.isUnique() );
        CHECK( *three == 3 );
        CHECK( *threeCopy == 3 );
        CHECK( three.isUnique() );
    }

    {
        Ptr<int> four = 4;
        const Ptr<int> fourCopy = four;
        CHECK( four == fourCopy );
        CHECK( !four.isUnique() );
        CHECK( !fourCopy.isUnique() );
        *four;
        CHECK( four != fourCopy );
        CHECK( four.isUnique() );
        CHECK( fourCopy.isUnique() );
    }

}
