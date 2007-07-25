#include "cxxtest/TestSuite.h"

class BufferTest : public CxxTest::TestSuite
{
    public: 
        void testAddition( void ) {
            TS_ASSERT(  1 + 1 > 1 );
            TS_ASSERT_EQUAL( 1 + 1 , 2 );
        }
}
