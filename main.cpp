#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Pencere boyutları
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Kamera değişkenleri
float cameraRadius = 3.0f;
float cameraHeight = 0.0f;
float cameraAngle = 0.0f;

// Dönüş hızı değişkenleri
float rotationSpeedX = 0.5f;
float rotationSpeedY = 0.7f;

// Fare kontrol değişkenleri
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float yaw = -90.0f;  // Yaw, Y ekseninde dönüş
float pitch = 0.0f;  // Pitch, X ekseninde dönüş

// Shader sınıfı - shader programını yükleme ve kullanma fonksiyonlarını içerir
class Shader {
public:
    unsigned int ID; // Program ID
    
    // Constructor - shader dosyalarını okur ve derler
    Shader(const char* vertexPath, const char* fragmentPath) {
        // 1. Shader dosyalarını oku
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        
        // İstisna fırlatmayı etkinleştir
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        
        try {
            // Dosyaları aç
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            
            // Akışlardan oku
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            
            // Dosyaları kapat
            vShaderFile.close();
            fShaderFile.close();
            
            // Akışları string'e dönüştür
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (std::ifstream::failure& e) {
            std::cerr << "HATA: Shader dosyası okunamadı" << std::endl;
        }
        
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        
        // 2. Shader'ları derle
        unsigned int vertex, fragment;
        int success;
        char infoLog[512];
        
        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        
        // Derleme hatalarını kontrol et
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cerr << "HATA: Vertex shader derleme hatası\n" << infoLog << std::endl;
        }
        
        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        
        // Derleme hatalarını kontrol et
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cerr << "HATA: Fragment shader derleme hatası\n" << infoLog << std::endl;
        }
        
        // Shader programı
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        
        // Bağlama hatalarını kontrol et
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cerr << "HATA: Shader programı bağlama hatası\n" << infoLog << std::endl;
        }
        
        // Artık bağlandıkları için shader nesnelerini sil
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    // Programı aktif et
    void use() {
        glUseProgram(ID);
    }
    
    // Uniform değişkenlerini ayarla
    void setBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    
    void setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    
    void setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    
    void setVec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    
    void setMat4(const std::string &name, const float* mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat);
    }
};

// Matris işlemleri için yardımcı fonksiyonlar
namespace MatrixUtils {
    // Model matrisini oluşturur - dünya uzayındaki konumu ve yönelimi belirler
    void createModelMatrix(float* matrix, float angleX, float angleY) {
        // Birim matris ile başla
        for (int i = 0; i < 16; i++) {
            matrix[i] = 0.0f;
        }
        matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
        
        // X ekseni etrafında döndürme
        float cosX = cos(angleX);
        float sinX = sin(angleX);
        
        // Y ekseni etrafında döndürme
        float cosY = cos(angleY);
        float sinY = sin(angleY);
        
        // Döndürme matrisini oluştur (X ve Y ekseni etrafında)
        matrix[0] = cosY;
        matrix[2] = -sinY;
        matrix[4] = sinX * sinY;
        matrix[5] = cosX;
        matrix[6] = sinX * cosY;
        matrix[8] = -cosX * sinY;
        matrix[9] = -sinX;
        matrix[10] = cosX * cosY;
    }
    
    // Görüntüleme matrisini oluşturur - kamera konumunu ve bakış yönünü belirler
    void createViewMatrix(float* matrix, float* position, float* target, float* up) {
        // Bakış yönü vektörünü hesapla (kameradan hedefe doğru)
        float direction[3];
        for (int i = 0; i < 3; i++) {
            direction[i] = target[i] - position[i];
        }
        
        // Vektörü normalize et
        float length = sqrt(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
        for (int i = 0; i < 3; i++) {
            direction[i] /= length;
        }
        
        // Sağ vektörünü hesapla (up vektörü ile bakış yönü vektörünün çapraz çarpımı)
        float right[3];
        right[0] = up[1] * direction[2] - up[2] * direction[1];
        right[1] = up[2] * direction[0] - up[0] * direction[2];
        right[2] = up[0] * direction[1] - up[1] * direction[0];
        
        // Right vektörünü normalize et
        length = sqrt(right[0] * right[0] + right[1] * right[1] + right[2] * right[2]);
        for (int i = 0; i < 3; i++) {
            right[i] /= length;
        }
        
        // Yeni yukarı vektörünü hesapla (bakış yönü ve sağ vektörünün çapraz çarpımı)
        float newUp[3];
        newUp[0] = direction[1] * right[2] - direction[2] * right[1];
        newUp[1] = direction[2] * right[0] - direction[0] * right[2];
        newUp[2] = direction[0] * right[1] - direction[1] * right[0];
        
        // Birim matris ile başla
        for (int i = 0; i < 16; i++) {
            matrix[i] = 0.0f;
        }
        
        // Görüntüleme matrisi oluştur
        matrix[0] = right[0];
        matrix[4] = right[1];
        matrix[8] = right[2];
        
        matrix[1] = newUp[0];
        matrix[5] = newUp[1];
        matrix[9] = newUp[2];
        
        matrix[2] = -direction[0];
        matrix[6] = -direction[1];
        matrix[10] = -direction[2];
        
        matrix[15] = 1.0f;
        
        // Kamera konumunu matriste ayarla
        matrix[12] = -(right[0] * position[0] + right[1] * position[1] + right[2] * position[2]);
        matrix[13] = -(newUp[0] * position[0] + newUp[1] * position[1] + newUp[2] * position[2]);
        matrix[14] = (direction[0] * position[0] + direction[1] * position[1] + direction[2] * position[2]);
    }
    
    // Perspektif projeksiyon matrisini oluşturur - 3D görüntüyü 2D ekrana yansıtır
    void createPerspectiveMatrix(float* matrix, float fov, float aspect, float near, float far) {
        // Matris elemanlarını sıfırla
        for (int i = 0; i < 16; i++) {
            matrix[i] = 0.0f;
        }
        
        float tanHalfFovy = tan(fov / 2.0f);
        
        matrix[0] = 1.0f / (aspect * tanHalfFovy); // X ekseni için skala faktörü
        matrix[5] = 1.0f / tanHalfFovy;            // Y ekseni için skala faktörü
        matrix[10] = -(far + near) / (far - near);  // Z değerini normalize etme
        matrix[11] = -1.0f;                        // W bileşeni için çarpan
        matrix[14] = -(2.0f * far * near) / (far - near); // Perspektif için öteleme
    }
}

// Pencere boyutu değiştiğinde çağrılacak fonksiyon
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Fare hareketini işleyen fonksiyon
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Ters çevrildi (y koordinatları yukarıdan aşağıya doğru)
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Pitch açısını sınırlandır (-89° ile 89° arasında)
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Kamera pozisyonunu hesapla (küresel koordinatlar)
    cameraAngle = yaw * 3.14159f / 180.0f;
    float pitchRad = pitch * 3.14159f / 180.0f;
    
    // Kamera pozisyonunu güncelle
    cameraHeight = sin(pitchRad) * cameraRadius;
}

// Fare tekerleği girdisini işleyen fonksiyon
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Kamera mesafesini ayarla (zoom)
    cameraRadius -= static_cast<float>(yoffset) * 0.2f;
    
    // Kamera mesafesi kontrolü
    if (cameraRadius < 1.0f)
        cameraRadius = 1.0f;
    if (cameraRadius > 10.0f)
        cameraRadius = 10.0f;
}

// Klavye girdilerini işleyen fonksiyon
void processInput(GLFWwindow *window) {
    // Escape tuşu - pencereyi kapat
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // Dönüş hızını ayarlama tuşları
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rotationSpeedX += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rotationSpeedX -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rotationSpeedY += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rotationSpeedY -= 0.01f;
    
    // Dönüş hızı alt sınırı (negatif olmaması için)
    if (rotationSpeedX < 0.0f)
        rotationSpeedX = 0.0f;
    if (rotationSpeedY < 0.0f)
        rotationSpeedY = 0.0f;
        
    // Dönüşü sıfırlama
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        rotationSpeedX = 0.5f;
        rotationSpeedY = 0.7f;
    }
}

int main() {
    // GLFW'yi başlat
    if (!glfwInit()) {
        std::cerr << "GLFW başlatılamadı" << std::endl;
        return -1;
    }
    
    std::cout << "GLFW başlatıldı" << std::endl;
    
    // OpenGL versiyonunu 3.3 olarak ayarla
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Pencere oluştur
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Modern OpenGL 3D Küp", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Pencere oluşturulamadı" << std::endl;
        glfwTerminate();
        return -1;
    }
    std::cout << "Pencere oluşturuldu" << std::endl;
    
    // Pencereyi mevcut context olarak ayarla
    glfwMakeContextCurrent(window);
    std::cout << "Context ayarlandı" << std::endl;
    
    // GLAD'i başlat
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD başlatılamadı" << std::endl;
        return -1;
    }
    std::cout << "GLAD başlatıldı" << std::endl;
    
    // Viewport'u ayarla
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    
    // Pencere boyutu değiştiğinde çağrılacak callback'i ayarla
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Fare kontrollerini ayarla
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Fare imlecini yakalama (FPS kamera için)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Derinlik testini etkinleştir
    glEnable(GL_DEPTH_TEST);
    
    // Shader programını yükle
    Shader ourShader("/home/mrbasaran/Documents/projects/playground/opengl-claude-code-test/shaders/vertex.glsl", 
                     "/home/mrbasaran/Documents/projects/playground/opengl-claude-code-test/shaders/fragment.glsl");
    
    // Küp için vertex verileri - konum ve renk
    float vertices[] = {
        // Koordinatlar (X, Y, Z)     // Renkler (R, G, B)
        // Ön yüz (kırmızı)
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        
        // Arka yüz (yeşil)
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        
        // Üst yüz (mavi)
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        
        // Alt yüz (sarı)
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
        
        // Sağ yüz (turkuaz)
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
        
        // Sol yüz (mor)
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f
    };
    
    // Yüzleri oluşturmak için indeksler
    unsigned int indices[] = {
        // Ön yüz
        0, 1, 2,  2, 3, 0,
        // Arka yüz
        4, 5, 6,  6, 7, 4,
        // Üst yüz
        8, 9, 10, 10, 11, 8,
        // Alt yüz
        12, 13, 14, 14, 15, 12,
        // Sağ yüz
        16, 17, 18, 18, 19, 16,
        // Sol yüz
        20, 21, 22, 22, 23, 20
    };
    
    // Vertex Array Object (VAO), Vertex Buffer Object (VBO), ve Element Buffer Object (EBO)
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // VAO'yu bağla
    glBindVertexArray(VAO);
    
    // VBO'yu bağla ve vertex verilerini yükle
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // EBO'yu bağla ve indeks verilerini yükle
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Vertex pozisyon özniteliğini ayarla
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Vertex renk özniteliğini ayarla
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // VAO ve VBO bağlantısını kaldır (artık tanımlı ve kullanıma hazır)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Matrisleri oluştur
    float modelMatrix[16], viewMatrix[16], projectionMatrix[16];
    
    // Kamera parametreleri (değerler render döngüsünde güncellenecek)
    float cameraPos[] = {0.0f, 0.0f, 3.0f};
    float cameraTarget[] = {0.0f, 0.0f, 0.0f};
    float cameraUp[] = {0.0f, 1.0f, 0.0f};
    
    // Projeksiyon parametreleri
    float fov = 45.0f * 3.14159f / 180.0f; // FOV 45 derece (radyan cinsinden)
    float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    
    // Animasyon değişkenleri
    float angleX = 0.0f;
    float angleY = 0.0f;
    
    // Projeksiyon matrisini oluştur
    MatrixUtils::createPerspectiveMatrix(projectionMatrix, fov, aspectRatio, nearPlane, farPlane);
    
    // Render döngüsü
    std::cout << "Render döngüsü başlıyor" << std::endl;
    while (!glfwWindowShouldClose(window)) {
        // Girdi işleme
        processInput(window);
        
        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Shader programını aktif et
        ourShader.use();
        
        // Zamanla değişen dönüş açılarını hesapla
        float timeValue = glfwGetTime();
        angleX = timeValue * rotationSpeedX;
        angleY = timeValue * rotationSpeedY;
        
        // Kamera pozisyonunu güncelle
        cameraPos[0] = cos(cameraAngle) * cameraRadius;
        cameraPos[1] = cameraHeight;
        cameraPos[2] = sin(cameraAngle) * cameraRadius;
        
        // Görünüm matrisini güncelle
        MatrixUtils::createViewMatrix(viewMatrix, cameraPos, cameraTarget, cameraUp);
        
        // Model matrisini güncelle (dönüş animasyonu için)
        MatrixUtils::createModelMatrix(modelMatrix, angleX, angleY);
        
        // Matrisleri shader'a gönder
        ourShader.setMat4("model", modelMatrix);
        ourShader.setMat4("view", viewMatrix);
        ourShader.setMat4("projection", projectionMatrix);
        
        // Ortam ışığı şiddetini güncelle (isteğe bağlı - animasyon için)
        float ambientValue = (sin(timeValue) * 0.2f) + 0.3f; // 0.1 - 0.5 arasında değişen ambient değeri
        ourShader.setFloat("ambientStrength", ambientValue);
        
        // Küpü çiz
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        // Tamponları değiştir ve olayları sorgula
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // OpenGL nesnelerini temizle
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    // GLFW'yi sonlandır
    glfwTerminate();
    return 0;
}