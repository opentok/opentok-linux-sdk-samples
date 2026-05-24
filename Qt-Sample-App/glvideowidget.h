#ifndef GLVIDEOWIDGET_H
#define GLVIDEOWIDGET_H

#include "opentok.h"
#include <iostream>

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLPixelTransferOptions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

class GLVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GLVideoWidget(QWidget *parent);
    ~GLVideoWidget()
    {
        makeCurrent();

        delete shaderProgram;
        delete texture;

        vboQuad.destroy();
        vaoQuad.destroy();

        doneCurrent();
    }
    void setTexture(const otc_video_frame *frame);

protected:
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void initializeGL() override;

private:
    QOpenGLVertexArrayObject vaoQuad;
    QOpenGLBuffer vboQuad;
    QOpenGLShaderProgram *shaderProgram;

    QOpenGLTexture *texture;

    void resizeTexture(const otc_video_frame *image);
    void setTextureData(const otc_video_frame *image);

    int lastImageWidth = 0;
    int lastImageHeight = 0;

public slots:
    void frame(std::shared_ptr<const otc_video_frame> frame);
};

#endif // GLVIDEOWIDGET_H
