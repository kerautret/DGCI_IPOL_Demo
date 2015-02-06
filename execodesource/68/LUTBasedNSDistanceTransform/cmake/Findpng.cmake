FIND_PATH(PNG_INCLUDE_DIRS png.h
    /usr/local/include
    /usr/include
)


find_library(PNG_LIBRARIES
             NAMES png
             PATHS /usr/lib
                  /usr/local/lib
                  /opt/local/lib/
                  ENV LIBRARY_PATH
                  ENV LD_LIBRARY_PATH
             PATH_SUFFIXES png
            )

SET(PNG_FOUND 0)
IF(PNG_LIBRARIES AND PNG_INCLUDE_DIRS)
  SET(PNG_FOUND 1)
ENDIF(PNG_LIBRARIES AND PNG_INCLUDE_DIRS)


