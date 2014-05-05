project(org_commontk_dah_testhost)

set(PLUGIN_export_directive "org_commontk_dah_testhost_EXPORT")

set(PLUGIN_SRCS
  ctkTestDicomHostPlugin.cpp
  ctkTestDicomHostPlugin_p.h
  ctkTestHostLogic.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
  ctkTestDicomHostPlugin_p.h
  ctkTestHostLogic.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)

# Additional directories to include - Note that CTK_INCLUDE_LIBRARIES is already included
set(PLUGIN_include_directories
)
#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  INCLUDE_DIRECTORIES ${PLUGIN_include_directories}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
)

ADD_SUBDIRECTORY(Testing)