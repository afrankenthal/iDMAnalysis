cmake_minimum_required(VERSION 3.1)

project(macroRun VERSION 1.0
                 DESCRIPTION "macroRun"
                 LANGUAGES CXX
 )

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(ROOT 6.18 CONFIG REQUIRED)

# Generate ROOT dictionary for TSelector -- use manual mode below

#find_package(ROOT REQUIRED)
#include(${ROOT_USE_FILE})
#ROOT_GENERATE_DICTIONARY("${CMAKE_CURRENT_SOURCE_DIR}/TSelectors/mainAnalysisSelectorDict" "${CMAKE_CURRENT_SOURCE_DIR}/TSelectors/mainAnalysisSelector.h" MODULE mainAnalysisSelector LINKDEF "${CMAKE_CURRENT_SOURCE_DIR}/TSelectors/Linkdef.h")

# manual mode
add_custom_command(
    OUTPUT "${CMAKE_SOURCE_DIR}/TSelectors/mainAnalysisSelectorDict.cxx"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/TSelectors
    COMMAND rootcling -f mainAnalysisSelectorDict.cxx
                         mainAnalysisSelector.h
                         Linkdef.h
    )

add_executable(macroRun macroRun.C RDFAnalysis.C mSaveCanvases.C mCutflowTables.C mMainAnalysis.C mSROptimization.C mNminus1Plots.C mMakePlotsFromFile.C mSumGenWgts.C utils/common.C utils/CMS_lumi.C utils/tdrstyle.C TSelectors/mainAnalysisSelector.C "${CMAKE_SOURCE_DIR}/TSelectors/mainAnalysisSelectorDict.cxx")

target_include_directories(macroRun PUBLIC utils)
target_compile_features(macroRun PUBLIC cxx_std_17)
set_target_properties(macroRun PROPERTIES CXX_EXTENSIONS OFF COMPILE_FAGS "-m64" LINK_FLAGS "-m64")

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(macroRun PUBLIC ${MATH_LIBRARY})
endif()

target_link_libraries(macroRun PUBLIC stdc++fs)
target_link_libraries(macroRun PUBLIC pthread)

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

# Copy ROOT dictionaries to final bin location, needed during runtime
add_custom_command(TARGET macroRun POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                   "${CMAKE_CURRENT_SOURCE_DIR}/TSelectors/mainAnalysisSelectorDict.cxx"
                   $<TARGET_FILE_DIR:macroRun>
                   )
add_custom_command(TARGET macroRun POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                   "${CMAKE_CURRENT_SOURCE_DIR}/TSelectors/mainAnalysisSelectorDict_rdict.pcm"
                   $<TARGET_FILE_DIR:macroRun>
                   )

install (TARGETS macroRun DESTINATION "${CMAKE_SOURCE_DIR}/bin")
install (FILES $<TARGET_FILE_DIR:macroRun>/mainAnalysisSelectorDict.cxx DESTINATION "${CMAKE_SOURCE_DIR}/bin")
install (FILES $<TARGET_FILE_DIR:macroRun>/mainAnalysisSelectorDict_rdict.pcm DESTINATION "${CMAKE_SOURCE_DIR}/bin")
