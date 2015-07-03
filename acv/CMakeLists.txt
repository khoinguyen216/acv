SET ( MOD_NAME acv )

# Perform dependency check
IF ( NOT Boost_INCLUDE_DIRS OR NOT Boost_LIBRARY_DIR OR NOT Boost_LIBRARIES )
    MESSAGE ( FATAL_ERROR "Boost is required." )
ENDIF()

# Configure Qt5
SET ( CMAKE_AUTOMOC ON )
SET ( CMAKE_INCLUDE_CURRENT_DIR ON )

# Congfigure Visual Studio user file for execution environment
SET ( DLL_PATHS "PATH=${Boost_LIBRARY_DIRS};${Qt5_LIBDIR};${OpenCV_LIBDIR};\$PATH")
CONFIGURE_FILE ( ${CMAKE_SOURCE_DIR}/cmake/acv.vcxproj.user.in
	acv.vcxproj.user @ONLY )	

INCLUDE_DIRECTORIES (
	../plugin
	${Boost_INCLUDE_DIRS}
	${FFmpeg_INCLUDE_DIRS}
	${LiveMedia_INCLUDE_DIRS}
	${Lua_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS})

LINK_DIRECTORIES (
	${FFmpeg_LIBDIR}
	${LiveMedia_LIBDIR}
	${Lua_LIBDIR} )

IF ( WITH_GUI )
	QT5_WRAP_UI ( QT_UI_HEADERS mainwindow.ui )
	SET ( HEADER_FILES ${HEADER_FILES} ${QT_UI_HEADERS} gui.h )
	SET ( SOURCE_FILES ${SOURCE_FILES} gui.cpp )
ENDIF()

SET ( HEADER_FILES ${HEADER_FILES}
	version.h inst_info.h cable_info.h plugin_factory.h inst_graph.h
	script_if.h
	acv_config.h )

SET ( SOURCE_FILES ${SOURCE_FILES}
    main.cpp acv.cpp acv_config.cpp plugin_factory.cpp inst_graph.cpp
	inst_info.cpp cable_info.cpp
	script_if.cpp
	ffmpeg_link.cpp livemedia_link.cpp opencv_link.cpp )

# Fix RPATH
SET ( CMAKE_SKIP_BUILD_RPATH FALSE )
SET ( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
SET ( CMAKE_INSTALL_RPATH "\$ORIGIN" )
SET ( CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE )

SET ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILD_TYPE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )

IF ( MSVC )
	SET ( CMAKE_EXE_LINKER_FLAGS "/OPT:NOREF ${CMAKE_EXE_LINKER_FLAGS}")
ENDIF()

ADD_EXECUTABLE ( ${MOD_NAME} ${SOURCE_FILES} ${HEADER_FILES} )
QT5_USE_MODULES ( ${MOD_NAME} Core Xml Network )
IF ( WITH_GUI )
	QT5_USE_MODULES ( ${MOD_NAME} Widgets )
ENDIF()

TARGET_LINK_LIBRARIES ( ${MOD_NAME}
	${Boost_LIBRARIES} ${FFmpeg_LIBRARIES} ${LiveMedia_LIBRARIES}
	${Lua_LIBRARIES}
	${OpenCV_LIBRARIES} )

IF ( WIN32 )
	TARGET_LINK_LIBRARIES ( ${MOD_NAME} ws2_32 )
ENDIF()

# Copy configuration files
ADD_CUSTOM_COMMAND ( TARGET acv POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/sample/acv.xml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )

INSTALL ( TARGETS ${MOD_NAME}
			RUNTIME DESTINATION .
			COMPONENT ${MOD_NAME} )

# Make a standalone application with all dependencies in place
IF ( WIN32 )
	SET ( APPS "${CMAKE_INSTALL_PREFIX}/acv.exe" )
ELSE()
	SET ( APPS "${CMAKE_INSTALL_PREFIX}/acv" )
ENDIF()
	
SET ( DIRS
	${Boost_LIBRARY_DIRS}
	${FFmpeg_LIBDIR}
	${FFmpeg_BINDIR}
	${LiveMedia_LIBDIR}
	${Qt5_LIBDIR}
	${OpenCV_LIBDIR} )
INSTALL ( CODE "
    INCLUDE ( ${CMAKE_SOURCE_DIR}/cmake/BundleUtilities.cmake )
    fixup_bundle (\"${APPS}\" \"\" \"${DIRS}\") "
    COMPONENT ${MOD_NAME} )
