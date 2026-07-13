#pragma once

/**
 * @file    ShaderHelper.h
 * @brief   Loads, compiles, and links vertex and fragment shaders for all platforms.
 *          Supports both inline source strings and external shader files/assets.
 */

#include "Platform.h"

#include <fstream>
#include <sstream>
#include <string>

#ifdef PLATFORM_ANDROID
#include <android/asset_manager.h>
#endif

class ShaderHelper {
public:
    // --- Core compile/link, shared by both loading methods below ---
    static GLuint compileShader(GLenum type, const char* source)
    {
        GLuint shader = glCreateShader(type);   // Create vertex shader object
        glShaderSource(shader, 1, &source, nullptr);    // Load source code
        glCompileShader(shader);    // Compile shader

        // Verify compilation
        GLint compiled = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        // If compiled failed?
        if (compiled == GL_FALSE) {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::string log(static_cast<size_t>(length), '\0');

            if (length > 0){
                GLsizei written = 0;
                glGetShaderInfoLog(shader, length, &written, log.data());
                log.resize(static_cast<size_t>(written));
                LOGE("Shader compilation failed:\n%s", log.c_str());
            }
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    // --- Core compile/link, shared by both loading methods below ---
    static GLuint linkProgram(GLuint vert, GLuint frag)
    {
        GLuint program = glCreateProgram();
        glAttachShader(program, vert);
        glAttachShader(program, frag);
        glLinkProgram(program);

        GLint linked = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (linked == GL_FALSE) {
            GLint length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            std::string log(static_cast<size_t>(length), '\0');

            if (length > 0) {
                GLsizei written = 0;
                glGetProgramInfoLog(program, length, &written, log.data());
                log.resize(static_cast<size_t>(written));
                LOGE("Shader program link failed:\n%s", log.c_str());

            }
            glDeleteProgram(program);
            return 0;
        }
        return program;
    }

    // Method 1: build directly from inline shader source strings.
    // Retained for models that prefer embedded shader source.
    static GLuint buildProgram(const char* vertSource, const char* fragSource)
    {
        GLuint vert = compileShader(GL_VERTEX_SHADER, vertSource);
        if (!vert) return 0;

        GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSource);
        if (!frag) {
            glDeleteShader(vert);
            return 0;
        }

        GLuint program = linkProgram(vert, frag);
        glDeleteShader(vert);
        glDeleteShader(frag);
        return program;
    }

#ifdef PLATFORM_ANDROID
    static std::string loadAsset(AAssetManager* mgr, const char* path)
    {
        AAsset* asset = mgr ? AAssetManager_open(mgr, path, AASSET_MODE_BUFFER) : nullptr;

        if (!asset) {
            LOGE("Shader asset not found: %s", path);
            return {};
        }

        const off_t length = AAsset_getLength(asset);
        std::string source(static_cast<size_t>(length), '\0');
        const int bytesRead = AAsset_read(asset, source.data(), source.size());
        AAsset_close(asset);

        if (bytesRead != length) {
            LOGE("Failed to read shader asset: %s", path);
            return {};
        }

        return source;
    }

    static GLuint buildProgramFromAssets(AAssetManager* mgr, const char* vertPath, const char* fragPath)
    {
        const std::string vertSource = loadAsset(mgr, vertPath);
        const std::string fragSource = loadAsset(mgr, fragPath);
        if (vertSource.empty() || fragSource.empty()) return 0;

        // Load asset -> buildProgram -> compileShaders and link them
        return buildProgram(vertSource.c_str(), fragSource.c_str());
    }
#else
    static std::string loadFile(const char* filename)
    {
        // Check different paths
        const char* prefixes[] = {
            "assets/shader/",
            "",
            "shader/",
            "assets/"
        };


        // Loop through prefixes and test if valid
        for (const char* prefix : prefixes) {
            const std::string path = std::string(prefix) + filename;
            std::ifstream file(path);
            if (!file) continue;
            std::ostringstream source;
            source << file.rdbuf();
            return source.str();
        }

        LOGE("Shader file not found: %s", filename);
        return {};
    }

    static GLuint buildProgramFromFile(const char* vertFile, const char* fragFile)
    {
        const std::string vertSource = loadFile(vertFile);
        const std::string fragSource = loadFile(fragFile);
        if (vertSource.empty() || fragSource.empty()) return 0;
        return buildProgram(vertSource.c_str(), fragSource.c_str());
    }
#endif
};
