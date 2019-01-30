//TFG José Luis Calvo Subirá, Enero 2019

//El objetivo es eliminar aquellos obstaculos que no cumplan con un area mínima, que sera entrada de la funcion junto con la matriz de obstaculos obtenida del sistema de vision. Se
//devolvera entonces la matriz obstaculo con aquellas filas que definen obstaculos de area menor que la minima eliminadas.
//IMPORTANTE: la funcion esta definida para que cada obstaculo se redefina a traves de convex_hull, es decir, aunque un obstaculo cumpla la condicion del area, si su convex_hull es distinto 
//del original, el primero pasara a ser el nuevo obstaculo. Se regeneran los obstaculos concavos y se obtiene su convexo.



#include<vector>
#include<iostream>
#include<cstdlib>

#include <boost/assign/std/vector.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/assign.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/io/dsv/write.hpp>

using namespace boost::assign;

static void obstacles_filter(std::vector<std::vector<std::vector <double> > > *obstaculo, double minimum_area) {
	
	typedef boost::geometry::model::d2::point_xy<double> point_xy;			//Definicion de una clase punto de boost
	boost::geometry::model::polygon<point_xy> polygon, polygon_hull;				//Dos variables poligono: -polygon sera el poligono definido por los puntos de cada obstaculo
																			//						      -polygon_hull: poligono convex_hull a partir de polygon como entrada.
	std::vector<point_xy> points, points_hull;										
	point_xy point_begin;													//Primer punto de cada obstaculo, despues explico por que
	int it = 0;																//iterador, se añade una unidad cuando se elimina el primer obstaculo. Es necesario porque cuando el primer bucle 
																			//for se vuelve a ejecutar, si se ha eliminado una fila de obsatculo, su dimension y la posicion del resto de
																			//obstaculos cambia, por lo que i debe cambiar tambien para poder acceder a todos los obstaculos sin saltarnos alguno
	for (int i = 0; (i - it) < obstaculo->size(); ++i) {
		std::vector<std::vector<double>> temp_obst = (*obstaculo)[i - it];
		for (int j = 0; j < temp_obst.size(); ++j) {
			if (j == 0) { point_begin = point_xy(temp_obst[j][0], temp_obst[j][1]); }
			points.push_back(point_xy(temp_obst[j][0], temp_obst[j][1]));				//Vector de puntos que pertenecen a cada obstaculo
		}
		points.push_back(point_begin);											//Se alamcena el primero y luego se añade al final del vector points porque la implementacion de "polygon"
																				//de la libreria c++ requiere que el primer punto y el ultimo sean los mismos para cerrar el poligono, 
																				//de lo contrario se queda abierto y da error
		boost::geometry::assign_points(polygon, points);						//Crea el poligono a partir de points
		boost::geometry::convex_hull(polygon, polygon_hull);							//polygon_hull es el convex hull de polygon
		std::cout << "Polygon: " << boost::geometry::dsv(polygon) << std::endl; //dsv escribe en pantalla los vertices del poligono
		std::cout << "polygon_hull " << boost::geometry::dsv(polygon_hull) <<
			" has an area of " << abs(boost::geometry::area(polygon_hull)) << std::endl;	//Area de polygon_hull
		if (boost::geometry::area(polygon_hull) < minimum_area) {							//Si el area de polygon_hull es menor que la requerida, esta fila se elimina y se suma una unidad al iterador
			*obstaculo->erase(obstaculo->begin() + i - it);
			std::cout << "vector " << i << " has been erased" << std::endl;
			++it;
		}
		else {																	//En el caso de que el obstaculo cumple la condicion, se mantiene en la matriz obstaculo. No obstante
																				//sus valores se cambian por los correspondientes al poligono envolvente, es decir, al obtenido tras 
																				//aplicar la funcion convex_hull
			for (std::vector<point_xy>::iterator hull_it = polygon_hull.outer().begin(); hull_it != polygon_hull.outer().end(); ++hull_it)
			{ 
				points_hull.push_back(*hull_it);					//Se itera sobre polygon_hull para obtener unn vector de puntos que luego reemplazan a los originales del obstaculo
																	//Nota: si el poligono original no tiene angulos concavos, polygon_hull = polygon, por lo que no habra alteracion
			}
			std::vector<std::vector<double>> new_temp_obst;			//Vector 2D al que se le asignan los nuevos puntos, es necesario hacer esta conversion, puesto que en el programa principal 
																	//las coordenadas de los puntos de los obstaculos se definen con un vector y no con tipo Point.
			for (int j = 0; j < (points_hull.size()-1); ++j) {
				std::vector<double> temp_point;
				temp_point.push_back(points_hull[j].x());
				temp_point.push_back(points_hull[j].y());
				new_temp_obst.push_back(temp_point);
				temp_point.clear();
			}
			obstaculo->at(i-it)=new_temp_obst;						//Se usa la funcion at para reemplazar el valor (en este caso valores) de la posicion (i-it) por new_temp_obst
			new_temp_obst.clear();
		}
		temp_obst.clear();
		points.clear();
		points_hull.clear();
		polygon.clear();
		polygon_hull.clear();
		
	}
}




