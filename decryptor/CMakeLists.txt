cmake_minimum_required (VERSION 3.8)

add_executable (decryptor
				"decryptor.cpp"
				"processor.h"
				"util.h"
				"host.cpp" "host.h"
				"packet.h"
				"crypt_basics.h"
				"crypt.cpp" "crypt.h"
				"checksum.h"
				"compression.h")

				
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.68 REQUIRED COMPONENTS filesystem date_time)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(decryptor PRIVATE ${Boost_LIBRARIES})

find_package(nlohmann_json 3.3.0 REQUIRED)
target_link_libraries(decryptor PRIVATE nlohmann_json::nlohmann_json)

find_package(LZO REQUIRED)
include_directories(${LZO_INCLUDE_DIRS})
target_link_libraries(decryptor PRIVATE ${LZO_LIBRARIES})
