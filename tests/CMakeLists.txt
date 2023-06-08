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

limes_get_catch2 ()

add_executable (lserial_tests)

target_sources (lserial_tests PRIVATE Node.cpp Concepts.cpp Enums.cpp)

target_link_libraries (lserial_tests PRIVATE limes::lserializing)

limes_configure_test_target (
	lserial_tests
	BUNDLE_ID Limes.lserializing.tests
	VERSION_MAJOR "${lserializing_VERSION_MAJOR}"
	FULL_VERSION "${lserializing_VERSION}"
	TEST_CATEGORY serializing)
