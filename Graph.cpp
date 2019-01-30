//TFG José Luis Calvo Subirá, Enero 2019
#include"Graph.h"

using namespace boost;


	std::vector<Point2D> Graph::check_continuity(Point2D point, std::vector<Segment> segments) {

		std::vector<Point2D> locator;
		for (int i = 0; i < segments.size(); ++i) {
			if ((point.x == segments[i].p0.x) && (point.y == segments[i].p0.y)) {
				locator.push_back(Point2D(segments[i].p1.x, segments[i].p1.y));
			}
			if ((point.x == segments[i].p1.x) && (point.y == segments[i].p1.y)) {
				locator.push_back(Point2D(segments[i].p0.x, segments[i].p0.y));
			}
		}
		return (locator);
	}



	double Graph::set_weights(Point2D p1, Point2D p2) {
		double weight(sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2)));
		return (weight);
	}

	void Graph::print_nodes(std::vector<Point2D> vertices) {
		std::cout << "Vertices Graph:\n";
		for (unsigned int j = 0; j < vertices.size(); ++j) {
			std::cout << vertices[j].x << "    " << vertices[j].y << "\n";
		}
	}


	//Constructor
	Graph::Graph( std::vector<Point2D> vertices_, std::vector<Segment> segments) {
		typedef adjacency_list < listS, vecS, directedS,
			no_property, property < edge_weight_t, double > > graph_t;
		int count = vertices_.size();
		graph_t g(count);

		std::vector<double> weights;
		//Para cada vertice, se evaluan el resto de ellos buscando conexiones en el conjunto segments.
		//Si las hay, se añade una conexion entre sus nodos representativos con un peso correspondiente al calculado con la funcion set_weigths
		for (unsigned int k = 0; k < vertices_.size(); ++k) {
			std::vector<Point2D> locator_(check_continuity(vertices_[k], segments));
			for (unsigned int ii = 0; ii < locator_.size(); ++ii) {
				
				for (unsigned int jj = 0; jj < vertices_.size(); ++jj) {
					if ((locator_[ii].x == vertices_[jj].x) && (locator_[ii].y == vertices_[jj].y)) {
						add_edge(k, jj, (set_weights(vertices_[k], vertices_[jj])), g); //Se añade cone
						//Check
						weights.push_back(set_weights(vertices_[k], vertices_[jj]));

					}
				}
			}
		}
		this->graph = g;
		
	}
