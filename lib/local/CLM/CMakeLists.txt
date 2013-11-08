include_directories(${BOOST_INCLUDE_DIR})

SET(SOURCE
    src/CCNF_patch.cc
	src/CLM.cc
    src/FCheck.cc
	src/IO.cc
    src/Patch.cc
	src/PAW.cc
    src/PDM.cc
	src/TrackerCLM.cc
)

SET(HEADERS
    include/CCNF_patch.h
	include/CLM.h
    include/FCheck.h
	include/IO.h
    include/Patch.h
	include/PAW.h	
    include/PDM.h
	include/TrackerCLM.h		
)

include_directories(./include)
include_directories(${CLM_SOURCE_DIR}/include)

add_library( CLM ${SOURCE} ${HEADERS})

install (TARGETS CLM DESTINATION bin)
install (FILES HEADERS DESTINATION include)