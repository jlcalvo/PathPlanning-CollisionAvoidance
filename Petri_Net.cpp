//TFG José Luis Calvo Subirá, Enero 2019
#include"Petri_Net.h"

Input_S4PR::Input_S4PR(std::vector<std::vector<std::vector<double>>> paths) {
	std::vector<std::vector<int>> path_translated_to_regions;
	std::vector<std::vector<int>> cap;
	int region = 1;
	for (unsigned int i = 0; i < paths.size(); ++i) {
		std::vector<std::vector<double>> temp_path = paths[i];
		std::vector<int> temp;
		std::vector<int> temp_cap;
		for (unsigned int j = 0; j < temp_path.size(); ++j) {
			temp_cap.push_back(1);									//Se inicializan las capacidades de cada region a 1 robot unicamente, ya que al tratarse de puntos del plano 2D, dos robots
																	//no pueden coexistir en el mismo punto al mismo tiempo
			if (j == 0) { temp.push_back(region);
			++region;
			points_involved.push_back(temp_path[j]);
			}
			else {
				int region_marker = 0;
				bool boolean = false;
				for (unsigned int k = 0; k < points_involved.size(); ++k) {
					if (points_involved[k][0] == temp_path[j][0] && points_involved[k][1] == temp_path[j][1]) {
						boolean = true;
						region_marker = k + 1;
					}
				}
				if (boolean) { temp.push_back(region_marker); }
				else {
					temp.push_back(region);
					++region;
					points_involved.push_back(temp_path[j]);
				}
			}
		}
		//A cada punto del conjunto de la trayectorias se le asigna un numero a modo de marcador-region. Si un punto se repite en el conjunto de trayectorias, este marcador-region aparecera
		//mas de una vez. El objetivo es facilitar los calculos posteriores, ya que se trabajara a partir de ahora con regiones idenrificadas por un numero, y no con puntos identificados 
		//por sus coordenadas x y su coordenadas y.
		path_translated_to_regions.push_back(temp);               
		cap.push_back(temp_cap);
		temp.clear();
		temp_path.clear();
		temp_cap.clear();
	}

	/*for (unsigned int i = 0; i < path_translated_to_regions.size(); ++i) {
		std::cout << "path_translated_to_regions" << i << " :\n";
		for (unsigned int j = 0; j < path_translated_to_regions[i].size(); ++j) {
			std::cout << path_translated_to_regions[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "points_involved" << " :\n";
	for (unsigned int i = 0; i < points_involved.size(); ++i) {
		std::cout << "(";
		for (unsigned int j = 0; j < points_involved[i].size(); ++j) {
			std::cout << points_involved[i][j] << ",";
		}
		std::cout << ")" << " ";
	}
	std::cout << std::endl;
	for (unsigned int i = 0; i < cap.size(); ++i) {
		std::cout << "cap" << i << " :\n";
		for (unsigned int j = 0; j < cap[i].size(); ++j) {
			std::cout << cap[i][j] << " ";
		}
		std::cout << std::endl;
	}*/
	
	num_procesos = path_translated_to_regions.size(); //Número de trayectorias obtenidas, una trayectoria por robot
	std::cout << num_procesos << std::endl;

	for (unsigned int i = 0; i < path_translated_to_regions.size(); ++i) {
		std::vector<int> temp_path = path_translated_to_regions[i];
		std::vector<int> temp_capacities = cap[i];			//Tanto paths como capacities tienen las mismas dimensiones, puesto que a cada region le corresponde una capacidad
		std::vector<int> temp_processes;
		for (unsigned int j = 0; j < temp_path.size(); ++j) {
			num_lugares += 1;										//Cada punto del camino definido representa un lugar de proceso
			num_transiciones += 1;
			++lugar_1;
			//Se almacenan las regiones
			regiones.push_back(temp_path[j]); 
			capacity_by_region.push_back(temp_capacities[j]);
			temp_processes.push_back(lugar_1);				//No se introduce el numero representativo de cada region en particular

			if (j == 0) { m_0.push_back(1); 
			idle.push_back(temp_path[j]);
			}												//Marcado inicial: se consideran marcados únicamente los lugares idle, es decir, el punto de partida de
			else m_0.push_back(0);							// de cada trayectoria.

		}
		processes.push_back(temp_processes); //Matriz en la que la fila i corresponde a la sucesión de regiones del proceso i
											 //Ej: dos robots--->dos trayectorias con tres puntos cada uno---> Matriz 2x3

	}
	
	set_to_null_matrix();				//Matriz Pre y Post de dimensiones num_lugares x num_transiciones, se inicializa cada componente a 0.
										//Nota: por ahora los recursos no están incluidos, puesto que todavía no se han obtenido.

	int count = 0;						//"count" es un contador que indica la fila de las matrices Pre y Post sobre la que se actúa, de modo que tras actuar sobre 
										//cada lugar (contenido), dicho contador se aumenta en una unidad para actuar sobre la siguiente fila correspondiente 
										//al siguiente lugar.
	int it = 0;							//"it" es un contador que indica la columna sobre la que se actúa sumado a "j", ya que los procesos están almacenados en vectores
										//diferentes.
	for (unsigned int i = 0; i < processes.size(); ++i) {
		
		for (unsigned int j = 0; j < processes[i].size(); ++j) {

			if (j == 0)																		//Caso lugar idle.
			{
				Pre[count][j + it] = 1;
				Post[count][processes[i].size() + it - 1] = 1;
			}
			else if (j == (processes[i].size() - 1))										//Caso estado final: antes de retornar al lugar idle.
			{
				Pre[count][j + it] = 1;
				Post[count][j + it - 1] = 1;
			}

			else {
				Pre[count][j + it] = 1;
				Post[count][j + it - 1] = 1;
			}
			++count;
		}
		it += processes[i].size();
	}
	//Obtención del número de recursos y su inclusión en matrices Pre y Post
	//Se analizan todos los lugares almacenados y se comparan (coordenada por coordenada, puesto que representan vertices) con el resto. Si coinciden, se crea un recurso
	//con marcado inicial 1, y se añade a las matrices Pre y Post.
	std::vector<int> rs_created;
	std::vector<int> vx;
	for (unsigned int i = 0; i < regiones.size(); ++i) {
		std::vector<int> temp_post_R;
		std::vector<int> temp_pre_R;
		int capacity = 0;
		//std::vector<int> vx;
		for (unsigned int k = 0; k < num_transiciones; ++k) {
			temp_pre_R.push_back(0);
			temp_post_R.push_back(0);
		}
		bool shared_resource_required = false;		//Si se requiere la creacion de un recurso, se cambia dicho valor a true, y permite entonces incluirlo en las matrices Pre,
													//Post, m_0 y resources
		bool single_resource_required = true;
		for (unsigned int j = 0; j < regiones.size(); ++j) {
			if (j > i){							//Se establece j>i para evitar dobles definiciones de rescursos, pues al aparecer dos veces una region, se definirian 2 recursos						
				if (regiones[i] == regiones[j]) {
					vx.push_back(j);
					if (r_not_created(regiones[j], rs_created)) {
						rs_created.push_back(regiones[j]);
						shared_resource_required = true;
					}
					temp_pre_R[i - 1] = 1;
					temp_pre_R[j - 1] = 1;
					temp_post_R[i] = 1;
					temp_post_R[j] = 1;
					//Se asigna la capacidad al recurso. Dado la capacidad se define cada vez que una region forma parte de una trayectoria, aunque los valores deben ser iguales, se comprueba si
					//lo son y en caso contrario se opta por el maximo de ellos.
					//Ejemplo: dos robots, c4 perteneciente a sendas trayectorias: 
					//			-definida en R1 con capacidad 1
					//			-definida en R2 con capacidad 2
					//No tiene sentido, podria deberse a un error en la introduccion de los datos, lo que se soluciona asignando el maximo
					if (capacity_by_region[i] == capacity_by_region[j]) {
						capacity = capacity_by_region[i];
					}
					else { capacity = std::max(capacity_by_region[i], capacity_by_region[j]); }	
				}
				/*if (!shared_resource_required) { single_resource_required = true; }
				else { single_resource_required = false; }*/
				if (shared_resource_required) { single_resource_required = false; }
			}
			if (!(r_not_created(i, vx))&& single_resource_required) { single_resource_required = false; }
		}
		//Se espera a salir del bucle, para que, en el caso de haber más de dos robots, el recurso se pueda enlazar con más de dos lugares.
		if (shared_resource_required) {
			num_lugares += 1;
			++lugar_1;
			resources.push_back(lugar_1);
			Pre.push_back(temp_pre_R);
			Post.push_back(temp_post_R);
			m_0.push_back(capacity);
		}
		if ( (single_resource_required) && (!is_idle(regiones[i])) ) {
			temp_pre_R[i - 1] = 1;
			temp_post_R[i] = 1;

			num_lugares += 1;
			++lugar_1;
			resources.push_back(lugar_1);
			Pre.push_back(temp_pre_R);
			Post.push_back(temp_post_R);
			m_0.push_back(capacity_by_region[i]);

		}

	}
	
	std::cout << std::endl;
	add_minus_1(&resources);
	for (unsigned int i = 0; i < processes.size(); ++i) {
		add_minus_1(&processes[i]);
	}
	
}


Input_S4PR::Input_S4PR(std::vector<std::vector<int>> paths, std::vector<std::vector<int>> capacities) {

	num_procesos = paths.size(); //Número de trayectorias obtenidas, una trayectoria por robot

	for (unsigned int i = 0; i < paths.size(); ++i) {
		std::vector<int> temp_path = paths[i];
		std::vector<int> temp_capacities = capacities[i];			//Tanto paths como capacities tienen las mismas dimensiones, puesto que a cada region le corresponde una capacidad
		std::vector<int> temp_processes;
		for (unsigned int j = 0; j < temp_path.size(); ++j) {
			num_lugares += 1;										//Cada punto del camino definido representa un lugar de proceso
			num_transiciones += 1;
			++lugar_1;
			//Se almacenan las regiones
			regiones.push_back(temp_path[j]);
			capacity_by_region.push_back(temp_capacities[j]);
			temp_processes.push_back(lugar_1);				//No se introduce el numero representativo de cada region en particular

			if (j == 0) {
				m_0.push_back(1);
				idle.push_back(temp_path[j]);
			}												//Marcado inicial: se consideran marcados únicamente los lugares idle, es decir, el punto de partida de
			else m_0.push_back(0);							// de cada trayectoria.

		}
		processes.push_back(temp_processes); //Matriz en la que la fila i corresponde a la sucesión de regiones del proceso i
											 //Ej: dos robots--->dos trayectorias con tres puntos cada uno---> Matriz 2x3

	}
	/*for (unsigned int i = 0; i < processes.size(); ++i) {
		std::cout << "Proceso" << i << " :\n";
		for (unsigned int j = 0; j < processes[i].size(); ++j) {
			std::cout << processes[i][j] << " ";
		}
		std::cout << std::endl;
	}*/

	set_to_null_matrix();				//Matriz Pre y Post de dimensiones num_lugares x num_transiciones, se inicializa cada componente a 0.
										//Nota: por ahora los recursos no están incluidos, puesto que todavía no se han obtenido.

	int count = 0;						//"count" es un contador que indica la fila de las matrices Pre y Post sobre la que se actúa, de modo que tras actuar sobre 
										//cada lugar (contenido), dicho contador se aumenta en una unidad para actuar sobre la siguiente fila correspondiente 
										//al siguiente lugar.
	int it = 0;							//"it" es un contador que indica la columna sobre la que se actúa sumado a "j", ya que los procesos están almacenados en vectores
										//diferentes.
	for (unsigned int i = 0; i < processes.size(); ++i) {

		for (unsigned int j = 0; j < processes[i].size(); ++j) {

			if (j == 0)																		//Caso lugar idle.
			{
				Pre[count][j + it] = 1;
				Post[count][processes[i].size() + it - 1] = 1;
			}
			else if (j == (processes[i].size() - 1))										//Caso estado final: antes de retornar al lugar idle.
			{
				Pre[count][j + it] = 1;
				Post[count][j + it - 1] = 1;
			}

			else {
				Pre[count][j + it] = 1;
				Post[count][j + it - 1] = 1;
			}
			++count;
		}
		it += processes[i].size();
	}

	//Obtención del número de recursos y su inclusión en matrices Pre y Post
	//Se analizan todos los lugares almacenados y se comparan (coordenada por coordenada, puesto que representan vertices) con el resto. Si coinciden, se crea un recurso
	//con marcado inicial 1, y se añade a las matrices Pre y Post.
	std::vector<int> rs_created;
	std::vector<int> vx;
	for (unsigned int i = 0; i < regiones.size(); ++i) {
		std::vector<int> temp_post_R;
		std::vector<int> temp_pre_R;
		int capacity = 0;
		//std::vector<int> vx;
		for (unsigned int k = 0; k < num_transiciones; ++k) {
			temp_pre_R.push_back(0);
			temp_post_R.push_back(0);
		}
		bool shared_resource_required = false;		//Si se requiere la creacion de un recurso, se cambia dicho valor a true, y permite entonces incluirlo en las matrices Pre,
													//Post, m_0 y resources
		bool single_resource_required = true;
		for (unsigned int j = 0; j < regiones.size(); ++j) {
			if (j > i) {							//Se establece j>i para evitar dobles definiciones de rescursos, pues al aparecer dos veces una region, se definirian 2 recursos						
				if (regiones[i] == regiones[j]) {
					vx.push_back(j);
					if (r_not_created(regiones[j], rs_created)) {
						rs_created.push_back(regiones[j]);
						shared_resource_required = true;
					}
					temp_pre_R[i - 1] = 1;
					temp_pre_R[j - 1] = 1;
					temp_post_R[i] = 1;
					temp_post_R[j] = 1;
					//Se asigna la capacidad al recurso. Dado la capacidad se define cada vez que una region forma parte de una trayectoria, aunque los valores deben ser iguales, se comprueba si
					//lo son y en caso contrario se opta por el maximo de ellos.
					//Ejemplo: dos robots, c4 perteneciente a sendas trayectorias: 
					//			-definida en R1 con capacidad 1
					//			-definida en R2 con capacidad 2
					//No tiene sentido, podria deberse a un error en la introduccion de los datos, lo que se soluciona asignando el maximo
					if (capacity_by_region[i] == capacity_by_region[j]) {
						capacity = capacity_by_region[i];
					}
					else { capacity = std::max(capacity_by_region[i], capacity_by_region[j]); }
				}
				/*if (!shared_resource_required) { single_resource_required = true; }
				else { single_resource_required = false; }*/
				if (shared_resource_required) { single_resource_required = false; }
			}
			if (!(r_not_created(i, vx)) && single_resource_required) { single_resource_required = false; }
		}
		//Se espera a salir del bucle, para que, en el caso de haber más de dos robots, el recurso se pueda enlazar con más de dos lugares.
		if (shared_resource_required) { //se requiere la creacion de un nuevo recurso compartido
			num_lugares += 1;
			++lugar_1;
			resources.push_back(lugar_1);
			Pre.push_back(temp_pre_R);
			Post.push_back(temp_post_R);
			m_0.push_back(capacity);
		}
		if ((single_resource_required) && (!is_idle(regiones[i]))) { //se requiere la creacion de un nuevo recurso privado. Se condiciona a que la region no corresponda a un lugar idle,
																	 //ya que estos no requieren la introducción de recursos según la definición de la subclase S4PR de redes de Petri.
			temp_pre_R[i - 1] = 1;
			temp_post_R[i] = 1;

			num_lugares += 1;
			++lugar_1;
			resources.push_back(lugar_1);
			Pre.push_back(temp_pre_R);
			Post.push_back(temp_post_R);
			m_0.push_back(capacity_by_region[i]);

		}

	}
	std::cout << std::endl;
	add_minus_1(&resources);
	for (unsigned int i = 0; i < processes.size(); ++i) {
		add_minus_1(&processes[i]);
	}
}


bool Input_S4PR::r_not_created(int x, std::vector<int> vec) {
	bool boolean = true;
	for (int i = 0; i < vec.size(); ++i) {
		if (x == vec[i]) { boolean = false; }
	}
	return boolean;
}

void Input_S4PR::set_to_null_matrix() {
	for (unsigned int i = 0; i < num_lugares; ++i) {
		std::vector<int> temp;
		for (unsigned int j = 0; j < num_transiciones; ++j) {
			temp.push_back(0);
		}
		Pre.push_back(temp);
		Post.push_back(temp);
	}
}

void Input_S4PR::add_minus_1(std::vector<int>* vec) {
	vec->push_back(-1);
}

bool Input_S4PR::is_idle(int place) {
	bool boolean = false;
	std::vector<int> vec = this->idle;
	for (int i = 0; i < vec.size(); ++i) {
		if (place == vec[i]) {
			boolean = true;
		}
	}
	return boolean;
}

int Input_S4PR::get_nprocesses() { return num_procesos; }
int Input_S4PR::get_dimP() { return num_lugares; }
int Input_S4PR::get_dimT() { return num_transiciones; }
std::vector<std::vector<int>> Input_S4PR::get_Pre() { return Pre; }
std::vector<std::vector<int>> Input_S4PR::get_Post() { return Post; }
std::vector<std::vector<int>> Input_S4PR::get_processes() { return processes; }
std::vector<int> Input_S4PR::get_m_0() { return m_0; }
std::vector<int> Input_S4PR::get_resources() { return resources; }
std::vector<std::vector<double>> Input_S4PR::get_points_involved() { return points_involved; }


void Input_S4PR::printParameters() {
	std::cout << "PARAMETERS S4PR PETRI NET" << std::endl;
	std::cout << "Number of processes:" << num_procesos << "\n";
	std::cout << "Number of places: " << num_lugares << "\n";
	std::cout << "NumberTrans:" << num_transiciones << "\n";

	for (unsigned int i = 0; i < (processes.size()); ++i) {
		std::cout << "Proceso " << i + 1 << " :\n";
		for (unsigned int j = 0; j < (processes[i].size() - 1); ++j) {
			std::cout << "p" << processes[i][j] << " ";
		}
		std::cout << std::endl;
	}

	std::cout << "Matriz Pre-incidencia:" << std::endl;

	for (unsigned int i = 0; i < Pre.size(); ++i) {
		for (unsigned int j = 0; j < Pre[i].size(); ++j) {
			std::cout << Pre[i][j] << " ";
		}
		std::cout << std::endl;
	}

	std::cout << "Matriz Post-incidencia:" << std::endl;

	for (unsigned int i = 0; i < Post.size(); ++i) {
		for (unsigned int j = 0; j < Post[i].size(); ++j) {
			std::cout << Post[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "Recursos" << " :\n";
	for (unsigned int i = 0; i < (resources.size() - 1); ++i) {

		std::cout << "p" << resources[i] << " ";

	}
	std::cout << std::endl;
	std::cout << "Marcado inicial:  " << std::endl;
	for (unsigned int i = 0; i < m_0.size(); ++i) {
		std::cout << m_0[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "--------------------------------------------------------------------" << std::endl;
}