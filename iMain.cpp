# include "iGraphics.h"
#include "mathlib.h"

const int OUTLINE_MAX_NUM_OF_POINTS = 10;
typedef struct {
    Point2D convex_polygon_points[OUTLINE_MAX_NUM_OF_POINTS];
    int number_of_points;
    Point2D origin; // left_bottom corner (base)
    Point2D broad_max, broad_min; // Broad Phase data, change on rotate, origin change (absolute not relative)

    Vector2D normal_to_edge[OUTLINE_MAX_NUM_OF_POINTS];

} Outline;

/// Draw Outline
void Outline_Draw(const Outline* outline) {
    int size = outline->number_of_points, i;
    double x[size], y[size];

    for(i=0; i<size; i++) {
        x[i] = outline->convex_polygon_points[i].x + outline->origin.x;
        y[i] = outline->convex_polygon_points[i].y + outline->origin.y;
    }

    iPolygon(x, y, size);
    iPoint(outline->broad_max.x, outline->broad_max.y, 3);
    iPoint(outline->broad_min.x, outline->broad_min.y, 3);
}

/// Calculate Broad Phase for collusion detection
const double MAX_ORIGIN = 1e100;
void Outline_calculate_broad_phase(Outline* outline) {
    outline->broad_min = {MAX_ORIGIN, MAX_ORIGIN};
    outline->broad_max = {-MAX_ORIGIN, -MAX_ORIGIN};

    int i;
    for(i=0; i<outline->number_of_points; i++) {
        if (outline->broad_max.x < outline->convex_polygon_points[i].x) outline->broad_max.x = outline->convex_polygon_points[i].x;
        if (outline->broad_max.y < outline->convex_polygon_points[i].y) outline->broad_max.y = outline->convex_polygon_points[i].y;

        if (outline->broad_min.x > outline->convex_polygon_points[i].x) outline->broad_min.x = outline->convex_polygon_points[i].x;
        if (outline->broad_min.y > outline->convex_polygon_points[i].y) outline->broad_min.y = outline->convex_polygon_points[i].y;
    }

    outline->broad_min.x += outline->origin.x;
    outline->broad_max.x += outline->origin.x;

    outline->broad_min.y += outline->origin.y;
    outline->broad_max.y += outline->origin.y;
}

/// Calculate Narrow Phase Normal Vectors
void Outline_calculate_narrow_phase(Outline* outline) {
    if(outline->number_of_points>0) {
        outline->normal_to_edge[0] = normal(vector2D_p(outline->convex_polygon_points[outline->number_of_points-1], outline->convex_polygon_points[0]));
    }

    int i;
    for(i=1; i<outline->number_of_points; i++) {
        outline->normal_to_edge[i] = normal(vector2D_p(outline->convex_polygon_points[i-1], outline->convex_polygon_points[i]));
    }

    printf("DEBUG\n");
    for(i=0; i<outline->number_of_points; i++) {
        printf("%d\t%.2f\ti +\t%.2f\tj\n", i, outline->normal_to_edge[i].x, outline->normal_to_edge[i].y);
    }
}


void Outline_set_Origin(Outline* outline, double x, double y) {
    outline->origin = {x, y};
    Outline_calculate_broad_phase(outline);
}

/// New Outline Creator with with array of corner Points
void Outline_Creator(Outline* outline, Point2D points[], int array_size) {
    if(array_size > OUTLINE_MAX_NUM_OF_POINTS) {
        printf("Error, OUTLINE_MAX_NUM_OF_POINTS exceeded");
        outline->number_of_points = 0;
        return;
    }

    int i;
    outline->number_of_points = array_size;

    for(i=0; i<array_size; i++) {
        outline->convex_polygon_points[i] = points[i];
    }

    Outline_set_Origin(outline, 0.0, 0.0);
    Outline_calculate_narrow_phase(outline);
}

void Outline_increase_origin(Outline* outline, double dx, double dy) {
    outline->origin.x += dx;
    outline->origin.y += dy;

    outline->broad_min.x += dx;
    outline->broad_max.x += dx;

    outline->broad_min.y += dy;
    outline->broad_max.y += dy;
}

Point2D Outline_get_point(const Outline* outline, int index) {
    return (Point2D) {  outline->convex_polygon_points[index].x+outline->origin.x,
                        outline->convex_polygon_points[index].y+outline->origin.y   };
}

inline int within(double minimum, double maximum, double check) {
    // printf("%f %f -- %f :: %d\n", minimum, maximum, check, minimum<=check && check<=maximum);
    return minimum<=check && check<=maximum;
}

inline int point_within(Point2D bottomleft, Point2D topright, double x, double y) {

    return within(bottomleft.x, topright.x, x) && within(bottomleft.y, topright.y, y);
}

inline int rect_within(Point2D bottomleft, Point2D topright, Point2D checkbottomleft, Point2D checktopright) {
    return  point_within(bottomleft, topright, checkbottomleft.x, checkbottomleft.y) ||
            point_within(bottomleft, topright, checktopright.x, checktopright.y) ||
            point_within(bottomleft, topright, checkbottomleft.x, checktopright.y) ||
            point_within(bottomleft, topright, checktopright.x, checkbottomleft.y);
}

int Outline_collision_check_broad_phase(const Outline* ls, const Outline* rs) {
    return  rect_within(ls->broad_min, ls->broad_max, rs->broad_min, rs->broad_max) ||
            rect_within(rs->broad_min, rs->broad_max, ls->broad_min, ls->broad_max) ;
}

////////////////////////////
/// DEBUG
////////////////////////////
int Outline_collision_check_narrow_phase(const Outline* const ls, const Outline* const rs) {
    /// Objective is to find projection divided on both side....

    Point2D base_left, base_right, point_this, point_other;
    double cycle_this, cycle_other;

    int i, j, sat_found = FALSE;
    for(i=0; i<ls->number_of_points; i++) {

        base_left = Outline_get_point(ls, i);
        base_right = Outline_get_point(ls, (i+1)%(ls->number_of_points)); // Cycle for Max==0
        point_this = Outline_get_point(ls, (i+2)%(ls->number_of_points)); // without the line
        cycle_this = cycle(base_left, base_right, point_this);

        for (j=0; j<rs->number_of_points; j++) {
            point_other = Outline_get_point(rs, j);
            cycle_other = cycle(base_left, base_right, point_other);

            if( cycle_this*cycle_other > 0 || cycle_other==0.0) break; /// Same side, sign says
        }

        if(j == rs->number_of_points) {
            return FALSE; /// Separated Axis Found, no collusion
        }
    }

    return TRUE;
}

int Outline_collision_check (const Outline* ls, const Outline* rs) {
    /// If do not pass broad phase, no need to check narrow phase
    if(!Outline_collision_check_broad_phase(ls, rs)) return FALSE;

    /// Check for each line, using Separating Axis Theorem
    return Outline_collision_check_narrow_phase(ls, rs) && Outline_collision_check_narrow_phase(rs, ls);
}

Outline box, pentagon;


/*
	function iDraw() is called again and again by the system.
*/
void iDraw()
{
    //place your drawing codes here
    iClear();
    iSetColor(20,200,255);
    Outline_Draw(&box);
    Outline_Draw(&pentagon);
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
        Outline_increase_origin(&box, 10, 10);
    }
    if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        //place your codes here
        Outline_increase_origin(&box, -10, -10);
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
    else if(key == 'a') {
        Outline_increase_origin(&box, -10, 0);
    }
    else if(key == 'd') {
        Outline_increase_origin(&box, +10, 0);
    }
    else if(key == 's') {
        Outline_increase_origin(&box, 0, -10);
    }
    else if(key == 'w') {
        Outline_increase_origin(&box, 0, +10);
    }

    printf("Collide : %d\n", Outline_collision_check(&box, &pentagon));

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
    Point2D points_[] = {{0, 0}, {100, 10}, {100, 110}, {10, 100}};
    Outline_Creator(&box, points_, 4);


    Point2D points[] = {{0, 0}, {50, 0}, {70, 40}, {40, 70}, {0, 70}};
    Outline_Creator(&pentagon, points, 5);
    Outline_set_Origin(&pentagon, 300, 300);


    iInitialize(400, 400, "iGameEngine");
    return 0;
}
