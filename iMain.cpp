# include "iGraphics.h"

int x = 0, y = 0, r = 15;

typedef struct {
    double x, y;
} Point2D;

const int OUTLINE_MAX_NUM_OF_POINTS = 10;
typedef struct {
    Point2D convex_polygon_points[OUTLINE_MAX_NUM_OF_POINTS];
    int number_of_points;
} Outline;

void Outline_Draw(const Outline* outline) {
    int size = outline->number_of_points, i;
    double x[size], y[size];

    for(i=0; i<size; i++) {
        x[i] = outline->convex_polygon_points[i].x;
        y[i] = outline->convex_polygon_points[i].y;
    }

    iFilledPolygon(x, y, size);
}

void Outline_Creator(Outline* outline, Point2D points[], int array_size) {
    int i;

    outline->number_of_points = array_size;
    for(i=0; i<array_size; i++) {
        outline->convex_polygon_points[i] = points[i];
    }
}

typedef struct {

} CollisionData;

Outline box;


/*
	function iDraw() is called again and again by the system.
*/
void iDraw()
{
    //place your drawing codes here
    iClear();
    iSetColor(20,200,255);
    Outline_Draw(&box);
}

/*
	function iMouseMove() is called when the user presses and drags the mouse.
	(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my)
{
    //printf("x = %d, y= %d\n",mx,my);
    //place your codes here
}

/*
	function iMouse() is called when the user presses/releases the mouse.
	(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        //place your codes here
        //printf("x = %d, y= %d\n",mx,my);
        x += 5;
        y += 5;

    }
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        //place your codes here
        x -= 5;
        y -= 5;
    }
}

/*
	function iKeyboard() is called whenever the user hits a key in keyboard.
	key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key)
{
    if(key == 'q')
    {
        exit(0);
    }
    //place your codes for other keys here
}

/*
	function iSpecialKeyboard() is called whenver user hits special keys like-
	function keys, home, end, pg up, pg down, arraows etc. you have to use
	appropriate constants to detect them. A list is:
	GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
	GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12,
	GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
	GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
*/
void iSpecialKeyboard(unsigned char key)
{

    if(key == GLUT_KEY_END)
    {
        exit(0);
    }
    //place your codes for other keys here
}


int main()
{
    //place your own initialization codes here.
    Point2D points[] = {{0, 0}, {100, 0}, {100, 70}, {40, 100}, {30, 50}, {20, 45}, {10, 100} };
    Outline_Creator(&box, points, 7);


    iInitialize(400, 400, "iGameEngine");
    return 0;
}
