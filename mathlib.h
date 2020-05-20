#include <math.h>

#define PI 3.14159265358979323846

typedef struct {
    double x, y;
} Point2D;

static inline double cycle(Point2D a, Point2D b, Point2D c)         { return a.x*b.y+b.x*c.y+c.x*a.y - (a.y*b.x+b.y*c.x+c.y*a.x); }

static inline double radian(double degree)                          { return PI*degree/180; }
static inline double degree(double radian)                          { return radian*180/PI; }

typedef struct { double x, y; } Vector2D;
static inline Vector2D vector2D (double x, double y)                { return (Vector2D) {x, y}; }

static inline Vector2D vector2D_p_origin(Point2D p)                 { return (Vector2D) {p.x, p.y}; }
static inline Vector2D vector2D_p (Point2D from, Point2D to)        { return (Vector2D) {to.x-from.x, to.y-from.y}; }

static inline Vector2D add(Vector2D ls, Vector2D rs)                { return (Vector2D) {ls.x+rs.x, ls.y+rs.y}; }
static inline Vector2D add_s(Vector2D ls, double rs)                  { return (Vector2D) {ls.x+rs, ls.y+rs}; }

static inline Vector2D sub(Vector2D ls, Vector2D rs)                { return (Vector2D) {ls.x-rs.x, ls.y-rs.y}; }
static inline Vector2D sub_s(Vector2D ls, double rs)                  { return (Vector2D) {ls.x-rs, ls.y-rs}; }

static inline Vector2D mul(Vector2D ls, double rs)                  { return (Vector2D) {ls.x*rs, ls.y*rs}; }
static inline double dot(Vector2D ls, Vector2D rs)                  { return ls.x*rs.x + ls.y*rs.y; }
static inline Vector2D cross(Vector2D ls, Vector2D rs)              { return (Vector2D) {ls.x*rs.y, -ls.y*rs.x}; }

static inline double length(Vector2D vct)                           { return sqrt(vct.x*vct.x + vct.y*vct.y); }

static inline double projection(Vector2D direction, Vector2D of)    { return dot(direction, of)/length(direction); }
static inline Vector2D unit(Vector2D vct)                           { return mul(vct, 1/length(vct)); }
static inline Vector2D normal(Vector2D vct)                         { return (Vector2D) {vct.y, -vct.x}; }

static inline double angle_radian(Vector2D ls, Vector2D rs)         { return acos(dot(ls, rs)/(length(ls)*length(rs))); }
