# ----------------------------------------------------------------
# Ollie CMake BuildSystem - 2007
#
# Derrick Wippler 2007 (thrawn01@gmail.com) 
# ----------------------------------------------------------------

# Add the ollie Library
ADD_LIBRARY(ollie Ollie.cpp Page.cpp PageBuffer.cpp File.cpp IOHandle.cpp )
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ENABLE_TESTING()

# Add our Tests
# -----------------
ADD_CXXTEST(IOHandleTests IOHandleTests.h )
TARGET_LINK_LIBRARIES(IOHandleTests ollie )

ADD_CXXTEST(FileTests FileTests.h )
TARGET_LINK_LIBRARIES(FileTests ollie )

ADD_CXXTEST(PPtrListTests PPtrListTests.h )
TARGET_LINK_LIBRARIES(PPtrListTests ollie )

ADD_CXXTEST(PageTests PageTests.h )
TARGET_LINK_LIBRARIES(PageTests ollie )

ADD_CXXTEST(PageBufferTests PageBufferTests.h )
TARGET_LINK_LIBRARIES(PageBufferTests ollie )

#ADD_CXXTEST(BufferTests BufferTests.h )
#TARGET_LINK_LIBRARIES(BufferTests ollie )
