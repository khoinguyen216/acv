SET ( MOD_NAME frame_in )

INCLUDE_DIRECTORIES (
	../
	${Boost_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
	${FFmpeg_INCLUDE_DIRS}
	${LiveMedia_INCLUDE_DIRS}
	${Qt5Widgets_INCLUDE_DIRS} )

LINK_DIRECTORIES (
	${OpenCV_LIBDIR}
	${LiveMedia_LIBDIR}
	${FFmpeg_LIBDIR} )

IF ( WITH_GUI )
	QT5_WRAP_UI ( QT_UI_HEADERS frame_in.ui )
ENDIF()

SET ( HEADER_FILES ${QT_UI_HEADERS} frame_in.h frame_source.h
	video/video_source.h
	http_mjpg/http_mjpg_source.h
	http_mjpg/MjpgCapture.h
	rtsp/rtsp_source.h
	rtsp/rtsp_handlers.h
	rtsp/sched_thread.h
	rtsp/H264DecoderSink.h
	rtsp/MPRTSPClient.h )
SET ( SOURCE_FILES frame_in.cpp
	video/video_source.cpp
	http_mjpg/http_mjpg_source.cpp
	http_mjpg/MjpgCapture.cpp
	rtsp/rtsp_source.cpp
	rtsp/rtsp_handlers.cpp
	rtsp/H264DecoderSink.cpp
	rtsp/MPRTSPClient.cpp )

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

ADD_LIBRARY ( ${MOD_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES} )
QT5_USE_MODULES ( ${MOD_NAME} Widgets )
TARGET_LINK_LIBRARIES ( ${MOD_NAME}
	${LiveMedia_LIBRARIES}
	${OpenCV_LIBRARIES} ${FFmpeg_LIBRARIES} )

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
