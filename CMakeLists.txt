cmake_minimum_required(VERSION 3.16)
project(Coursework)

# Стандарт C++ задаётся декларативно
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Поиск исходников проекта
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Главный файл
set(MAIN_CPP "main.cpp"
        include/DynamicArray.h)

# ImGui core
file(GLOB IMGUI_SOURCES
        "external/imgui/*.cpp"
        "external/backends/imgui_impl_glfw.cpp"
        "external/backends/imgui_impl_opengl3.cpp"
)

# GLFW — добавляем как подпроект
add_subdirectory(external/glfw)

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} ${MAIN_CPP} ${SOURCES} ${IMGUI_SOURCES})

# Инклуд директории
target_include_directories(${PROJECT_NAME} PRIVATE
        include
        src
        external/imgui
        external/backends
        external/glfw/include
)

# Линкуем с GLFW и OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL::GL)

# Компилятор-специфичные предупреждения через generator expressions
target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /EHsc>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -g>
)
