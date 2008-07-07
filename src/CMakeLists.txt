# ----------------------------------------------------------------
# Ollie CMake BuildSystem - 2007
#
# Derrick Wippler 2007 (thrawn01@gmail.com) 
# ----------------------------------------------------------------

# Add the ollie Library
ADD_LIBRARY(ollie Ollie.cpp Page.cpp Buffer.cpp )
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ENABLE_TESTING()

# Add our Tests
# -----------------
ADD_CXXTEST(PPtrListTests PPtrListTests.h )
TARGET_LINK_LIBRARIES(PPtrListTests ollie )

ADD_CXXTEST(PageTests PageTests.h )
TARGET_LINK_LIBRARIES(PageTests ollie )

ADD_CXXTEST(BufferTests BufferTests.h )
TARGET_LINK_LIBRARIES(BufferTests ollie )

#ADD_CXXTEST(ioTests iotests.h )
#TARGET_LINK_LIBRARIES(ioTests ollie )

#ADD_CXXTEST(fileTests filetests.h )
#TARGET_LINK_LIBRARIES(fileTests ollie )
