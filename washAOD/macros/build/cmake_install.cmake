# Install script for directory: /uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin/macroRun" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin/macroRun")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin/macroRun"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin/macroRun")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin" TYPE EXECUTABLE FILES "/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/build/macroRun")
  if(EXISTS "$ENV{DESTDIR}/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin/macroRun" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin/macroRun")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin/macroRun"
         OLD_RPATH "/cvmfs/sft.cern.ch/lcg/views/LCG_96python3/x86_64-slc6-gcc8-opt/lib:/cvmfs/sft.cern.ch/lcg/releases/vdt/0.4.3-992df/x86_64-slc6-gcc8-opt/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/cvmfs/sft.cern.ch/lcg/releases/binutils/2.30-e5b21/x86_64-slc6/bin/strip" "$ENV{DESTDIR}/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin/macroRun")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin/mainAnalysisSelectorDict.cxx")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin" TYPE FILE FILES "/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/build/mainAnalysisSelectorDict.cxx")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin/mainAnalysisSelectorDict_rdict.pcm")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/bin" TYPE FILE FILES "/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/build/mainAnalysisSelectorDict_rdict.pcm")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/uscms/home/mreid/nobackup/CRAB/signal_region_analysis/CMSSW_10_2_18/src/iDMSkimmer/washAOD/macros/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
