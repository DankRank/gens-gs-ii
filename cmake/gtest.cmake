# Macros for enabling unit testing in Gens/GS II.

# Enable testing.
INCLUDE(CTest)

# Google Test is bundled in extlib.
# We're always using the bundled version.
IF(BUILD_TESTING)
	SET(GTEST_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extlib/gtest/include")
	SET(GTEST_LIBRARY gtest)
ENDIF(BUILD_TESTING)
