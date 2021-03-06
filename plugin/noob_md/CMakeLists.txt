SET ( MOD_NAME noob_md )

INCLUDE_DIRECTORIES (
	../
	${Boost_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
	${Qt5Widgets_INCLUDE_DIRS} )

LINK_DIRECTORIES (
	${Boost_LIBRARY_DIRS}
	${OpenCV_LIBDIR} )

IF ( WITH_GUI )
	QT5_WRAP_UI ( QT_UI_HEADERS noob_md.ui )
	SET ( HEADER_FILES ${HEADER_FILES} ${QT_UI_HEADERS} )
ENDIF()

SET ( HEADER_FILES ${HEADER_FILES} noob_md.h md_worker.h
	MotionDetection.h target.h bwlabel.h utilities.h ImageEnhancement.h
	TrackingManager.h CompressiveTracker.h tracklet.h DLBPUtilities.h
	ssim.h )
SET ( SOURCE_FILES ${SOURCE_FILES} noob_md.cpp md_worker.cpp
	MotionDetection.cpp target.cpp utilities.cpp ImageEnhancement.cpp )
	#TrackingManager.cpp CompressiveTracker.cpp tracklet.cpp DLBPUtilities.cpp

	#ssim.cpp )

ADD_DEFINITIONS ( -DQT_PLUGIN )

# Fix RPATH
IF ( UNIX )
	SET ( CMAKE_SKIP_BUILD_RPATH FALSE )
	SET ( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
	SET ( CMAKE_INSTALL_RPATH "\$ORIGIN:\$ORIGIN/.." )
	SET ( CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE )
ENDIF()

# Fix library build output directory
IF ( WIN32 )
	SET ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${BUILD_TYPE} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )	
ENDIF()

IF ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
	ADD_DEFINITIONS ( -fpermissive )
ENDIF()

ADD_LIBRARY ( ${MOD_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES} )
QT5_USE_MODULES ( ${MOD_NAME} Core )
IF ( WITH_GUI )
	QT5_USE_MODULES ( ${MOD_NAME} Widgets )
ENDIF()
TARGET_LINK_LIBRARIES ( ${MOD_NAME} ${OpenCV_LIBRARIES} ${Boost_LIBRARIES} )

# Install
IF ( WIN32 )
	INSTALL ( TARGETS ${MOD_NAME}
				RUNTIME DESTINATION plugin
				COMPONENT ${MOD_NAME} )
ELSE()
	INSTALL ( TARGETS ${MOD_NAME}
				LIBRARY DESTINATION plugin
				COMPONENT ${MOD_NAME} )
ENDIF()
