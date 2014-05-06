PROJECT(ctkTestHost)

configure_file(
  ctkTestHostMain.h.in
  ${PROJECT_BINARY_DIR}/ctkTestHostMain.h @ONLY
  )

#
# See CTK/CMake/ctkMacroBuildApp.cmake for details
#
  
# Source files
SET(KIT_SRCS
  ctkTestHostMain.cpp
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  )

# UI files
SET(KIT_UI_FORMS
)

# Resources
SET(KIT_resources
)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

ctkMacroBuildApp(
  NAME ${PROJECT_NAME}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  INCLUDE_DIRECTORIES ${PROJECT_BINARY_DIR}
  )

# SET(KIT ${PROJECT_NAME})

# CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cpp
  # ctkTestHost1.cpp
  # )

# SET (TestsToRun ${Tests})
# REMOVE (TestsToRun ${KIT}CppTests.cpp)

# # Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# # The following macro will read the target libraries from the file 'target_libraries.cmake'
# ctkFunctionGetTargetLibraries(KIT_target_libraries)

# ADD_EXECUTABLE(${KIT}CppTests ${Tests})
# TARGET_LINK_LIBRARIES(${KIT}CppTests ${KIT_target_libraries} ${QT_LIBRARIES})

# SET( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)
# IF(WIN32)
  # SET(KIT_TESTS ${CPP_TEST_PATH}/${CMAKE_BUILD_TYPE}/${KIT}CppTests)
# ENDIF(WIN32)

# MACRO( SIMPLE_TEST  TESTNAME )
  # ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} )
  # SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
# ENDMACRO( SIMPLE_TEST  )

# #
# # Add Tests
# #
# SIMPLE_TEST(ctkTestHost1)
