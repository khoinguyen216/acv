SET ( HEADERS HOGPyramid.h Intersector.h LBFGS.h Mixture.h
	Model.h Object.h Patchwork.h Rectangle.h Scene.h )
SET ( SOURCES HOGPyramid.cpp LBFGS.cpp Mixture.cpp Model.cpp
	Object.cpp Patchwork.cpp Rectangle.cpp Scene.cpp )

FIND_PACKAGE ( LibXml2 REQUIRED )

INCLUDE_DIRECTORIES (
	${CMAKE_CURRENT_SOURCE_DIR}
	${LIBXML2_INCLUDE_DIR}
	${Eigen_INCLUDE_DIRS} )

ADD_LIBRARY ( ffld SHARED ${HEADERS} ${SOURCES} )
TARGET_LINK_LIBRARIES ( ffld ${OpenCV_LIBRARIES} ${FFTW3_LIBRARIES}
	${LIBXML2_LIBRARIES} )
	
INSTALL ( TARGETS ffld
			LIBRARY DESTINATION plugins
			COMPONENT ${MOD_NAME} )
