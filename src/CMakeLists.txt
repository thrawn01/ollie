# ----------------------------------------------------------------
# Ollie CMake BuildSystem - 2007
#
# Derrick Wippler 2007 (thrawn01@gmail.com) 
# ----------------------------------------------------------------

# Add the ollie Library
ADD_LIBRARY(ollie ollie.cpp buffer.cpp file.cpp iohandle.cpp asciibuffer.cpp )

# Add our Tests
# -----------------
ADD_CXXTEST(bufferTests buffertests.h )
TARGET_LINK_LIBRARIES(bufferTests ollie )
