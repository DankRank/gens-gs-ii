# Win32-specific CFLAGS/CXXFLAGS.

# Basic platform flags:
# - Enable strict type checking in the Windows headers.
# - Define WIN32_LEAN_AND_MEAN to reduce the number of Windows headers included.
# - Define NOMINMAX to disable the MIN() and MAX() macros.
SET(GENS_C_FLAGS_WIN32 "-DSTRICT -DWIN32_LEAN_AND_MEAN -DNOMINMAX")

# Enable secure template overloads for C++.
# References:
# - MinGW's _mingw_secapi.h
# - http://msdn.microsoft.com/en-us/library/ms175759%28v=VS.100%29.aspx
SET(GENS_CXX_FLAGS_WIN32 "-D_CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES=1")
SET(GENS_CXX_FLAGS_WIN32 "${GENS_CXX_FLAGS_WIN32} -D_CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES_MEMORY=1")
SET(GENS_CXX_FLAGS_WIN32 "${GENS_CXX_FLAGS_WIN32} -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1")
SET(GENS_CXX_FLAGS_WIN32 "${GENS_CXX_FLAGS_WIN32} -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT=1")
SET(GENS_CXX_FLAGS_WIN32 "${GENS_CXX_FLAGS_WIN32} -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_MEMORY=1")

# Compiler-specific Win32 flags.
IF(MSVC)
	INCLUDE(cmake/platform/win32-msvc.cmake)
ELSE(MSVC)
	INCLUDE(cmake/platform/win32-gcc.cmake)
ENDIF(MSVC)
