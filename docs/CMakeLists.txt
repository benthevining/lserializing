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

limes_default_doxygen_settings ()

set (LSERIAL_DOC_TAGFILE "${CMAKE_CURRENT_LIST_DIR}/../doc/lserializing.tag"
     CACHE FILEPATH "Path where the Doxygen tagfile for lserializing will be generated")

mark_as_advanced (LSERIAL_DOC_TAGFILE)

file (GLOB headers LIST_DIRECTORIES false CONFIGURE_DEPENDS
                                          "${CMAKE_CURRENT_LIST_DIR}/../include/lserializing/*.h")

set (DOXYGEN_GENERATE_TAGFILE "${LSERIAL_DOC_TAGFILE}")

doxygen_add_docs (
    lserial_docs ${headers} ${examples} "${DOXYGEN_USE_MDFILE_AS_MAINPAGE}" USE_STAMP_FILE
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/.."
    COMMENT "Building lserializing documentation...")

set_property (
    TARGET lserial_docs APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${DOXYGEN_OUTPUT_DIRECTORY}"
                                        "${DOXYGEN_WARN_LOGFILE}" "${DOXYGEN_GENERATE_TAGFILE}")

limes_add_docs_coverage (lserial_docs)
