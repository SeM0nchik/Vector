cmake_minimum_required(VERSION 3.10)
project(VectorProject LANGUAGES CXX)

# Явно указываем стандарт C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Добавляем исполняемый файл
add_executable(vector_public_test vector_public_test.cpp)

# Включаем тестирование
enable_testing()
add_test(NAME VectorTests COMMAND vector_public_test)