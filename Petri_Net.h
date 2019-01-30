//TFG José Luis Calvo Subirá, Enero 2019
#include<vector>
#include <iostream>
#include<algorithm>

#ifndef PETRI_NET
#define PETRI_NET


class Input_S4PR {
private:
	int num_procesos = 0;					//Numero de procesos
	int num_lugares = 0;				//idle+lugares+recursos procesos--->rows
	int num_transiciones = 0;			//transiciones: cols. Se presupone que, para cada procesos por separado, a cada lugar le corresponde solamente una transición, puesto
										//que la trayectoria es única para cada robot. (No hay posibilidad de elegir entre más de un punto final a partir de uno inicial)
	std::vector<std::vector<int>> processes;	//Lugares en orden de sucesión que componen cada uno de los procesos: {P1 P2 P3 -1},{P4 P5 P6 -1},...{}. NOTA: -1 porque así lo requiere el 
												//constructor que se usa perteneciente a la clase S4PR del archivo petri.cpp
	std::vector<int>m_0;						//Marcado inicial
	std::vector<std::vector<int>>Pre;			//Matriz Pre
	std::vector<std::vector<int>>Post;			//Matriz Post
	std::vector<int> resources;					//Recursos: R1 R2 R3...Ri -1. NOTA: -1 mismo razonamiento que en variable processes

	int lugar_1 = 0;							//Esta variable indica el número total de lugares definidos hasta el momento, es util a la hora de crear nuevos lugares.
												//Cada vez que uno nuevo se defina, se identificará con este número y se incluye en el vector processes o resources
	std::vector<std::vector<double>> lugares;
	std::vector<int> regiones;
	std::vector<int> capacity_by_region;
	std::vector<int> idle;
	std::vector<std::vector<double>> points_involved;

public:
	Input_S4PR(std::vector<std::vector<std::vector<double>>> paths);												//Constructor---> trayectorias = puntos del plano
	Input_S4PR(std::vector<std::vector<int>> paths, std::vector<std::vector<int>> capacities);						//Constructor---> trayectorias = regiones del plano
	bool r_not_created(int x, std::vector<int> vec);				//Devuelve true si el recurso necesario para restringir una region todavia no ha sido creado. Se utiliza para evitar 
																	//multiples definiciones de recursos compartidos cuando una region aparece representada en las trayectorias mas de una vez.
	void set_to_null_matrix();										//Inicializa las matrices de incidencias pre y post a 0 
	void add_minus_1(std::vector<int>* vec);						//Añade -1 al final de un vector, se usa para cumplir con las especificaciones del constructor read_from_file2(en petri.cpp)
	bool is_idle(int place);										//Devuelve true si el lugar place corresponde a uno de los lugares idle (inicio de las trayectorias)
	int get_nprocesses();
	int get_dimP();
	int get_dimT();
	std::vector<std::vector<int>> get_Pre();
	std::vector<std::vector<int>> get_Post();
	std::vector<std::vector<int>> get_processes();
	std::vector<int> get_m_0();
	std::vector<int> get_resources();
	std::vector<std::vector<double>> get_points_involved();
	void printParameters();

};


#endif // PETRI_NET

