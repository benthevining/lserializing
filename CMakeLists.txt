# ======================================================================================
#  __    ____  __  __  ____  ___
# (  )  (_  _)(  \/  )( ___)/ __)
#  )(__  _)(_  )    (  )__) \__ \
# (____)(____)(_/\/\_)(____)(___/
#
#  This file is part of the Limes open source library and is licensed under the terms of the GNU Public License.
#
#  Commercial licenses are available; contact the maintainers at ben.the.vining@gmail.com to inquire for details.
#
# ======================================================================================

cmake_minimum_required (VERSION 3.25 FATAL_ERROR)

project (
	lserializing
	VERSION 0.0.1
	LANGUAGES CXX C
	DESCRIPTION "C++ serializing library"
	HOMEPAGE_URL "https://github.com/benthevining/lserializing/")

set_property (DIRECTORY APPEND PROPERTY LABELS lserializing)

include (CMakeDependentOption)

cmake_dependent_option (LSERIAL_TESTS "Build the lserializing tests" "${lserializing_IS_TOP_LEVEL}"
						"NOT IOS" OFF)

cmake_dependent_option (LSERIAL_CLI "Build the lserializing CLI app" "${lserializing_IS_TOP_LEVEL}"
						"NOT IOS" OFF)

option (LSERIAL_DOCS "Build the lserializing docs" "${lserializing_IS_TOP_LEVEL}")

include (GNUInstallDirs)

set (LSERIAL_INSTALL_DEST "${CMAKE_INSTALL_LIBDIR}/cmake/lserializing"
	 CACHE STRING "Path where package files will be installed, relative to the install prefix")

mark_as_advanced (LSERIAL_INSTALL_DEST LSERIAL_TESTS LHASH_DOCS LSERIAL_CLI)

set_property (DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${CMAKE_CURRENT_LIST_DIR}/logs"
										"${CMAKE_CURRENT_LIST_DIR}/deploy")

# use the directory-scoped commands so that Catch2 gets these flags too
if (EMSCRIPTEN)
	add_compile_options (-sNO_DISABLE_EXCEPTION_CATCHING -fexceptions)
	add_link_options (-fexceptions)
endif ()

set (CMAKE_CXX_VISIBILITY_PRESET hidden)
set (CMAKE_VISIBILITY_INLINES_HIDDEN ON)

set (CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO")
set (CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
set (CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")

add_library (lserializing)
add_library (limes::lserializing ALIAS lserializing)

target_compile_features (lserializing PUBLIC cxx_std_20)

set_target_properties (lserializing PROPERTIES DEBUG_POSTFIX -d VERSION "${lserializing_VERSION}"
											   SOVERSION "${lserializing_VERSION_MAJOR}")

set (public_header include/lserializing/lserializing.h)

set (
	private_headers
	include/lserializing/lserializing_Enums.h
	include/lserializing/lserializing_KnownFormats.h
	include/lserializing/lserializing_Node.h
	include/lserializing/lserializing_Printer.h
	include/lserializing/lserializing_Schema.h
	include/lserializing/lserializing_SerializableData.h
	include/lserializing/lserializing_SerializingFormat.h)

set (generated_headers_dir "${CMAKE_CURRENT_BINARY_DIR}/generated/lserializing")

set (export_header "${generated_headers_dir}/lserializing_Export.h")

include (GenerateExportHeader)

# cmake-format: off
generate_export_header (
	lserializing
	EXPORT_MACRO_NAME LSERIAL_EXPORT
	NO_EXPORT_MACRO_NAME LSERIAL_NO_EXPORT
	EXPORT_FILE_NAME "${export_header}")
# cmake-format: on

set (version_header "${generated_headers_dir}/lserializing_Version.h")

configure_file ("${CMAKE_CURRENT_LIST_DIR}/cmake/version.in" "${version_header}" @ONLY
				NEWLINE_STYLE UNIX)

target_sources (lserializing # PUBLIC "${public_header}"
				PRIVATE ${private_headers} "${export_header}" "${version_header}")

target_include_directories (
	lserializing
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
		   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
		   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

install (FILES "${public_header}" "${export_header}" "${version_header}" ${private_headers}
		 DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lserializing" COMPONENT lserializing_dev)

if (WIN32)
	target_compile_definitions (lserializing PRIVATE NOMINMAX)
endif ()

add_subdirectory (src)

if (LSERIAL_CLI)
	add_subdirectory (cli)
endif ()

install (
	TARGETS lserializing
	EXPORT lserializing
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT lserializing
			NAMELINK_COMPONENT lserializing_dev
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT lserializing_dev
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT lserializing
	INCLUDES
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install (
	EXPORT lserializing
	NAMESPACE limes::
	FILE Targets.cmake
	DESTINATION "${LSERIAL_INSTALL_DEST}"
	COMPONENT lserializing_dev)

include (CMakePackageConfigHelpers)

write_basic_package_version_file (lserializing-config-version.cmake
								  VERSION "${lserializing_VERSION}" COMPATIBILITY SameMajorVersion)

configure_package_config_file (cmake/lserializing-config.in lserializing-config.cmake
							   INSTALL_DESTINATION "${LSERIAL_INSTALL_DEST}" NO_SET_AND_CHECK_MACRO)

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/lserializing-config-version.cmake"
			   "${CMAKE_CURRENT_BINARY_DIR}/lserializing-config.cmake"
		 DESTINATION "${LSERIAL_INSTALL_DEST}" COMPONENT lserializing_dev)

include (CPackComponent)

cpack_add_component (
	lserializing
	DISPLAY_NAME "Limes serializing library"
	DESCRIPTION
		"The Limes C++ serializing library. This is the runtime component needed by end users."
	GROUP limes_serializing)

cpack_add_component (
	lserializing_dev
	DISPLAY_NAME "Limes serializing library headers"
	DESCRIPTION
		"Development package for the Limes C++ serializing library. This component includes the library's header files and CMake package files."
	GROUP limes_serializing
	DEPENDS lserializing)

cpack_add_component_group (limes_serializing DISPLAY_NAME "Limes serializing"
						   DESCRIPTION "Limes serializing library" PARENT_GROUP limes)

if (LSERIAL_TESTS)
	enable_testing ()

	add_subdirectory (tests)

	include (CTest)
endif ()

if (LSERIAL_DOCS)
	add_subdirectory (docs)
endif ()

if (lserializing_IS_TOP_LEVEL)
	include (CPack)
endif ()
