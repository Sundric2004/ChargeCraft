cmake_minimum_required(VERSION 4.0)
project(ChargeCraft C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

set(SHARED_SOURCES
        csv_loader.c csv_loader.h
        events.c events.h
        json_loader.c json_loader.h
        nary.c nary.h
        queue.c queue.h
        rules.c
        slist.c slist.h
        stack.c stack.h
        station_index.c station_index.h
        advanced_queries.h advanced_queries.c
        mru_advanced.h mru_advanced.c
        scenario_rush_hour.c scenario_rush_hour.h
)

add_executable(ChargeCraft_V1
        main.c
        ${SHARED_SOURCES}
)


add_executable(ChargeCraft_V2
        main_partie2.c
        ${SHARED_SOURCES}
)