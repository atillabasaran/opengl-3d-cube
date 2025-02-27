cmake_minimum_required(VERSION 3.10)
project(glad)

# Kaynak dosyaları
set(SOURCES
    src/glad.c
)

# Kütüphane oluştur
add_library(glad STATIC ${SOURCES})

# Include dizinlerini ayarla
target_include_directories(glad PUBLIC include)