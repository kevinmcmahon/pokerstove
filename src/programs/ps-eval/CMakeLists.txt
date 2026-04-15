project(eval)

add_executable(ps-eval main.cpp)

target_link_libraries(ps-eval
        peval
        penum
        ${Boost_LIBRARIES}
)

add_test(NAME PsEvalInvalidGame
    COMMAND ${CMAKE_COMMAND}
        -DPS_EVAL=$<TARGET_FILE:ps-eval>
        "-DTEST_ARGS=--game;x;AcAs;Kh4d"
        -DEXPECTED_REGEX=Invalid\ game:\ x
        -P ${CMAKE_CURRENT_SOURCE_DIR}/AssertCommandFails.cmake)

add_test(NAME PsEvalInvalidBoard
    COMMAND ${CMAKE_COMMAND}
        -DPS_EVAL=$<TARGET_FILE:ps-eval>
        "-DTEST_ARGS=AhKh;KdQd;--board;AcAc"
        -DEXPECTED_REGEX=Invalid\ board:\ AcAc
        -P ${CMAKE_CURRENT_SOURCE_DIR}/AssertCommandFails.cmake)

add_test(NAME PsEvalInvalidHand
    COMMAND ${CMAKE_COMMAND}
        -DPS_EVAL=$<TARGET_FILE:ps-eval>
        "-DTEST_ARGS=AcAsKd"
        -DEXPECTED_REGEX=Invalid\ hand:\ AcAsKd
        -P ${CMAKE_CURRENT_SOURCE_DIR}/AssertCommandFails.cmake)

add_test(NAME PsEvalNoArgs
    COMMAND ${CMAKE_COMMAND}
        -DPS_EVAL=$<TARGET_FILE:ps-eval>
        -DEXPECTED_REGEX=ps-eval,\ a\ poker\ hand\ evaluator
        -P ${CMAKE_CURRENT_SOURCE_DIR}/AssertCommandFails.cmake)

add_test(NAME PsEvalMalformedArguments
    COMMAND ${CMAKE_COMMAND}
        -DPS_EVAL=$<TARGET_FILE:ps-eval>
        "-DTEST_ARGS=--board"
        "-DEXPECTED_REGEX=ps-eval:"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/AssertCommandFails.cmake)

add_test(NAME PsEvalTwoHandOutput
    COMMAND ${CMAKE_COMMAND}
        -DPS_EVAL=$<TARGET_FILE:ps-eval>
        "-DTEST_ARGS=AcAs;KhQh;--board;2c3d4h5s6c"
        "-DEXPECTED_REGEXES=The hand AcAs has 50 % equity;The hand KhQh has 50 % equity"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/AssertCommandSucceeds.cmake)

add_test(NAME PsEvalSingleHandUsesRandomHandLabel
    COMMAND ${CMAKE_COMMAND}
        -DPS_EVAL=$<TARGET_FILE:ps-eval>
        "-DTEST_ARGS=AcAs;--board;2c3d4h5s6c"
        "-DEXPECTED_REGEXES=The hand AcAs has;A random hand has"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/AssertCommandSucceeds.cmake)

add_test(NAME PsEvalQuiet
    COMMAND ${CMAKE_COMMAND}
        -DPS_EVAL=$<TARGET_FILE:ps-eval>
        "-DTEST_ARGS=AcAs;KhQh;--quiet;--board;2c3d4h5s6c"
        "-DFORBIDDEN_REGEXES=."
        -P ${CMAKE_CURRENT_SOURCE_DIR}/AssertCommandSucceeds.cmake)

add_test(NAME PsEvalConflictingCards
    COMMAND ${CMAKE_COMMAND}
        -DPS_EVAL=$<TARGET_FILE:ps-eval>
        "-DTEST_ARGS=AcAs;KhQh;--board;Ac2d3h4s5c"
        "-DEXPECTED_REGEX=Invalid showdown: no disjoint hand/board combinations"
        -P ${CMAKE_CURRENT_SOURCE_DIR}/AssertCommandFails.cmake)
