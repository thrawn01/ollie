# ----------------------------------------------------------------
# Ollie CMake BuildSystem - 2007
#
# Derrick Wippler 2007 (thrawn01@gmail.com) 
# ----------------------------------------------------------------

# Add the ollie Library
ADD_LIBRARY(ollie ollie.cpp buffer.cpp file.cpp iohandle.cpp utf8.cpp )

# Add our Tests
# -----------------
ADD_CXXTEST(utf8Tests utf8tests.h )
TARGET_LINK_LIBRARIES(utf8Tests ollie )

ADD_CXXTEST(ioTests iotests.h )
TARGET_LINK_LIBRARIES(ioTests ollie )

ADD_CXXTEST(fileTests filetests.h )
TARGET_LINK_LIBRARIES(fileTests ollie )
