#include "ForgeUI/Backend/ForgeBackend_OpenGL.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace forge {

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_TEXTURE0                       0x84C0
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_BLEND                          0x0BE2
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_CULL_FACE                      0x0B44
#define GL_DEPTH_TEST                     0x0B71
#define GL_SCISSOR_TEST                   0x0C11
#define GL_TRIANGLES                      0x0004
#define GL_FLOAT                          0x1406
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_UNSIGNED_INT                   0x1405
#define GL_FALSE                          0
#define GL_TRUE                           1
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_LINEAR                         0x2601
#define GL_RGBA                           0x1908
#define GL_RGB                            0x1907

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef unsigned int GLbitfield;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLboolean;
typedef char GLchar;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
typedef float GLfloat;

#if defined(_WIN32)
#define GL_APIENTRY APIENTRY
#else
#define GL_APIENTRY
#endif

typedef void (GL_APIENTRY *PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint *arrays);
typedef void (GL_APIENTRY *PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (GL_APIENTRY *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint *arrays);
typedef void (GL_APIENTRY *PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void (GL_APIENTRY *PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (GL_APIENTRY *PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (GL_APIENTRY *PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
typedef void (GL_APIENTRY *PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
typedef GLuint (GL_APIENTRY *PFNGLCREATESHADERPROC)(GLenum type);
typedef void (GL_APIENTRY *PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (GL_APIENTRY *PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (GL_APIENTRY *PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint *params);
typedef void (GL_APIENTRY *PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (GL_APIENTRY *PFNGLDELETESHADERPROC)(GLuint shader);
typedef GLuint (GL_APIENTRY *PFNGLCREATEPROGRAMPROC)(void);
typedef void (GL_APIENTRY *PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (GL_APIENTRY *PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (GL_APIENTRY *PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint *params);
typedef void (GL_APIENTRY *PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (GL_APIENTRY *PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (GL_APIENTRY *PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef GLint (GL_APIENTRY *PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar *name);
typedef void (GL_APIENTRY *PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (GL_APIENTRY *PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (GL_APIENTRY *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (GL_APIENTRY *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (GL_APIENTRY *PFNGLVERTEXATTRIBIPOINTERPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
typedef void (GL_APIENTRY *PFNGLACTIVETEXTUREPROC)(GLenum texture);

typedef void (GL_APIENTRY *PFNGLENABLEPROC)(GLenum cap);
typedef void (GL_APIENTRY *PFNGLDISABLEPROC)(GLenum cap);
typedef void (GL_APIENTRY *PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef void (GL_APIENTRY *PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (GL_APIENTRY *PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void *indices);
typedef void (GL_APIENTRY *PFNGLGENTEXTURESPROC)(GLsizei n, GLuint *textures);
typedef void (GL_APIENTRY *PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (GL_APIENTRY *PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint *textures);
typedef void (GL_APIENTRY *PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (GL_APIENTRY *PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
typedef void (GL_APIENTRY *PFNGLTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);

static PFNGLGENVERTEXARRAYSPROC        glGenVertexArrays_ = nullptr;
static PFNGLBINDVERTEXARRAYPROC        glBindVertexArray_ = nullptr;
static PFNGLDELETEVERTEXARRAYSPROC     glDeleteVertexArrays_ = nullptr;
static PFNGLGENBUFFERSPROC             glGenBuffers_ = nullptr;
static PFNGLBINDBUFFERPROC             glBindBuffer_ = nullptr;
static PFNGLBUFFERDATAPROC             glBufferData_ = nullptr;
static PFNGLBUFFERSUBDATAPROC          glBufferSubData_ = nullptr;
static PFNGLDELETEBUFFERSPROC          glDeleteBuffers_ = nullptr;
static PFNGLCREATESHADERPROC           glCreateShader_ = nullptr;
static PFNGLSHADERSOURCEPROC          glShaderSource_ = nullptr;
static PFNGLCOMPILESHADERPROC          glCompileShader_ = nullptr;
static PFNGLGETSHADERIVPROC            glGetShaderiv_ = nullptr;
static PFNGLGETSHADERINFOLOGPROC       glGetShaderInfoLog_ = nullptr;
static PFNGLDELETESHADERPROC           glDeleteShader_ = nullptr;
static PFNGLCREATEPROGRAMPROC          glCreateProgram_ = nullptr;
static PFNGLATTACHSHADERPROC           glAttachShader_ = nullptr;
static PFNGLLINKPROGRAMPROC            glLinkProgram_ = nullptr;
static PFNGLGETPROGRAMIVPROC           glGetProgramiv_ = nullptr;
static PFNGLGETPROGRAMINFOLOGPROC      glGetProgramInfoLog_ = nullptr;
static PFNGLUSEPROGRAMPROC             glUseProgram_ = nullptr;
static PFNGLDELETEPROGRAMPROC          glDeleteProgram_ = nullptr;
static PFNGLGETUNIFORMLOCATIONPROC     glGetUniformLocation_ = nullptr;
static PFNGLUNIFORMMATRIX4FVPROC       glUniformMatrix4fv_ = nullptr;
static PFNGLUNIFORM1IPROC              glUniform1i_ = nullptr;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray_ = nullptr;
static PFNGLVERTEXATTRIBPOINTERPROC    glVertexAttribPointer_ = nullptr;
static PFNGLVERTEXATTRIBIPOINTERPROC   glVertexAttribIPointer_ = nullptr;
static PFNGLACTIVETEXTUREPROC          glActiveTexture_ = nullptr;

#define GL_COLOR_BUFFER_BIT               0x00004000

typedef void (GL_APIENTRY *PFNGLCLEARPROC)(GLbitfield mask);
typedef void (GL_APIENTRY *PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (GL_APIENTRY *PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);

static PFNGLCLEARPROC                  glClear_ = nullptr;
static PFNGLCLEARCOLORPROC             glClearColor_ = nullptr;
static PFNGLVIEWPORTPROC               glViewport_ = nullptr;
static PFNGLENABLEPROC                 glEnable_ = nullptr;
static PFNGLDISABLEPROC                glDisable_ = nullptr;
static PFNGLBLENDFUNCPROC              glBlendFunc_ = nullptr;
static PFNGLSCISSORPROC                glScissor_ = nullptr;
static PFNGLDRAWELEMENTSPROC           glDrawElements_ = nullptr;
static PFNGLGENTEXTURESPROC            glGenTextures_ = nullptr;
static PFNGLBINDTEXTUREPROC            glBindTexture_ = nullptr;
static PFNGLDELETETEXTURESPROC         glDeleteTextures_ = nullptr;
static PFNGLTEXPARAMETERIPROC          glTexParameteri_ = nullptr;
static PFNGLTEXIMAGE2DPROC             glTexImage2D_ = nullptr;
static PFNGLTEXSUBIMAGE2DPROC          glTexSubImage2D_ = nullptr;

static void* GetGLProc(const char* name) {
#if defined(_WIN32)
    static HMODULE hModule = LoadLibraryA("opengl32.dll");
    void* p = nullptr;
    typedef PROC (WINAPI *PFNWGLGETPROCADDRESS)(LPCSTR);
    static PFNWGLGETPROCADDRESS wglGetProcAddress_ = (PFNWGLGETPROCADDRESS)GetProcAddress(hModule, "wglGetProcAddress");
    if (wglGetProcAddress_) {
        p = (void*)wglGetProcAddress_(name);
    }
    if (!p || p == (void*)0x1 || p == (void*)0x2 || p == (void*)0x3 || p == (void*)-1) {
        p = (void*)GetProcAddress(hModule, name);
    }
    return p;
#else
    return nullptr;
#endif
}

ForgeBackend_OpenGL::ForgeBackend_OpenGL()
    : m_DisplayWidth(1920)
    , m_DisplayHeight(1080)
    , m_VAO(0)
    , m_VBO(0)
    , m_IBO(0)
    , m_VBOSize(0)
    , m_IBOSize(0)
    , m_GLLoaded(false) {
    for (size_t i = 0; i < static_cast<size_t>(ForgePipelineType::Count); ++i) {
        m_Programs[i] = 0;
        m_LocProj[i] = -1;
        m_LocTexture[i] = -1;
    }
}

ForgeBackend_OpenGL::~ForgeBackend_OpenGL() {
    Shutdown();
}

bool ForgeBackend_OpenGL::LoadGLFunctions() {
    if (m_GLLoaded) return true;

    glGenVertexArrays_ = (PFNGLGENVERTEXARRAYSPROC)GetGLProc("glGenVertexArrays");
    glBindVertexArray_ = (PFNGLBINDVERTEXARRAYPROC)GetGLProc("glBindVertexArray");
    glDeleteVertexArrays_ = (PFNGLDELETEVERTEXARRAYSPROC)GetGLProc("glDeleteVertexArrays");
    glGenBuffers_ = (PFNGLGENBUFFERSPROC)GetGLProc("glGenBuffers");
    glBindBuffer_ = (PFNGLBINDBUFFERPROC)GetGLProc("glBindBuffer");
    glBufferData_ = (PFNGLBUFFERDATAPROC)GetGLProc("glBufferData");
    glBufferSubData_ = (PFNGLBUFFERSUBDATAPROC)GetGLProc("glBufferSubData");
    glDeleteBuffers_ = (PFNGLDELETEBUFFERSPROC)GetGLProc("glDeleteBuffers");
    glCreateShader_ = (PFNGLCREATESHADERPROC)GetGLProc("glCreateShader");
    glShaderSource_ = (PFNGLSHADERSOURCEPROC)GetGLProc("glShaderSource");
    glCompileShader_ = (PFNGLCOMPILESHADERPROC)GetGLProc("glCompileShader");
    glGetShaderiv_ = (PFNGLGETSHADERIVPROC)GetGLProc("glGetShaderiv");
    glGetShaderInfoLog_ = (PFNGLGETSHADERINFOLOGPROC)GetGLProc("glGetShaderInfoLog");
    glDeleteShader_ = (PFNGLDELETESHADERPROC)GetGLProc("glDeleteShader");
    glCreateProgram_ = (PFNGLCREATEPROGRAMPROC)GetGLProc("glCreateProgram");
    glAttachShader_ = (PFNGLATTACHSHADERPROC)GetGLProc("glAttachShader");
    glLinkProgram_ = (PFNGLLINKPROGRAMPROC)GetGLProc("glLinkProgram");
    glGetProgramiv_ = (PFNGLGETPROGRAMIVPROC)GetGLProc("glGetProgramiv");
    glGetProgramInfoLog_ = (PFNGLGETPROGRAMINFOLOGPROC)GetGLProc("glGetProgramInfoLog");
    glUseProgram_ = (PFNGLUSEPROGRAMPROC)GetGLProc("glUseProgram");
    glDeleteProgram_ = (PFNGLDELETEPROGRAMPROC)GetGLProc("glDeleteProgram");
    glGetUniformLocation_ = (PFNGLGETUNIFORMLOCATIONPROC)GetGLProc("glGetUniformLocation");
    glUniformMatrix4fv_ = (PFNGLUNIFORMMATRIX4FVPROC)GetGLProc("glUniformMatrix4fv");
    glUniform1i_ = (PFNGLUNIFORM1IPROC)GetGLProc("glUniform1i");
    glEnableVertexAttribArray_ = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GetGLProc("glEnableVertexAttribArray");
    glVertexAttribPointer_ = (PFNGLVERTEXATTRIBPOINTERPROC)GetGLProc("glVertexAttribPointer");
    glVertexAttribIPointer_ = (PFNGLVERTEXATTRIBIPOINTERPROC)GetGLProc("glVertexAttribIPointer");
    glActiveTexture_ = (PFNGLACTIVETEXTUREPROC)GetGLProc("glActiveTexture");

    glClear_ = (PFNGLCLEARPROC)GetGLProc("glClear");
    glClearColor_ = (PFNGLCLEARCOLORPROC)GetGLProc("glClearColor");
    glViewport_ = (PFNGLVIEWPORTPROC)GetGLProc("glViewport");
    glEnable_ = (PFNGLENABLEPROC)GetGLProc("glEnable");
    glDisable_ = (PFNGLDISABLEPROC)GetGLProc("glDisable");
    glBlendFunc_ = (PFNGLBLENDFUNCPROC)GetGLProc("glBlendFunc");
    glScissor_ = (PFNGLSCISSORPROC)GetGLProc("glScissor");
    glDrawElements_ = (PFNGLDRAWELEMENTSPROC)GetGLProc("glDrawElements");
    glGenTextures_ = (PFNGLGENTEXTURESPROC)GetGLProc("glGenTextures");
    glBindTexture_ = (PFNGLBINDTEXTUREPROC)GetGLProc("glBindTexture");
    glDeleteTextures_ = (PFNGLDELETETEXTURESPROC)GetGLProc("glDeleteTextures");
    glTexParameteri_ = (PFNGLTEXPARAMETERIPROC)GetGLProc("glTexParameteri");
    glTexImage2D_ = (PFNGLTEXIMAGE2DPROC)GetGLProc("glTexImage2D");
    glTexSubImage2D_ = (PFNGLTEXSUBIMAGE2DPROC)GetGLProc("glTexSubImage2D");

    if (!glGenVertexArrays_ || !glBindVertexArray_ || !glGenBuffers_ || !glBindBuffer_ ||
        !glBufferData_ || !glCreateShader_ || !glShaderSource_ || !glCompileShader_ ||
        !glCreateProgram_ || !glAttachShader_ || !glLinkProgram_ || !glUseProgram_ ||
        !glDrawElements_ || !glEnable_ || !glDisable_) {
        return false;
    }

    m_GLLoaded = true;
    return true;
}

uint32_t ForgeBackend_OpenGL::CompileShader(uint32_t type, const char* source) {
    if (!m_GLLoaded || !source) return 0;
    GLuint shader = glCreateShader_(type);
    glShaderSource_(shader, 1, &source, nullptr);
    glCompileShader_(shader);

    GLint success = 0;
    glGetShaderiv_(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog_(shader, sizeof(infoLog), nullptr, infoLog);
        printf("[ForgeUI OpenGL Error] Shader compilation failed:\n%s\n", infoLog);
        glDeleteShader_(shader);
        return 0;
    }
    return shader;
}

uint32_t ForgeBackend_OpenGL::LinkProgram(uint32_t vs, uint32_t fs) {
    if (!m_GLLoaded || !vs || !fs) return 0;
    GLuint program = glCreateProgram_();
    glAttachShader_(program, vs);
    glAttachShader_(program, fs);
    glLinkProgram_(program);

    GLint success = 0;
    glGetProgramiv_(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog_(program, sizeof(infoLog), nullptr, infoLog);
        printf("[ForgeUI OpenGL Error] Program link failed:\n%s\n", infoLog);
        glDeleteProgram_(program);
        return 0;
    }
    return program;
}

bool ForgeBackend_OpenGL::CreatePipelineShaders() {
    if (!LoadGLFunctions()) {
        return true;
    }

    for (size_t i = 0; i < static_cast<size_t>(ForgePipelineType::Count); ++i) {
        ForgePipelineType type = static_cast<ForgePipelineType>(i);
        ForgeShaderSource src = ForgePipelines::GetShaderSource(type);

        GLuint vs = CompileShader(GL_VERTEX_SHADER, src.vertexSource);
        GLuint fs = CompileShader(GL_FRAGMENT_SHADER, src.fragmentSource);

        if (!vs || !fs) {
            if (vs) glDeleteShader_(vs);
            if (fs) glDeleteShader_(fs);
            return false;
        }

        GLuint program = LinkProgram(vs, fs);
        glDeleteShader_(vs);
        glDeleteShader_(fs);

        if (!program) return false;

        m_Programs[i] = program;
        m_LocProj[i] = glGetUniformLocation_(program, "u_Proj");
        m_LocTexture[i] = glGetUniformLocation_(program, "u_Texture");
    }

    glGenVertexArrays_(1, &m_VAO);
    glGenBuffers_(1, &m_VBO);
    glGenBuffers_(1, &m_IBO);

    glBindVertexArray_(m_VAO);
    glBindBuffer_(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

    glEnableVertexAttribArray_(0);
    glVertexAttribPointer_(0, 2, GL_FLOAT, GL_FALSE, sizeof(ForgeDrawVertex), (void*)offsetof(ForgeDrawVertex, pos));

    glEnableVertexAttribArray_(1);
    glVertexAttribPointer_(1, 2, GL_FLOAT, GL_FALSE, sizeof(ForgeDrawVertex), (void*)offsetof(ForgeDrawVertex, uv));

    glEnableVertexAttribArray_(2);
    if (glVertexAttribIPointer_) {
        glVertexAttribIPointer_(2, 1, GL_UNSIGNED_INT, sizeof(ForgeDrawVertex), (void*)offsetof(ForgeDrawVertex, color));
    } else {
        glVertexAttribPointer_(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ForgeDrawVertex), (void*)offsetof(ForgeDrawVertex, color));
    }

    glEnableVertexAttribArray_(3);
    glVertexAttribPointer_(3, 4, GL_FLOAT, GL_FALSE, sizeof(ForgeDrawVertex), (void*)offsetof(ForgeDrawVertex, clipRect));

    glEnableVertexAttribArray_(4);
    glVertexAttribPointer_(4, 4, GL_FLOAT, GL_FALSE, sizeof(ForgeDrawVertex), (void*)offsetof(ForgeDrawVertex, params));

    glBindVertexArray_(0);
    glBindBuffer_(GL_ARRAY_BUFFER, 0);
    glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

void ForgeBackend_OpenGL::DestroyPipelineShaders() {
    if (!m_GLLoaded) return;

    for (size_t i = 0; i < static_cast<size_t>(ForgePipelineType::Count); ++i) {
        if (m_Programs[i]) {
            glDeleteProgram_(m_Programs[i]);
            m_Programs[i] = 0;
        }
    }

    if (m_VAO) { glDeleteVertexArrays_(1, &m_VAO); m_VAO = 0; }
    if (m_VBO) { glDeleteBuffers_(1, &m_VBO); m_VBO = 0; }
    if (m_IBO) { glDeleteBuffers_(1, &m_IBO); m_IBO = 0; }
}

bool ForgeBackend_OpenGL::Initialize() {
    return CreatePipelineShaders();
}

void ForgeBackend_OpenGL::Shutdown() {
    DestroyPipelineShaders();
}

void ForgeBackend_OpenGL::BeginFrame(uint32_t displayWidth, uint32_t displayHeight) {
    m_DisplayWidth = displayWidth;
    m_DisplayHeight = displayHeight;
}

void ForgeBackend_OpenGL::RenderDrawList(const ForgeDrawList* drawList) {
    if (!drawList || drawList->GetCommandCount() == 0 || !m_GLLoaded) {
        return;
    }

    uint32_t vertexCount = drawList->GetVertexCount();
    uint32_t indexCount = drawList->GetIndexCount();
    if (vertexCount == 0 || indexCount == 0) return;

    glBindVertexArray_(m_VAO);
    glBindBuffer_(GL_ARRAY_BUFFER, m_VBO);
    size_t vboBytes = vertexCount * sizeof(ForgeDrawVertex);
    if (vboBytes > m_VBOSize) {
        m_VBOSize = (uint32_t)vboBytes + 65536;
        glBufferData_(GL_ARRAY_BUFFER, m_VBOSize, nullptr, GL_DYNAMIC_DRAW);
    }
    glBufferSubData_(GL_ARRAY_BUFFER, 0, vboBytes, drawList->GetVertices());

    glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    size_t iboBytes = indexCount * sizeof(uint32_t);
    if (iboBytes > m_IBOSize) {
        m_IBOSize = (uint32_t)iboBytes + 65536;
        glBufferData_(GL_ELEMENT_ARRAY_BUFFER, m_IBOSize, nullptr, GL_DYNAMIC_DRAW);
    }
    glBufferSubData_(GL_ELEMENT_ARRAY_BUFFER, 0, iboBytes, drawList->GetIndices());

    float L = 0.0f;
    float R = static_cast<float>(m_DisplayWidth);
    float T = 0.0f;
    float B = static_cast<float>(m_DisplayHeight);
    float orthoProj[4][4] = {
        { 2.0f / (R - L),    0.0f,              0.0f,  0.0f },
        { 0.0f,              2.0f / (T - B),    0.0f,  0.0f },
        { 0.0f,              0.0f,             -1.0f,  0.0f },
        { (R + L) / (L - R), (T + B) / (B - T), 0.0f,  1.0f },
    };

    if (glViewport_) {
        glViewport_(0, 0, static_cast<GLsizei>(m_DisplayWidth), static_cast<GLsizei>(m_DisplayHeight));
    }

    if (glClearColor_ && glClear_) {
        glClearColor_(0.05f, 0.05f, 0.07f, 1.0f);
        glClear_(GL_COLOR_BUFFER_BIT);
    }

    glEnable_(GL_BLEND);
    glBlendFunc_(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable_(GL_CULL_FACE);
    glDisable_(GL_DEPTH_TEST);
    glEnable_(GL_SCISSOR_TEST);

    uint32_t currentProgram = 0;
    uint64_t currentTexture = (uint64_t)-1;

    for (uint32_t i = 0; i < drawList->GetCommandCount(); ++i) {
        const ForgeDrawCmd& cmd = drawList->GetCommands()[i];
        if (cmd.elemCount == 0) continue;

        uint32_t pipeIdx = static_cast<uint32_t>(cmd.pipeline);
        if (pipeIdx >= static_cast<uint32_t>(ForgePipelineType::Count)) continue;

        uint32_t prog = m_Programs[pipeIdx];
        if (prog != currentProgram) {
            glUseProgram_(prog);
            currentProgram = prog;
            if (m_LocProj[pipeIdx] != -1) {
                glUniformMatrix4fv_(m_LocProj[pipeIdx], 1, GL_FALSE, &orthoProj[0][0]);
            }
            if (m_LocTexture[pipeIdx] != -1) {
                glUniform1i_(m_LocTexture[pipeIdx], 0);
            }
        }

        if (cmd.textureHandle != currentTexture) {
            if (glActiveTexture_) glActiveTexture_(GL_TEXTURE0);
            if (glBindTexture_) glBindTexture_(GL_TEXTURE_2D, static_cast<GLuint>(cmd.textureHandle));
            currentTexture = cmd.textureHandle;
        }

        GLint sx = static_cast<GLint>(cmd.clipRect.x);
        GLint sy = static_cast<GLint>(static_cast<float>(m_DisplayHeight) - cmd.clipRect.w);
        GLsizei sw = static_cast<GLsizei>(cmd.clipRect.z - cmd.clipRect.x);
        GLsizei sh = static_cast<GLsizei>(cmd.clipRect.w - cmd.clipRect.y);
        if (sw > 0 && sh > 0) {
            glScissor_(sx, sy, sw, sh);
        }

        glDrawElements_(GL_TRIANGLES, static_cast<GLsizei>(cmd.elemCount), GL_UNSIGNED_INT, (void*)(static_cast<uintptr_t>(cmd.elemOffset) * sizeof(uint32_t)));
    }

    glDisable_(GL_SCISSOR_TEST);
    glBindVertexArray_(0);
    glUseProgram_(0);
}

void ForgeBackend_OpenGL::EndFrame() {}

uint64_t ForgeBackend_OpenGL::CreateTexture(uint32_t width, uint32_t height, const uint8_t* pixels, bool isRGBA) {
    if (!LoadGLFunctions() || !glGenTextures_ || !glBindTexture_ || !glTexParameteri_ || !glTexImage2D_) return 0;

    GLuint tex = 0;
    glGenTextures_(1, &tex);
    glBindTexture_(GL_TEXTURE_2D, tex);

    glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri_(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLenum format = isRGBA ? GL_RGBA : GL_RGB;
    glTexImage2D_(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
    glBindTexture_(GL_TEXTURE_2D, 0);

    return static_cast<uint64_t>(tex);
}

void ForgeBackend_OpenGL::UpdateTexture(uint64_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t* pixels) {
    if (!handle || !pixels || !glBindTexture_ || !glTexSubImage2D_) return;
    glBindTexture_(GL_TEXTURE_2D, static_cast<GLuint>(handle));
    glTexSubImage2D_(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture_(GL_TEXTURE_2D, 0);
}

void ForgeBackend_OpenGL::DestroyTexture(uint64_t handle) {
    if (handle && glDeleteTextures_) {
        GLuint tex = static_cast<GLuint>(handle);
        glDeleteTextures_(1, &tex);
    }
}

}
