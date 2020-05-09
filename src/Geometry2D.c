#include "Geometry2D.h"
#include "gfc_matrix.h"
#include <math.h>
#include "local.h"
#include "simple_logger.h"
#include "level.h"
#define FLT_EPSILON 1.192092896e-07F 
#define DEG2RAD 0.01745329251994329576923690768489f

#define CMP(x,y) (fabsf((x)-(y))) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x),fabsf(y))) //CMP macro for comparing floats
void path2dTotal(void* data, void* context);
void drawPath(void* data, void* context);
void drawLine(void* data, void* context);

Path2D path2d(List* lines)
{
	Line2D* tempLine;
	Path2D path;
	path.lines = lines;
	path.totalLength = 0.f;
	gfc_list_foreach(path.lines, path2dTotal, &path);
	tempLine = (Line2D*)gfc_list_get_nth(lines, 0);
	path.start = &tempLine->start;
	tempLine = (Point2D*)gfc_list_get_nth(lines, lines->count-1);
	path.end = &tempLine->end;
	return path;
}

void path2dTotal(void* data, void* context) {
	Line2D* line= (Line2D*)data;
	Path2D* path = (Path2D*)context;
	float len = LengthLine2D(*line);
	path->totalLength += len;
}

Bool CircleRectangle(Circle circle, Rectangle2D rect)
{
	Line2D left;
	Line2D top;
	Line2D right;
	Line2D bottom;
	Bool result = false;
	Vector2D rectMin = GetMinRect(rect);
	Vector2D rectMax = GetMaxRect(rect);
	if (PointInRectangle(circle.position, rect)) {
		return true;
	}
	left = line2d(rectMin, vector2d(rectMin.x, rectMax.y));
	if (LineCircle(left, circle)) {
		return true;
	}
	top = line2d(vector2d(rectMin.x, rectMax.y), rectMax);
	if (LineCircle(top, circle)) {
		return true;
	}
	right = line2d(vector2d(rectMax.x, rectMin.y), rectMax);
	if (LineCircle(right, circle)) {
		return true;
	}
	bottom = line2d(rectMin, vector2d(rectMax.x, rectMin.y));
	if (LineCircle(bottom, circle)) {
		return true;
	}
	return false;
}

Bool CircleCircle(Circle circle1, Circle circle2)
{
	Line2D measure;
	float posDist;
	float radDelta;

	if (PointInCircle(circle2.position, circle1)) {
		return true;
	}
	measure = line2d(circle1.position, circle2.position);
	posDist = LengthLine2D(measure);
	radDelta = circle1.radius + circle2.radius;

	if (posDist < radDelta) {
		return true;
	}
	/*
	
	measureC1 = line2d(circle1.position, vector2d(circle1.position.x + circle1.radius, circle1.position.y));
	measureC2 = line2d(circle2.position, vector2d(circle2.position.x + circle2.radius, circle2.position.y));
	//radiiDistSq1= circle1.radius * circle1.radius + circle2.radius * circle2.radius;
	radiiDistSq1 = LengthSqLine2D(measureC1);
	radiiDistSq2 = LengthSqLine2D(measureC2);
	measure = line2d(circle1.position, circle2.position);
	posDistSq = LengthSqLine2D(measure);

	xDeltaSq = (circle2.position.x - circle1.position.x) * (circle2.position.x - circle1.position.x);
	yDeltaSq = (circle2.position.y - circle1.position.y) * (circle2.position.y - circle1.position.y);
	radDeltaSq = (circle2.radius - circle1.radius) * (circle2.radius - circle1.radius);
	if (xDeltaSq + yDeltaSq <= radDeltaSq) {
		return true;
	}
	if (posDistSq <= radiiDistSq1+radiiDistSq2) {
		return true;
	}
	*/
	return false;
}

void drawPaths() {
	Level* level;
	List* paths;
	List* collisionPaths;
	level = get_loaded_level();
	paths = level->paths;
	Path2D* testPath = gfc_list_get_nth(paths, 0);
	SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 0, 255, 0, 255);
	gfc_list_foreach(paths, drawPath, paths);
	SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 165, 0, 255);
	gfc_list_foreach(level->collisionPaths, drawLine, NULL);
	SDL_SetRenderDrawColor(gf2d_graphics_get_renderer(), 255, 255, 255, 255);
}

void drawPath(void* data, void* context) {
	Path2D* path = (Path2D*)data;
	List* lines = path->lines;
	gfc_list_foreach(lines, drawLine, NULL);
}

void drawLine(void* data, void* context) {
	Line2D* line = (Line2D*)data;
	SDL_RenderDrawLine(gf2d_graphics_get_renderer(), line->start.x, line->start.y, line->end.x, line->end.y);
}

Point2D point2d(float x, float y) {
	Point2D vec;
	vector2d_set(vec, x, y);
	return vec;
}

Point2D point2d_zero()
{
	Point2D p;
	p.x = 0.f;
	p.y = 0.f;
	return p;
}

Line2D line2d(Point2D start, Point2D end) {
	Line2D line;
	line.start = start;
	line.end = end;
	return line;
}

float LengthLine2D(Line2D line) {
	Point2D vec;
	vector2d_sub(vec, line.end, line.start);
	return vector2d_magnitude(vec);
}

float LengthSqLine2D(Line2D line) {
	Point2D vec;
	vector2d_sub(vec, line.end, line.start);
	return vector2d_magnitude_squared(vec);
}

Circle circle(Point2D p, float r) {
	Circle circ;
	circ.position = p;
	circ.radius = r;
	return circ;
}

Rectangle2D rectangle2d(Point2D origin, Vector2D size) {
	Rectangle2D rect;
	rect.origin = origin;
	rect.size = size;
	return rect;
}

Vector2D GetMinRect(Rectangle2D rect) {
	Vector2D p1 = rect.origin;
	Vector2D p2;
	vector2d_add(p2,rect.origin,rect.size);

	return vector2d(fminf(p1.x, p2.x), fminf(p1.y, p2.y));
}

Vector2D GetMaxRect(Rectangle2D rect) {
	Vector2D p1 = rect.origin;
	Vector2D p2;
	vector2d_add(p2,rect.origin,rect.size);

	return vector2d(fmaxf(p1.x, p2.x), fmaxf(p1.y, p2.y));
}

Rectangle2D FromMinMaxRect(Vector2D min, Vector2D max) {
	Vector2D vec;
	vector2d_sub(vec, max, min);
	return rectangle2d(min, vec);
}

OrientedRectangle orientedRectangle(Point2D position, Vector2D halfExtents, float rotation)
{
	OrientedRectangle oRect;
	oRect.position = position;
	oRect.halfExtents = halfExtents;
	oRect.rotation = rotation;
	return oRect;
}

Bool PointOnLine(Point2D point, Line2D line)
{
	// Find the slope
	float dy = (line.end.y - line.start.y);
	float dx = (line.end.x - line.start.x);
	float M = dy / dx;
	// Find the Y-Intercept
	float B = line.start.y - M * line.start.x;
	// Check line equation
	return CMP(point.y, M * point.x + B);
}

Bool PointInCircle(Point2D point, Circle c)
{
	Line2D line = line2d(point, c.position);
	if (LengthSqLine2D(line) < c.radius * c.radius) {
		return true;
	}
	return false;
}

Bool PointInRectangle(Point2D point, Rectangle2D rectangle)
{
	Vector2D min = GetMinRect(rectangle);
	Vector2D max = GetMaxRect(rectangle);

	return  min.x <= point.x &&	min.y <= point.y &&	point.x <= max.x &&	point.y <= max.y;
}

Bool PointInOrientedRectangle(Point2D point, OrientedRectangle rectangle)
{
	Vector2D rotVector;
	Vector2D sizeVector;
	Vector2D localPoint;
	Rectangle2D localRectangle;

	vector2d_sub(rotVector,point,rectangle.position);
	float theta = -DEG2RAD*(rectangle.rotation);
	float zRotation2x2[] = {
	   cosf(theta), sinf(theta),
	   -sinf(theta), cosf(theta)
	};
	Multiply(rotVector.asArray,	vector2d(rotVector.x, rotVector.y).asArray,	1, 2, zRotation2x2, 2, 2);

	vector2d_scale(sizeVector, rectangle.halfExtents, 2.0f);
	localRectangle = rectangle2d(point2d_zero(), sizeVector);
	vector2d_add(localPoint, rotVector, rectangle.halfExtents);
	return PointInRectangle(localPoint, localRectangle);
}

Bool LineCircle(Line2D line, Circle circle)
{
	if (PointInCircle(line.start, circle) || PointInCircle(line.end, circle)) {
		return true;
	}
	Vector2D ab;
	vector2d_sub(ab,line.end,line.start);
	Vector2D dif;
	vector2d_sub(dif, circle.position, line.start);
	float t = vector2d_dot_product(dif, ab) / vector2d_dot_product(ab, ab);
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	Point2D closestPoint;
	Vector2D scale;
	vector2d_scale(scale, ab, t);
	vector2d_add(closestPoint, line.start, scale);

	Line2D circleToClosest = line2d(circle.position, closestPoint);
	return LengthSqLine2D(circleToClosest) < circle.radius * circle.radius;
}

Bool LineRectangle(Line2D l, Rectangle2D r)
{
	if (PointInRectangle(l.start, r) ||
		PointInRectangle(l.end, r)) {
		return true;
	}

	Vector2D norm;
	vector2d_sub(norm, l.end, l.start);
	vector2d_normalize(&norm);
	norm.x = (norm.x != 0) ? 1.0f / norm.x : 0;
	norm.y = (norm.y != 0) ? 1.0f / norm.y : 0;
	Vector2D min;
	vector2d_sub(min, GetMinRect(r), l.start);
	vector2d_multiply(&min, min, norm);
	Vector2D max;
	vector2d_sub(max, GetMaxRect(r), l.start);
	vector2d_multiply(&max, max, norm);

	float tmin = fmaxf(
		fminf(min.x, max.x),
		fminf(min.y, max.y)
	);
	float tmax = fminf(
		fmaxf(min.x, max.x),
		fmaxf(min.y, max.y)
	);
	if (tmax< 0 || tmin>tmax) {
		return false;
	}
	float t = (tmin < 0.0f) ? tmax : tmin;
	return t > 0.0f && t * t < LengthSqLine2D(l);
}

Bool LineOrientedRectangle(Line2D line, OrientedRectangle rectangle)
{
	float theta = -DEG2RAD*(rectangle.rotation);
	float zRotation2x2[] = {
	   cosf(theta), sinf(theta),
	   -sinf(theta), cosf(theta)
	};
	Line2D localLine;

	Vector2D rotVector;
	vector2d_sub(rotVector, line.start, rectangle.position);
	Multiply(rotVector.asArray,
		vector2d(rotVector.x, rotVector.y).asArray,
		1, 2, zRotation2x2, 2, 2);
	vector2d_add(localLine.start,rotVector, rectangle.halfExtents);

	vector2d_sub(rotVector, line.end, rectangle.position);
	Multiply(rotVector.asArray,
		vector2d(rotVector.x, rotVector.y).asArray,
		1, 2, zRotation2x2, 2, 2);
	vector2d_add(localLine.end, rotVector, rectangle.halfExtents);

	Vector2D size;
	vector2d_scale(size, rectangle.halfExtents, 2.0f);
	Rectangle2D localRectangle = rectangle2d(point2d_zero(), size);
	return LineRectangle(localLine, localRectangle);
}
