#    This file ( CMakeLists.txt ) is part of MinimalSerialMonitor.
#    (C) 31.1.2021 Fidoriel
#
#    MinimalSerialMonitor is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    MinimalSerialMonitor is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with MinimalSerialMonitor.  If not, see <http://www.gnu.org/licenses/>.

#Change to directory where wxwidgets is located. 

#UNIX like wxWidgets
#create build directory Mac: mkdir macbuild; cd macbuild
#create build directory Linux: mkdir gtkbuild; cd gtkbuild
#build instrucions for wxWidgets Platform Linux und macOS: ../configure --disable-shared --disable-sys-libs
#compile with: make -j4

#change to project directory and: mkdir build; cd build
#cmake corfigure with: cmake ..
#if Linux AppImage should be created cmake .. -D RELEASE=1
#compile with: make -j4

#Windows
#change build directory Win32: cd build\msw
#build instrucions for wxWidgets Platform Windows MinGW: mingw32-make SHELL=CMD.exe -j4 -f makefile.gcc BUILD=release UNICODE=1 SHARED=0 MONOLITHIC=0 DwxUSE_LIBPNG=builtin DwxUSE_ZLIB=builtin DCMAKE_POSITION_INDEPENDENT_CODE=ON
#mingw-w64 workaround i686 and Version 6.3
#mingw32-make SHELL=CMD.exe -j4 -f makefile.gcc BUILD=release UNICODE=1 SHARED=0 MONOLITHIC=0 DwxUSE_LIBPNG=builtin DwxUSE_ZLIB=builtin DCMAKE_POSITION_INDEPENDENT_CODE=ON LDFLAGS=-Wl,--allow-multiple-definition
#the build command will fail the first and second time. the thrid will be succesful
#mingw32 Compile: mingw32-make -j4

#change to project directory and: mkdir build; cd build
#cmake CMD.exe: cmake .. -G "MinGW Makefiles"
#mingw32 Compile: mingw32-make -j4

cmake_minimum_required( VERSION 3.0 )
set( CMAKE_BIULD_TYPE Debug )
set( CMAKE_CXX_STANDARD 11 )

project( "MinimalSerialMonitor" )

set( WXWIDGETS_VERSION_IN_HOME_DIR "wxWidgets-3.1.4" )
set( RELEASE 1 )

message( "CXX Compiler: ${CMAKE_CXX_COMPILER}" )
message( "CXX Compiler ID: ${CMAKE_CXX_COMPILER_ID}" )
message( "CXX Compiler is GNU: ${CMAKE_COMPILER_IS_GNUCXX}" )

if( WIN32 )
    message( "Target: Windows32" )
    if( RELEASE )
        set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_NAME}/" )
    endif()
    #MONOLITHIC compiled value
    #directory where wxwidget is located root folder
    set( wxWidgets_BASE_DIR_WIN "C:/Users/$ENV{username}/${WXWIDGETS_VERSION_IN_HOME_DIR}" )
    include_directories( 
        includes
        "${wxWidgets_BASE_DIR_WIN}/include/"
        "${wxWidgets_BASE_DIR_WIN}/lib/gcc_lib/mswu/"
     )

    link_directories( 
        "${wxWidgets_BASE_DIR_WIN}/lib/gcc_lib/"
     )

    if( NOT MONO )
        set( WX_LIBS
            #MONOLITHIK=0
            libwxmsw31u_core.a #core needs to be the first one
            libwxbase31u.a
            libwxbase31u_net.a
            libwxbase31u_xml.a
            libwxexpat.a
            libwxjpeg.a
            libwxmsw31u_adv.a
            libwxmsw31u_aui.a
            libwxmsw31u_gl.a
            libwxmsw31u_html.a
            libwxmsw31u_media.a
            libwxmsw31u_propgrid.a
            libwxmsw31u_ribbon.a
            libwxmsw31u_richtext.a
            libwxmsw31u_stc.a
            libwxmsw31u_webview.a
            libwxmsw31u_xrc.a
            libwxpng.a
            libwxregexu.a
            libwxscintilla.a
            libwxtiff.a
            libwxzlib.a
         )
    elseif( MONO )
        set( WX_LIBS
            #MONOLITHIK=1
            libwxexpat.a
            libwxjpeg.a
            libwxmsw31u.a
            libwxmsw31u_gl.a
            libwxpng.a
            libwxregexu.a
            libwxscintilla.a
            libwxzlib.a
            libwxtiff.a
         )
    endif()

elseif( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
    message( "Target: Linux" )
    #directory where wxwidget is located root folder
    set( wxWidgets_ROOT_DIR "$ENV{HOME}/${WXWIDGETS_VERSION_IN_HOME_DIR}" )
    set( wxWidgets_CONFIG_EXECUTABLE "${wxWidgets_ROOT_DIR}/gtkbuild/wx-config" )

    if( RELEASE )
        set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "AppDir/usr/bin/" )
    endif()
elseif( APPLE )
    message( "Target: Apple" )
    #directory where wxwidget is located root folder
	set( wxWidgets_ROOT_DIR "$ENV{HOME}/${WXWIDGETS_VERSION_IN_HOME_DIR}" )
	set( wxWidgets_CONFIG_EXECUTABLE "${wxWidgets_ROOT_DIR}/macbuild/wx-config" )
endif()

if( NOT WIN32 )
    #find_package( wxWidgets COMPONENTS core base gl adv html xml xrc aui REQUIRED ) Modified Linux has not all??
    find_package( wxWidgets COMPONENTS core base adv html xml xrc aui REQUIRED )

    #if libs are static use --libs --static instead for standard libs.
    set( WX_CONFIG_ARGS_LIBS "--libs --static" )
    set( WXWINDOWS_LIBRARIES "`${CMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE} ${WX_CONFIG_ARGS_LIBS}`" )

    set( CMAKE_WXWINDOWS_CXX_FLAGS "`${CMAKE_WXWINDOWS_WXCONFIG_EXECUTABLE} --cxxflags`" )

    if( wxWidgets_FOUND )
	    include( "${wxWidgets_USE_FILE}" )
	    message( "wxWidgets was found" )
    else( wxWidgets_FOUND )
	    message( "wxWidgets was not found" )
    endif( wxWidgets_FOUND )
endif()

file( GLOB_RECURSE SRC_FILES
    src/*.h
    src/*.hpp
    src/*.c
    src/*.cpp
    src/*/*.h
    src/*/*.hpp
    src/*/*.c
    src/*/*.cpp
    )

if( WIN32 )
    # Include a RC file for windows
    list( APPEND SRC_FILES icons/icon_win.rc )
elseif( APPLE )
    # Add an icon for the apple .app file
    list( APPEND SRC_FILES icons/AppIcon.icns )
endif()

if( WIN32 )
    #-mwindows suppresses CMD
    set( CMAKE_CXX_FLAGS "-mwindows -luxtheme -loleacc -lversion -lshlwapi -lcomctl32 -static-libstdc++ -static-libgcc -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic" )
    add_executable( ${PROJECT_NAME} WIN32 ${SRC_FILES} )
    target_link_libraries( ${PROJECT_NAME} ${WX_LIBS} ${CMAKE_CXX_FLAGS} )
elseif( APPLE )
    add_executable( ${PROJECT_NAME} MACOSX_BUNDLE ${SRC_FILES} )
    target_link_libraries( ${PROJECT_NAME} ${wxWidgets_LIBRARIES} )
elseif( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
    set( CMAKE_CXX_FLAGS "-no-pie" )
    add_executable( ${PROJECT_NAME} ${SRC_FILES} )
    target_link_libraries( ${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${CMAKE_CXX_FLAGS} )
endif()

#After compilation processing
if( APPLE )
    set_target_properties( ${PROJECT_NAME} PROPERTIES
        RESOURCE "icons/AppIcon.icns"
        MACOSX_BUNDLE_ICON_FILE AppIcon.icns
        MACOSX_BUNDLE_COPYRIGHT "Copyright fidoriel 2020"
        MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_NAME}"
        )
    add_custom_command( 
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND plutil -replace NSHighResolutionCapable -bool true ${PROJECT_NAME}.app/Contents/Info.plist
        )
elseif( CMAKE_SYSTEM_NAME STREQUAL "Linux" AND RELEASE )
    EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
    message( STATUS "Architecture: ${ARCHITECTURE}" )

    if( "${ARCHITECTURE}" STREQUAL "armv7l" OR "${ARCHITECTURE}" STREQUAL "armv7" )
        set( AppImageRelease "appimagetool-armhf.AppImage" )
    elseif( "${ARCHITECTURE}" STREQUAL "aarch64" OR "${ARCHITECTURE}" STREQUAL "armv8" )
        set( AppImageRelease "appimagetool-aarch64.AppImage" )
    elseif( "${ARCHITECTURE}" STREQUAL "i386" )
        set( AppImageRelease "appimagetool-i686.AppImage" )
    else()
        set( AppImageRelease "appimagetool-x86_64.AppImage" )
    endif()

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND mkdir -p "AppDir/usr/share/"
        COMMAND mkdir -p "AppDir/usr/share/icons"
        COMMAND wget -N https://github.com/AppImage/AppImageKit/releases/download/continuous/${AppImageRelease}
        COMMAND chmod a+x ${AppImageRelease}
        COMMAND cp ../icons/AppIcon.png AppDir/AppIcon.png

        COMMAND rm -f AppDir/${PROJECT_NAME}.desktop
        COMMAND touch AppDir/${PROJECT_NAME}.desktop
        COMMAND echo '[Desktop Entry]' >> AppDir/${PROJECT_NAME}.desktop
        COMMAND echo 'Name=${PROJECT_NAME}' >> AppDir/${PROJECT_NAME}.desktop
        COMMAND echo 'Exec=${PROJECT_NAME}' >> AppDir/${PROJECT_NAME}.desktop
        COMMAND echo 'Icon=AppIcon' >> AppDir/${PROJECT_NAME}.desktop
        COMMAND echo 'Type=Application' >> AppDir/${PROJECT_NAME}.desktop
        COMMAND echo 'Categories=Utility\\;' >> AppDir/${PROJECT_NAME}.desktop

        COMMAND wget -N https://raw.githubusercontent.com/AppImage/AppImageKit/master/src/AppRun.c
        COMMAND gcc AppRun.c -o AppDir/AppRun

        COMMAND ./${AppImageRelease} AppDir
	    )
endif()