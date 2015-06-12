SET ( BOOST_ROOT "~/lib/boost_1_58_0" )
SET ( OpenCV_DIR ~/lib/opencv-2.4.10-release/share/OpenCV )
SET ( FFmpeg_LIBRARIES avcodec avformat swresample swscale )
SET ( FFmpeg_LIBDIR	~/lib/ffmpeg_build/lib )
SET ( Eigen_INCLUDE_DIRS ~/lib/eigen-eigen-10219c95fe65 )
SET ( FFTW3_ROOT ~/lib/fftw-3.3.4 )

LIST ( APPEND CMAKE_PREFIX_PATH 
	"/home/nguyen/lib/qt-5.4.1/lib/cmake/Qt5" 
	"/home/nguyen/lib/libxml2-2.9.2" )
SET ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/devbuild" )
SET ( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins" )
