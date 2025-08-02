PROJECT (PBSimpleNewsFeed)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6.0)

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

IF (NOT PLATFORM)
	SET (PLATFORM FC)
ENDIF (NOT PLATFORM)

IF (PLATFORM STREQUAL "NX")
	SET (TOOLCHAIN_PATH "../../PBDEV/PBSDK")
	SET (TOOLCHAIN_PREFIX "arm-linux")
	SET (TOOLCHAIN_INSTALL "")
	ADD_DEFINITIONS(-DPLATFORM_NX)
ELSE ()
	SET (TOOLCHAIN_PATH "../../PBDEV/SDK_481")
	SET (TOOLCHAIN_PREFIX "arm-obreey-linux-gnueabi")
	SET (TOOLCHAIN_INSTALL "sysroot/usr")
	ADD_DEFINITIONS(-DPLATFORM_FC)
ENDIF (PLATFORM STREQUAL "NX")
#SET (CMAKE_VERBOSE_MAKEFILE ON)

IF (NOT TARGET_TYPE)
	SET (TARGET_TYPE Linux)
ENDIF (NOT TARGET_TYPE)

IF (NOT CMAKE_BUILD_TYPE)
	SET (CMAKE_BUILD_TYPE Debug)
ENDIF (NOT CMAKE_BUILD_TYPE)

IF (TARGET_TYPE STREQUAL "ARM")
	SET(CMAKE_INSTALL_PREFIX "${TOOLCHAIN_PATH}/${TOOLCHAIN_PREFIX}/${TOOLCHAIN_INSTALL}" CACHE PATH "Install path prefix" FORCE)

	SET (CMAKE_C_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc)
	SET (CMAKE_CXX_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++)
	SET (CMAKE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++)
	SET (CMAKE_ARR ${CMAKE_CURRENT_SOURCE_DIR}/${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-arr)
	SET (CMAKE_STRIP ${CMAKE_CURRENT_SOURCE_DIR}/${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-strip)

	SET (TARGET_INCLUDE "")
	SET (TARGET_LIB pthread inkview freetype z curl)
	
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib_arm)
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib_arm)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin_arm)
ELSE()
	SET(CMAKE_INSTALL_PREFIX "${TOOLCHAIN_PATH}" CACHE PATH "Install path prefix" FORCE)

	FIND_PACKAGE (Freetype REQUIRED)
	FIND_PACKAGE (JPEG REQUIRED)
	FIND_PACKAGE (ZLIB REQUIRED)
	FIND_PACKAGE (CURL REQUIRED)
	FIND_PACKAGE (GTK2 REQUIRED)
	
	LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/${CMAKE_INSTALL_PREFIX}/lib ${TOOLCHAIN_PATH}/lib)
	SET (TARGET_INCLUDE ${CMAKE_INSTALL_PREFIX}/include ${TOOLCHAIN_PATH}/include ${FREETYPE_INCLUDE_DIRS} ${JPEG_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})
	SET (TARGET_LIB pthread inkview png curl ${FREETYPE_LIBRARIES} ${JPEG_LIBRARIES} ${ZLIB_LIBRARIES} ${GTK2_LIBRARIES} ${CURL_LIBRARIES})
	
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib_pc)
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib_pc)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin_pc)
	
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Dpng_set_gray_1_2_4_to_8=png_set_expand_gray_1_2_4_to_8" )
ENDIF(TARGET_TYPE STREQUAL "ARM")

MESSAGE (STATUS "Using ${TOOLCHAIN_PATH}")

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")

	IF (TARGET_TYPE STREQUAL "Linux")
		MESSAGE (STATUS "Build for Linux Debug")
		SET (CMAKE_C_FLAGS_DEBUG "-DDEBUG -W -Wall -Wextra -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -std=c++11 -O0 -g3 -D__EMU__ -DIVSAPP")
		SET (CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -W -Wall -Wextra -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -std=c++11 -O0 -g3 -D__EMU__ -DIVSAPP")
	ENDIF (TARGET_TYPE STREQUAL "Linux")

	IF (TARGET_TYPE STREQUAL "Windows")
		MESSAGE (STATUS "Build for Windows Debug")
		SET (CMAKE_C_FLAGS_DEBUG "-DDEBUG -W -Wall -Wextra -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -std=c++11 -O0 -g3 -D__EMU__ -DIVSAPP")
		SET (CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -W -Wall -Wextra -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -std=c++11 -O0 -g3 -D__EMU__ -DIVSAPP")
	ENDIF (TARGET_TYPE STREQUAL "Windows")

	IF (TARGET_TYPE STREQUAL "ARM")
		MESSAGE (STATUS "Build for ARM Debug")
		SET (CMAKE_C_FLAGS_DEBUG "-DDEBUG -fsigned-char -W -Wall -Wextra -Wno-format-y2k -fomit-frame-pointer -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -std=c++11 -O0 -g3")
		SET (CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -fsigned-char -W -Wall -Wextra -Wno-format-y2k -fomit-frame-pointer -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -std=c++11 -O0 -g3")
	ENDIF (TARGET_TYPE STREQUAL "ARM")

ELSE ()

	IF (TARGET_TYPE STREQUAL "Linux")
		MESSAGE (STATUS "Build for Linux Release")
		SET (CMAKE_C_FLAGS_RELEASE "-DNDEBUG -s -Wall -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -std=c++11 -O2 -D__EMU__ -DIVSAPP")
		SET (CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -s -Wall -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -std=c++11 -O2 -D__EMU__ -DIVSAPP")
	ENDIF (TARGET_TYPE STREQUAL "Linux")

	IF (TARGET_TYPE STREQUAL "Windows")
		MESSAGE (STATUS "Build for Windows Release")
		SET (CMAKE_C_FLAGS_RELEASE "-DNDEBUG -s -Wall -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -std=c++11 -O2 -D__EMU__ -DIVSAPP")
		SET (CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -s -Wall -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -std=c++11 -O2 -D__EMU__ -DIVSAPP")
	ENDIF (TARGET_TYPE STREQUAL "Windows")

	IF (TARGET_TYPE STREQUAL "ARM")
		MESSAGE (STATUS "Build for ARM Release")
		SET (CMAKE_C_FLAGS_RELEASE "-DNDEBUG -s -fsigned-char -Wall -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -std=c++11 -O2 -Wno-format-y2k -fomit-frame-pointer")
		SET (CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -s -fsigned-char -Wall -Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -std=c++11 -O2 -Wno-format-y2k -fomit-frame-pointer")
	ENDIF (TARGET_TYPE STREQUAL "ARM")

ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

FILE(GLOB_RECURSE SRC_LIST
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.cpp
	${CMAKE_SOURCE_DIR}/src/*.c
	${CMAKE_SOURCE_DIR}/3rdParty/gumbo-parser/src/*.h
	${CMAKE_SOURCE_DIR}/3rdParty/gumbo-parser/src/*.cpp
	${CMAKE_SOURCE_DIR}/3rdParty/gumbo-parser/src/*.c
)

ADD_EXECUTABLE (PBSimpleNewsFeed.app 
		${SRC_LIST})

INCLUDE_DIRECTORIES(${TARGET_INCLUDE} ${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${TARGET_INCLUDE} ${CMAKE_SOURCE_DIR}/3rdParty/gumbo-parser/src)
TARGET_LINK_LIBRARIES (PBSimpleNewsFeed.app ${TARGET_LIB})

INSTALL (TARGETS PBSimpleNewsFeed.app DESTINATION bin)

