#include "glvideowidget.h"
#include <algorithm>

namespace {
QVector<GLfloat> GetVertexData(float ratio)
{
    const auto x = std::min(1.0f, ratio);
    const auto y = std::min(1.0f, 1 / ratio);
    return {-x, y, 0.0, 0.0, x, y, 1.0, 0.0, x, -y, 1.0, 1.0, -x, -y, 0.0, 1.0};
}
} // namespace

constexpr const char *vertexSource = "#version 330\n"
                                     "layout(location = 0) in vec2 position;\n"
                                     "layout(location = 1) in vec2 texCoord;\n"
                                     "out vec4 texc;\n"
                                     "void main( void )\n"
                                     "{\n"
                                     " gl_Position = vec4(position, 0.0, 1.0);\n"
                                     " texc = vec4(texCoord, 0.0, 1.0);\n"
                                     "}\n";

constexpr const char *fragmentSource = "#version 330\n"
                                       "uniform sampler2D tex;\n"
                                       "in vec4 texc;\n"
                                       "out vec4 fragColor;\n"
                                       "void main( void )\n"
                                       "{\n"
                                       " fragColor = texture(tex, texc.st);\n"
                                       "}\n";

GLVideoWidget::GLVideoWidget(QWidget *parent) : QOpenGLWidget(parent), texture(0) {}

void GLVideoWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // shader
    shaderProgram = new QOpenGLShaderProgram;
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource);
    shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSource);

    // bind location for the vertex shader
    shaderProgram->bindAttributeLocation("position", 0);
    shaderProgram->link();
    shaderProgram->bind();

    // Vertex array
    vaoQuad.create();
    vaoQuad.bind();

    // Vertex buffer
    vboQuad.create();
    vboQuad.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vboQuad.bind();
    const auto vertexData = GetVertexData(1.0f);
    vboQuad.allocate(vertexData.constData(), vertexData.count() * sizeof(GLfloat));

    // Connect inputs to the shader
    shaderProgram->enableAttributeArray(0);
    shaderProgram->enableAttributeArray(1);
    shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 4 * sizeof(GLfloat));
    shaderProgram->setAttributeBuffer(1, GL_FLOAT, 2 * sizeof(GLfloat), 2, 4 * sizeof(GLfloat));

    vaoQuad.release();
    vboQuad.release();
    shaderProgram->release();
}

void GLVideoWidget::resizeGL(int w, int h)
{
    if (lastImageHeight == 0 || lastImageWidth == 0 || w == 0 || h == 0 || vboQuad.size() == 0)
        return;
    // Force update vertex data when we resize the widget
    vboQuad.bind();
    auto ptr = vboQuad.map(QOpenGLBuffer::WriteOnly);
    const auto ratio = static_cast<float>(lastImageWidth) / lastImageHeight * height() / width();

    const auto vertexData = GetVertexData(ratio);
    memcpy(ptr, vertexData.constData(), vertexData.count() * sizeof(GLfloat));
    vboQuad.unmap();
    vboQuad.release();
}

void GLVideoWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (texture == nullptr)
        return;
    if (!shaderProgram->bind())
        return;
    vaoQuad.bind();
    if (texture->isCreated())
        texture->bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    vaoQuad.release();
    shaderProgram->release();
}

void GLVideoWidget::setTexture(const otc_video_frame *frame)
{
    if ((otc_video_frame_get_width(frame) != lastImageWidth
         || otc_video_frame_get_height(frame) != lastImageHeight)) {
        vboQuad.bind();
        auto ptr = vboQuad.map(QOpenGLBuffer::WriteOnly);
        const auto ratio = static_cast<float>(otc_video_frame_get_width(frame))
                           / otc_video_frame_get_height(frame) * (float) height() / (float) width();

        const auto vertexData = GetVertexData(ratio);
        if (ptr != nullptr)
            memcpy(ptr, vertexData.constData(), vertexData.count() * sizeof(GLfloat));
        vboQuad.unmap();
        vboQuad.release();

        resizeTexture(frame);
        lastImageWidth = otc_video_frame_get_width(frame);
        lastImageHeight = otc_video_frame_get_height(frame);
    }
    setTextureData(frame);
    update();
}

void GLVideoWidget::resizeTexture(const otc_video_frame *image)
{
    if (image == nullptr) {
        return;
    }
    if (texture != nullptr) {
        delete texture;
    }
    makeCurrent();
    texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    QOpenGLContext *context = QOpenGLContext::currentContext();
    if (!context) {
        qWarning("QOpenGLTexture::setData() requires a valid current context");
        return;
    }
    if (context->isOpenGLES() && context->format().majorVersion() < 3)
        texture->setFormat(QOpenGLTexture::RGBAFormat);
    else
        texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
    texture->setSize(otc_video_frame_get_width(image), otc_video_frame_get_height(image));
    texture->setMipLevels(texture->maximumMipLevels());
    texture->allocateStorage(QOpenGLTexture::BGRA, QOpenGLTexture::UInt8);
    doneCurrent();
}

void GLVideoWidget::setTextureData(const otc_video_frame *image)
{
    QOpenGLPixelTransferOptions uploadOptions;
    uploadOptions.setAlignment(1);
    texture->setData(0,
                     QOpenGLTexture::BGRA,
                     QOpenGLTexture::UInt8,
                     otc_video_frame_get_buffer(image),
                     &uploadOptions);
}

void GLVideoWidget::frame(std::shared_ptr<const otc_video_frame> frame)
{
    if (frame != nullptr) {
        setTexture(frame.get());
    }
}
