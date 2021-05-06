find_package(Curses)

if(CURSES_FOUND)
    add_definitions(-DHAS_CURSES)
    set(CURSES_LIB ${CURSES_LIBRARY})
    include_directories(SYSTEM ${CURSES_INCLUDE_DIR})
endif(CURSES_FOUND)

