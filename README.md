# OpenGL 3D Küp Projesi

Bu proje, modern OpenGL (3.3) kullanarak interaktif bir 3D küp uygulamasını göstermektedir. Küp, farklı renklere sahip yüzleriyle oluşturulmuş ve fare/klavye kontrolleriyle etkileşimli hale getirilmiştir.

## Özellikler

- Modern OpenGL 3.3 Core Profile kullanımı
- GLFW pencere yönetimi
- GLAD OpenGL yükleyici
- 3D küp geometrisi oluşturma
- Shader programları (vertex ve fragment shader'lar)
- Matris dönüşümleri (Model-View-Projection)
- Temel aydınlatma efektleri
- Etkileşimli kamera kontrolü:
  - Fare ile etrafında dönme
  - Fare tekerleği ile zoom
  - Tuşlar ile dönüş hızı kontrolü

## Kontroller

- **Fare hareketi:** Kamerayı döndürür
- **Fare tekerleği:** Zoom yapar (kamera mesafesini ayarlar)
- **Yukarı/Aşağı tuşları:** X ekseni etrafındaki dönüş hızını artırır/azaltır
- **Sağ/Sol tuşları:** Y ekseni etrafındaki dönüş hızını artırır/azaltır
- **R tuşu:** Dönüş hızlarını varsayılana sıfırlar
- **ESC tuşu:** Uygulamayı kapatır

## Gereksinimler

- C++ derleyici (C++17 desteği)
- CMake (en az 3.10)
- OpenGL geliştirme kütüphaneleri
- GLFW kütüphanesi

## Kurulum

### Linux (Ubuntu/Debian)

```bash
# Gerekli paketleri yükleyin
sudo apt update
sudo apt install build-essential cmake libgl1-mesa-dev libglfw3-dev

# Projeyi derleyin
mkdir build && cd build
cmake ..
make

# Uygulamayı çalıştırın
./OpenGLProject
```

### macOS

```bash
# Homebrew kullanarak gerekli paketleri yükleyin
brew install cmake glfw

# Projeyi derleyin
mkdir build && cd build
cmake ..
make

# Uygulamayı çalıştırın
./OpenGLProject
```

### Windows (Visual Studio ile)

1. [CMake](https://cmake.org/download/) indirin ve yükleyin
2. [GLFW](https://www.glfw.org/download.html) indirin
3. CMake GUI kullanarak projeyi yapılandırın ve Visual Studio çözümü oluşturun
4. Visual Studio'da projeyi açın ve derleyin

## Proje Yapısı

- `main.cpp`: Ana uygulama kodu
- `shaders/vertex.glsl`: Vertex shader kodu
- `shaders/fragment.glsl`: Fragment shader kodu
- `glad/`: GLAD OpenGL yükleyici dosyaları
- `CMakeLists.txt`: CMake yapılandırma dosyası

## Teknik Detaylar

- Vertex Array Objects (VAO) ve Vertex Buffer Objects (VBO) kullanımı
- Element Buffer Objects (EBO) ile indeksli çizim
- Model, View ve Projection matrislerinin manuel oluşturulması
- Shader programlarının yüklenmesi ve uniform değişkenlerin ayarlanması