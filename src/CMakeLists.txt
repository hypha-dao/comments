cmake_minimum_required(VERSION 3.12)
project(comments-hypha-build)
set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( comments.hypha comments
        comments.cpp
)

target_include_directories( comments PUBLIC ${CMAKE_SOURCE_DIR}/../include )
