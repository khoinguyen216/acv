SET ( MOD_NAME acv )

# Perform dependency check
IF ( NOT Boost_INCLUDE_DIRS OR NOT Boost_LIBRARY_DIR OR NOT Boost_LIBRARIES )
    MESSAGE ( FATAL_ERROR "Boost is required." )
ENDIF()

# Configure Qt5
SET ( CMAKE_AUTOMOC ON )
SET ( CMAKE_INCLUDE_CURRENT_DIR ON )

INCLUDE_DIRECTORIES (
	../plugin
	${Boost_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS})

LINK_DIRECTORIES (
	${FFmpeg_LIBDIR} ) 

QT5_WRAP_UI ( QT_UI_HEADERS mainwindow.ui )

SET ( HEADER_FILES ${QT_UI_HEADERS}
	gui.h version.h inst_info.h cable_info.h plugin_factory.h inst_graph.h
	acv_config.h )

SET ( SOURCE_FILES
    main.cpp acv.cpp gui.cpp plugin_factory.cpp inst_graph.cpp cable_info.cpp
	opencv_link.cpp )

SET ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )

# Fix RPATH
SET ( CMAKE_SKIP_BUILD_RPATH FALSE )
SET ( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
SET ( CMAKE_INSTALL_RPATH "\$ORIGIN:\$ORIGIN/plugins" )
SET ( CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE )

ADD_EXECUTABLE ( ${MOD_NAME} ${SOURCE_FILES} ${HEADER_FILES} )
QT5_USE_MODULES ( ${MOD_NAME} Widgets Xml )

TARGET_LINK_LIBRARIES ( ${MOD_NAME}
	${Boost_LIBRARIES} ${FFmpeg_LIBRARIES}
	${OpenCV_LIBRARIES} )

# Copy configuration files
ADD_CUSTOM_COMMAND ( TARGET acv POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/sample/acv.xml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )

INSTALL ( TARGETS ${MOD_NAME}
			RUNTIME DESTINATION .
			COMPONENT ${MOD_NAME} )

# Make a standalone application with all dependencies in place
SET ( APPS "${CMAKE_INSTALL_PREFIX}/acv" )
SET ( DIRS
	${Boost_LIBRARY_DIRS}
	${FFmpeg_LIBDIR}
	${Qt5_LIBDIR}
	${OpenCV_LIBDIR} )
INSTALL ( CODE "
    INCLUDE ( BundleUtilities )
    fixup_bundle (\"${APPS}\" \"\" \"${DIRS}\") "
    COMPONENT ${MOD_NAME} )
