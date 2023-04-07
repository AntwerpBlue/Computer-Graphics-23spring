//   Copyright © 2021, Renjie Chen @ USTC

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#define FREEGLUT_STATIC
#include "gl_core_3_3.h"
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#define TW_STATIC
#include <AntTweakBar.h>

#include <vector>
#include <string>
#include <stack>
#include <queue>

#include "glprogram.h"
#include "MyImage.h"
#include "VAOImage.h"
#include "VAOMesh.h"

GLProgram MyMesh::prog;

MyMesh M;
int viewport[4] = { 0, 0, 1280, 960 };

bool showATB = true;

std::string imagefile = "boy.png";
std::string eimagefile= "boy_saliency.png";

MyImage img;
int resize_width, resize_height,resize_step;

int mousePressButton;
int mouseButtonDown;
int mousePos[2];


void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, viewport[2], viewport[3]);
    M.draw(viewport);

    if (showATB) TwDraw();
    glutSwapBuffers();
}

void onKeyboard(unsigned char code, int x, int y)
{
    if (!TwEventKeyboardGLUT(code, x, y)) {
        switch (code) {
        case 17:
            exit(0);
        case 'f':
            glutFullScreenToggle();
            break;
        case ' ':
            showATB = !showATB;
            break;
        }
    }

    glutPostRedisplay();
}

void onMouseButton(int button, int updown, int x, int y)
{
    if (!showATB || !TwEventMouseButtonGLUT(button, updown, x, y)) {
        mousePressButton = button;
        mouseButtonDown = updown;

        mousePos[0] = x;
        mousePos[1] = y;
    }

    glutPostRedisplay();
}


void onMouseMove(int x, int y)
{
    if (!showATB || !TwEventMouseMotionGLUT(x, y)) {
        if (mouseButtonDown == GLUT_DOWN) {
            if (mousePressButton == GLUT_MIDDLE_BUTTON) {
                M.moveInScreen(mousePos[0], mousePos[1], x, y, viewport);
            }
        }
    }

    mousePos[0] = x; mousePos[1] = y;

    glutPostRedisplay();
}


void onMouseWheel(int wheel_number, int direction, int x, int y)
{
    if (glutGetModifiers() & GLUT_ACTIVE_CTRL) {
    }
    else
        M.mMeshScale *= direction > 0 ? 1.1f : 0.9f;

    glutPostRedisplay();
}

int initGL(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowSize(960, 960);
    glutInitWindowPosition(200, 50);
    glutCreateWindow(argv[0]);//build the ui window

    // !Load the OpenGL functions. after the opengl context has been created
    if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
        return -1;

    glClearColor(1.f, 1.f, 1.f, 0.f);

    glutReshapeFunc([](int w, int h) { viewport[2] = w; viewport[3] = h; TwWindowSize(w, h); });
    glutDisplayFunc(display);
    glutKeyboardFunc(onKeyboard);
    glutMouseFunc(onMouseButton);
    glutMotionFunc(onMouseMove);
    glutMouseWheelFunc(onMouseWheel);//reaction to the user's motion
    glutCloseFunc([]() {exit(0); });
    return 0;
}

void uploadImage(const MyImage& img)
{
    int w = img.width();
    int h = img.height();
    float x[] = { 0, 0, 0, w, 0, 0, w, h, 0, 0, h, 0 };
    M.upload(x, 4, nullptr, 0, nullptr);

    M.tex.setImage(img);
    M.tex.setClamping(GL_CLAMP_TO_EDGE);
}


void readImage(const std::string& file)
{
    int w0 = img.width(), h0 = img.height();
    img = MyImage(file);
    uploadImage(img);
    resize_width = img.width();
    resize_height = img.height();
    resize_step = 1;
    if (w0 != img.width() || h0 != img.height()) M.updateBBox();
}

inline double min(double a, double b) { return a <= b ? a : b; }
inline double min(double a, double b, double c) { return min(a, b) <= c ? min(a,b):c; }
bool notin(std::vector<int> vec, int x) {
    while (!vec.empty()) {
        if (x == vec.back()) { return false; }
        vec.pop_back();
    }
    return true;
}

MyImage trans(MyImage img) {
    std::vector<BYTE> Vec;
    for (int c = 1; c <= img.width(); c++) {
        for (int r = 1; r <= img.height(); r++) {
            for (int d = 1; d <= img.dim(); d++) {
                Vec.push_back(img(r, c, d));
            }
        }
    }
    MyImage nimg = MyImage(Vec.data(), img.height(), img.width(), img.height() * img.dim(), img.dim());
    return nimg;
}

std::vector<int> joint(std::vector<int> vec) {
    std::vector<int> Hash;
    while (!vec.empty()) {
        if (notin(Hash, vec.back())) {
            Hash.push_back(vec.back());
        }
        vec.pop_back();
    }
    return Hash;
}



BYTE average(MyImage img, int r, int c,int d) {
    int sum; int num;
    if (r == 1) {
        if (c == 1) {sum = img(1, 2,d) + img(2, 1,d); num = 2;}
        else if (c == img.width()) { sum = img(1, c - 1,d) + img(2, c,d); num = 2; }
        else { sum = img(1, c - 1,d) + img(1, c + 1,d) + img(2, c,d); num = 3; }
    }
    else if (r == img.height()) {
        if (c == 1) { sum = img(r - 1, 1,d) + img(r, 2,d); num = 2; }
        else if (c == img.width()) { sum = img(r, c - 1,d) + img(r - 1, c,d); num = 2; }
        else { sum = img(r, c - 1,d) + img(r, c + 1,d) + img(r - 1, c,d); num = 3; }
    }
    else {
        if (c == 1) { sum = img(r - 1, 1,d) + img(r + 1, 1,d) + img(r, 2,d); num = 3; }
        else if (c == img.width()) { sum = img(r, c - 1,d) + img(r - 1, c,d) + img(r + 1, c,d); num = 3; }
        else { sum = img(r - 1, c,d) + img(r + 1, c,d) + img(r, c + 1,d) + img(r, c - 1,d); num = 4; }
    }
    return sum / num;
}

BYTE GaussConv(MyImage img, int r, int c, int d) {
    double Ker[9] = { 1 / 16,2 / 16,1 / 16,2 / 16,4 / 16,2 / 16,1 / 16,2 / 16,1 / 16 },conv=0;
    int num;
    BYTE p[9] ;
    for (int i = r-1; i < r+2; i++) {
        for (int j = c-1; j < c+2; j++) {
            num = img.dim() * ((i - 1) * img.width() + j - 1) + d - 1;
            if (num<0 || num>img.dim() * img.height() * img.width()) {
                p[3 * (i-r+1) + j-c+1] = img(r+1,c+1,d+1);
            }
            else p[3 * (i - r + 1) + j - c + 1] = img.data()[num];
        }
    }
    for (int i = 0; i < 9; i++) {
        conv += p[i] * Ker[i];
    }
    return (BYTE)conv;
}



std::vector<MyImage> widthAdding(MyImage eimg, MyImage img,MyImage img_ast,MyImage eimg_ast, int w) {
    //unfinished, this function is trying to construct an assistive img, when adding a seam on the row img, 
    //one seam of img_ast is deleted and transported to the img, which will solve the problem of crossing seams.
    std::vector<std::vector<double> > Mat;
    std::vector<double> erow;
    std::queue<BYTE> row, cpr;
    std::stack<std::queue<BYTE> > si,cpi;
    std::queue<BYTE> emrow,cper;
    std::stack<std::queue<BYTE> > se,cpe;
    int dim = img.dim();
    int wd = img.width();
    int ht = img.height();


    for (int c = 1; c <= wd; c++) { erow.push_back(double(eimg(1, c))); }
    Mat.push_back(erow);
    erow.clear();
    for (int r = 2; r <= eimg.height(); r++) {
        for (int c = 1; c <= wd; c++) {
            if (c == 1) {
                erow.push_back(double(eimg(r, c)) + min(Mat[r - 2][0], Mat[r - 2][1]));
            }
            else if (c == wd) {
                erow.push_back(double(eimg(r, c)) + min(Mat[r - 2][c - 2], Mat[r - 2][c - 1]));
            }
            else erow.push_back(double(eimg(r, c)) + min(Mat[r - 2][c - 2], Mat[r - 2][c - 1], Mat[r - 2][c]));
        }
        Mat.push_back(erow);
        erow.clear();
    }

    std::vector<int> Loc;
    int loc=0; double temp = Mat[ht - 1][0];;
    for (int i = 0; i < wd; i++) {
        if (temp >= Mat[ht - 1][i]) {
            if (i!=loc) {
                temp = Mat[ht - 1][i];
                loc = i;
            }
        }
    }
    Loc.push_back(loc);

    for (int r = ht - 1; r >= 0; r--) {
        for (int i = 0; i < wd; i++) {
            if (i!=Loc.back()) {
                for (int d = 0; d < dim; d++) {
                    row.push(img(r + 1, i + 1, d + 1));
                    emrow.push(eimg(r + 1, i + 1, d + 1));
                    cpr.push(img(r + 1, i + 1, d + 1));
                    cper.push(eimg(r + 1, i + 1, d + 1));
                }
            }
            else if (wd < w) {
                for (int d = 0; d < dim; d++) {
                    cpr.push(img(r + 1, i + 1, d + 1));
                    cper.push(eimg(r + 1, i + 1, d + 1));
                }
            }
        }
        si.push(row);
        cpi.push(cpr);
        se.push(emrow);
        cpe.push(cper);
        while (!row.empty())row.pop();
        while (!cpr.empty())cpr.pop();
        while (!emrow.empty())emrow.pop();
        while (!cper.empty())cper.pop();
        if (r == 0)break;

        if (loc == 0) {
            loc = Mat[r - 1][0] > Mat[r - 1][1] ? 1 : 0;
        }
        else if (loc == wd - 1) {
            loc = Mat[r - 1][wd - 2] > Mat[r - 1][wd - 1] ? wd - 1 : wd - 2;
        }
        else {
            if (Mat[r - 1][loc - 1] > Mat[r - 1][loc + 1]) {
                if (Mat[r - 1][loc] > Mat[r - 1][loc + 1])loc = loc + 1;
            }
            else if (Mat[r - 1][loc] > Mat[r - 1][loc - 1])loc = loc - 1;
        }


        loc = 0; temp = Mat[r][0];
        for (int j = 0; j < wd; j++) {
            if (temp >= Mat[r][j]) {
                if (j!=loc) {
                    temp = Mat[r][j];
                    loc = j;
                }
            }
        }
        Loc.push_back(loc);
    }
   


    std::vector<BYTE> Imsc,Imcopy;
    std::vector<BYTE> Eimsc,Eimcopy;
    for (int i = 0; i < ht; i++) {
        for (int j = 0; j < wd; j++) {
            Imcopy.push_back(cpi.top().front()); cpi.top().pop();
            Eimcopy.push_back(cpe.top().front()); cpe.top().pop();
            if (j == Loc[i]) {
                for (int d = 0; d < dim; d++) {
                    Imcopy.push_back(si.top().front());
                    Imsc.push_back(si.top().front());
                    si.top().pop();
                    Eimcopy.push_back(se.top().front());
                    Eimsc.push_back(se.top().front());
                    se.top().pop();
                }
            }
            if (!si.top().empty()) { 
                Imsc.push_back(si.top().front()); 
                si.top().pop();
                Eimsc.push_back(se.top().front());
                se.top().pop();
            }
        }
        si.pop(); cpi.pop();
        se.pop(); cpe.pop();
    }




    //while (!si.empty()) {
    //    while (!si.top().empty()) {
    //        Imsc.push_back(si.top().front());
    //        Imcopy.push_back(cpi.top().front());
    //        si.top().pop();
    //    }
    //    si.pop();
    //}
    //while (!se.empty()) {
    //    while (!se.top().empty()) {
    //        Eimsc.push_back(se.top().front());
    //        se.top().pop();
    //    }
    //    se.pop();
    //}
    MyImage imsc, eimsc;
    MyImage imcopy;
    MyImage eimcopy;

    imsc = MyImage(Imsc.data(), wd - 1, ht, (wd - 1) * dim, dim);
    eimsc = MyImage(Eimsc.data(), wd - 1, ht, (wd - 1) * dim, dim);
    imcopy = MyImage(Imcopy.data(), wd + 1, ht, (wd + 1) * dim, dim);
    eimcopy = MyImage(Eimcopy.data(), wd + 1, ht, (wd + 1) * dim, dim);
    std::vector<MyImage> vec;
    vec.push_back(imcopy);
    vec.push_back(eimcopy);
    vec.push_back(imsc);
    vec.push_back(eimsc);
    return vec;
}


std::vector<MyImage> widthResize(MyImage eimg,MyImage img, int w,int step) {
    std::vector<std::vector<double> > Mat;
    std::vector<double> erow;
    std::queue<BYTE> row;
    std::stack<std::queue<BYTE> > si;
    std::queue<BYTE> emrow;
    std::stack<std::queue<BYTE> > se;
    int dim = img.dim();
    int wd = img.width();
    int ht = img.height();
    step = step > abs(wd - w) ? abs(wd - w) : step;

    for (int c = 1; c <= wd; c++) {erow.push_back(double(eimg(1, c)));}
    Mat.push_back(erow); 
    erow.clear();
    for (int r = 2; r <= eimg.height(); r++) {
        for (int c = 1; c <= wd; c++) {
            if (c == 1) {
                erow.push_back(double(eimg(r,c))+min(Mat[r - 2][0], Mat[r - 2][1]));
            }
            else if (c == wd) {
                erow.push_back(double(eimg(r,c)) + min(Mat[r - 2][c - 2], Mat[r - 2][c - 1]));
            }
            else erow.push_back(double(eimg(r,c)) + min(Mat[r - 2][c - 2], Mat[r - 2][c - 1], Mat[r - 2][c]));
        }
        Mat.push_back(erow);
        erow.clear();
    }
    std::vector<int> Loc;
    int loc, num = 0; double temp;
    while (num < step) {
        loc = 0;
        temp = Mat[ht - 1][0];
        for (int i = 0; i < wd; i++) {
            if (temp >= Mat[ht - 1][i]) {
                if (Loc.empty()||notin(Loc,i)) {
                    temp = Mat[ht - 1][i];
                    loc = i;
                }
            }
        }
        Loc.push_back(loc);
        num++;
    }

    for (int r = ht - 1; r >= 0; r--) {
        for (int i = 0; i < wd; i++) {
            if (notin(Loc,i)) {
                for (int d = 0; d < dim; d++) {
                    row.push(img(r + 1, i + 1, d + 1));
                    emrow.push(eimg(r + 1, i + 1, d + 1));
                }
            }
            else if (wd < w) {
                for (int d = 0; d < dim; d++) {
                    row.push(average(img,r+1,i+1,d+1));
                    emrow.push(average(eimg,r+1,i+1,d+1));
                }
                for (int d = 0; d < dim; d++) {
                    row.push(img(r + 1, i + 1, d + 1));
                    emrow.push(eimg(r + 1, i + 1, d + 1));
                }
            }
        }
        si.push(row);
        se.push(emrow);
        while (!row.empty())row.pop();
        while (!emrow.empty())emrow.pop();
        if (r == 0)break;

        for (int i = 0; i < step; i++) {
            if (Loc[i] == 0) {
                Loc[i] = Mat[r - 1][0] > Mat[r - 1][1] ? 1 : 0;
            }
            else if (Loc[i] == wd - 1) {
                Loc[i] = Mat[r - 1][wd - 2] > Mat[r - 1][wd - 1] ? wd - 1 : wd - 2;
            }
            else {
                if (Mat[r - 1][Loc[i] - 1] > Mat[r - 1][Loc[i] + 1]) {
                    if (Mat[r - 1][Loc[i]] > Mat[r - 1][Loc[i] + 1])Loc[i] = Loc[i] + 1;
                }
                else if (Mat[r - 1][Loc[i]] > Mat[r - 1][Loc[i] - 1])Loc[i] = Loc[i] - 1;
            }
        }
        Loc = joint(Loc);
        for (int i = Loc.size(); i < step; i++) {
            loc = 0; temp = Mat[r][0];
            for (int j = 0; j < wd; j++) {
                if (temp >= Mat[r][j]) {
                    if (Loc.empty() || notin(Loc, j)) {
                        temp = Mat[r][j];
                        loc = j;
                    }
                }
            }
            Loc.push_back(loc);
        }

    }
    std::vector<BYTE> Imsc;
    std::vector<BYTE> Eimsc;
    while (!si.empty()) {
        while (!si.top().empty()) {
            Imsc.push_back(si.top().front());
            si.top().pop();
        }
        si.pop();
    }
    while (!se.empty()) {
        while (!se.top().empty()) {
            Eimsc.push_back(se.top().front());
            se.top().pop();
        }
        se.pop();
    }
    MyImage imsc, eimsc;
    if (wd>w) {
        imsc = MyImage(Imsc.data(), wd - step, ht, (wd - step) * dim, dim);
        eimsc = MyImage(Eimsc.data(), wd - step, ht, (wd - step) * dim, dim);
    }
    else {
        imsc = MyImage(Imsc.data(), wd + step, ht, (wd + step) * dim, dim);
        eimsc = MyImage(Eimsc.data(), wd + step, ht, (wd + step) * dim, dim);
    }
    std::vector<MyImage> vec;
    vec.push_back(imsc);
    vec.push_back(eimsc);
    return vec;
}






std::vector<MyImage> heightResize(MyImage eimg, MyImage img, int h,int step) {
    std::vector<std::vector<double> > Mat;
    std::vector<double> erow;
    std::stack<BYTE> row;
    std::vector<std::stack<BYTE> > si;
    std::stack<BYTE> emrow;
    std::vector<std::stack<BYTE> > se;

    int dim = img.dim();
    int ht = img.height();
    int wd = img.width();
    step = step > abs(ht - h) ? abs(ht - h) : step;

    for (int i = 0; i < ht; i++) {
        si.push_back(row);
        se.push_back(emrow);
    }

    for (int r = 1; r <= ht; r++) {
        erow.push_back(eimg(r , 1));
        Mat.push_back(erow);
        erow.clear();
    }
    for (int c = 2; c <=wd ; c++) {
        for (int r = 1; r <= ht; r++) {
            if(r==1) {
                Mat[0].push_back(double(eimg(r, c)) + min(Mat[0][c - 2], Mat[1][c - 2]));
            }
            else if (r == ht) {
                Mat[r - 1].push_back(double(eimg(r, c)) + min(Mat[r - 1][c - 2], Mat[r - 2][c - 2]));
            }
            else Mat[r - 1].push_back(double(eimg(r, c)) + min(Mat[r - 2][c - 2], Mat[r - 1][c - 2], Mat[r][c - 2]));
        }
    }
    std::vector<int> Loc;
    int loc,num=0;double temp;
    while(num<step){
        loc=0;temp=Mat[0][wd-1];
        for(int i=0;i<ht;i++){
            if(temp>Mat[i][wd-1]){
                if(Loc.empty()||notin(Loc,i)){
                    temp=Mat[i][wd-1];
                    loc=i;
                }
            }
        }
        Loc.push_back(loc);
        num++;
    }

    for (int c = ht - 1; c >= 0; c--) {
        for (int r = 0 ; r <ht; r++) {
            if(notin(Loc,r)){
                for(int d=dim;d>0;d--){
                    si[r].push(img(r+1,c+1,d));
                    se[r].push(eimg(r + 1, c + 1, d));
                }
            }
            else if(ht<h){
                for (int d = dim; d > 0; d--) {
                    si[r].push(average(img, r + 1, c + 1, d));
                    se[r].push(average(eimg, r + 1, c + 1, d));
                }
                for (int d = dim; d > 0; d--) {
                    si[r].push(img(r + 1, c + 1, d));
                    se[r].push(eimg(r + 1, c + 1, d));
                }
            }
        }
        if (c == 0)break;
        for (int i = 0; i < step; i++) {
            if (Loc[i] == 0) {
                Loc[i] = Mat[0][c-1] > Mat[1][c-1] ? 1 : 0;
            }
            else if (Loc[i] == ht - 1) {
                Loc[i] = Mat[ht-2][c-1] > Mat[ht - 1][c-1] ? ht - 1 : ht - 2;
            }
            else {
                if (Mat[Loc[i] - 1][c - 1] > Mat[Loc[i] + 1][c-1]) {
                    if (Mat[Loc[i]][c-1] > Mat[Loc[i] + 1][c-1])Loc[i] = Loc[i] + 1;
                }
                else if (Mat[Loc[i]][c-1] > Mat[Loc[i] - 1][c-1])Loc[i] = Loc[i] - 1;
            }
        }
        Loc = joint(Loc);

        for (int i = Loc.size(); i < step; i++) {
            loc = 0; temp = Mat[0][c];
            for (int j = 0; j < ht; j++) {
                if (temp >= Mat[j][c]) {
                    if (Loc.empty() || notin(Loc, j)) {
                        temp = Mat[j][c];
                        loc = j;
                    }
                }
            }
            Loc.push_back(loc);
        }

    }

    std::vector<BYTE> Imsc;
    std::vector<BYTE> Eimsc;
    while (!si.empty()) {
        while (!si[0].empty()) {
            Imsc.push_back(si[0].top());
            si[0].pop();
        }
        si.erase(si.begin());
    }
    while (!se.empty()) {
        while (!se[0].empty()) {
            Eimsc.push_back(se[0].top());
            se[0].pop();
        }
        se.erase(se.begin());
    }
    MyImage imsc, eimsc;
    if (ht>h) {
        imsc = MyImage(Imsc.data(), wd, ht-step, wd * dim, dim);
        eimsc = MyImage(Eimsc.data(), wd, ht-step, wd * dim, dim);
    }
    else {
        imsc = MyImage(Imsc.data(), wd , ht+step, wd * dim, dim);
        eimsc = MyImage(Eimsc.data(), wd, ht+step, wd * dim, dim);
    }
    std::vector<MyImage> vec;
    vec.push_back(imsc);
    vec.push_back(eimsc);
    return vec;
}




MyImage seamCarving(const MyImage& img, int w, int h,int step)
{   
    // TODO
    MyImage imsc=img;
    MyImage eimg=MyImage(eimagefile);
    MyImage emsc = eimg;
    while (imsc.width() != w) {
        std::vector<MyImage>vec = widthResize(eimg, imsc, w, step);
        imsc = vec[0];
        eimg = vec[1];
    }   
    if (imsc.height() != h) {
        emsc = trans(eimg);
        imsc = trans(imsc);
        while (imsc.width() != h) {
            std::vector<MyImage>vec = widthResize(trans(emsc), trans(imsc), h, step);
            imsc = vec[0];
            emsc = vec[1];
        }
        emsc = trans(emsc);
        imsc = trans(imsc);
    }
    return imsc;
}

void createTweakbar()
{
    //Create a tweak bar
    TwBar *bar = TwNewBar("Image Viewer");
    TwDefine(" 'Image Viewer' size='220 150' color='0 128 255' text=dark alpha=128 position='5 5'"); // change default tweak bar size and color

    TwAddVarRW(bar, "Scale", TW_TYPE_FLOAT, &M.mMeshScale, " min=0 step=0.1");

    TwAddVarRW(bar, "Image filename", TW_TYPE_STDSTRING, &imagefile, " ");
    TwAddButton(bar, "Read Image", [](void*) { readImage(imagefile); }, nullptr, "");

    TwAddVarRW(bar, "Resize Width", TW_TYPE_INT32, &resize_width, "group='Seam Carving' min=1 ");
    TwAddVarRW(bar, "Resize Height", TW_TYPE_INT32, &resize_height, "group='Seam Carving' min=1 ");
    TwAddVarRW(bar, "Step", TW_TYPE_INT32, &resize_step, "group='Seam Carving' min=1");
    TwAddButton(bar, "Run Seam Carving", [](void* img) {
        MyImage newimg = seamCarving(*(const MyImage*)img, resize_width, resize_height,resize_step);
        uploadImage(newimg);
        }, 
        &img, "");
}


int main(int argc, char *argv[])
{
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), { 100, 5000 });//building the screen buffer

    if (initGL(argc, argv)) {
        fprintf(stderr, "!Failed to initialize OpenGL!Exit...");
        exit(-1);
    }

    MyMesh::buildShaders();


    float x[] = { 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0 };
    float uv[] = { 0, 0, 1, 0, 1, 1, 0, 1 };
    int t[] = { 0, 1, 2, 2, 3, 0 };

    M.upload(x, 4, t, 2, uv);//

    //////////////////////////////////////////////////////////////////////////
    TwInit(TW_OPENGL_CORE, NULL);
    //Send 'glutGetModifers' function pointer to AntTweakBar;
    //required because the GLUT key event functions do not report key modifiers states.
    TwGLUTModifiersFunc(glutGetModifiers);
    glutSpecialFunc([](int key, int x, int y) { TwEventSpecialGLUT(key, x, y); glutPostRedisplay(); }); // important for special keys like UP/DOWN/LEFT/RIGHT ...
    TwCopyStdStringToClientFunc([](std::string& dst, const std::string& src) {dst = src; });


    createTweakbar();

    //////////////////////////////////////////////////////////////////////////
    atexit([] { TwDeleteAllBars();  TwTerminate(); });  // Called after glutMainLoop ends

    glutTimerFunc(1, [](int) { readImage(imagefile); },  0);


    //////////////////////////////////////////////////////////////////////////
    glutMainLoop();

    return 0;
}