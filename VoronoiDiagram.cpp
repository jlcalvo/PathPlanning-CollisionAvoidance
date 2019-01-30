//TFG José Luis Calvo Subirá, Enero 2019
//El objetivo principal consiste en generar el diagrama de Voronoi de un entorno formado por sus condiciones de contorno y los obstaculos contenidos en el.
//Se devolvera al programa dicho diagrama en forma de un vector de segmentos

#include <cstdio>
#include <vector>
#include<cstddef>
#include<iostream>
#include<fstream>
#include<math.h>

#include"Segment.h"
#include"Point.h"


#include <boost/polygon/voronoi.hpp>
using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;
using boost::polygon::x;
using boost::polygon::y;
using boost::polygon::low;
using boost::polygon::high;


//Mapeo recomendado por la librería boost en example_voronoi
namespace boost {
	namespace polygon {

		template <>
		struct geometry_concept<Point2D> {
			typedef point_concept type;
		};

		template <>
		struct point_traits<Point2D> {
			typedef int coordinate_type;

			static inline coordinate_type get(
				const Point2D& point, orientation_2d orient) {
				return (orient == HORIZONTAL) ? point.x : point.y;
			}
		};

		template <>
		struct geometry_concept<Segment> {
			typedef segment_concept type;
		};

		template <>
		struct segment_traits<Segment> {
			typedef int coordinate_type;
			typedef Point2D point_type;

			static inline point_type get(const Segment& segment, direction_1d dir) {
				return dir.to_int() ? segment.p1 : segment.p0;
			}
		};
	}  // polygon
}  // boost


//{
class VoronoiDiagram
{
public:
	// devuelve true si el segmento definido por los vertices v_init y v_end no coincide con ninguno almacenado en el vector segments_voronoi.

	static bool check_double_solution(const voronoi_diagram<double>::vertex_type* v_init, const voronoi_diagram<double>::vertex_type* v_end, std::vector<Segment>  segments) {
		bool initialization = true;
		for (int unsigned i = 0; i < segments.size(); ++i) {
			if ((v_init->x() == segments[i].p1.x) && (v_init->y() == segments[i].p1.y)) {
				if ((v_end->x() == segments[i].p0.x) && (v_end->y() == segments[i].p0.y)) {
					initialization = false;
				}
			}
		}
		return (initialization);
	}

public:
	// devuelve true si el los vertices del segmento definido por v_init y v_end no coinciden con los de ningún segmento de entrada.

	static bool intersect_with_inputgeometry(const voronoi_diagram<double>::vertex_type* v_init, const voronoi_diagram<double>::vertex_type* v_end, std::vector<Segment>  segments) {
		bool initialization = true;
		for (unsigned int i = 0; i < segments.size(); ++i) {
			if (((v_init->x() == segments[i].p0.x) && (v_init->y() == segments[i].p0.y)) || ((v_end->x() == segments[i].p0.x) && (v_end->y() == segments[i].p0.y))) {
				initialization = false;
			}
			else if (((v_end->x() == segments[i].p0.x) && (v_end->y() == segments[i].p0.y)) || ((v_init->x() == segments[i].p0.x) && (v_init->y() == segments[i].p0.y))) {
				initialization = false;
			}
		}
		return (initialization);
	}

public:
	//Convierte la entrada en un cojunto de segmentos
	static std::vector<Segment> create_segments(int w, int h, std::vector<std::vector<std::vector<double> > > obstacles)
	{
		std::vector<Segment> segments;
		//Se incluyen los segmentos que forman las condiciones de contorno del espacio de trabajo
		segments.push_back(Segment(0, 0, 0, h));
		segments.push_back(Segment(0, h, w, h));
		segments.push_back(Segment(w, h, w, 0));
		segments.push_back(Segment(w, 0, 0, 0));

		for (unsigned int i = 0; i < obstacles.size(); i++) {

			std::vector<std::vector<double> > temp_obst = obstacles[i];

			for (unsigned int j = 0; j < temp_obst.size(); j++) {

				if ((j + 1) != temp_obst.size()) {

					segments.push_back(Segment(temp_obst[j][0], temp_obst[j][1], temp_obst[j + 1][0], temp_obst[j + 1][1]));
				}
				else
					segments.push_back(Segment(temp_obst[j][0], temp_obst[j][1], temp_obst[0][0], temp_obst[0][1]));
			}
		}

		return(segments);

	}


public:
	 //Genera el diagrama de Voronoi de un entorno compuesto por sus límites de contorno y los obstaculos contenidos en el.
	 static std::vector<Segment> v_diagram(int w, int h, std::vector<std::vector<std::vector<double> > > obstacles) {
		//Las condiciones de contorno y los obstaculos (definidos por puntos) han de convertirse a un conjunto de segmentos para poder utilizar el constructor del diagrama de Voronoi.
		std::vector<Segment> segments = create_segments(w, h, obstacles);

		// Construccion de Voronoi Diagram.

		voronoi_diagram<double> vd;
		construct_voronoi(segments.begin(), segments.end(),
			&vd);

		std::vector<Segment> segments_voronoi;

		for (voronoi_diagram<double>::const_edge_iterator it = vd.edges().begin();
			it != vd.edges().end(); ++it) {
			if (it->is_primary()) {
				const voronoi_diagram<double>::vertex_type* vertex_init = it->vertex0();
				const voronoi_diagram<double>::vertex_type* vertex_end = it->vertex1();
				//A continuacion, se condiciona el guardado de los segmentos procedentes del diagrama de Voronoi, segun las tres discriminaciones que se detallan en la memoria
				if ((check_double_solution(vertex_init, vertex_end, segments_voronoi)) && (intersect_with_inputgeometry(vertex_init, vertex_end, segments))) {
					
					segments_voronoi.push_back(Segment(vertex_init->x(), vertex_init->y(), vertex_end->x(), vertex_end->y()));
				}

			}

		}
		// Se escriben los resultados en un fichero de texto
		std::ofstream fout("segments_voronoi_diagram.txt");

		fout << "x-coordinates\n";
		for (unsigned int j = 0; j < segments_voronoi.size(); ++j) {
			fout << segments_voronoi[j].p0.x << "\n";
			fout << segments_voronoi[j].p1.x << "\n";
		}
		fout << "y-coordinates\n";
		for (unsigned int j = 0; j < segments_voronoi.size(); ++j) {
			fout << segments_voronoi[j].p0.y << "\n";
			fout << segments_voronoi[j].p1.y << "\n";
		}

		fout.close();
		
		return(segments_voronoi);
	}

};