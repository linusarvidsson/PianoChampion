#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "RtMidi::rtmidi_static" for configuration "Release"
set_property(TARGET RtMidi::rtmidi_static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(RtMidi::rtmidi_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/rtmidi_static.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS RtMidi::rtmidi_static )
list(APPEND _IMPORT_CHECK_FILES_FOR_RtMidi::rtmidi_static "${_IMPORT_PREFIX}/lib/rtmidi_static.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
