

#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

GLuint  g_textureName;

int     g_nX;

int     g_nY;

int     g_nPandaWidth;

int     g_nPandaHeight;

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

auto gVertexShader =
    "attribute vec4 vPosition;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
    "}\n";

auto gFragmentShader =
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "}\n";

GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

GLuint gProgram;
GLuint gvPositionHandle;

bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);
    gProgram = createProgram(gVertexShader, gFragmentShader);
    if (!gProgram) {
        LOGE("Could not create program.");
        return false;
    }
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
            gvPositionHandle);

    glViewport(0, 0, w, h);
    checkGlError("glViewport");
    return true;
}
/*
void setTextureData(jbyte *data, int width, int height)
{
    g_nPandaWidth = width;
    g_nPandaHeight = height;
    glGenTextures(1, &g_textureName);
    glBindTexture(GL_TEXTURE_2D, g_textureName);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data);

}*/


const GLfloat gTriangleVertices[] = { -1.0f,-1.0f, 1.0f, -1.0f, -1.0f,1.0f, 1.0f, 1.0f
                                        };

void renderFrame() {
//    GLfloat vertices[12] = {
//            g_nX                ,   g_nY+g_nPandaHeight ,   0.0f,   // LEFT  | BOTTOM
//            g_nX+g_nPandaWidth  ,   g_nY+g_nPandaHeight ,   0.0f,   // RIGHT | BOTTOM
//            g_nX                ,   g_nY                ,   0.0f,   // LEFT  | TOP
//            g_nX+g_nPandaWidth  ,   g_nY                ,   0.0f    // RIGHT | TOP
//    };
//    GLfloat texture[8] = {
//            0   , 1,
//            1   , 1,
//            0   , 0,
//            1   , 0
//    };
//    glEnable(GL_TEXTURE_2D);
//    glGenTextures(1, &g_textureName);
//    glBindTexture(GL_TEXTURE_2D, g_textureName);
//    glTexImage2D()
//
//    glEnableVertexAttribArray(GL_VERTEX_ARRAY_BINDING_OES);
//    glVertexAttribPointer(GL_VERTEX_ARRAY_BINDING_OES,3, GL_FLOAT,GL_FALSE, 0, vertices);
//    glEnableClientState(GL_VERTEX_ARRAY);
//    glVertexPointer(3, GL_FLOAT, 0, vertices);
//    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//    glTexCoordPointer(2, GL_FLOAT, 0, texture);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//    glDisableClientState(GL_VERTEX_ARRAY);
//    glDisable(GL_TEXTURE_2D);

//    glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, g_textureName);
//    glEnableClientState(GL_VERTEX_ARRAY);
//    glVertexPointer(3, GL_FLOAT, 0, vertices);
//    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//    glTexCoordPointer(2, GL_FLOAT, 0, texture);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//    glDisableClientState(GL_VERTEX_ARRAY);
//    glDisable(GL_TEXTURE_2D);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    checkGlError("glClearColor");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glUseProgram(gProgram);
    checkGlError("glUseProgram");

    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(gvPositionHandle);
    checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_TRIANGLE_STRIP, 0,4);
    checkGlError("glDrawArrays");

}

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
   // JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_setTextureData(JNIEnv * env, jobject obj);
};

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj)
{
    renderFrame();
}
/*
JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_setTextureData(JNIEnv* env, jobject thiz, jintArray arr, jint width, jint height)
{

    jbyte *data = env->GetByteArrayElements((jbyteArray)arr, 0);
   /setTextureData(data, width, height);
    env->ReleaseByteArrayElements((jbyteArray)arr,data, JNI_ABORT);

}*/




