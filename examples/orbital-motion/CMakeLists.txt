# Orbital Motion Example
# Conditional build: only if SVG converter and ffmpeg are available

find_program(RSVG_CONVERT rsvg-convert)
find_program(INKSCAPE inkscape)
find_program(FFMPEG ffmpeg)

if((RSVG_CONVERT OR INKSCAPE) AND FFMPEG)
    message(STATUS "Building orbital-motion example (found: ${RSVG_CONVERT}${INKSCAPE}, ${FFMPEG})")
    
    add_executable(orbital_mechanics orbital_mechanics.cpp)
    target_link_libraries(orbital_mechanics PRIVATE lam::symbols)
    target_compile_features(orbital_mechanics PRIVATE cxx_std_23)
else()
    if(NOT RSVG_CONVERT AND NOT INKSCAPE)
        message(STATUS "Skipping orbital-motion example (missing rsvg-convert or inkscape)")
    endif()
    if(NOT FFMPEG)
        message(STATUS "Skipping orbital-motion example (missing ffmpeg)")
    endif()
endif()
