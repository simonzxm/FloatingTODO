cmake_minimum_required(VERSION 3.16)

project(todo_hello LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql Test)

qt_standard_project_setup()
enable_testing()

get_target_property(QT_CORE_LOCATION Qt6::Core IMPORTED_LOCATION_RELEASE)
if(NOT QT_CORE_LOCATION)
    get_target_property(QT_CORE_LOCATION Qt6::Core IMPORTED_LOCATION)
endif()
get_filename_component(QT_BIN_DIR "${QT_CORE_LOCATION}" DIRECTORY)
get_filename_component(CXX_COMPILER_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
set(FLOATINGTODO_TEST_PATH "${QT_BIN_DIR}\\;${CXX_COMPILER_DIR}\\;$ENV{PATH}")

set(FLOATINGTODO_CORE_SOURCES
    src/model/TodoItem.h
    src/model/TodoItem.cpp
    src/repository/TodoRepository.h
    src/repository/TodoRepository.cpp
    src/service/TodoService.h
    src/service/TodoService.cpp
    src/controller/TodoController.h
    src/controller/TodoController.cpp
)

set(FLOATINGTODO_VIEW_SOURCES
    src/view/MainWindow.h
    src/view/MainWindow.cpp
    src/view/TodoListView.h
    src/view/TodoListView.cpp
    src/view/TodoItemWidget.h
    src/view/TodoItemWidget.cpp
    src/view/TodoEditorDialog.h
    src/view/TodoEditorDialog.cpp
)

qt_add_library(floatingtodo_core STATIC
    ${FLOATINGTODO_CORE_SOURCES}
)

target_include_directories(floatingtodo_core PUBLIC src)
target_link_libraries(floatingtodo_core PUBLIC Qt6::Core Qt6::Gui Qt6::Sql)

qt_add_executable(todo_hello
    src/main.cpp
    ${FLOATINGTODO_VIEW_SOURCES}
)

target_link_libraries(todo_hello PRIVATE floatingtodo_core Qt6::Widgets)

set_target_properties(todo_hello PROPERTIES
    WIN32_EXECUTABLE ON
)

function(add_floatingtodo_test target source)
    qt_add_executable(${target}
        ${source}
    )
    target_link_libraries(${target} PRIVATE floatingtodo_core Qt6::Test Qt6::Widgets)
    add_test(NAME ${target} COMMAND ${target})
    set_tests_properties(${target} PROPERTIES
        ENVIRONMENT "PATH=${FLOATINGTODO_TEST_PATH}"
    )
endfunction()

add_floatingtodo_test(test_models tests/TestModels.cpp)
add_floatingtodo_test(test_todo_repository tests/TestTodoRepository.cpp)
add_floatingtodo_test(test_todo_service tests/TestTodoService.cpp)
