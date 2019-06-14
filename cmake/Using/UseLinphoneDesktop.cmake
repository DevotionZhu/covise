MACRO(USE_LINPHONEDESKTOP)

  FIND_PACKAGE(LinphoneDesktop)

  IF (NOT LINPHONEDESKTOP_FOUND)
    USING_MESSAGE("Skipping because of missing LINPHONEDESKTOP")
    RETURN()
  ENDIF(NOT LINPHONEDESKTOP_FOUND)

  IF(NOT LINPHONEDESKTOP_USED AND LINPHONEDESKTOP_FOUND)
    SET(LINPHONEDESKTOP_USED TRUE)
    INCLUDE_DIRECTORIES(SYSTEM ${LINPHONEDESKTOP_INCLUDE_DIR})
    SET(EXTRA_LIBS ${EXTRA_LIBS} ${LINPHONEDESKTOP_LIB_LIST})
  ENDIF()

ENDMACRO(USE_LINPHONEDESKTOP)
