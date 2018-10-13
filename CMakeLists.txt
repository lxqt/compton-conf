cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
# CMP0000: Call the cmake_minimum_required() command at the beginning of the top-level
# CMakeLists.txt file even before calling the project() command.
# The cmake_minimum_required(VERSION) command implicitly invokes the cmake_policy(VERSION)
# command to specify that the current project code is written for the given range of CMake
# versions.
project(compton-conf)

set(LXQTBT_MINIMUM_VERSION "0.5.0")

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

find_package(lxqt-build-tools ${LXQTBT_MINIMUM_VERSION} REQUIRED)

set(COMPTONCONF_MAJOR_VERSION 0)
set(COMPTONCONF_MINOR_VERSION 4)
set(COMPTONCONF_PATCH_VERSION 0)
set(COMPTONCONF_VERSION ${COMPTONCONF_MAJOR_VERSION}.${COMPTONCONF_MINOR_VERSION}.${COMPTONCONF_PATCH_VERSION})
add_definitions(
    "-DCOMPTONCONF_VERSION=\"${COMPTONCONF_VERSION}\""
)


include(LXQtPreventInSourceBuilds)
include(LXQtConfigVars)
include(LXQtTranslateTs)
include(LXQtTranslateDesktop)

# Support Qt4
option(USE_QT4 "Build with Qt4." $ENV{USE_QT4})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)

if(USE_QT4)
  set(USE_QT5 FALSE)
  find_package(Qt4 REQUIRED QtCore QtGui QtDbus)
  include(${QT_USE_FILE})
  add_definitions(-DUSE_QT4)
  message(STATUS "Building with Qt${QTVERSION}")
else()
  set(USE_QT5 TRUE)
  cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5DBus REQUIRED)
  find_package(Qt5LinguistTools REQUIRED)
  message(STATUS "Building with Qt${Qt5Core_VERSION_STRING}")
endif()

# libconfig using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCONFIG REQUIRED
  libconfig
)

include(GNUInstallDirs REQUIRED)
include(LXQtCompilerSettings NO_POLICY_SCOPE) # must be after cmake_minimum_required()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_KEYWORDS")

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
    USE_QT5 ${USE_QT5}
    UPDATE_TRANSLATIONS ${UPDATE_TRANSLATIONS}
    SOURCES
        ${compton-conf_HS}
        ${compton-conf_SRCS}
        ${compton-conf_UIS}
    INSTALL_DIR
        ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/translations
)

lxqt_translate_desktop(compton-conf_DESKTOP_FILES SOURCES ${compton-conf_DESKTOP_FILES_IN})

if(USE_QT4)
  qt4_wrap_ui(compton-conf_UI_H ${compton-conf_UIS})
else()
  qt5_wrap_ui(compton-conf_UI_H ${compton-conf_UIS})
endif()

set_property(SOURCE ${compton-conf_UI_H} PROPERTY SKIP_AUTOMOC ON)

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
  ${LIBCONFIG_LDFLAGS}
)

install(TARGETS compton-conf RUNTIME DESTINATION bin)
install(FILES compton.conf.example DESTINATION ${CMAKE_INSTALL_DATADIR}/compton-conf)
install(FILES ${compton-conf_DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/applications/)

# merged from lxqt-common
add_subdirectory(autostart)

