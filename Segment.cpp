//TFG José Luis Calvo Subirá, Enero 2019
#include"Segment.h"
#include"Point.h"




Segment::Segment(double x1, double y1, double x2, double y2) {
	this->p0 = Point2D(x1, y1);
	this->p1 = Point2D(x2, y2);
}



