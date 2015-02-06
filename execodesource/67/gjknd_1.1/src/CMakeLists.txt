# Copyright (c) 2012   Laurent Provot <provot.research@gmail.com>,
# Yan Gerard <yan.gerard@free.fr> and Fabien Feschet <research@feschet.fr>
# All rights reserved.
#
# This is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


INCLUDE_DIRECTORIES( ${GJK_ND_SOURCE_DIR}/src )
INCLUDE_DIRECTORIES( ${GJK_ND_SOURCE_DIR} )

INCLUDE_DIRECTORIES( ${PNG_INCLUDE_DIR} )


SET( TEST_OTSU_SRCS
  utils.cpp
  testOtsu.cpp
)

ADD_EXECUTABLE( testOtsu ${TEST_OTSU_SRCS} )
TARGET_LINK_LIBRARIES( testOtsu ${PNG_LIBRARY} )


SET( TEST_BOUNDARIES_SRCS
  utils.cpp
  Array2D.hpp
  BoundariesExtractor.cpp
  testBoundaries.cpp
)

ADD_EXECUTABLE( testBoundaries ${TEST_BOUNDARIES_SRCS} )
TARGET_LINK_LIBRARIES( testBoundaries ${PNG_LIBRARY} )


SET( TEST_DECOMPOSITION_SRCS
  utils.cpp
  Array2D.hpp
  Segment.hpp
  Dummy.hpp
  BoundariesExtractor.cpp
  GreedyDecomposition.hpp
  testDecomposition.cpp
)

ADD_EXECUTABLE( testDecomposition ${TEST_DECOMPOSITION_SRCS} )
TARGET_LINK_LIBRARIES( testDecomposition ${PNG_LIBRARY} )


SET( DLL_SEQUENCE_SRCS
  utils.cpp
  Array2D.hpp
  GJK_nD.cpp
  Segment.hpp
  StraightLine.cpp
  Circle.cpp
  Conic.cpp
  BoundariesExtractor.cpp
  GreedyDecomposition.hpp
  dll_sequence.cpp
)

ADD_EXECUTABLE( dll_sequence ${DLL_SEQUENCE_SRCS} )
TARGET_LINK_LIBRARIES( dll_sequence ${PNG_LIBRARY} )


SET( DLL_DECOMPOSITION_SRCS
  utils.cpp
  Array2D.hpp
  GJK_nD.cpp
  Segment.hpp
  StraightLine.cpp
  Circle.cpp
  Conic.cpp
  BoundariesExtractor.cpp
  GreedyDecomposition.hpp
  dll_decomposition.cpp
)

ADD_EXECUTABLE( dll_decomposition ${DLL_DECOMPOSITION_SRCS} )
TARGET_LINK_LIBRARIES( dll_decomposition ${PNG_LIBRARY} )
