#OpenCOVER plugins calit2 PointCloud
USING(E57 optional)

SET(HEADERS
  PointCloud.h
  PointCloudGeometry.h
  PointCloudInteractor.h
)

SET(SOURCES
  PointCloud.cpp
  PointCloudGeometry.cpp
  PointCloudInteractor.cpp
)

cover_add_plugin(PointCloud)

SET(HEADERS
)
SET(SOURCES
  PointSort/PointConvert.cpp
)

ADD_COVISE_EXECUTABLE(PointConvert)
COVISE_WNOERROR(PointConvert)

TARGET_LINK_LIBRARIES(PointConvert ${EXTRA_LIBS} ${OPENSCENEGRAPH_LIBRARIES} )
SET(HEADERS
)
SET(SOURCES
  PointSort/PointSort.cpp
)

ADD_COVISE_EXECUTABLE(PointSort)
COVISE_WNOERROR(PointSort)

TARGET_LINK_LIBRARIES(PointSort ${EXTRA_LIBS} ${OPENSCENEGRAPH_LIBRARIES} )

IF(UNIX OR MINGW)
      SET(WITH_OPENMP "TRUE")
      IF(MINGW)
         SET(WITH_OPENMP "FALSE")
      ENDIF()
      IF(APPLE)
          IF (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            SET(WITH_OPENMP "FALSE")
         ENDIF()
      ENDIF(APPLE)

      IF(WITH_OPENMP)
         ADD_COVISE_COMPILE_FLAGS(PointSort "-fopenmp")
         ADD_COVISE_LINK_FLAGS(PointSort "-fopenmp")
      ENDIF()
ENDIF(UNIX OR MINGW)

IF(MSVC)
      ADD_COVISE_COMPILE_FLAGS(PointSort "/openmp")
ENDIF(MSVC)


if (NOT E57_FOUND)
    return()
endif()

SET(HEADERS
)
SET(SOURCES
  TransformE57.cpp
)

ADD_COVISE_EXECUTABLE(TransformE57)
COVISE_WNOERROR(TransformE57)

TARGET_LINK_LIBRARIES(TransformE57 ${EXTRA_LIBS} ${OPENSCENEGRAPH_LIBRARIES} )
