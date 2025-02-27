#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

#if defined(_WIN32)
#include <windows.h>
static HMODULE libGL;

static void* load_gl_func(const char* name) {
    void* proc = (void*)GetProcAddress(libGL, name);
    return proc;
}

static int open_gl_lib() {
    libGL = LoadLibraryA("opengl32.dll");
    if(libGL != NULL) {
        return 1;
    }
    return 0;
}

static void close_gl_lib() {
    if(libGL != NULL) {
        FreeLibrary(libGL);
        libGL = NULL;
    }
}
#else
#include <dlfcn.h>
static void* libGL;

static void* load_gl_func(const char* name) {
    void* proc = (void*)dlsym(libGL, name);
    return proc;
}

static int open_gl_lib() {
    libGL = dlopen("libGL.so.1", RTLD_LAZY | RTLD_GLOBAL);
    if(libGL != NULL) {
        return 1;
    }
    
    libGL = dlopen("libGL.so", RTLD_LAZY | RTLD_GLOBAL);
    if(libGL != NULL) {
        return 1;
    }
    return 0;
}

static void close_gl_lib() {
    if(libGL != NULL) {
        dlclose(libGL);
        libGL = NULL;
    }
}
#endif

/* Fonksiyon işaretçileri */
PFNGLCLEARPROC glClear;
PFNGLCLEARCOLORPROC glClearColor;
PFNGLVIEWPORTPROC glViewport;
PFNGLENABLEPROC glEnable;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLDRAWELEMENTSPROC glDrawElements;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

/* GLAD başlatma fonksiyonu */
int gladLoadGLLoader(GLADloadproc load) {
    static int opened = 0;
    int status = 1;
    
    if(load == NULL) {
        if(!opened) {
            opened = open_gl_lib();
        }
        if(!opened) return 0;
        load = load_gl_func;
    }
    
    /* Fonksiyonları yükle */
    glClear = (PFNGLCLEARPROC)load("glClear");
    glClearColor = (PFNGLCLEARCOLORPROC)load("glClearColor");
    glViewport = (PFNGLVIEWPORTPROC)load("glViewport");
    glEnable = (PFNGLENABLEPROC)load("glEnable");
    glGenBuffers = (PFNGLGENBUFFERSPROC)load("glGenBuffers");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)load("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)load("glBindVertexArray");
    glBindBuffer = (PFNGLBINDBUFFERPROC)load("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)load("glBufferData");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)load("glVertexAttribPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)load("glEnableVertexAttribArray");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)load("glDeleteBuffers");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)load("glDeleteVertexArrays");
    glDrawElements = (PFNGLDRAWELEMENTSPROC)load("glDrawElements");
    glCreateShader = (PFNGLCREATESHADERPROC)load("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)load("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)load("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)load("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)load("glGetShaderInfoLog");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)load("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)load("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)load("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)load("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)load("glGetProgramInfoLog");
    glDeleteShader = (PFNGLDELETESHADERPROC)load("glDeleteShader");
    glUseProgram = (PFNGLUSEPROGRAMPROC)load("glUseProgram");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)load("glGetUniformLocation");
    glUniform1i = (PFNGLUNIFORM1IPROC)load("glUniform1i");
    glUniform1f = (PFNGLUNIFORM1FPROC)load("glUniform1f");
    glUniform3f = (PFNGLUNIFORM3FPROC)load("glUniform3f");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)load("glUniformMatrix4fv");
    
    /* Yükleme başarılı mı kontrol et - temel fonksiyonlar */
    if(glClear == NULL || glClearColor == NULL || glViewport == NULL) {
        status = 0;
    }
    
    return status;
}