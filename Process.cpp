//TFG José Luis Calvo Subirá, Enero 2019
//Planificación de trayectorias
#include <cstdio>
#include <vector>
#include<cstddef>
#include<iostream>

#include<math.h>

#include"Point.h"
#include"Segment.h"
#include"VoronoiDiagram.cpp"
#include"dijkstra.cpp"
#include"Graph.h"
#include <libs/polygon/example/voronoi_visual_utils.hpp>

#include <boost/polygon/voronoi.hpp>
using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;

using namespace std;



class Process
{
	public:
	//une point al vértice mas cercano pertenciente al conjunto segments
	static Point2D nearest_vertex(Point2D point, std::vector<Segment> segments) {
		Point2D vertex(Point2D(0, 0));
		float distance = 50000; //La distancia comparacion inicial debe ser de orden mayor a las dimensiones en las que se trabaja, para asegurar la existencia de un punto cercano
		for (unsigned int i = 0; i < segments.size(); ++i) {
			float d_to_vertex0 = sqrt(pow((segments[i].p0.x - point.x), 2) + pow((segments[i].p0.y - point.y), 2));
			float d_to_vertex1 = sqrt(pow((segments[i].p1.x - point.x), 2) + pow((segments[i].p1.y - point.y), 2));

			if (d_to_vertex0 < d_to_vertex1) {
				if (d_to_vertex0 < distance) {
					distance = d_to_vertex0;
					vertex = Point2D(segments[i].p0.x, segments[i].p0.y);
				}
			}
			else if (d_to_vertex1 < distance) {
				distance = d_to_vertex1;
				vertex = Point2D(segments[i].p1.x, segments[i].p1.y);
			}
		}

		return vertex;

	}

public:
	// Comprueba si point se encuentra en un conjunto dado points_
	static bool check_vertex(Point2D point, std::vector<Point2D> points_) {
		bool t_f = true;
		for (unsigned int i = 0; i < points_.size(); ++i) {
			if ((point.x == points_[i].x) && (point.y == points_[i].y)) {
				t_f = false;
			}
		}
		return t_f;
	}

	public:
	static std::vector<vector<vector <double> > > processVoronoi(int width, int height, std::vector<vector<vector<double> > > obstacles, std::vector<vector<double> > robot, std::vector<vector<double> > Targets)
	{
		std::vector<vector<vector<double> > > paths;
		width = width - 13.2;
		height = height - 13.2;
		//NOTA: 13.2 corresponde a la dimensión de los marcadores situados en las esquinas del espacio de trabajo de la plataforma multi-robot
		//ya que el sistema de visión utiliza los puntos centrales de dichos marcadores como referencias. Asi, se trabaja con las dimensiones reales de la plataforma, asegurando que los 
		//marcadores no quedan incluidos en el espacio de trabajo
		std::wcout << "width: " << width << std::endl;
		std::wcout << "height: " << height << std::endl;

		// Construction of the Voronoi Diagram.
		// Se llama a la función que genera el diagrama de Voronoi a partir de los datos de entrada
		std::vector<Segment> temp_path = VoronoiDiagram::v_diagram(width, height, obstacles);

		
		for (unsigned int i = 0; i < robot.size(); ++i) {
			std::vector<Segment> common_path = temp_path;
			cout << "ROBOT   " << i << ":\n";
			Point2D start(Point2D(robot[i][0], robot[i][1]));
			Point2D target(Point2D(Targets[i][0], Targets[i][1]));
		    //Para cada robot se calcula el vertice de los segmentos procedentes del diagrma de Voronoi (teniendo en cuenta las discriminaciones) mas cercano a su punto inicial, 
			//y se crea un segmento formado por ambos puntos, el cual se añade al conjunto de segmentos que representan el espacio de trabajo 
			Point2D nearest_start = nearest_vertex(start, common_path);
			//Mismo procedimiento que el utilizado para el punto inicial
			Point2D nearest_target = nearest_vertex(target, common_path);
			common_path.push_back(Segment(target.x, target.y, nearest_target.x, nearest_target.y));
			common_path.push_back(Segment(start.x, start.y, nearest_start.x, nearest_start.y));

			std::cout << "Point2D start: " << start.x << " , " << start.y << std::endl;
			std::cout << "Point neares start: " << nearest_start.x << " , " << nearest_start.y << std::endl;

			std::cout << "Point target: " << target.x << " , " << target.y << std::endl;
			std::cout << "Point neares target: " << nearest_target.x << " , " << nearest_target.y << std::endl;

			static std::vector<Point2D> vertices_; //Se extraen los vertices del conjunto de segmmentos con vistas a la dsicretizacion del entorno mediante un grafo (seran los nodos)

			for (unsigned int j = 0; j < common_path.size(); ++j) {
				if (j == 0) {
					vertices_.push_back(Point2D(common_path[j].p0.x, common_path[j].p0.y));
					vertices_.push_back(Point2D(common_path[j].p1.x, common_path[j].p1.y));
				}

				else if (check_vertex(common_path[j].p0, vertices_)) {
					vertices_.push_back(Point2D(common_path[j].p0.x, common_path[j].p0.y));
				}
				if (check_vertex(common_path[j].p1, vertices_)) {
					vertices_.push_back(Point2D(common_path[j].p1.x, common_path[j].p1.y));
				}
			}
			
			//Conjunto de vertices extraidos en un archivo de texto
			std::ofstream fout("vertices_voronoi_diagram.txt");
			fout << "x-coordinates\n";
			for (unsigned int j = 0; j < vertices_.size(); ++j) {
				fout << vertices_[j].x << "\n";
			
			}
			fout << "y-coordinates\n";
			for (unsigned int j = 0; j < vertices_.size(); ++j) {
				fout << vertices_[j].y << "\n";
			}
			fout.close();

			std::cout << "GRAPH" << std::endl;
			//Se obtiene un grafo cuyos nodos representan los vertices anteriormente extraidos. Las uniones vienen dadas por el conjunto de segmentos que contiene las conexiones 
			//entre los vértices
			Graph g(vertices_, common_path);
			//g.print_nodes(vertices_);

			std::vector<std::vector<double>> path; //Trayectoria individual de cada robot

			//Trayectoria optima para cada robot mediante un algoritmo de busqueda en grafos, en particular en el grafo g obtenido mediante la clase Graph
			path = Dijkstra::dijkstra(g.getGraph(), start, target, vertices_); 
			paths.push_back(path);
			
			
		}
		//std::cout << "fin process" << std::endl;

		//Se devuelve el conjunto de trayectorias obtenidas
		return paths;
	}
};