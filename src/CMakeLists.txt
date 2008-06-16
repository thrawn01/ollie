# ----------------------------------------------------------------
# Ollie CMake BuildSystem - 2007
#
# Derrick Wippler 2007 (thrawn01@gmail.com) 
# ----------------------------------------------------------------

# Add the ollie Library
ADD_LIBRARY(ollie ollie.cpp file.cpp iohandle.cpp utf8.cpp )
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ENABLE_TESTING()

# Add our Tests
# -----------------
ADD_CXXTEST(bufferTests buffertests.h )
TARGET_LINK_LIBRARIES(bufferTests ollie )

ADD_CXXTEST(ioTests iotests.h )
TARGET_LINK_LIBRARIES(ioTests ollie )

ADD_CXXTEST(fileTests filetests.h )
TARGET_LINK_LIBRARIES(fileTests ollie )
