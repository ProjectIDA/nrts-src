cmake_minimum_required(VERSION 3.9)
project(isi330)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-D$ENV{OSNAME} -D$ENV{OSTYPE} -DOSVER=$ENV{OSVER} -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS)

include_directories(/ida/home/dauerbach/src/include /ida/home/dauerbach/src3rd/lib330)
link_directories(/ida/home/dauerbach/lib/$ENV{PLATFORM})

add_executable(isi330
        callbacks.c
        exit.c
        init.c
        isi330.h
        log.c
        main.c
        q330.c
        q330db.c
        q330utils.c
        ReleaseNotes.c
        signals.c)
