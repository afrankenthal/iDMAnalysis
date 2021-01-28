cmake_minimum_required(VERSION 3.1)

set(macro_libs mCalcABCD mCalcABCDProfiles mCalcABCDRatios mCutflowTables mMainAnalysis mMake1DPlotsFromFile mMake2DPlotsFromFile mMakeTemplates mMapABCDClosure mMapABCDNormalization mMapABCDSensitivity mMapABCDSensitivityYearly mMergeYears mNminus1Plots mNminus1Analysis mSaveCanvases mScanABCD mSumGenWgts mMake1DEffPlotsFromFile)

project(macroRun VERSION 1.0
                 DESCRIPTION "macroRun"
                 LANGUAGES CXX
 )

find_package(ROOT 6.18 CONFIG REQUIRED)

# Generate ROOT dictionary for TSelector -- use manual mode below (n.b. but TSelector code is OBSOLETE)

#find_package(ROOT REQUIRED)
#include(${ROOT_USE_FILE})
#ROOT_GENERATE_DICTIONARY("${CMAKE_CURRENT_SOURCE_DIR}/TSelectors/mainAnalysisSelectorDict" "${CMAKE_CURRENT_SOURCE_DIR}/TSelectors/mainAnalysisSelector.h" MODULE mainAnalysisSelector LINKDEF "${CMAKE_CURRENT_SOURCE_DIR}/TSelectors/Linkdef.h")

# manual mode --> TSelector is OBSOLETE (broken backward compatibility as of March 2020)
#add_custom_command(
#    OUTPUT "${CMAKE_SOURCE_DIR}/TSelectors/mainAnalysisSelectorDict.cxx"
#    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/TSelectors
#    COMMAND rootcling -f mainAnalysisSelectorDict.cxx
#                         mainAnalysisSelector.h
#                         Linkdef.h
#    )

set_property(GLOBAL PROPERTY CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY COMPILE_FLAGS "-m64")
set_property(GLOBAL PROPERTY LINK_FLAGS "-m64")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(MATH_LIBRARY m)

foreach (macro_lib ${macro_libs})
    #if("x_${macro_lib}" STREQUAL "x_mMainAnalysis" OR "x_${macro_lib}" STREQUAL "x_mSaveCanvases")
    if ("x_${macro_lib}" STREQUAL "x_mMainAnalysis")
        add_library(${macro_lib} SHARED ${macro_lib}.C utils/tdrstyle.C utils/CMS_lumi.C
        RDataFrameSelectors/RDFAnalysis.C RDataFrameSelectors/nMinus1Selector.C
        RDataFrameSelectors/CosmicMuonAnalysis.C)
        #TSelectors/mainAnalysisSelector.C "${CMAKE_SOURCE_DIR}/TSelectors/mainAnalysisSelectorDict.cxx"
    else()
        add_library(${macro_lib} SHARED ${macro_lib}.C utils/tdrstyle.C utils/CMS_lumi.C)
    endif()
    #else()
    #    add_library(${macro_lib} SHARED ${macro_lib}.C utils/tdrstyle.C utils/CMS_lumi.C)
    #endif()
    set_property(TARGET ${macro_lib} PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_include_directories(${macro_lib} PUBLIC utils)
    if (MATH_LIBRARY)
        target_link_libraries(${macro_lib} PUBLIC ${MATH_LIBRARY})
    endif()
    target_link_libraries(${macro_lib} PUBLIC stdc++fs pthread)
    target_link_libraries(${macro_lib} PUBLIC 
        ROOT::Core
        ROOT::Gpad
        ROOT::Graf3d
        ROOT::Graf
        ROOT::Hist
        ROOT::Imt
        ROOT::MathCore
        ROOT::Matrix
        ROOT::MultiProc
        ROOT::Net
        ROOT::Physics
        ROOT::Postscript
        ROOT::RIO
        ROOT::ROOTDataFrame
        ROOT::ROOTVecOps
        ROOT::Rint
        ROOT::Thread
        ROOT::TreePlayer
        ROOT::Tree
    )
endforeach()


add_executable(macroRun macroRun.C) 

target_include_directories(macroRun PUBLIC utils)

if(MATH_LIBRARY)
    target_link_libraries(macroRun PUBLIC ${MATH_LIBRARY})
endif()

target_link_libraries(macroRun PUBLIC stdc++fs pthread dl)

target_link_libraries(macroRun PUBLIC 
    ROOT::Core
    ROOT::Gpad
    ROOT::Graf3d
    ROOT::Graf
    ROOT::Hist
    ROOT::Imt
    ROOT::MathCore
    ROOT::Matrix
    ROOT::MultiProc
    ROOT::Net
    ROOT::Physics
    ROOT::Postscript
    ROOT::RIO
    ROOT::ROOTDataFrame
    ROOT::ROOTVecOps
    ROOT::Rint
    ROOT::Thread
    ROOT::TreePlayer
    ROOT::Tree
)

# Install step

# Copy ROOT dictionaries to final bin location, needed during runtime --> TSelector is OBSOLETE (March 2020)
#add_custom_command(TARGET macroRun POST_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                   "${CMAKE_CURRENT_SOURCE_DIR}/TSelectors/mainAnalysisSelectorDict.cxx"
#                   $<TARGET_FILE_DIR:macroRun>
#                   )
#add_custom_command(TARGET macroRun POST_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                   "${CMAKE_CURRENT_SOURCE_DIR}/TSelectors/mainAnalysisSelectorDict_rdict.pcm"
#                   $<TARGET_FILE_DIR:macroRun>
#                   )

install (TARGETS macroRun DESTINATION "${CMAKE_SOURCE_DIR}/bin")
foreach (macro_lib ${macro_libs})
    install (TARGETS ${macro_lib} DESTINATION "${CMAKE_SOURCE_DIR}/bin")
endforeach()

# TSelector is OBSOLETE (March 2020)
#install (FILES $<TARGET_FILE_DIR:macroRun>/mainAnalysisSelectorDict.cxx DESTINATION "${CMAKE_SOURCE_DIR}/bin")
#install (FILES $<TARGET_FILE_DIR:macroRun>/mainAnalysisSelectorDict_rdict.pcm DESTINATION "${CMAKE_SOURCE_DIR}/bin")
