SET(HEADERS
 Profiling.h
 Helper.h
 openGA.h
 SensorPlacement.h
 Zone.h
 Sensor.h
 UI.h
 DataManager.h
 Factory.h
 GA.h
 Camera.h
)
SET(SOURCES
  SensorPlacement.cpp
  Zone.cpp
  Sensor.cpp
  UI.cpp
  DataManager.cpp
  Factory.cpp
  GA.cpp
  Camera.cpp
)
cover_add_plugin(SensorPlacement ${HEADERS} ${SOURCES})