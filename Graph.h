//TFG José Luis Calvo Subirá, Enero 2019
//El objetivo es obtener el grafo que representa un conjunto de vertices, cuyas conexiones vienen dadas por otro conjunto de segmentos

#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/config.hpp>

#include"Point.h"
#include"Segment.h"
using namespace boost;


class Graph 
{
private:
	typedef adjacency_list < listS, vecS, directedS,
		no_property, property < edge_weight_t, double > > graph_t;
	graph_t graph;

public: 
	Graph(std::vector<Point2D> vertices_, std::vector<Segment> segments);					//Constructor de la clase Graph
	double set_weights(Point2D p1, Point2D p2);												//Calcula el peso entre dos nodos, en este caso, la distancia euclidea entre los vertices correspondientes
	std::vector<Point2D> check_continuity(Point2D point, std::vector<Segment> segments);	//Extrae los puntos del conjunto segments que estan unidos al punto point 
	graph_t getGraph() { return graph; }
	void print_nodes(std::vector<Point2D> vertices);
};
