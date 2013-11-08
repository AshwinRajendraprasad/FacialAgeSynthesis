
SET(SOURCE
	src/CLMParameters.cpp
	src/CLMTracker.cpp
)

SET(HEADERS
    include/CLMParameters.h
	include/CLMTracker.h
)

# depends on CLM
include_directories(${CLM_SOURCE_DIR}/include)
include_directories(../CLM/include)

include_directories(./include)
include_directories(${CLMWrapper_SOURCE_DIR}/include)

add_library( CLMWrapper ${SOURCE} ${HEADERS})

install (TARGETS CLMWrapper DESTINATION bin)
install (FILES HEADERS DESTINATION include)