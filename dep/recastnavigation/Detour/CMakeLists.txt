# Copyright (C) 2011-2016 Project SkyFire <http://www.projectskyfire.org/>
# Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

set(Detour_STAT_SRCS
    Source/DetourAlloc.cpp 
    Source/DetourCommon.cpp 
    Source/DetourNavMesh.cpp 
    Source/DetourNavMeshBuilder.cpp 
    Source/DetourNavMeshQuery.cpp 
    Source/DetourNode.cpp 
)


if(WIN32)
  include_directories(
    ${CMAKE_SOURCE_DIR}/dep/zlib
  )
endif()

add_library(Detour STATIC ${Detour_STAT_SRCS})

target_include_directories(Detour 
  PUBLIC
    Include
)

target_compile_definitions(Detour 
  PRIVATE
    -DNDEBUG
)

target_link_libraries(Detour ${ZLIB_LIBRARIES})
