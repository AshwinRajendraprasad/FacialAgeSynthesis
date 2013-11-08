SET(SOURCE
	src/PoseDetectorHaar.cpp
)
SET(HEADERS
    include/PoseDetectorHaar.h
	include/PoseDetectorHaarParameters.h
)

include_directories(./include)
include_directories(${PoseDetectorHaar_SOURCE_DIR}/include)

add_library( PoseDetectorHaar ${SOURCE} ${HEADERS})

install (TARGETS PoseDetectorHaar DESTINATION bin)
install (FILES HEADERS DESTINATION include)