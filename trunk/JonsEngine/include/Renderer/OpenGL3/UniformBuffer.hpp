#pragma once

#include "include/Renderer/OpenGL3/ShaderProgram.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/EngineDefs.h"
#include "include/Core/Logging/Logger.h"

#include "GL/glew.h"
#include <string>

namespace JonsEngine
{
    /* Counter of uniform binding index */
    static GLuint gNextUniformBindingIndex = 0;

    /* UniformBuffer definition */
    template <typename ContentType>
    class UniformBuffer
    {
    public:
        UniformBuffer(const std::string& name, Logger& logger, const ShaderProgram& program, size_t defaultSize = 5) : mName(name), mLogger(logger), mBindingIndex(++gNextUniformBindingIndex), mBlockOffset(0), mMaxSize(defaultSize), mCurrentSize(0)
        {
            glGenBuffers(1, &mBuffer);
            CHECK_GL_ERROR(mLogger);
            glGenBuffers(1, &mCopyBuffer);
            CHECK_GL_ERROR(mLogger);

            GLint uniformBufferAlignSize;
            glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignSize);
            CHECK_GL_ERROR(mLogger);

            mBlockOffset = sizeof(ContentType);
			mBlockOffset += uniformBufferAlignSize - (mBlockOffset % uniformBufferAlignSize);
            Resize(defaultSize);

            GLuint blockIndex = glGetUniformBlockIndex(program.GetHandle(), mName.c_str());
            CHECK_GL_ERROR(mLogger);
            glUniformBlockBinding(program.GetHandle(), blockIndex, mBindingIndex);
            CHECK_GL_ERROR(mLogger);
        }

        ~UniformBuffer()
        {
            glDeleteBuffers(1, &mBuffer);
            CHECK_GL_ERROR(mLogger);
            glDeleteBuffers(1, &mCopyBuffer);
            CHECK_GL_ERROR(mLogger);
        }


        void ClearData()
        {
            glBindBuffer(GL_UNIFORM_BUFFER, mBuffer);
            CHECK_GL_ERROR(mLogger);
            glBufferData(GL_UNIFORM_BUFFER, mMaxSize * mBlockOffset, 0, GL_DYNAMIC_DRAW);
            CHECK_GL_ERROR(mLogger);
            glBindBufferRange(GL_UNIFORM_BUFFER, mBindingIndex, mBuffer, 0, sizeof(ContentType));
            CHECK_GL_ERROR(mLogger);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            CHECK_GL_ERROR(mLogger);

            mCurrentSize = 0;
        }

        void AddData(const ContentType& data)
        {

            if (mCurrentSize >= mMaxSize)
                Resize(mMaxSize * 2);

            glBindBuffer(GL_UNIFORM_BUFFER, mBuffer);
            CHECK_GL_ERROR(mLogger);
            glBufferSubData(GL_UNIFORM_BUFFER, mCurrentSize * mBlockOffset, sizeof(data), &data);
            CHECK_GL_ERROR(mLogger);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            CHECK_GL_ERROR(mLogger);

            mCurrentSize++;
        }

        void SetData(const ContentType& data)
        {
            ClearData();
            AddData(data);
            SetActiveIndex(0);
        }


        size_t Size() const     { return mMaxSize; }

        void Resize(size_t newSize)
        {
            glBindBuffer(GL_COPY_READ_BUFFER, mBuffer);
            CHECK_GL_ERROR(mLogger);
            glBindBuffer(GL_COPY_WRITE_BUFFER, mCopyBuffer);
            CHECK_GL_ERROR(mLogger);

            glBufferData(GL_COPY_WRITE_BUFFER, mCurrentSize * mBlockOffset, 0, GL_DYNAMIC_COPY);
            CHECK_GL_ERROR(mLogger);
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, mCurrentSize * mBlockOffset);
            CHECK_GL_ERROR(mLogger);

            glBufferData(GL_COPY_READ_BUFFER, newSize * mBlockOffset, 0, GL_DYNAMIC_DRAW);
            CHECK_GL_ERROR(mLogger);
            glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_COPY_READ_BUFFER, 0, 0, (mCurrentSize <= newSize ? mCurrentSize : newSize) * mBlockOffset);
            CHECK_GL_ERROR(mLogger);

            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            CHECK_GL_ERROR(mLogger);
            glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
            CHECK_GL_ERROR(mLogger);

            mMaxSize = newSize;
        }


        void SetActiveIndex(size_t index)
        {
            if (index <= mCurrentSize) 
            {
                glBindBuffer(GL_UNIFORM_BUFFER, mBuffer);
                CHECK_GL_ERROR(mLogger);
                glBindBufferRange(GL_UNIFORM_BUFFER, mBindingIndex, mBuffer, index * mBlockOffset, sizeof(ContentType));
                CHECK_GL_ERROR(mLogger);
                glBindBuffer(GL_UNIFORM_BUFFER, 0);
                CHECK_GL_ERROR(mLogger);
            }
        }


    private:
        const std::string mName;
        Logger& mLogger;

        GLuint mBindingIndex;
        GLuint mBuffer;
        GLuint mCopyBuffer;         // used as an intermediate buffer for copying data when resizing
        GLuint mBlockOffset;
        size_t mMaxSize;
        size_t mCurrentSize;
    };
}