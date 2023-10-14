cmake_minimum_required(VERSION 3.5)

project(BreakOut LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS 
	Widgets 
	multimedia 
	REQUIRED
)
QT5_ADD_RESOURCES(RCC_FILES BreakOut.qrc)

################################################################################
# Source groups
################################################################################
set(Headers
    src/mainwindow.h
    src/HomePage/homepage.h
    src/HomePage/game_gl_widget.h
    src/HomePage/game_object.h
    src/HomePage/game_level.h
    src/HomePage/sprite_renderer.h
    src/HomePage/collision_helper.h
	src/HomePage/particle_generator.h
	src/HomePage/post_processor.h
	src/HomePage/power_up.h
	src/HomePage/power_up_manager.h
	src/HomePage/game_state.h
	src/common/singleton.h
	src/common/resource_manager.h
	src/common/audio_manager.h
	src/common/text_renderer.h
	src/common/shader.h
)
#source_group("Headers" FILES ${Headers})

set(Sources
    src/main.cc
    src/mainwindow.cc
    src/HomePage/homepage.cc
    src/HomePage/game_gl_widget.cc
    src/HomePage/game_object.cc
    src/HomePage/game_level.cc
    src/HomePage/sprite_renderer.cc
    src/HomePage/collision_helper.cc
	src/HomePage/particle_generator.cc
	src/HomePage/post_processor.cc
	src/HomePage/power_up.cc
	src/HomePage/power_up_manager.cc
	src/HomePage/game_state.cc
	src/common/resource_manager.cc
	src/common/audio_manager.cc
	src/common/text_renderer.cc
	src/common/shader.cc
)
#source_group("Sources" FILES ${Sources})

add_executable(${PROJECT_NAME} 
	${Headers}
	${Sources}
	${RCC_FILES}
)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} 
PRIVATE
	src/HomePage
	src/common
	3rdparty/freetype/include
	3rdparty/glm
)

################################################################################
# Dependencies
################################################################################
target_link_libraries(${PROJECT_NAME} 
PRIVATE 
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Multimedia
	freetyped
)

target_link_directories(${PROJECT_NAME} 
PRIVATE
	3rdparty/freetype/lib
)

################################################################################
# Set target properties
################################################################################
set_target_properties(${PROJECT_NAME}
PROPERTIES
	VS_PLATFORM_TOOLSET v141
)
