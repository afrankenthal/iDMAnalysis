cmake_minimum_required(VERSION 3.1)

project(macroRun VERSION 1.0
                 DESCRIPTION "macroRun"
                 LANGUAGES CXX
)

## Set global properties

set_property(GLOBAL PROPERTY CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY COMPILE_FLAGS "-m64")
set_property(GLOBAL PROPERTY LINK_FLAGS "-m64")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## Find required libraries

find_library(MATH_LIBRARY m)
find_package(ROOT 6.22 CONFIG REQUIRED)

set(all_root_libs 
    ROOT::Core ROOT::Gpad ROOT::Graf3d ROOT::Graf ROOT::Hist
    ROOT::Imt ROOT::MathCore ROOT::Matrix ROOT::MultiProc ROOT::Net
    ROOT::Physics ROOT::Postscript ROOT::RIO ROOT::ROOTDataFrame
    ROOT::ROOTVecOps ROOT::Rint ROOT::Thread ROOT::TreePlayer ROOT::Tree
)

## Generate ROOT dictionary for TSelector -- but use manual mode below (n.b. but TSelector code is OBSOLETE)
# find_package(ROOT REQUIRED)
# include(${ROOT_USE_FILE})
# ROOT_GENERATE_DICTIONARY("${CMAKE_CURRENT_SOURCE_DIR}/Selectors/TSelectors/mainAnalysisSelectorDict" \
#     "${CMAKE_CURRENT_SOURCE_DIR}/Selectors/TSelectors/mainAnalysisSelector.h" MODULE mainAnalysisSelector LINKDEF \
#     "${CMAKE_CURRENT_SOURCE_DIR}/Selectors/TSelectors/Linkdef.h")
## [TSelector OBSELETE since March 2020 -- kept for pedagogical purposes]
## manual mode 
# add_custom_command(
#     OUTPUT "${CMAKE_SOURCE_DIR}/Selectors/TSelectors/mainAnalysisSelectorDict.cxx"
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Selectors/TSelectors
#     COMMAND rootcling -f mainAnalysisSelectorDict.cxx
#                          mainAnalysisSelector.h
#                          Linkdef.h
#     )


## Create shared libraries for each macro in macros folder

file(GLOB macro_files "macros/*.C")
foreach (macro_file ${macro_files})
    get_filename_component(macro_lib ${macro_file} NAME_WLE)
    if ("x_${macro_lib}" STREQUAL "x_mMainAnalysis") # OR "x_${macro_lib}" STREQUAL "x_mSaveCanvases")
        add_library(${macro_lib} SHARED ${macro_file} utils/tdrstyle.C utils/CMS_lumi.C utils/ScaleFactors.cc
            Selectors/RDataFrame/RDFAnalysis.C Selectors/RDataFrame/nMinus1Selector.C
            Selectors/RDataFrame/CosmicMuonAnalysis.C)
        # Selectors/TSelectors/mainAnalysisSelector.C "${CMAKE_SOURCE_DIR}/Selectors/TSelectors/mainAnalysisSelectorDict.cxx"
    else()
        add_library(${macro_lib} SHARED ${macro_file} utils/tdrstyle.C utils/CMS_lumi.C)
    endif()
    # set_property(TARGET ${macro_lib} PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_include_directories(${macro_lib} PUBLIC utils)
    target_link_libraries(${macro_lib} PUBLIC stdc++fs pthread ${all_root_libs})
    if (MATH_LIBRARY)
        target_link_libraries(${macro_lib} PUBLIC ${MATH_LIBRARY})
    endif()
endforeach()


## Create executable macroRun (no need to link macro libraries as they are dynamically loaded)

add_executable(macroRun macroRun.cc) 
target_include_directories(macroRun PUBLIC utils)
target_link_libraries(macroRun PUBLIC stdc++fs pthread dl ${all_root_libs})
if(MATH_LIBRARY)
    target_link_libraries(macroRun PUBLIC ${MATH_LIBRARY})
endif()


# Install step -- move macro shared libraries and macroRun executable to bin folder

## [TSelector OBSELETE since March 2020 -- kept for pedagogical purposes]
## Copy ROOT dictionaries to final bin location, needed during runtime
# add_custom_command(TARGET macroRun POST_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                   "${CMAKE_CURRENT_SOURCE_DIR}/Selectors/TSelectors/mainAnalysisSelectorDict.cxx"
#                   $<TARGET_FILE_DIR:macroRun>
#                   )
# add_custom_command(TARGET macroRun POST_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                   "${CMAKE_CURRENT_SOURCE_DIR}/Selectors/TSelectors/mainAnalysisSelectorDict_rdict.pcm"
#                   $<TARGET_FILE_DIR:macroRun>
#                   )
#
## TSelector is OBSOLETE (March 2020)
# install (FILES $<TARGET_FILE_DIR:macroRun>/mainAnalysisSelectorDict.cxx DESTINATION "${CMAKE_SOURCE_DIR}/bin")
# install (FILES $<TARGET_FILE_DIR:macroRun>/mainAnalysisSelectorDict_rdict.pcm DESTINATION "${CMAKE_SOURCE_DIR}/bin")

install (TARGETS macroRun DESTINATION "${CMAKE_SOURCE_DIR}/bin")
foreach (macro_file ${macro_files})
    get_filename_component(macro_lib ${macro_file} NAME_WLE)
    install (TARGETS ${macro_lib} DESTINATION "${CMAKE_SOURCE_DIR}/lib")
endforeach()
