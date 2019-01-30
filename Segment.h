//TFG José Luis Calvo Subirá, Enero 2019
#include"Point.h"
#ifndef SEGMENT_H
#define SEGMENT_H



class Segment
{
public:

	Point2D p0 = Point2D(0, 0);
	Point2D p1 = Point2D(0, 0);

	Segment(double x1, double y1, double x2, double y2);

};


#endif
