#version 330 core

// Vertex giriş verileri
layout (location = 0) in vec3 aPos;    // Vertex pozisyonu (x, y, z)
layout (location = 1) in vec3 aColor;  // Vertex rengi (r, g, b)

// Fragment shader'a çıkış verileri
out vec3 vertexColor;  // Fragment shader'a aktarılacak renk bilgisi

// Uniform değişkenler (her çizimdeki ortak veriler)
uniform mat4 model;      // Model matrisi (yerel koordinatlardan dünya koordinatlarına)
uniform mat4 view;       // Görünüm matrisi (dünya koordinatlarından kamera koordinatlarına)
uniform mat4 projection; // Projeksiyon matrisi (kamera koordinatlarından kesme koordinatlarına)

void main() {
    // MVP matrisi uygulaması (Model-View-Projection)
    // Vertex konumunun 4D homojen koordinatlar olarak hesaplanması
    // Matris dönüşümleri sağdan sola doğru uygulanır
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Vertex rengini fragment shader'a ilet
    vertexColor = aColor;
    
    // Alternatif olarak, pozisyona göre renk hesaplanabilir
    // vertexColor = 0.5 * (aPos + vec3(1.0, 1.0, 1.0)); // Pozisyona göre renklendirme
}