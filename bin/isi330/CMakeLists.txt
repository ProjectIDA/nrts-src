cmake_minimum_required(VERSION 3.9)
project(isi330)

set(CMAKE_CXX_STANDARD 11asdasd)

add_definitions(-D$ENV{OSNAME} -D$ENV{OSTYPE} -DOSVER=$ENV{OSVER} -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS)

include_directories(~/src-git/include ~/src3rd/lib330)
#link_directories(~/lib/$ENV{PLATFORM})

link_directories(~/lib/darwin.16.7.x86_64)

add_executable(isi330
        callbacks.c
        exit.c
        init.c
        isi330.h
        log.c
        main.c
        q330.c
        ReleaseNotes.c
        signals.c
        string.c)
