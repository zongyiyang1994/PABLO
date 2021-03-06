#Specify the version being used as well as the language
cmake_minimum_required(VERSION 2.8)

#---------------------------------------

#Build test0.cpp
SET(test0_src test0.cpp)

add_executable(test0 ${test0_src})

IF(WITHOUT_MPI EQUAL 0)
target_link_libraries(test0 mpi)
ENDIF(WITHOUT_MPI EQUAL 0)
TARGET_LINK_LIBRARIES(test0 PABLO)

#---------------------------------------

#Build test1.cpp
SET(test1_src test1.cpp)

add_executable(test1 ${test1_src})

IF(WITHOUT_MPI EQUAL 0)
target_link_libraries(test1 mpi)
ENDIF(WITHOUT_MPI EQUAL 0)
TARGET_LINK_LIBRARIES(test1 PABLO)

#---------------------------------------

#Build test1b.cpp
SET(test1b_src test1b.cpp)

add_executable(test1b ${test1b_src})

IF(WITHOUT_MPI EQUAL 0)
target_link_libraries(test1b mpi)
ENDIF(WITHOUT_MPI EQUAL 0)
TARGET_LINK_LIBRARIES(test1b PABLO)

#---------------------------------------

#Build test2.cpp
SET(test2_src test2.cpp)

add_executable(test2 ${test2_src})

IF(WITHOUT_MPI EQUAL 0)
target_link_libraries(test2 mpi)
ENDIF(WITHOUT_MPI EQUAL 0)
TARGET_LINK_LIBRARIES(test2 PABLO)

#---------------------------------------

#Build test3.cpp
SET(test3_src test3.cpp)

add_executable(test3 ${test3_src})

IF(WITHOUT_MPI EQUAL 0)
target_link_libraries(test3 mpi)
ENDIF(WITHOUT_MPI EQUAL 0)
TARGET_LINK_LIBRARIES(test3 PABLO)

#---------------------------------------

#Build test4.cpp
SET(test4_src test4.cpp)

add_executable(test4 ${test4_src})

IF(WITHOUT_MPI EQUAL 0)
target_link_libraries(test4 mpi)
ENDIF(WITHOUT_MPI EQUAL 0)
TARGET_LINK_LIBRARIES(test4 PABLO)

#---------------------------------------

#Build test5.cpp
SET(test5_src test5.cpp)

add_executable(test5 ${test5_src})

IF(WITHOUT_MPI EQUAL 0)
target_link_libraries(test5 mpi)
ENDIF(WITHOUT_MPI EQUAL 0)
TARGET_LINK_LIBRARIES(test5 PABLO)

#---------------------------------------

#Build test7.cpp
SET(test7_src test7.cpp)

add_executable(test7 ${test7_src})

IF(WITHOUT_MPI EQUAL 0)
target_link_libraries(test7 mpi)
ENDIF(WITHOUT_MPI EQUAL 0)
TARGET_LINK_LIBRARIES(test7 PABLO)

#---------------------------------------

#Build test12.cpp
IF(WITHOUT_MPI EQUAL 0)
SET(test12_src test12.cpp)

add_executable(test12 ${test12_src})

target_link_libraries(test12 mpi)
TARGET_LINK_LIBRARIES(test12 PABLO)
ENDIF(WITHOUT_MPI EQUAL 0)

#---------------------------------------

IF(WITHOUT_MPI EQUAL 0)
#Build test13.cpp
SET(test13_src test13.cpp)

add_executable(test13 ${test13_src})

target_link_libraries(test13 mpi)
TARGET_LINK_LIBRARIES(test13 PABLO)
ENDIF(WITHOUT_MPI EQUAL 0)

#---------------------------------------

IF(WITHOUT_MPI EQUAL 0)
#Build test14.cpp
SET(test14_src test14.cpp)

add_executable(test14 ${test14_src})

target_link_libraries(test14 mpi)
TARGET_LINK_LIBRARIES(test14 PABLO)
ENDIF(WITHOUT_MPI EQUAL 0)

#---------------------------------------

IF(WITHOUT_MPI EQUAL 0)
#Build test15.cpp
SET(test15_src test15.cpp)

add_executable(test15 ${test15_src})

target_link_libraries(test15 mpi)
TARGET_LINK_LIBRARIES(test15 PABLO)
ENDIF(WITHOUT_MPI EQUAL 0)

#---------------------------------------

IF(WITHOUT_MPI EQUAL 0)
#Build test16.cpp
SET(test16_src test16.cpp)

add_executable(test16 ${test16_src})

target_link_libraries(test16 mpi)
TARGET_LINK_LIBRARIES(test16 PABLO)
ENDIF(WITHOUT_MPI EQUAL 0)

#---------------------------------------

IF(WITHOUT_MPI EQUAL 0)
#Build test17.cpp
SET(test17_src test17.cpp)

add_executable(test17 ${test17_src})

target_link_libraries(test17 mpi)
TARGET_LINK_LIBRARIES(test17 PABLO)
ENDIF(WITHOUT_MPI EQUAL 0)

#---------------------------------------

#Build test104.cpp
SET(test104_src test104.cpp)

add_executable(test104 ${test104_src})

IF(WITHOUT_MPI EQUAL 0)
target_link_libraries(test104 mpi)
ENDIF(WITHOUT_MPI EQUAL 0)
TARGET_LINK_LIBRARIES(test104 PABLO)

#---------------------------------------

IF(WITHOUT_MPI EQUAL 0)
#Build test116.cpp
SET(test116_src test116.cpp)

add_executable(test116 ${test116_src})

target_link_libraries(test116 mpi)
TARGET_LINK_LIBRARIES(test116 PABLO)
ENDIF(WITHOUT_MPI EQUAL 0)

#---------------------------------------

IF(WITHOUT_MPI EQUAL 0)
#Build test120.cpp
SET(test120_src test120.cpp)

add_executable(test120 ${test120_src})

target_link_libraries(test120 mpi)
TARGET_LINK_LIBRARIES(test120 PABLO)
ENDIF(WITHOUT_MPI EQUAL 0)

#---------------------------------------

#Build testBubbles.cpp
SET(testBubbles_src testBubbles.cpp)

add_executable(testBubbles ${testBubbles_src})

IF(WITHOUT_MPI EQUAL 0)
target_link_libraries(testBubbles mpi)
ENDIF(WITHOUT_MPI EQUAL 0)
TARGET_LINK_LIBRARIES(testBubbles PABLO)

#---------------------------------------

#Build testBubbles3D.cpp
SET(testBubbles3D_src testBubbles3D.cpp)

add_executable(testBubbles3D ${testBubbles3D_src})

IF(WITHOUT_MPI EQUAL 0)
target_link_libraries(testBubbles3D mpi)
ENDIF(WITHOUT_MPI EQUAL 0)
TARGET_LINK_LIBRARIES(testBubbles3D PABLO)

#---------------------------------------

