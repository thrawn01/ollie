# ----------------------------------------------------------------
# Ollie CMake BuildSystem - 2008
#
# Derrick Wippler 2008 (thrawn01@gmail.com) 
# ----------------------------------------------------------------

#ADD_DEFINITIONS( ${KDE4_DEFINITIONS} )

FIND_PACKAGE(KDE4 REQUIRED)
INCLUDE_DIRECTORIES( ${KDE4_INCLUDES} )

kde4_add_executable(ktexteditor main.cpp mainwindow.cpp )
TARGET_LINK_LIBRARIES(ktexteditor ${KDE4_KDEUI_LIBS} ${KDE4_KTEXTEDITOR_LIBS} )

