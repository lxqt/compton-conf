project(compton-conf)

cmake_minimum_required(VERSION 2.8.6)

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

#Note: no run-time dependency on liblxqt, just a build dependency for lxqt_translate_ts/desktop
find_package(lxqt REQUIRED)
include(LXQtTranslateTs)
include(LXQtTranslateDesktop)

# Support Qt4
option(USE_QT4 "Build with Qt4." $ENV{USE_QT4})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)

if(USE_QT4)
  find_package(Qt4 REQUIRED QtCore QtGui QtDbus)
  include(${QT_USE_FILE})
  message(STATUS "Building with Qt${QTVERSION}")
else()
  cmake_minimum_required(VERSION 2.8.11)
  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5DBus REQUIRED)
  find_package(Qt5LinguistTools REQUIRED QUIET)
  message(STATUS "Building with Qt${Qt5Core_VERSION_STRING}")
endif()

# libconfig using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCONFIG REQUIRED
  libconfig
)

include(GNUInstallDirs REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_KEYWORDS -fno-exceptions")

if(USE_QT4)
  set(QTX_INCLUDE_DIRS ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTDBUS_DIR})
  set(QTX_LIBRARIES ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY})
else()
  set(QTX_INCLUDE_DIRS "")
  set(QTX_LIBRARIES Qt5::Widgets Qt5::DBus)
endif()

include_directories(
  ${QTX_INCLUDE_DIRS}
  ${LIBCONFIG_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
)


set(compton-conf_HS
    compton-conf.h
    maindialog.h
)

set(compton-conf_SRCS
  maindialog.cpp
  compton-conf.cpp
)

set(compton-conf_UIS
  maindialog.ui
)

set(compton-conf_DESKTOP_FILES_IN
    compton-conf.desktop.in
)

lxqt_translate_ts(compton-conf_QM_FILES
    USE_QT4 ${USE_QT4}
    UPDATE_TRANSLATIONS ${UPDATE_TRANSLATIONS}
    SOURCES
        ${compton-conf_HS}
        ${compton-conf_SRCS}
        ${compton-conf_UIS}
    INSTALL_DIR
        ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/translations
    PULL_TRANSLATIONS
        ${PULL_TRANSLATIONS}
    CLEAN_TRANSLATIONS
        ${CLEAN_TRANSLATIONS}
    TRANSLATIONS_REPO
        ${TRANSLATIONS_REPO}
    TRANSLATIONS_REFSPEC
        ${TRANSLATIONS_REFSPEC}
)

lxqt_translate_desktop(compton-conf_DESKTOP_FILES SOURCES ${compton-conf_DESKTOP_FILES_IN})

if(USE_QT4)
  qt4_wrap_ui(compton-conf_UI_H ${compton-conf_UIS})
else()
  qt5_wrap_ui(compton-conf_UI_H ${compton-conf_UIS})
endif()

add_definitions(
  -DCOMPTON_CONF_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/compton-conf"
)

add_executable(compton-conf
  ${compton-conf_SRCS}
  ${compton-conf_UI_H}
  ${compton-conf_QM_FILES}
  ${compton-conf_QM_LOADER}
  ${compton-conf_DESKTOP_FILES}
)

target_link_libraries(compton-conf
  ${QTX_LIBRARIES}
  ${LIBCONFIG_LIBRARIES}
)

install(TARGETS compton-conf RUNTIME DESTINATION bin)
install(FILES compton.conf.example DESTINATION ${CMAKE_INSTALL_DATADIR}/compton-conf)
install(FILES ${compton-conf_DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/applications/)

# building tarball with CPack -------------------------------------------------
# To create a source distribution, type:
# make package_source
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set (CPACK_PACKAGE_VENDOR "")
set (CPACK_PACKAGE_VERSION_MAJOR "0")
set (CPACK_PACKAGE_VERSION_MINOR "1")
set (CPACK_PACKAGE_VERSION_PATCH "0")
set (CPACK_GENERATOR TBZ2)
set (CPACK_SOURCE_GENERATOR TBZ2)
set (CPACK_SOURCE_IGNORE_FILES /build/;.gitignore;.*~;.git;.kdev4;temp)
include (CPack)
