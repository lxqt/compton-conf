project(compton-conf)

cmake_minimum_required(VERSION 2.8.6)

# Qt
find_package(Qt4 REQUIRED QtCore QtGui)

# libconfig using pkg-config
find_package(PkgConfig)
pkg_check_modules(LIBCONFIG REQUIRED
  libconfig
)

set(CMAKE_CXX_FLAGS "-DQT_NO_KEYWORDS -fno-exceptions")

include_directories(
  ${QT_INCLUDES}
  ${LIBCONFIG_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
)

set(CMAKE_AUTOMOC TRUE)

set(compton-conf_SRCS
  maindialog.cpp
  compton-conf.cpp
)

set(compton-conf_UIS
  maindialog.ui
)

qt4_wrap_ui(compton-conf_UI_H
  ${compton-conf_UIS}
)

add_executable(compton-conf
  ${compton-conf_SRCS}
  ${compton-conf_UI_H}
)

target_link_libraries(compton-conf
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${LIBCONFIG_LIBRARIES}
)

install(TARGETS compton-conf RUNTIME DESTINATION bin)

# building tarball with CPack -------------------------------------------------
# To create a source distribution, type:
# make package_source
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README")
set (CPACK_PACKAGE_VENDOR "")
set (CPACK_PACKAGE_VERSION_MAJOR "0")
set (CPACK_PACKAGE_VERSION_MINOR "1")
set (CPACK_PACKAGE_VERSION_PATCH "0")
set (CPACK_GENERATOR TBZ2)
set (CPACK_SOURCE_GENERATOR TBZ2)
set (CPACK_SOURCE_IGNORE_FILES /build/;.gitignore;.*~;.git;.kdev4;temp)
include (CPack)
