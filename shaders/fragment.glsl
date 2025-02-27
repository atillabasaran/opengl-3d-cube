#version 330 core

// Vertex shader'dan gelen veriler
in vec3 vertexColor;  // Vertex shader'dan gelen renk bilgisi

// Çıkış değişkeni (frame buffer'a yazılacak piksel rengi)
out vec4 FragColor;

// Uniform değişkenler
uniform float ambientStrength = 0.3; // Ortam ışık şiddeti

void main() {
    // Temel renk hesaplaması - vertex shader'dan gelen renkle doğrudan boyama
    vec3 baseColor = vertexColor;
    
    // Basit ortam ışığı (ambient light) hesaplaması
    // Gerçek bir sahne için directional light, point light, veya spotlight eklenebilir
    vec3 ambientColor = ambientStrength * baseColor;
    
    // Basit sahte aydınlatma (fake lighting) hesaplaması
    // Gerçek bir uygulama için normal vektörleri ve ışık yönü kullanılır
    
    // Aydınlatma olmadan doğrudan vertex rengi kullanımı
    // FragColor = vec4(baseColor, 1.0);
    
    // Basit ortam ışığı ile renklendirme
    FragColor = vec4(ambientColor + baseColor * (1.0 - ambientStrength), 1.0);
    
    // Alternatif: Ekrana normalize edilmiş koordinatları göstermek için
    // FragColor = vec4(gl_FragCoord.x / 800.0, gl_FragCoord.y / 600.0, 0.0, 1.0);
}