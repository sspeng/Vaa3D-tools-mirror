#include "QGLRefSys.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

using namespace teramanager;

QGLRefSys::QGLRefSys(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    /**/itm::debug(itm::LEV1, 0, __itm__current__function__);

    xRot = 0;
    yRot = 0;
    zRot = 0;

    setAttribute(Qt::WA_TranslucentBackground,true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );
    setWindowOpacity(0.5);
    bool t=testAttribute ( Qt::WA_TranslucentBackground);
    qDebug()<<"transparency"<<t;
}

QGLRefSys::~QGLRefSys()
{
    /**/itm::debug(itm::LEV1, 0, __itm__current__function__);
}

QSize QGLRefSys::minimumSizeHint() const
{
    return QSize(20, 20);
}

QSize QGLRefSys::sizeHint() const
{
    return QSize(400, 400);
}

//static void qNormalizeAngle(int &angle)
//{
//    while (angle < 0)
//        angle += 360 * 16;
//    while (angle > 360 * 16)
//        angle -= 360 * 16;
//}

void QGLRefSys::setXRotation(int angle)
{
    //qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void QGLRefSys::setYRotation(int angle)
{
    //qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void QGLRefSys::setZRotation(int angle)
{
    //qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void QGLRefSys::initializeGL()
{
    qglClearColor(QWidget::palette().color(QWidget::backgroundRole())); // background color
    glEnable(GL_DEPTH_TEST);       // activate the depth buffer
    glShadeModel(GL_SMOOTH);       // select shade model (can be either smooth or flat)
    glEnable(GL_MULTISAMPLE);      // activate multisample
}


/*
 *  Sets up the OpenGL viewport, projection, etc. Gets called whenever the widget has been resized
 *  (and also when it is shown for the first time because all newly created widgets get a resize event automatically).
 */
void QGLRefSys::resizeGL (int width, int height)
{
    // define a square viewport based on the length of the smallest side of the widget to ensure that
    // the scene is not distorted if the widget has sides of unequal length
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 30.0);
    glMatrixMode(GL_MODELVIEW);
}

/*
 * Renders the OpenGL scene. Gets called whenever the widget needs to be updated.
 */
void QGLRefSys::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //delete color and depth buffer
    glLoadIdentity();                                   //resets projection and modelview matrices
    glTranslatef(0.0, 0.0, -15.0);                      //move along z-axis
    glRotatef(xRot, 1.0, 0.0, 0.0);                     //rotate around x-axis
    glRotatef(-180+yRot, 0.0, 1.0, 0.0);                 //rotate around y-axis
    glRotatef(-180+zRot, 0.0, 0.0, 1.0);                 //rotate around z-axis

    // CUBE FACES
    if(this->isEnabled())
        glColor3f(1,1,1);
    else
        glColor3f(0.75,0.75,0.75);
    glBegin(GL_QUADS);
        //---------front-----------//
        glVertex3f(1.0,1.0,1.0);
        glVertex3f(-1.0,1.0,1.0);
        glVertex3f(-1.0,-1.0,1.0);
        glVertex3f(1.0,-1.0,1.0);
        //---------back----------//
        glVertex3f(1.0,1.0,-1.0);
        glVertex3f(-1.0,1.0,-1.0);
        glVertex3f(-1.0,-1.0,-1.0);
        glVertex3f(1.0,-1.0,-1.0);
        //---------top-----------//
        glVertex3f(-1.0,1.0,1.0);
        glVertex3f(1.0,1.0,1.0);
        glVertex3f(1.0,1.0,-1.0);
        glVertex3f(-1.0,1.0,-1.0);
        //-------bottom----------//
        glVertex3f(1.0,-1.0,1.0);
        glVertex3f(1.0,-1.0,-1.0);
        glVertex3f(-1.0,-1.0,-1.0);
        glVertex3f(-1.0,-1.0,1.0);
        //--------right----------//
        glVertex3f(1.0,1.0,1.0);
        glVertex3f(1.0,-1.0,1.0);
        glVertex3f(1.0,-1.0,-1.0);
        glVertex3f(1.0,1.0,-1.0);
        //---------left----------//
        glVertex3f(-1.0,1.0,1.0);
        glVertex3f(-1.0,-1.0,1.0);
        glVertex3f(-1.0,-1.0,-1.0);
        glVertex3f(-1.0,1.0,-1.0);
    glEnd();

    // CUBE CONTOUR
    if(this->isEnabled())
        glColor3f(0.0,0.0,0.0);
    else
        glColor3f(0.5,0.5,0.5);
    glLineWidth(2.0);
    glBegin(GL_LINES);
        //-------top lines---------//
        glVertex3f(-1.0, 1.0, -1.0);
        glVertex3f( 1.0, 1.0, -1.0);
        glVertex3f( 1.0, 1.0, -1.0);
        glVertex3f( 1.0, 1.0,  1.0);
        glVertex3f( 1.0, 1.0,  1.0);
        glVertex3f(-1.0, 1.0,  1.0);
        glVertex3f(-1.0, 1.0,  1.0);
        glVertex3f(-1.0, 1.0, -1.0);
        //------bottom lines-------//
        glVertex3f(-1.0, -1.0, -1.0);
        glVertex3f( 1.0, -1.0, -1.0);
        glVertex3f( 1.0, -1.0, -1.0);
        glVertex3f( 1.0, -1.0,  1.0);
        glVertex3f( 1.0, -1.0,  1.0);
        glVertex3f(-1.0, -1.0,  1.0);
        glVertex3f(-1.0, -1.0,  1.0);
        glVertex3f(-1.0, -1.0, -1.0);
        //--------side lines-------//
        glVertex3f(-1.0, 1.0, -1.0);
        glVertex3f(-1.0, -1.0, -1.0);
        glVertex3f( 1.0, 1.0, -1.0);
        glVertex3f( 1.0, -1.0, -1.0);
        glVertex3f( 1.0, 1.0,  1.0);
        glVertex3f( 1.0, -1.0,  1.0);
        glVertex3f(-1.0, 1.0,  1.0);
        glVertex3f(-1.0, -1.0,  1.0);
    glEnd(); // GL_LINES

    // ARROWS
    double radius=0.2;
    double height=1.4;
    float headRadius = 2.3*radius;
    float headHeight = 0.7;
    const double PI = 3.1415926535897;
    double resolution = PI/200;
    double shift = 1.0;
    if(isEnabled())
        glColor3f(0.0,200.0/255,0.0);
    else
        glColor3f(0.6,0.6,0.6);
    //----------------------------------Y-head----------------------------------//
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, shift+height+headHeight, 0);  /* center */
        for (double i = 0; i <= 2 * PI; i += resolution)
            glVertex3f(headRadius * cos(i), shift+height, headRadius * sin(i));
    glEnd();
    //----------------------------------Y-base----------------------------------//
//    glBegin(GL_TRIANGLE_FAN);
//        glVertex3f(0, shift, 0);  /* center */
//        for (double i = 2 * PI; i >= 0; i -= resolution)
//            glVertex3f(radius * cos(i), shift, radius * sin(i));
//        glVertex3f(radius, shift+height, 0);
//    glEnd();
    //-------------------------------Y-middle tube------------------------------//
    glBegin(GL_QUAD_STRIP);
        for (double i = 0; i <= 2 * PI; i += resolution)
        {
            glVertex3f(radius * cos(i), shift, radius * sin(i));
            glVertex3f(radius * cos(i), shift+height, radius * sin(i));
        }
        glVertex3f(radius, shift, 0);
        glVertex3f(radius, shift+height, 0);
    glEnd();


    if(isEnabled())
        glColor3f(1.0,0.0,0.0);
    else
        glColor3f(0.6,0.6,0.6);
    //----------------------------------X-head----------------------------------//
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(shift+height+headHeight, 0, 0);  /* center */
        for (double i = 0; i <= 2 * PI; i += resolution)
            glVertex3f(shift+height, headRadius * cos(i), headRadius * sin(i));
    glEnd();
    //----------------------------------X-base----------------------------------//
//    glBegin(GL_TRIANGLE_FAN);
//        glVertex3f(shift, 0, 0);  /* center */
//        for (double i = 2 * PI; i >= 0; i -= resolution)
//            glVertex3f(shift, radius * cos(i), radius * sin(i));
//        glVertex3f(shift+height, radius, 0);
//    glEnd();
    //-------------------------------X-middle tube------------------------------//
    glBegin(GL_QUAD_STRIP);
        for (double i = 0; i <= 2 * PI; i += resolution)
        {
            glVertex3f(shift, radius * cos(i), radius * sin(i));
            glVertex3f(shift+height, radius * cos(i), radius * sin(i));
        }
        /* close the loop back to zero degrees */
        glVertex3f(shift, radius, 0);
        glVertex3f(shift+height, radius, 0);
    glEnd();

    if(isEnabled())
        glColor3f(0.0,0.0,1.0);
    else
        glColor3f(0.6,0.6,0.6);
    //----------------------------------Z-head----------------------------------//
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, shift+height+headHeight);  /* center */
        for (double i = 0; i <= 2 * PI; i += resolution)
            glVertex3f(headRadius * cos(i), headRadius * sin(i), shift+height);
    glEnd();
    //----------------------------------Z-base----------------------------------//
//    glBegin(GL_TRIANGLE_FAN);
//        glVertex3f(0, shift, 0);  /* center */
//        for (double i = 2 * PI; i >= 0; i -= resolution)
//            glVertex3f(radius * cos(i), shift, radius * sin(i));
//        glVertex3f(radius, shift+height, 0);
//    glEnd();
    //-------------------------------Z-middle tube------------------------------//
    glBegin(GL_QUAD_STRIP);
        for (double i = 0; i <= 2 * PI; i += resolution)
        {
            glVertex3f(radius * cos(i), radius * sin(i), shift);
            glVertex3f(radius * cos(i), radius * sin(i), shift+height);
        }
        glVertex3f(radius, 0, shift);
        glVertex3f(radius, 0, shift+height);
    glEnd();

}

void QGLRefSys::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void QGLRefSys::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }
    lastPos = event->pos();
}
