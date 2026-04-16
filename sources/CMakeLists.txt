
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "../include/y/*.h")

find_package(mio CONFIG REQUIRED)

add_library(mo SHARED
        mo.cpp)
target_sources(mo
        PRIVATE mo.cpp
        PUBLIC ${HEADER_LIST})

target_include_directories(mo PUBLIC ../include)
target_link_libraries(mo PUBLIC mio::mio mio::mio-headers)

install(TARGETS mo DESTINATION lib)
install(DIRECTORY ../include/ DESTINATION include)