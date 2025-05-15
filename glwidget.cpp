#include "glwidget.h"
#include <cmath>

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent) {}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2, 2, -2, 2, 1, 10);
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0, 0, -5);

    // Draw control points and polygon for curve 1
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    glColor3f(1, 0, 0);
    for (auto& pt : controlPoints1) glVertex2f(pt.first, pt.second);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(0.7f, 0.7f, 0.7f);
    for (auto& pt : controlPoints1) glVertex2f(pt.first, pt.second);
    glEnd();

    // Draw Bezier curve 1 (green)
    glBegin(GL_LINE_STRIP);
    glColor3f(0, 1, 0);
    for (auto& pt : bezierPoints1) glVertex2f(pt.first, pt.second);
    glEnd();

    // Draw control points and polygon for curve 2
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    glColor3f(0, 0, 1);
    for (auto& pt : controlPoints2) glVertex2f(pt.first, pt.second);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(0.7f, 0.7f, 0.7f);
    for (auto& pt : controlPoints2) glVertex2f(pt.first, pt.second);
    glEnd();

    // Draw Bezier curve 2 (yellow)
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 0);
    for (auto& pt : bezierPoints2) glVertex2f(pt.first, pt.second);
    glEnd();

    glPointSize(12.0f);
    glBegin(GL_POINTS);
    glColor3f(1, 1, 1);
    for (auto& pt : intersectionPoints)
        glVertex2f(pt.first, pt.second);
    glEnd();
}

void GLWidget::mousePressEvent(QMouseEvent* event) {
    float x = screenToGLX(event->position().x());
    float y = screenToGLY(event->position().y());
    QPair<float, float> clickPos(x, y);

    if (currentCurve == 1 && controlPoints1.size() < maxPoints) {
        controlPoints1.append(clickPos);
        if (controlPoints1.size() == maxPoints) {
            generateBezierCurve(controlPoints1, bezierPoints1);
            currentCurve = 2;
        }
    } else if (currentCurve == 2 && controlPoints2.size() < maxPoints) {
        controlPoints2.append(clickPos);
        if (controlPoints2.size() == maxPoints) {
            generateBezierCurve(controlPoints2, bezierPoints2);
            findIntersections(); // Add this line
            currentCurve = 1;
        }
    }
    update();
}

void GLWidget::generateBezierCurve(QVector<QPair<float, float>>& ctrl, QVector<QPair<float, float>>& curve) {
    curve.clear();
    if (ctrl.size() < 2) return;
    auto deCasteljau = [](double t, const QVector<QPair<float, float>>& pts) {
        QVector<QPair<float, float>> temp = pts;
        int n = temp.size() - 1;
        for (int r = 1; r <= n; ++r)
            for (int i = 0; i <= n - r; ++i) {
                temp[i].first = (1 - t) * temp[i].first + t * temp[i + 1].first;
                temp[i].second = (1 - t) * temp[i].second + t * temp[i + 1].second;
            }
        return temp[0];
    };
    for (int i = 0; i <= 100; ++i) {
        float t = float(i) / 100.0f;
        curve.push_back(deCasteljau(t, ctrl));
    }
}

bool segmentIntersect(const QPair<float, float>& p1, const QPair<float, float>& p2,
                      const QPair<float, float>& q1, const QPair<float, float>& q2,
                      QPair<float, float>& intersection) {
    float x1 = p1.first, y1 = p1.second;
    float x2 = p2.first, y2 = p2.second;
    float x3 = q1.first, y3 = q1.second;
    float x4 = q2.first, y4 = q2.second;

    float denom = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
    if (fabs(denom) < 1e-6) return false; // Parallel

    float px = ((x1*y2 - y1*x2)*(x3-x4) - (x1-x2)*(x3*y4 - y3*x4)) / denom;
    float py = ((x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4 - y3*x4)) / denom;

    // Check if intersection is within both segments
    auto between = [](float a, float b, float c) { return (a >= std::min(b,c)-1e-6) && (a <= std::max(b,c)+1e-6); };
    if (between(px, x1, x2) && between(px, x3, x4) &&
        between(py, y1, y2) && between(py, y3, y4)) {
        intersection = {px, py};
        return true;
    }
    return false;
}

void GLWidget::findIntersections() {
    intersectionPoints.clear();
    for (int i = 0; i < bezierPoints1.size() - 1; ++i) {
        for (int j = 0; j < bezierPoints2.size() - 1; ++j) {
            QPair<float, float> inter;
            if (segmentIntersect(bezierPoints1[i], bezierPoints1[i+1],
                                 bezierPoints2[j], bezierPoints2[j+1], inter)) {
                intersectionPoints.append(inter);
            }
        }
    }
}

float GLWidget::screenToGLX(float x) {
    float ndcX = (2.0f * x) / width() - 1.0f;
    return ndcX * 2.0f;
}

float GLWidget::screenToGLY(float y) {
    float ndcY = 1.0f - (2.0f * y) / height();
    return ndcY * 2.0f;
}