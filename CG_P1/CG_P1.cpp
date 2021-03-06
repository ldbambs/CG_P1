#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>
using namespace std;
int pntX1, pntY1, r;
struct Point
{
    GLint x;
    GLint y;
};

struct Bounds
{
    int x0;
    int x1;
    int y;
};

struct ScanPoint
{

    int x;
    int y;
};

struct Color
{
    GLfloat r;
    GLfloat g;
    GLfloat b;
};

Point startPoint;
Color oldColor;

vector<ScanPoint> upAgenda;
vector<ScanPoint> downAgenda;

Color getPixelColor(GLint x, GLint y)
{
    Color color;
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &color);
    return color;
}

void setPixelColor(GLint x, GLint y, Color color)
{
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
    glFlush();
}

void plot(int x, int y)
{
    glBegin(GL_POINTS);
    glVertex2i(x + pntX1, y + pntY1);
    glEnd();
}

void myInit(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glColor3f(0.0f, 0.0f, 0.0f);
    // glPointSize(4.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 640.0, 0.0, 480.0);
}

// void floodFill(GLint x, GLint y, Color oldColor, Color newColor)
// {
//     Color color;
//     color = getPixelColor(x, y);

//     if (color.r == oldColor.r && color.g == oldColor.g && color.b == oldColor.b)
//     {
//         setPixelColor(x, y, newColor);
//         floodFill(x + 1, y, oldColor, newColor);
//         floodFill(x, y + 1, oldColor, newColor);
//         floodFill(x - 1, y, oldColor, newColor);
//         floodFill(x, y - 1, oldColor, newColor);
//     }
//     return;
// }

void initializeAgenda()
{
    // First we initialize the upAgenda
    ScanPoint initial;
    ScanPoint downInitial;
    int x = startPoint.x, y = startPoint.y;
    initial.y = y;
    initial.x = x;
    // Color oldColor = getPixelColor(x, y);
    Color iter = getPixelColor(x, y);
    while (iter.r == oldColor.r && iter.g == oldColor.g && iter.b == oldColor.b)
    {
        x--;
        iter = getPixelColor(x, y);
    }
    int x0 = x + 1;
    x = startPoint.x;
    iter = getPixelColor(x, y);
    while (iter.r == oldColor.r && iter.g == oldColor.g && iter.b == oldColor.b)
    {
        x++;
        iter = getPixelColor(x, y);
    }
    int x1 = x - 1;
    upAgenda.push_back(initial);

    //Initializing the down agenda
    y -= 1;
    downInitial.y = y;
    x = x0;
    iter = getPixelColor(x, y);
    while (x <= x1 && (iter.r != oldColor.r || iter.g != oldColor.g || iter.b != oldColor.b))
    {
        x++;
        iter = getPixelColor(x, y);
    }
    downInitial.x = x;
    if (x <= x1)
    {
        downAgenda.push_back(downInitial);
    }
}
int agendaNotEmpty()
{
    if (upAgenda.size() || downAgenda.size())
    {
        return true;
    }
    return false;
}
int agendaCurrDirEmpty(int verticalDir)
{
    if (verticalDir == 0 && upAgenda.size() == 0)
    {
        return 1;
    }
    else if (verticalDir == 1 && downAgenda.size() == 0)
    {
        return 1;
    }
    return 0;
}

int alreadyInAgenda(int x0, int x1, int y)
{
    for (int i = 0; i < upAgenda.size(); i++)
    {
        if (upAgenda[i].y == y)
        {
            if (upAgenda[i].x >= x0 && upAgenda[i].x <= x1)
            {
                return 1;
            }
        }
    }
    for (int i = 0; i < downAgenda.size(); i++)
    {
        if (downAgenda[i].y == y)
        {
            if (downAgenda[i].x >= x0 && downAgenda[i].x <= x1)
            {
                return 1;
            }
        }
    }
    return 0;
}

void shadeHorizontally(int x0, int x1, int y, Color newColor)
{
    for (int x = x0; x <= x1; x++)
    {
        setPixelColor(x, y, newColor);
    }
}

int lookForTurns(int x0, int x1, int y, int update)
{ // S TURNS
    int rx;
    vector<Bounds> arr;
    int ny = y + update;
    Color iter;
    Bounds chk;
    chk.y = ny;
    int flag = 0;
    int allBoundary = 1;
    for (int x = x1; x >= x0; x--)
    {
        iter = getPixelColor(x, ny);
        if (flag == 0 && iter.r == oldColor.r && iter.g == oldColor.g && iter.b == oldColor.b)
        {
            chk.x1 = x;
            flag = 1;
            allBoundary = 0;
        }
        if (flag == 1 && (iter.r != oldColor.r || iter.g != oldColor.g || iter.b != oldColor.b))
        {
            chk.x0 = x + 1;
            flag = 0;
            arr.push_back(chk);
        }
        else if (flag == 1 && x == x0)
        {
            chk.x0 = x;
            flag = 0;
            arr.push_back(chk);
        }
    }
    if (allBoundary == 1)
    {
        rx = -1;
    }
    else if (arr.size() >= 1)
    {
        rx = arr[arr.size() - 1].x0;
    }
    if (arr.size() > 1)
    {
        ScanPoint nPoint;
        if (update == 1)
        {
            for (int i = 0; i <= arr.size() - 2; i++)
            {
                nPoint.x = arr[i].x0;
                nPoint.y = arr[i].y;
                upAgenda.push_back(nPoint);
            }
        }
        else
        {
            for (int i = 0; i <= arr.size() - 2; i++)
            {
                nPoint.x = arr[i].x0;
                nPoint.y = arr[i].y;
                downAgenda.push_back(nPoint);
            }
        }
    }

    //U TURN
    ny = y - update;
    chk.y = ny;
    flag = 0;
    arr.clear();

    for (int x = x1; x >= x0; x--)
    {
        iter = getPixelColor(x, ny);
        if (flag == 0 && iter.r == oldColor.r && iter.g == oldColor.g && iter.b == oldColor.b)
        {
            chk.x1 = x;
            flag = 1;
        }
        if (flag == 1 && (iter.r != oldColor.r || iter.g != oldColor.g || iter.b != oldColor.b))
        {
            chk.x0 = x + 1;
            flag = 0;
            arr.push_back(chk);
        }
        else if (flag == 1 && x == x0)
        {
            chk.x0 = x;
            flag = 0;
            arr.push_back(chk);
        }
    }

    if (arr.size() > 0)
    {
        ScanPoint nPoint;
        if (update == 1)
        {
            for (int i = 0; i < arr.size(); i++)
            {
                nPoint.x = arr[i].x0;
                nPoint.y = arr[i].y;
                downAgenda.push_back(nPoint);
            }
        }
        else
        {
            for (int i = 0; i < arr.size(); i++)
            {
                nPoint.x = arr[i].x0;
                nPoint.y = arr[i].y;
                upAgenda.push_back(nPoint);
            }
        }
    }
    return rx;
}

void shadeVertically(int verticalDir, Color newColor)
{
    ScanPoint curr;
    int update;
    if (verticalDir == 0)
    {
        curr = upAgenda[upAgenda.size() - 1];
        upAgenda.pop_back();
        update = 1;
    }
    else
    {
        curr = downAgenda[downAgenda.size() - 1];
        downAgenda.pop_back();
        update = -1;
    }
    int y_d = curr.y;
    int x_d = curr.x;
    int x0, x1;
    int x = x_d;
    int y = y_d;

    Color iter = getPixelColor(x, y);
    while (iter.r == oldColor.r && iter.g == oldColor.g && iter.b == oldColor.b)
    {
        x--;
        iter = getPixelColor(x, y);
    }
    x0 = x + 1;
    x = x_d;
    iter = getPixelColor(x, y);
    while (iter.r == oldColor.r && iter.g == oldColor.g && iter.b == oldColor.b)
    {
        x++;
        iter = getPixelColor(x, y);
    }
    x1 = x - 1;
    x = x_d;
    int xDEF;
    while (alreadyInAgenda(x0, x1, y) != 1)
    {
        shadeHorizontally(x0, x1, y, newColor);
        x = lookForTurns(x0, x1, y, update);
        y += update;
        if (x == -1)
        {
            break;
        }
        xDEF = x;
        iter = getPixelColor(x, y);
        while (iter.r == oldColor.r && iter.g == oldColor.g && iter.b == oldColor.b)
        {
            x--;
            iter = getPixelColor(x, y);
        }
        x0 = x + 1;
        x = xDEF;
        iter = getPixelColor(x, y);
        while (iter.r == oldColor.r && iter.g == oldColor.g && iter.b == oldColor.b)
        {
            x++;
            iter = getPixelColor(x, y);
        }
        x1 = x - 1;
    }
}

void shade(Color newColor)
{
    int verticalDir = 0;
    initializeAgenda();
    while (agendaNotEmpty())
    {
        if (agendaCurrDirEmpty(verticalDir))
        {
            if (verticalDir == 0)
            {
                verticalDir = 1;
            }
            else
            {
                verticalDir = 0;
            }
        }

        shadeVertically(verticalDir, newColor);
    }
}

void plot_circle(int x, int y,int cx,int cy)
{
   
    plot(cx + x, cy + y);
    plot(cx - x, cy + y);
    plot(cx + x, cy - y);
    plot(cx - x, cy - y);
    plot(cx + y, cy + x);
    plot(cx - y, cy + x);
    plot(cx + y, cy - x);
    plot(cx - y, cy - x);
}
/*! Mid-point algorithm*/
void midPointCircleAlgo(int cx,int cy,int r)
{
    int x = 0;
    int y = r;
    int d = 1 - r;
    int deltaE = 3;
    int deltaSE = -2 * r + 5;
    plot_circle(x, y,cx,cy);
    while (y > x)
    {
        if (d < 0)
        {
            d += deltaE;
            deltaE += 2;
            deltaSE += 2;
        }
        else
        {
            d += deltaSE;
            deltaE += 2;
            deltaSE += 4;
            --y;
        }
        ++x;
        plot_circle(x, y,cx,cy);
    }
}
void myDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    glPointSize(1.0);

    midPointCircleAlgo(0,0,r);
    r = r / 3;
    midPointCircleAlgo(-5,-r*3,r);
    midPointCircleAlgo(0, 0, r);
    Color newColor = { 1.0f, 0.0f, 0.0f };
    oldColor = { 1.0f, 1.0f, 1.0f };
    shade(newColor);
    //floodFill(200, 200, oldColor, newColor);
    glFlush();
}

int main(int argc, char** argv)
{
    cout << "Enter the coordinates of the center:\n\n"
        << endl;

    cout << "X-coordinate   : ";
    cin >> pntX1;
    cout << "\nY-coordinate : ";
    cin >> pntY1;
    cout << "\nEnter radius : ";
    cin >> r;
    
    int start_x = 0;
    int start_y = 0;
    cout << "X-coordinate of start point wrt centre of cirle   : ";
    cin >> start_x;
    cout << "\nY-coordinate of start point wrt centre of cirle   : ";
    cin >> start_y;
    startPoint.x = pntX1+start_x;
    startPoint.y = pntY1+start_y;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(100, 150);
    glutCreateWindow("Line Drawing Alogrithms");
    glutDisplayFunc(myDisplay);
    myInit();
    glutMainLoop();
    return 0;
}