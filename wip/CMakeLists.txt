PROJECT(linux-samples)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -stdlib=libc++")

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(LIBOPENTOK REQUIRED libopentok)

FIND_PACKAGE(Threads)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${OPENTOK_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/common/src)
LINK_DIRECTORIES(${SDL2_LIBRARY_DIRS} ${OPENTOK_LIBRARY_DIRS})

ADD_EXECUTABLE(connect-session connect-session/src/main.cpp common/src/renderer.cpp)
TARGET_LINK_LIBRARIES(connect-session ${SDL2_LIBRARIES} ${LIBOPENTOK_LIBRARIES})

ADD_EXECUTABLE(connect-session-with-settings connect-session-with-settings/src/main.cpp common/src/renderer.cpp)
TARGET_LINK_LIBRARIES(connect-session-with-settings ${SDL2_LIBRARIES} ${LIBOPENTOK_LIBRARIES})

ADD_EXECUTABLE(session-signals session-signals/src/main.cpp common/src/renderer.cpp)
TARGET_LINK_LIBRARIES(session-signals ${SDL2_LIBRARIES} ${LIBOPENTOK_LIBRARIES})

ADD_EXECUTABLE(session-publish session-publish/src/main.cpp common/src/renderer.cpp)
TARGET_LINK_LIBRARIES(session-publish ${SDL2_LIBRARIES} ${LIBOPENTOK_LIBRARIES})

ADD_EXECUTABLE(session-publish-subscribe session-publish-subscribe/src/main.cpp common/src/renderer.cpp)
TARGET_LINK_LIBRARIES(session-publish-subscribe ${SDL2_LIBRARIES} ${LIBOPENTOK_LIBRARIES})

ADD_EXECUTABLE(audio-device audio-device/src/main.cpp common/src/otk_thread.c common/src/renderer.cpp)
TARGET_LINK_LIBRARIES(audio-device pthread ${SDL2_LIBRARIES} ${LIBOPENTOK_LIBRARIES})

ADD_EXECUTABLE(video-capturer video-capturer/src/main.cpp common/src/otk_thread.c common/src/renderer.cpp)
TARGET_LINK_LIBRARIES(video-capturer pthread ${SDL2_LIBRARIES} ${LIBOPENTOK_LIBRARIES})
