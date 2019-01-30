//TFG José Luis Calvo Subirá, Enero 2019
//Calcula las trayectorias entre dos nodos pertenecientes a un grafo, y selecciona la optima en terminos de menor distancia
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/config.hpp>

#include"Point.h"

using namespace boost;

typedef adjacency_list < listS, vecS, directedS,
	no_property, property < edge_weight_t, double > > graph_t;
typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;

class Dijkstra 
{
public:
	static std::vector<std::vector<double>> dijkstra(graph_t g, Point2D start_p, Point2D target_p, std::vector<Point2D> vertices_) {


		property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);

		std::vector<vertex_descriptor> p(num_vertices(g));
		std::vector<double> d(num_vertices(g));
		
		int x, y;
		for (int i = 0; i < vertices_.size(); ++i) {
			if ((vertices_[i].x == start_p.x) && (vertices_[i].y == start_p.y)) {
				x = i;
			}
		}
		for (int i = 0; i < vertices_.size(); ++i) {
			if ((vertices_[i].x == target_p.x) && (vertices_[i].y == target_p.y)) {
				y = i;
			}
		}

		vertex_descriptor start = vertex(x, g);     //Nodo inicial
		vertex_descriptor target = vertex(y, g);	//Nodo final


		dijkstra_shortest_paths(g, start,
			predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).
			distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));

		/*std::cout << "distances and parents:" << std::endl;
		graph_traits < graph_t >::vertex_iterator vi, vend;
		for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
			std::cout << "distance(" << *vi << ") = " << d[*vi] << ", ";
			std::cout << "parent(" << *vi << ") = " << p[*vi] << std::
				endl;
		}*/

		// p[] es el mapa predecesor obtenido a través de dijkstra_shortest_paths. Para un nodo en particular, almacena los nodos anteriores 
		
		std::vector< vertex_descriptor > path;
		vertex_descriptor current = target;

		while (current != start) {
			path.push_back(current);
			current = p[current];
		}
		path.push_back(start);
		std::vector<std::vector<double>> trajectory;
		
		std::vector<vertex_descriptor >::reverse_iterator rit;
		//La trayectoria path se ha obtenido desde el nodo final, por lo que es necesario calcular la inversa, que comienza desde el nodo inicial
		for (rit = path.rbegin(); rit != path.rend(); ++rit) {
			std::vector<double> temp_t;
			/*std::cout << *rit << " ";*/
			temp_t.push_back(vertices_[*rit].x);
			temp_t.push_back(vertices_[*rit].y);

			trajectory.push_back(temp_t);

		}
		std::cout << std::endl;

		/*for (int r = 0; r < trajectory.size(); ++r) {
			std::cout << trajectory[r][0] << " , " << trajectory[r][1] << "-->\n";

		}*/
		
		return trajectory;

	}
};