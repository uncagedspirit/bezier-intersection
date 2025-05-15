#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector>
#include <QPair>
#include <QMouseEvent>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    GLWidget(QWidget* parent = nullptr);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QVector<QPair<float, float>> controlPoints1;
    QVector<QPair<float, float>> bezierPoints1;
    QVector<QPair<float, float>> controlPoints2;
    QVector<QPair<float, float>> bezierPoints2;
    QVector<QPair<float, float>> intersectionPoints; 
    int currentCurve = 1; // 1 or 2
    int maxPoints = 4;    // Cubic Bezier
    float pointRadius = 0.08f;

    void generateBezierCurve(QVector<QPair<float, float>>& ctrl, QVector<QPair<float, float>>& curve);
    void findIntersections();
    float screenToGLX(float x);
    float screenToGLY(float y);
};