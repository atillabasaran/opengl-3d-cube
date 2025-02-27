cmake_minimum_required(VERSION 3.10)
project(OpenGLProject)

# C++ standart sürümü
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Kaynak dosyaları
set(SOURCES
    main.cpp
)

# Yürütülebilir dosya
add_executable(${PROJECT_NAME} ${SOURCES})

# Paketleri bul
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# GLAD alt projesi
add_subdirectory(glad)

# Dahil edilecek dizinler
target_include_directories(${PROJECT_NAME} PRIVATE
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/glad/include
)

# Bağlanacak kütüphaneler
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    glfw
    glad
)

# Mac OS için ek ayarlar
if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa -framework IOKit -framework CoreVideo")
endif()