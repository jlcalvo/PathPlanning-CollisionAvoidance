#include "petri.h"
//Javier Oroz Joven TFG 23-10-16

PetriNetClass::PetriNetClass(){
	nplaces = 0;
	ntrans = 0;
	std::vector<int> m0;
	Matrix<int> Pre;
	Matrix<int> Post;
	Matrix<int> C;
	IdlePlace=0;
}

PetriNetClass::~PetriNetClass(){
} //destructor al final hay que programar

void PetriNetClass::setValues(const int nplaces, const int ntrans, std::vector<int> m0,Matrix<int> Pre,Matrix<int> Post,Matrix<int> C){
	this->nplaces = nplaces;
	this->ntrans = ntrans;
	this->m0 = m0;
	this->Pre = Pre;
	this->Post = Post;
	this->C = C;
}

bool PetriNetClass::isStateMachine(){
	std::set<int> preS,postS,tr;
	for(int i=0;i<ntrans;i++){
		tr.clear();
		tr.insert(i+1);
		preS = this->preset(tr,'t',0);
		postS = this->postset(tr,'t',0);
		if ((preS.size()!=1)||(postS.size()!=1)){
			return 0;
		}
	}
	return 1;
}

bool PetriNetClass::isSiphon(std::set<int> SetOfPlaces){//devuelve true si el conjunto de lugares de entrada es un sifon
	std::set<int> inputTr, outputTr;

	inputTr = this->preset(SetOfPlaces,'p',0);
	outputTr = this->postset(SetOfPlaces,'p',0);
	inputTr = inputTr - outputTr;
	if (inputTr.size() == 0)
		return 1;
	else
		return 0;
	return 1;

}


std::set<int> PetriNetClass::postset(std::set<int> nodes, char nodeType, const bool print){
	// Calcula el conjunto posterior al conjunto de nodos, nodes[]
	// nodeType indica si es "p" lugar o "t" transicion
	// print es un booleano, si vale 1 se imprime el resultado, si vale 0 no.
	std::set<int> post;
	std::set<int>::iterator it;
	int i;

    if (print){
        cout << "The output (post) set of the set of";
        if (nodeType == 'p') {  cout << " places: "<< nodes;}
        if (nodeType == 't') {  cout << " transitions: "<< nodes;}
    }

    if (nodeType == 'p') {      // nodeType = Place  (LUGARES)
		for (it=nodes.begin(); it!=nodes.end(); it++){
        	for (i=0; i < ntrans; i++){
        		if (Pre(*it,i+1)>0) {      //buscamos valores positivos en la fila del nodo
        			post.insert(i+1);
        		}
        	}
        }
    }

    if (nodeType == 't') {      // nodeType = Place  (LUGARES)
		for (it=nodes.begin(); it!=nodes.end(); it++){
        	for (i=0; i < nplaces; i++){
        		if (Post(i+1,*it)>0) {      //buscamos valores positivos en la fila del nodo
        			post.insert(i+1);
        		}
        	}
        }
    }


    if (print){
    	if (nodeType == 'p') {  cout << " is the set of transitions:"<< post<<endl;}
    	if (nodeType == 't') {  cout << " is the set of places:"<< post<<endl;}
    }
    return post;
}

std::set<int> PetriNetClass::preset(std::set<int> nodes, char nodeType, const bool print){
	// Calcula el conjunto anterior al conjunto de nodos, nodes
	// nodeType indica si es "p" lugar o "t" transicion
	// print es un booleano, si vale 1 se imprime el resultado, si vale 0 no.
	std::set<int> pre;
	std::set<int>::iterator it;
	int i;

    if (print){
        cout << "The input (pre) set of the set of";
        if (nodeType == 'p') {  cout << " places: "<< nodes;}
        if (nodeType == 't') {  cout << " transitions: "<< nodes;}
    }

    if (nodeType == 'p') {      // nodeType = Place  (LUGARES)
		for (it=nodes.begin(); it!=nodes.end(); it++){
        	for (i=0; i < ntrans; i++){
        		if (Post(*it,i+1)>0) {      //buscamos valores positivos en la fila del nodo
        			pre.insert(i+1);
        		}
        	}
        }
    }

    if (nodeType == 't') {      // nodeType = Place  (LUGARES)
		for (it=nodes.begin(); it!=nodes.end(); it++){
        	for (i=0; i < nplaces; i++){
        		if (Pre(i+1,*it)>0) {      //buscamos valores positivos en la fila del nodo
        			pre.insert(i+1);
        		}
        	}
        }
    }


    if (print){
    	if (nodeType == 'p') {  cout << " is the set of transitions:"<< pre<<endl;}
    	if (nodeType == 't') {  cout << " is the set of places:"<< pre<<endl;}
    }
    return pre;
}


Matrix<int> PetriNetClass::semiflows(Matrix<int> C){
	//entradas: matriz C de la red de petri
	//salidas: se obtiene la matriz "semi" pero para sacarla fuera la almacenamos en un puntero
	//semiflow es un puntero en el que se almacena la matriz dicha en una sola fila
	// print es un booleano, si vale 1 se imprime el resultado, si vale 0 no.
    int i, j, k, m;
    int rowsA = C.get_nrows();
    int colsA = C.get_ncols();
    int A[rowsA][colsA];
    for (i=0; i < rowsA; i++){
        for (j=0; j < colsA; j++){
            A[i][j]=C(i+1,j+1);
        }
    }
    //----------------- Obtener matriz identidad -----------------
    int D[rowsA][rowsA];

    for (i=0; i < rowsA; i++){
        for (j=0; j < rowsA; j++){
            if (j == i){
                D[i][j]=1;
            }
            else{
                D[i][j]=0;
            }
        }
    }
    //---------------------------------- Fin matriz identidad ---------------

    //Numero maximo de filas que pueden tener las matrices -->
    // --> maximo de multiplicar los positivos por los negativos de cada columna!
    unsigned int pos = 0;       //contador numeros positivos
    unsigned int neg = 0;       //contador numeros negativos
    unsigned int maximo = 0;    //variable que guarda el maximo

    for (j=0; j < colsA; j++){
        for (i=0; i < rowsA; i++){
            if (A[i][j] > 0){
                pos++;
            }
            else if (A[i][j] < 0){
                neg++;
            }
        }
        if (maximo < (pos*neg)){
            maximo = pos*neg;
        }
        /*else {
            maximo = maximo;
        }*/
        pos = 0;
        neg = 0; //reinicio contadores

    }
    //----- Fin busqueda del maximo ----------------------------------

    int sig = 0; //variable auxiliar para el proceso
    int filas = 0; //contador para ver las filas totales de las matrices nA y nD
    int filas_copia = 0; //contador para ver las filas que mantengo
    int colD = rowsA;
    int rowsDA = rowsA;

    //Hacerlo de forma ineficiente, reservando mucho espacio en la pila de memoria
    int csemi[rowsA + maximo][colD];
    int semi[rowsA + maximo][colD];

    int nA[rowsA + maximo][colsA];
    int nD[rowsA + maximo][colD];

    int nA_copia[rowsA + maximo][colsA];
    int nD_copia[rowsA + maximo][colD];

    //Copio en nA y nD las matrices que ya tengo A y D
    for (i=0; i < rowsA; i++){
        for (j=0; j < colsA; j++){
            nA[i][j] = A[i][j];
        }
    }
    for (i=0; i < rowsA; i++){
        for (j=0; j < colD; j++){
            nD[i][j] = D[i][j];
        }
    }

    //___________________________________________________________________________________________________________________

    for (i=0; i<= (colsA-1); i++){
        int nA_inst[colsA];
        int nA1[colsA];
        int nA2[colsA];
        int nD_inst[rowsA]; //nï¿½ de columnas de D que es igual al nï¿½ de filas de A
        int nD1[rowsA];
        int nD2[rowsA];
        int z; //contador para copiar las filas
        filas = rowsDA; //Actualizo las filas cada vez que entre al bucle

        for (j=0; j < (rowsDA-1); j++){
            for (k=(j+1); k < rowsDA; k++){
                if (fabs(nA[k][i]) > 0){
                    sig = nA[j][i]*nA[k][i];
                }
                else {
                    sig = 1;
                }
                if (sig < 0){
                    for (z=0; z<colD; z++){
                        nD1[z] = nD[j][z];
                        nD2[z] = nD[k][z]; //copio las filas de nD con las que quiero operar
                    }
                    for (z=0; z < colsA; z++){
                        nA1[z] = nA[j][z];
                        nA2[z] = nA[k][z]; //copio las filas de nA con las que quiero operar
                    }
                    //ConstxVect(fabs(nA[k][i]), nD1, colD);
                    for (m=0; m<colD;m++){
                        nD1[m]=fabs(nA[k][i])*nD1[m];
                    }
                    //ConstxVect(fabs(nA[j][i]), nD2, colD);
                    for (m=0; m<colD;m++){
                        nD2[m]=fabs(nA[j][i])*nD2[m];
                    }
                    //SumaVect(nD1,nD2,nD_inst,rowsA);
                    for (m=0; m<rowsA; m++){
                        nD_inst[m]=nD1[m]+nD2[m];
                    }
                    //ConstxVect(fabs(nA[k][i]), nA1, colsA);
                    for (m=0; m<colsA;m++){
                        nA1[m]=fabs(nA[k][i])*nA1[m];
                    }
                    //ConstxVect(fabs(nA[j][i]), nA2, colsA);
                    for (m=0; m<colsA;m++){
                        nA2[m]=fabs(nA[j][i])*nA2[m];
                    }
                    //SumaVect(nA1,nA2,nA_inst,colsA);
                    for (m=0; m<colsA; m++){
                        nA_inst[m]=nA1[m]+nA2[m];
                    }

                    int r; //contador para aï¿½adir las nuevas filas debajo de las matrices nD y nA
                    for (r=0; r<colD; r++){
                        nD[filas][r] = nD_inst[r];
                        }
                    for (r=0; r<colsA; r++){
                        nA[filas][r] = nA_inst[r];
                        }
                    filas = filas + 1;
                }
            }
        } // fin del bucle j
        //Eliminar de A|D las filas en las que la i-esima columna de A sea no nula
        rowsDA = filas;
        filas_copia = 0; // lo reinicio si no aumenta otra vez y repite vectores
        for (j=0; j < rowsDA; j++){
            if (fabs(nA[j][i]) <= 0){
                for (z=0; z < colD; z++){
                    nD_copia[filas_copia][z] = nD[j][z];
                }
                for (z=0; z < colsA; z++){
                    nA_copia[filas_copia][z] = nA[j][z];
                }
                filas_copia++;
            }
        }
        //Compruebo si hay vectores y cambio los valores de nA y nD para repetir el bucle
        if (filas_copia >= 1 ){
            //Inicializo las matrices nD y nA a 0 para no tener problemas
            for (j=0; j < (rowsA + int(maximo)); j++){
                for (z=0; z < colD; z++){
                    nD[j][z] = 0;
                }
                for (z=0; z < colsA; z++){
                    nA[j][z] = 0;
                }
            }
            //Ahora copio los valores de nA_copia y nD_copia en nA y nD para repetir el bucle i:
            for (j=0; j < filas_copia; j++){
                for (z=0; z < colD; z++){
                    nD[j][z] = nD_copia[j][z];
                }
                for (z=0; z < colsA; z++){
                    nA[j][z] = nA_copia[j][z];
                }
            }
        } // Fin de copiar los nuevos valores de nD y nA
        rowsDA = filas_copia; //Si no hago esto repite el bucle j con las filas de la matriz inicial A
    } //fin del bucle for i, i<= (colsA-1)----------------------------------------------------------------------------------
    //En este punto nD es una matriz de enteros que contiene los semiflujos, pero pueden no ser minimos
    //y pueden estar repetidos.

    //Eliminacion de semiflujos que contienen a otros_______________________________________________
    int esse[filas_copia][1];
    for (i=0; i< filas_copia; i++){
        esse[i][0] = 1; //inicializo el vector columna a 1os
    }
    // Hago el support de la matriz nD aprovechando la variable nD_copia por no ocupar mas memoria
    for (i=0; i < filas_copia; i++){
        for (j=0; j < colD; j++){
            if (nD[i][j] > 1){
                nD_copia[i][j] = (nD[i][j]/nD[i][j]);
            }
            else {
                nD_copia[i][j] = nD[i][j];
            }
        }
    }
    //Ahora nD_copia es una matriz igual a nD pero llena de 1s o 0s.
    int uni[colD];
    int minim_i = 0;
    int minim_j = 0;
    for (i=0; i < (filas_copia - 1); i++){
        for (j=(i+1); j < filas_copia; j++){
            for (k=0; k < colD; k++){
                uni[k] = ((nD_copia[i][k]) | (nD_copia[j][k]));
            }
            // Obtengo el minimo de restar los vectores nD_copia en la fila i e uni
            minim_i = nD_copia[i][0] - uni[0];
            for (k=0; k < colD; k++){
                if ((nD_copia[i][k] - uni[k]) <= minim_i){
                    minim_i = nD_copia[i][k] - uni[k];
                }
            }
            // Obtengo el minimo de restar los vectores nD_copia en la fila j e uni
            minim_j = nD_copia[j][0] - uni[0];
            for (k=0; k < colD; k++){
                if ((nD_copia[j][k] - uni[k]) <= minim_j){
                    minim_j = nD_copia[j][k] - uni[k];
                }
            }
            if (minim_i >= 0){ // i contiene a j
                esse[i][0] = 0;
            }
            else if (minim_j >= 0){ //j contiene a i
                esse[j][0] = 0; //en caso de ser iguales solo se eliminara uno de los dos
            }
        }
    }
    j=0; //contador para saber cuantos semiflujos tengo sin repetir
    for (i=0; i < filas_copia; i++){
        if (esse[i][0] == 1){
            for (k=0; k < colD; k++){
                csemi[j][k] = nD[i][k];
            }
            j = j + 1;
        }
    }
    // Fin de eliminacion de semiflujos que contienen a otros_________________________________________________
    //    *numPsemiflows_C = j;
    int mcd = 0;
    //Division de todos los semiflujos por el maximo comun divisor
    for (i=0; i < j; i++){
        mcd = csemi[i][0];
        for (k=1; k < colD; k++){
                //Calculo del Maximo comï¿½n divisor entre A y B    FUNCION gcd(mcd, csemi[i][k])
                int a=mcd;
                int b=csemi[i][k];
                int result = 1;
                for (int i=1; i <= a; i++){
                    if ((a%i == 0)&&(b%i == 0)){
                        result = i;
                    }
                }
                //fin de gcd
            mcd = result;
        }
        if (mcd > 0){
            for (k=0; k < colD; k++){
                semi[i][k] = csemi[i][k]/mcd;
            }
        }
        else {
            for (k=0; k < colD; k++){
                semi[i][k] = csemi[i][k];
            }
        }
    }
    Matrix<int> semiflujos(j,colD);
    for (i=0; i < j; i++){
    	for (k=0; k< colD; k++){
    		semiflujos(i+1,k+1) = semi[i][k];
    	}
    }
	return semiflujos;
}


Matrix<int> PetriNetClass::Psemiflows(){
	if (p_semi.isEmpty()){
		p_semi = semiflows(C);
	}
	return p_semi;
}

Matrix<int> PetriNetClass::Tsemiflows(){
	if (t_semi.isEmpty()){
		t_semi = semiflows(C.get_transpose());
	}
	return t_semi;
}

int PetriNetClass::getIdle(){return IdlePlace;}
void PetriNetClass::setIdle(int value){IdlePlace=value;}
Matrix<int> PetriNetClass::getC(){return C;}
Matrix<int> PetriNetClass::getPre(){return Pre;}
Matrix<int> PetriNetClass::getPost(){return Post;}
std::vector<int> PetriNetClass::getM0(){return m0;}



S4PR::S4PR(){
	nplaces = 0;
	ntrans = 0;
	std::vector<int> m0;
	Matrix<int> Pre;
	Matrix<int> Post;
	Matrix<int> C;
	numProcesses = 0;
	numResources = 0;
	numPmonitor = 0;
	Matrix<int> matrixPs;
	std::set<int> P0;
	std::set<int> Ps;
	std::set<int> Pr;

}

S4PR::~S4PR(){
} //destructor al final hay que programar


void S4PR::read_from_file(FILE *fichero){

	int i,j,num;

	fscanf(fichero, "%d", &nplaces);
	fscanf(fichero, "%d", &ntrans);

	Post = Matrix<int>(nplaces,ntrans);
	Post.fill_from_file(fichero,nplaces,ntrans);

	Pre = Matrix<int>(nplaces,ntrans);
	Pre.fill_from_file(fichero,nplaces,ntrans);

	C = Matrix<int>(nplaces,ntrans);
	C = Post - Pre;

	//leer el numero de procesos
	fscanf(fichero, "%d", &numProcesses);

	//leer la matrix de procesos
	matrixPs = Matrix<int>(numProcesses,nplaces);
	for (i=0; i<numProcesses; i++){
		j=0;
		num = 0;
		while(num!=-1){
			fscanf(fichero, "%d", &num);
            matrixPs(i+1,j+1) = num;  //matriz en la que guardaremos todos los procesos
        	j++;
            }
		}

	//leer lugares de espera
    for (int i=0;i<numProcesses;i++){
        P0.insert(matrixPs(i+1,1));
    }

    //leer lugares de procesos
    num = 0;
    int aux[nplaces];

    for (i=1; i<=numProcesses; i++){
    	j=2;
    	while(matrixPs(i,j)!=-1){
    		aux[num++] = matrixPs(i,j++);
    	}
    }
    Ps.insert(aux,aux+num);

    //leer lugares de recursos
    numResources = 0;
    fscanf(fichero, "%d", &num);
    while(num!=-1){
        aux[numResources++]=num;  //matriz en la que guardaremos todos los recursos
        fscanf(fichero, "%d", &num);
    }
    Pr.insert(aux,aux+numResources);
    //lugares monitor
    numPmonitor=0;

    //leer el marcado inicial
    m0.resize(nplaces);
    for(i=0; i<nplaces; i++){
    	 fscanf(fichero, "%d", &m0[i]);
    }
}



void S4PR::read_from_file2(int dimP, int dimT, int nProcesses, vector<vector<int> > pre, vector<vector<int> > post, vector<vector<int> > Processes, vector<int> Resources, vector<int> m_0){
	
	int i,j,num;

	nplaces = dimP;
	ntrans = dimT;
	Post = Matrix<int>(nplaces,ntrans);
	Post.fill_from_file2(post);
//cout<<"bu1"<<endl;
	Pre = Matrix<int>(nplaces,ntrans);
	Pre.fill_from_file2(pre);
//	cout<<"bu2"<<endl;
	C = Matrix<int>(nplaces,ntrans);
	C = Post - Pre;
//	cout<<"bu3"<<endl;
	numProcesses = nProcesses;

	//leer la matrix de procesos
	matrixPs = Matrix<int>(numProcesses,nplaces);
	for (i=0; i<numProcesses; i++){
		std::cout << "P" << i << endl;
		j=0;
//		num = 0;
		vector<int> tPr = Processes[i];
		//std::cout << "1" << endl;
		num = tPr[j];
		//std::cout << "2" << endl;
		while(num!=-1){
			cout << "num = " << num << endl;
//			fscanf(fichero, "%d", &num);
			//std::cout << "3" << endl;
            matrixPs(i+1,j+1) = num;  //matriz en la que guardaremos todos los procesos
			//std::cout << "4" << endl;
        	j++;
        	num = tPr[j];
			//std::cout << "5" << endl;

            }
		std::cout << endl;
			matrixPs(i+1,j+1) = -1;
			//std::cout << "6" << endl;
		}
//	cout<<"bu4"<<endl;
	//leer lugares de espera
    for (int i=0;i<numProcesses;i++){
        P0.insert(matrixPs(i+1,1));
		std::cout << "P0: " << endl;
		std::cout << matrixPs(i + 1, 1)<< endl;
    }
	std::cout << "P0" << endl;
//    cout<<"bu5"<<endl;
    //leer lugares de procesos
    num = 0;
    int aux[nplaces];
	cout << "lugares de proceso:" << endl;

	for (i = 1; i <= numProcesses; i++) {
		j = 2;
		cout << "P:" << i << endl;
		while (matrixPs(i, j) != -1) {
			aux[num++] = matrixPs(i, j);
			cout << matrixPs(i, j) << " ";
			j++;
		}
		cout << endl;
		/*cout << "P:" << i << endl;
		while (matrixPs(i, j) != -1) {
			aux[num++] = matrixPs(i, j++);
			cout << matrixPs(i, j) << " ";

		}
		cout << endl;*/
	}
    Ps.insert(aux,aux+num);
//    cout<<"bu6"<<endl;
    //leer lugares de recursos
    numResources = 0;
//    fscanf(fichero, "%d", &num);
    int cy = 0;
    num = Resources[cy];
    while(num!=-1){
        aux[numResources++]=num;  //matriz en la que guardaremos todos los recursos
        //fscanf(fichero, "%d", &num);
        cy++;
        num = Resources[cy];
    }
    Pr.insert(aux,aux+numResources);
    //lugares monitor
    numPmonitor=0;
//    cout<<"bu7"<<endl;
    //leer el marcado inicial
    m0.resize(nplaces);
    for(i=0; i<nplaces; i++){
    	 //fscanf(fichero, "%d", &m0[i]);
    	m0[i] = m_0[i];
    }//cout<<"bu8"<<endl;
}






int S4PR::getPlaces(){return nplaces;}
int S4PR::getTransitions(){return ntrans;}

std::set<int> S4PR::getProcessPlaces(int process){
	std::set<int> ret;
	if ((process < 1) || (process > numProcesses)){
		throw Exception("Subscript out of range");
	}
	for(int i=2;i<=nplaces;i++){
		if ((matrixPs(process,i) != 0) && (matrixPs(process,i) != -1))
			ret.insert(matrixPs(process,i));
	}
	return ret;
}

PetriNetClass S4PR::getProcessNet(int process, bool print){
	if ((process < 1) || (process > numProcesses)){
		throw Exception("Subscript out of range");
	}
	std::set<int> new_set_of_places;
    std::vector<int> new_m0;

	for(int i=1;i<=nplaces;i++){
		if ((matrixPs(process,i) != 0) && (matrixPs(process,i) != -1)){
			new_set_of_places.insert(matrixPs(process,i));
		}
	}
	int idle_place;

	std::set<int>::iterator it,idleIt;
	idleIt = new_set_of_places.find(matrixPs(process,1));
	new_m0.resize(new_set_of_places.size());
	int i=0;
	if (print){
		cout <<"--------------------------------------------------------"<<endl;
		cout <<"getProcessNet(process="<<process<<")"<<endl;
		cout <<"--------------------------------------------------------"<<endl;
	}
	for (it=new_set_of_places.begin(); it!=new_set_of_places.end(); ++it){
		new_m0[i++]= this->m0[*it-1];
		if (print){
			cout<<"Place "<<*it<<" of original net is mapped to "<<i<<" in the process net."<<endl;
		}
		if (*it == *idleIt){
			idle_place = i;
		}
	}

	std::set<int> new_set_of_trans;
	new_set_of_trans = this->preset(new_set_of_places,'p',0) + this->postset(new_set_of_places,'p',0);

	Matrix<int> PreNew = this->Pre.subMatrix(new_set_of_places,new_set_of_trans);
	Matrix<int> PostNew = this->Post.subMatrix(new_set_of_places,new_set_of_trans);
	Matrix<int> CNew = this->C.subMatrix(new_set_of_places,new_set_of_trans);

	PetriNetClass NetRet;
	NetRet.setIdle(idle_place);
	NetRet.setValues(new_set_of_places.size(),new_set_of_trans.size(),new_m0,PreNew,PostNew,CNew);
	if (print){
		cout <<"Process idle place is "<<idle_place<<" while in the original one is "<<*idleIt<<endl;
		cout <<"--------------------------------------------------------"<<endl;
	}
	return NetRet;
}

int S4PR::getProcessNo(){return numProcesses;}
int S4PR::getResourceNo(){return numResources;}
int S4PR::getPmonitorNo(){return numPmonitor;}
std::set<int> S4PR::getP0(){return P0;}
std::set<int> S4PR::getPs(){return Ps;}
std::set<int> S4PR::getPr(){return Pr;}
std::set<int> S4PR::getPmonitor(){return Pmonitor;}

//la siguiente funcion devuelve el lugar de reposo del proceso que se indique "process"
int S4PR::getIdlePlace(int process){return matrixPs(process,1);}


bool S4PR::isCond1(){
	std::set<int> uu,temp;

	uu = P0 + Ps + Pr;
	std::cout << "idle: " << P0.size() << endl;
	std::cout << "lugar de proceso: " << Ps.size() << endl;
	std::cout << "recurso: " << Pr.size() << endl;

	for(int i=1;i<=nplaces;i++){
		temp.insert(i);
	}
	std::cout << "nplaces: " << temp.size()<< endl;
	if ((temp-uu).size() == 0)
		return 1;
	else{
		cout<<"P <> P0 cup Ps cup Pr"<<endl;
		return 0;
	}
}

bool S4PR::isCond2(){
	std::set<int> ti,tj,inter;
	for(int i=1;i<=numProcesses-1;i++){
		ti = this->preset(this->getProcessPlaces(i),'p',0);
		for(int j=i+1;j<=numProcesses;j++){
			tj = this->preset(this->getProcessPlaces(j),'p',0);
			inter = ti & tj;
			if (inter.size() != 0){
				cout<<"Subnets "<<i<< " and "<<j<<" has common transitions:"<< inter <<endl;
				return 0;
			}
		}

	}
	return 1;
}

bool S4PR::isCond3(bool print){

	std::vector<int> row;
	std::set<int> sup_row,placesNew;

	for(int i=1;i<=numProcesses;i++){
		PetriNetClass newPN= this->getProcessNet(i,print);
		if (!newPN.isStateMachine()){
			cout<<"Subnet N"<<i<<" is not a state machine"<<endl;
			return 0;
		}
		if (print) 	cout<<"Subnet N"<<i<<" is a state machine Petri net"<<endl;
		Matrix<int> tsemis = newPN.Tsemiflows();//calcular T semiflujos

		for(int j=1;j<=tsemis.get_nrows();j++){//coger un t-semiflujo
			row = tsemis.rowFromMatrix(j);
			sup_row=!row;//el soporte del vector row
			placesNew = newPN.postset(sup_row,'t',0) + newPN.preset(sup_row,'t',0); //calcular el conjunto de lugares del circuito
			if (placesNew.find(newPN.getIdle()) == placesNew.end()){
				cout <<"Cycle composed by the set of places: "<<placesNew<<" and set of transitions: "<<sup_row<<" id not containing idle place "<<newPN.getIdle()<<endl;
				return 0;
			}
			if (print){
				cout <<"Cycle composed by the set of places: "<<placesNew<<" and set of transitions: "<<sup_row<<" contains idle place "<<newPN.getIdle()<<endl;
			}
		}

	}
	return 1;
}

bool S4PR::isCond4(bool print){
	std::set<int>::iterator it;
	Matrix<int> pSemiflows = this->Psemiflows();//hago una copia de los psemiflujos de la red s4pr

	if (print){
		cout <<"--------------------------------------------------------"<<endl;
		cout <<"Checking condition 4"<<endl;
		cout <<"--------------------------------------------------------"<<endl;
	}

	for (it=Pr.begin(); it!=Pr.end(); ++it){
		if (print){
			cout<<"Resource place: "<<*it<<endl;
		}
		//obten yr y comprobar que es unico
		int found=0;  //entero para obtener el numero de fila en la matriz pSemiflows correspondiente a yr
		for(int i=1;i<=pSemiflows.get_nrows();i++){
			if (pSemiflows(i,*it)!=0){
				if (found>0){
					cout<<"Resource place belongs to more than one P-semiflow!"<<endl;
					return 0;
				}else found=i;
			}
		}
		if (print){cout<<"yr="<<pSemiflows.rowFromMatrix(found)<<endl;} //escribir yr si print=1
		//check if yr[r]=1
		if (pSemiflows(found,*it)!=1){
			cout<<"yr["<<*it<<"]!=1"<<endl;
			return 0;
		}
		std::set<int> supp=!pSemiflows.rowFromMatrix(found); // supp es un conjunto conteniendo el soporte de yr
		if (!(supp & P0).empty()){ //has la interseccion entre soporte de yr y P0 y comprobar que es vacia
			cout<<"P0 cap ||yr|| neq emptyset"<<endl;
			return 0;
		}
		if ((Pr & supp).size()!=1){ //calcular la interseccion entre soporte de yr y Pr y comprobar que tiene solo un elemento
			cout<<"||yr|| has more than one esource place"<<endl;
			return 0;
		}
		if ((Ps & supp).empty()){ //calcular la interseccion entre soporte de yr y Ps y comprobar que no es vacia
			cout<<"yr has no process place"<<endl;
			return 0;
		}
	}
	return 1;
}

bool S4PR::isCond5(bool print){
	std::set<int>::iterator it;
	std::set<int> computeUnion;
	Matrix<int> pSemiflows = this->Psemiflows();//hago una copia de los psemiflujos de la red s4pr
	if (print){
		cout <<"--------------------------------------------------------"<<endl;
		cout <<"Checking condition 5"<<endl;
		cout <<"--------------------------------------------------------"<<endl;
	}

	for (it=Pr.begin(); it!=Pr.end(); ++it){
		if (print){
			cout<<"Resource place: "<<*it<<endl;
		}
		//obten yr
		int found=0;  //entero para obtener el numero de fila en la matriz pSemiflows correspondiente a yr
		for(int i=1;i<=pSemiflows.get_nrows();i++){
			if (pSemiflows(i,*it)!=0) found=i;
		}
		if (print){cout<<"yr="<<pSemiflows.rowFromMatrix(found)<<endl;} //escribir yr si print=1
		computeUnion += !pSemiflows.rowFromMatrix(found);
	}
	computeUnion = computeUnion - Pr;
	if (print){
		cout<<"Union of the support of all yr setminus Pr:"<<computeUnion<<endl;
	}
	cout << "Ps " << Ps<< endl;
	cout << "Pr " << Pr << endl;
	if (!(Ps == computeUnion)){
		cout << "falla5";
		return 0; 
	}
	return 1;
}

bool S4PR::isWellDefinedS4PR(){
	return (this->isCond1() & this->isCond2() & this->isCond3(0) & this->isCond4(0) & this->isCond5(1));
}
std::vector<std::set<int> > S4PR::D1_Siphons(){  // this is a protected class
	//La salida es un vector de sets, es decir, una matriz, en la que se almacenan
	//el conjunto de los SIFONES de clase D1 de la red de petri
	bool print=0;
	if (print){
			cout<<"--------------------------------------------------------"<<endl;
			cout<< "D1 Siphons of the Petri Net:"<<endl<<"Minimal siphons with only 1 Resource place"<<endl;
			cout<<"--------------------------------------------------------"<<endl;
	}
	Matrix<int> pSemiflows;
	pSemiflows=this->Psemiflows();
	std::vector<std::set<int> > D1Siphons;
	std::set<int>::iterator it;
	std::set<int>::iterator iter;
	std::set<int> Siphon;
	std::set<int> newSiphon,newSiphon2;
	std::set<int> Pr=this->getPr();
	if (print){cout<<"Resource places:"<<Pr<<endl;}
	for (it=Pr.begin(); it!=Pr.end(); ++it){
		if (print){cout<<"Resource place: "<<*it<<endl;}
		//obten yr
		int found=0;  //entero para obtener el numero de fila en la matriz pSemiflows correspondiente a yr
		for(int i=1;i<=pSemiflows.get_nrows();i++){
			if (pSemiflows(i,*it)!=0) found=i;
		}
		if (print){cout<<"yr="<<pSemiflows.rowFromMatrix(found)<<endl;} //escribir yr si print=1
		Siphon = !pSemiflows.rowFromMatrix(found); //soporte de yr
		if (print){cout<<"Siphon=||yr||="<<Siphon<<endl;}
		bool stop=0;
		newSiphon = Siphon;
		while (stop==0){
			stop = 1;
			newSiphon2 = newSiphon;
			for (iter=newSiphon.begin(); iter!=newSiphon.end(); ++iter){
				newSiphon2.erase(*iter);   //  S\{p}
				if (print){cout<<"Remove:"<<newSiphon2<<endl;}
				if (this->isSiphon(newSiphon2)){
					if (print){cout<<"newSiphon="<< newSiphon2 <<endl;}
					stop = 0;
				}
				else {
					newSiphon2.insert(*iter);    //newSiphon es D1
					}
			}
			newSiphon = newSiphon2;
		}
		std::set<int> temp;
		for (iter=newSiphon.begin(); iter!=newSiphon.end(); ++iter){
			temp.insert(*iter);
		}
		D1Siphons.push_back(temp);
	}

	return D1Siphons;
}
std::set<int> S4PR::S(int r){ //Resource Labelling Function S(r)=Dr
//introducimos el numero que tiene el recurso en el conjunto global de lugares,i.e. "r"
//pero no la posicion que tiene en el conjunto Pr, i.e. "num"
	std::set<int> Set;
	std::set<int>::iterator it;
	int pos=0;
	for (it=Pr.begin();it!=Pr.end();it++){
		if (*it==r){Set = this->D1_Siphons()[pos];}
		pos++;
	}
	return Set;
}
//typedef pair<int,int> edge;
//typedef set<pair<int,int>> Edges;

std::vector<pair<int,int> > S4PR::L(int r, int x){ //Arc Labelling Function L(r,x)={t,p}
	//devuelve un vector en el que la primera posicion es "t" y la segunda "p"
	std::set<int> R; R.insert(r);
	std::set<int> X; X.insert(x);
	std::set<int> r_post=this->postset(R,'p',0);
	std::set<int> Trx=(this->postset(this->S(r),'p',0)) & (this->postset(this->S(x),'p',0));
	std::set<int> intersec= this->preset(Trx,'t',0) & this->S(x) & Ps;
	std::set<int> t=r_post & Trx & this->postset(intersec,'p',0);
	std::set<int> p=(this->preset(t,'t',0)) & Ps;
	std::vector<pair<int,int> > L; //la pareja de valores de salida
	std::set<int>::iterator it_t, it_p;
	//mejor hace bucle con t[] hasta t.size()
	it_p=p.begin();
	for (it_t=t.begin(); (it_t!=t.end() or it_p!=p.end()); it_t++){
		std::pair<int,int> edge;
		edge.first=*it_t;
		edge.second=*it_p;
		//if((edge.first & edge.second)!=0){
			L.push_back(edge);
		//}
		it_p++;
	}
	return L;
}
std::set<int> S4PR::KG(std::pair<Graph,std::vector<int> >Gpair, int r){   //Pruning Labelling Function KG(r)
	//ALGORITHM 1
	// input: resource of the siphon Dr from D1 for which we will compute the the prunning set in the graph
	//		  the graph to be prunned
	//output: set of process places that will be prunned from Dr
	std::set<int> Tf;
	std::set<int> Ppartial;
	std::set<int> Kg;
	std::vector<int> Vertices = Gpair.second;
	std::vector<int>::iterator x;
	for (x=Vertices.begin(); x!=Vertices.end(); ++x){
		std::vector<pair<int,int> >::iterator it;
		std::vector<pair<int,int> > vectorL=L(*x,r);
		int i=0;
		for (it=vectorL.begin(); it!=vectorL.end(); ++it){
			int t=vectorL[i].first;
			Tf.insert(t);   	//Tf definido
			i++;
		}
	}
	for (x=Vertices.begin(); x!=Vertices.end(); ++x){
		std::vector<pair<int,int> >::iterator it;
		std::vector<pair<int,int> > vectorL=L(*x,r);
		int i=0;
		for (it=vectorL.begin(); it!=vectorL.end(); ++it){
			int p=vectorL[i].second;
			std::set<int> place; place.insert(p);
			std::set<int> p_post = this->postset(place,'p',0);
			if ((p_post & Tf) != p_post){
				Ppartial.insert(p); //Ppartial definido
			}
			if ((p_post & Tf) == p_post){  // es decir, si p_post es subconjunto de Tf
				Kg.insert(p);		//Kg definido
			}
			i++;
		}
	}
	std::set<int> R; R.insert(r);
	std::set<int> Tnew= (this->preset(Kg,'p',0)) - (this->postset(R,'p',0));
	while (Tnew.empty() != true){
		Tf= Tf + Tnew;
		std::set<int> Pnew= this->preset(Tnew,'t',0) & Ps & this->S(r);
		std::set<int>::iterator it;
		std::set<int> A;
		for (it=Pnew.begin(); it!=Pnew.end(); ++it){
			std::set<int> place; place.insert(*it);
			std::set<int> p_post = this->postset(place,'p',0);
			if ((p_post & Tf) == p_post){  // es decir, si p_post es subconjunto de Tf
				A+=place;
			}
		}
		std::set<int> B;
		for (it=Ppartial.begin(); it!=Ppartial.end(); ++it){
			std::set<int> place; place.insert(*it);
			std::set<int> p_post = this->postset(place,'p',0);
			if ((p_post & Tf) == p_post){// es decir, si p_post es subconjunto de Tf
				B+=place;
			}
		}
		std::set<int> C;
		for (it=Pnew.begin(); it!=Pnew.end(); ++it){
			std::set<int> place; place.insert(*it);
			std::set<int> p_post = this->postset(place,'p',0);
			if ((p_post & Tf) != p_post){
				C+=place;
			}
		}
		Kg= Kg + A + B;
		Ppartial=(Ppartial - B) + C;
		Tnew= this->preset((A+B),'p',0) - this->postset(R,'p',0);
		A.clear(); B.clear(); C.clear();
	}
	return Kg;  //Kg es el conjunto de lugares candidatos a ser podados
}

typedef adjacency_list<vecS,vecS,directedS> Graph;

std::vector<int> S4PR::remove_i_from_vector(unsigned int index, std::vector<int> V){
	//V is the vector which contains the vertex labels of the graph
	//the function removes from V the position of the vector given by index
	std::vector<int> Vout;
	for(vector<int>::size_type i=0; i<V.size();i++){
		if (i!=index){
			Vout.push_back(V[i]);
		}
	}
	return Vout;
}
std::vector<int> S4PR::removeSet_from_v_label(std::set<int> Set, std::vector<int> V){
	//Set es un conjunto de entrada que contiene las etiquetas de los vertices a eliminar, es decir (14,15...)
	//la funcion elimina de V los vertices que aparecen en Set
	std::set<int> indices;
	std::set<int>::iterator it;
	for (it=Set.begin();it!=Set.end();++it){
		for(std::vector<int>::size_type j=0;j<V.size();j++){
			if ((*it)==V[j])
				indices.insert(j);
		}
	}
	std::vector<int> Vout=V;
	for (it=indices.begin();it!=indices.end();++it){
		Vout=remove_i_from_vector(*it,Vout);
	}
	return Vout;
}
std::vector<int> S4PR::remove_indices_from_v(set<int>indices,std::vector<int>V){
	// elimina un conjunto de indices de un vector
	std::vector<int> V_out=V;
	std::set<int>::iterator it;
	int num_removed_indices=0;
	for (it=indices.begin();it!=indices.end();it++){
		unsigned int index=*it;
		index=index-num_removed_indices; //menos la primera iteracion, el resto habrÃ¡ que restar los vertices eliminados
		V_out=remove_i_from_vector(index,V_out); //cada vez que elimino un vertice, los indices cambian
		num_removed_indices++;
	}
	return V_out;
}
Graph S4PR::remove_vertices_from_g(set<int>indices,Graph G_in){
	// elimina un conjunto de indices, es decir, vertices de un Grafo y SUS ARCOSSS
	Graph G_out=G_in;
	std::set<int>::iterator it;
	int num_removed_indices=0;
	Graph::vertex_descriptor vert;
	for (it=indices.begin();it!=indices.end();it++){
		vert=*it;
		clear_vertex(vert,G_out);
	}
	for (it=indices.begin();it!=indices.end();it++){
		int index=*it;
		index=index-num_removed_indices; //menos la primera iteracion, el resto habrÃ¡ que restar los vertices eliminados
		remove_vertex(index,G_out); //cada vez que elimino un vertice, los indices cambian
		num_removed_indices++;
	}
	return G_out;
}
std::pair<Graph,std::vector<int> > S4PR::removeSet_from_graph_pair(
									std::set<int> Set,
									std::pair<Graph,std::vector<int> > in_graph)
{
	//Set es un conjunto de entrada que contiene las etiquetas de los vertices a eliminar
	//la funcion elimina de V y G los vertices que aparecen en Set
	Graph G_in=in_graph.first;
	std::vector<int> V_in=in_graph.second;
	Graph G_out;
	std::vector<int> V_out=V_in;
	std::set<int> indices;
	std::set<int>::iterator it;
	for (it=Set.begin();it!=Set.end();++it)
		for(std::vector<int>::size_type j=0;j<V_in.size();j++)
			if ((*it)==V_in[j])
				indices.insert(j);   //ahora ya se los indices que tengo que eliminar

	G_out=remove_vertices_from_g(indices,G_in);
	for (it=indices.begin();it!=indices.end();++it){
		V_out=remove_i_from_vector(*it,V_out);
	}
	std::pair<Graph,std::vector<int> > out_graph;
	out_graph.first=G_out;
	out_graph.second=V_out;
	return out_graph;
}
std::vector<std::pair<Graph, vector<int> > > S4PR::ComputeSCC(std::pair<Graph,std::vector<int> > Gpair){
	//Calcula las componentes fuertemente conexas (strong components) del grafo de entrada
	//y devuelve un vector con todos los subgrafos obtenidos
	Graph g=Gpair.first;
	std::vector<int> VertexLabels=Gpair.second;

	typedef graph_traits<adjacency_list<vecS, vecS, directedS> >::vertex_descriptor Vertex;
	std::vector<int> component(num_vertices(g)), discover_time(num_vertices(g));
	std::vector<default_color_type> color(num_vertices(g));
	std::vector<Vertex> root(num_vertices(g));
	int num_scc = strong_components(g, make_iterator_property_map(component.begin(), get(vertex_index, g)),
							  root_map(make_iterator_property_map(root.begin(), get(vertex_index, g))).
							  color_map(make_iterator_property_map(color.begin(), get(vertex_index, g))).
							  discover_time_map(make_iterator_property_map(discover_time.begin(), get(vertex_index, g))));
	// en num_scc se almacenan el numero de componentes fuertemente conexas

	std::vector<std::pair<Graph, vector<int> > > GraphsVector;
	for (int n_comp=0; n_comp<num_scc; n_comp++){  //"num_scc" es el numero de strong components
		std::set<int> Vertices_to_remove;
		vector<int>::size_type j;   //vertices iterator 0,1,2,3....
		for (j=0; j<component.size(); ++j){
			if (component[j]!=n_comp){  //si la componente de vertice j no pertenece a la componenete n_comp
				Vertices_to_remove.insert(j);		//entonces eliminamos el vertice j del grafo
			}
		}
		std::vector<int> newVertexLabels=remove_indices_from_v(Vertices_to_remove,VertexLabels);
		Graph g_i=remove_vertices_from_g(Vertices_to_remove,g);
		std::pair<Graph, vector<int> > temp_graph;
		temp_graph.first=g_i;
		temp_graph.second=newVertexLabels;
		GraphsVector.push_back(temp_graph);
	}
	return GraphsVector;
}
bool S4PR::is_contained(std::vector<int> V1,std::vector<int> V2){
	// devuelve TRUE si el vector V1 es subconjunto de V2
	std::set<int> S1,S2;
	std::vector<int>::iterator it;
	for (it=V1.begin();it!=V1.end();++it){
		S1.insert(*it);
	}
	for (it=V2.begin();it!=V2.end();++it){
		S2.insert(*it);
	}
	bool is_subset=0;
	if (((S1+S2)==S2)){  //si S1 es subconjunto de S2
		is_subset=1;
	}
	return is_subset;
}
int S4PR::resource2vertex(int resource){
//transforma un recurso numerado segun la numeracion de la red de petri
// a un vertice de un grafo teniendo en cuenta que el vertice 0 sera el primer lugar de recurso

	std::set<int> Resources=this->getPr();
	int vertex;
	int i=0;
	for (std::set<int>::iterator r=Resources.begin(); r!=Resources.end(); ++r){
		if(*r==resource){
			vertex=i;
		}
		i++;
	}
	return vertex;
}
std::set<int> S4PR::SG(std::pair<Graph,std::vector<int> >in_graph){
	// siendo Dr el conjunto de recurso del grafo G
	//paratodo r perteneciente a Dr:
	//devuelve un conjunto Sg = Union_para_r [S(r)-Kg(r)]
	Graph G = in_graph.first;
	std::vector<int> v_labels = in_graph.second;

	std::set<int> Sg;
	for(std::vector<int>::iterator r=v_labels.begin(); r!=v_labels.end();++r){
		Sg=Sg+((this->S(*r))-(this->KG(in_graph,*r)));
	}
	return Sg;
}

//std::vector<std::set<int> > S4PR::getSiphons(){
//	std::vector<std::set<int> > D1 = this->D1_Siphons();
//
//	std::vector<std::set<int> > Siphons = this->D1_Siphons(); //1. D:=D1
//	//2. Compute de pruning Graph G=(V,E) and the functions S, L, Kg
//	std::set<int> Resources=this->getPr(); //los vertices seran los lugares Pr de recurso
//	int numV=Resources.size();
//
//	typedef adjacency_list< vecS, vecS, directedS/*,  vertexName*/ > Graph;
//        /*		 listS: Store out-edges of each vertex in a std::list
//                 vecS: Store vertex set in a std::vector
//                 directedS: selects directed edges
//                 no_property: vertex properties
//                 no_property: edge properties*/
//	Graph g(numV);
//
//	for (std::set<int>::iterator r=Resources.begin(); r!=Resources.end(); ++r){
//		std::set<int> R;
//		R.insert(*r);
//		std::set<int>::iterator x;
//		for (x=Resources.begin(); x!=Resources.end(); ++x){
//				std::set<int> r_post=this->postset(R,'p',0);
//				std::set<int> Dr=this-> S(*r);
//				std::set<int> Dx=this-> S(*x);
//				std::set<int> Trx=this->postset(Dr,'p',0) & this->postset(Dx,'p',0);
//				std::set<int> intersec=(this->preset(Trx,'t',0) & Dx & Ps);
//				std::set<int> Ur=r_post & Trx & this->postset(intersec,'p',0);
//				// es decir Ur= r* & Trx & (*Trx & Dx & Ps)* with Trx= (Dx* & Dr*)
//				if (!(Ur.empty())){
//				// anadimos un arco entre los recursos r y  x si Ur es vacio
//					int vert_r=resource2vertex(*r);
//					int vert_x=resource2vertex(*x);
//					add_edge(vert_r, vert_x, g);
//				}
//		}
//		R.clear(); //eliminamos el recurso *it_r del conjunto R
//	}
//	std::vector<int> VertexLabels;
//	for (std::set<int>::iterator r=Resources.begin(); r!=Resources.end(); ++r){
//		VertexLabels.push_back(*r);
//	}
//	// Creamos una pareja Gpair que contendra
//	// primero el grafo con sus vertices (ej: 0,1,2,3...)
//	// segundo un vector en el que se almacenan los recursos (ej:14,15,16...) correspondientes a cada vertice
//	std::pair<Graph,std::vector<int> >Gpair;
//	Gpair.first=g;
//	Gpair.second=VertexLabels;
//
//	//2. Compute th funtions S, L, Kg
//
//	//3. Compute all maximal strongly connected subgraphs
//	std::vector<std::pair<Graph, vector<int> > > g_new=ComputeSCC(Gpair);
//	//4. g_good=0
//	std::vector<std::pair<Graph, vector<int> > > g_good; g_good.clear();
//
//	while (g_new.empty() == false){ //5. while (g_new not empty)
//		//6. extract G'=(V',E') from g_new
//		std::pair<Graph, vector<int> > pairG_i = *(g_new.begin()); //pairG_ es G'
//		//ahora creo nuevo vector temporal pero sin la primera componente que es la que extraigo
//		std::vector<std::pair<Graph, vector<int> > > g_new_temp (g_new.begin()+1, g_new.end());
//		g_new=g_new_temp;
//		Graph G_i=(pairG_i).first;
//		std::vector<int> V_i=(pairG_i).second; // V_i = V' = set of Vertices of the graph G_i from g_new
//
//		//7. if there exists r in V' such that Kg(r)=0
//		bool exists_in_Vi=0;
//		set<int> R;
//		for (std::vector<int>::iterator r=V_i.begin(); r!=V_i.end(); ++r){
//			if (KG(pairG_i,*r).empty()==true){
//				R.insert(*r);
//				exists_in_Vi=1;
//			}
//		}
//		if (exists_in_Vi==1){ //7. if there exists r in V' such that Kg(r)=0
//			std::set<int> A;
//			std::set<int>::iterator res;
//			for (res=R.begin(); res!=R.end(); res++){ // R contiene los recursos que cumplen que Kg(r)=0
//				A.insert(*res); //8. Compute A=
//			}
//
//			//9. if there is no graph in g_new nor in g_good with vetrices = V'-A
//			bool exists_in_g_new=0;
//			std::vector<std::pair<Graph, vector<int> > >::iterator it_g;
//			for (it_g=g_new.begin(); it_g!=g_new.end(); it_g++){
//				vector<int> V1=(*it_g).second;
//				vector<int> V2=removeSet_from_v_label(A,V_i);
//				if (V1==V2) exists_in_g_new=1;
//			}
//			bool exists_in_g_good=0;
//			for (it_g=g_good.begin(); it_g!=g_good.end(); it_g++){
//				vector<int> V1=(*it_g).second;
//				vector<int> V2=removeSet_from_v_label(A,V_i);
//				if (V1==V2) exists_in_g_good=1;
//			}
//			//9. if there is no graph in g_new NOR in g_good
//			if (!((exists_in_g_new)||(exists_in_g_good))){
//
//				//10. Compute subgraph of G' with V'-A, named G''
//				std::pair<Graph,std::vector<int> >pairG_ii;  //grafo
//				pairG_ii=removeSet_from_graph_pair(R,pairG_i);
//				//11. Compute scc of G''
//				std::vector<std::pair<Graph,std::vector<int> > > set_G_iii;  //conjunto de grafos
//				set_G_iii=ComputeSCC(pairG_ii);
//				std::vector<std::pair<Graph,std::vector<int> > >::iterator G_iii;   //iterador de conjunto de grafos = *grafo
//				int numSCC=0;
//				for(G_iii=set_G_iii.begin();G_iii!=set_G_iii.end();++G_iii){ //11. for each G'''
//
//					bool less_than_1_node=0;
//					if (((*G_iii).second).size()<=1){
//						less_than_1_node=1;
//					}
//					bool contained_in_gnew = 0;
//					std::vector<std::pair<Graph, vector<int> > >::iterator it_gnew; //iterador de conjunto de grafos = *grafo
//					for (it_gnew=g_new.begin(); ((it_gnew!=g_new.end()) && !contained_in_gnew); it_gnew++){
//						if (((*G_iii).second)==((*it_gnew).second)){
//							contained_in_gnew=1;
//						}
//					}
//					if((!less_than_1_node)&&(!contained_in_gnew)){
//						//Compute the functions S, L and KG to form the PG G'''
//						std::vector<int>::iterator it_Giii; //iterador del vector (*G_iii).second del grafo *G_iii
//						for (it_Giii=((*G_iii).second).begin();it_Giii!=((*G_iii).second).begin();++it_Giii){
//							int res=*it_Giii; //asÃ­ recorremos todos los lugares de recurso del grafo Giii
//							std::set<int> Kgiii=KG(*G_iii,res);
//						}
//						//add G''' to the g_new
//						g_new.push_back(*G_iii);
//					}
//					numSCC++;
//				}
//			}//CIERRA IF 9.
//		}//CIERRA IF 7.
//		else{ //13.
//			g_good.push_back(pairG_i); //14. add G' to g_good
//			//15. for each r from V'
//			for (std::vector<int>::iterator r=V_i.begin(); r!=V_i.end(); ++r){
//				std::set<int> R; R.insert(*r);
//				//16. if there is no graph in g_new nor in g_good with vetrices = V'-r
//				bool exists_in_g_new=0;
//				std::vector<std::pair<Graph, vector<int> > >::iterator it_g;
//				for (it_g=g_new.begin(); it_g!=g_new.end(); it_g++){
//					vector<int> V1=(*it_g).second;
//					vector<int> V2=removeSet_from_v_label(R,V_i);
//					if (V1==V2) exists_in_g_new=1;
//				}
//				bool exists_in_g_good=0;
//				for (it_g=g_good.begin(); it_g!=g_good.end(); it_g++){
//					vector<int> V1=(*it_g).second;
//					vector<int> V2=removeSet_from_v_label(R,V_i);
//					if (V1==V2) exists_in_g_good=1;
//				}
//				//if there is no graph in g_new NOR in g_good
//				if (!((exists_in_g_new)||(exists_in_g_good))){
//
//					//17.Compute the prunning subgrpah of G' ->  V' - r, named G''
//					std::pair<Graph,std::vector<int> >pairG_ii;  //grafo
//					pairG_ii=removeSet_from_graph_pair(R,pairG_i);
//					//18. Compute scc of G''
//					std::vector<std::pair<Graph,std::vector<int> > > set_G_iii;  //conjunto de grafos
//					set_G_iii=ComputeSCC(pairG_ii);
//					std::vector<std::pair<Graph,std::vector<int> > >::iterator G_iii;   //iterador de conjunto de grafos = *grafo
//					int (numSCC)=0;
//					for(G_iii=set_G_iii.begin();G_iii!=set_G_iii.end();++G_iii){ //18. for each G'''
//						bool less_than_1_node=0;
//						if (((*G_iii).second).size()<=1){
//							less_than_1_node=1;
//						}
//						bool contained_in_gnew = 0;
//						std::vector<std::pair<Graph, vector<int> > >::iterator it_gnew; //iterador de conjunto de grafos = *grafo
//						for (it_gnew=g_new.begin(); ((it_gnew!=g_new.end()) && !contained_in_gnew); it_gnew++){
//							if (((*G_iii).second)==((*it_gnew).second)){
//								contained_in_gnew=1;
//							}
//						}
//						if((!less_than_1_node)&&(!contained_in_gnew)){
//							//Compute the functions S, L and KG to form the PG G'''
//							std::vector<int>::iterator it_Giii; //iterador del vector (*G_iii).second del grafo *G_iii
//							for (it_Giii=((*G_iii).second).begin();it_Giii!=((*G_iii).second).begin();++it_Giii){
//								int res=*it_Giii; //asi recorremos todos los lugares de recurso del grafo Giii
//								std::set<int> Kgiii=KG(*G_iii,res);
//							}
//							//add G''' to the g_new
//							g_new.push_back(*G_iii);
//						}
//						numSCC++;
//					}
//				}//CIERRA IF 16.
//			}//CIERRA FOR 15.
//		}//CIERRA ELSE 13.
//	}//CIERRA WHILE 5.
//
//	std::vector<std::pair<Graph, vector<int> > >::iterator it_graph;
//	//23. for all G in g_good, if exists another G'
//	//satisfying: (1) V' is subset of V (or the same set) and (2) SG' is subset of SG
//	std::vector<std::pair<Graph, vector<int> > > graphs_to_remove;
//	for (it_graph=g_good.begin();it_graph!=g_good.end()-1;it_graph++){
//		vector<int> V = (*it_graph).second;
//		set<int> Sg = SG(*it_graph);
//		std::vector<std::pair<Graph, vector<int> > >::iterator it_graph_i;
//		for (it_graph_i=it_graph+1;it_graph_i!=g_good.end();it_graph_i++){
//			vector<int> V_i = (*it_graph_i).second;
//			bool condition1=0;
//			bool condition2=0;
//			if (is_contained(V_i,V)){  //si V_i es subconjunto de V
//				condition1=1;
//			}
//			set<int> Sg_i = SG(*it_graph_i);
//			if ((Sg_i+Sg)==Sg){  //si Sg_i es subconjunto de Sg
//				condition2=1;
//			}
//			if (condition1 && condition2){
//				graphs_to_remove.push_back(*it_graph_i);
//			}
//		}
//	}
//
//	std::vector<std::pair<Graph, vector<int> > > g_good_temp;
//	for (it_graph=g_good.begin();it_graph!=g_good.end();it_graph++){
//		vector<int> V = (*it_graph).second;
//		bool remove=0;
//		std::vector<std::pair<Graph, vector<int> > >::iterator it_graph_i;
//		for (it_graph_i=graphs_to_remove.begin();((it_graph_i!=graphs_to_remove.end())&& !remove);it_graph_i++){
//			vector<int> V_i = (*it_graph_i).second;
//			if (V==V_i){
//				remove=1;
//			}
//		}
//		if (!remove){
//			g_good_temp.push_back(*it_graph);
//		}
//	}
//	g_good=g_good_temp;
//
//	for (it_graph=g_good.begin();it_graph!=g_good.end();it_graph++){ //24. esto es asi seguro
//		std::set<int>newSiphon = this->SG(*it_graph);
//		Siphons.push_back(newSiphon);//25. Add to Siphons, D=SG'.
//	}
//	//ALGORITMO FINITO
//	siphons=Siphons;
//	return (Siphons);
//}

std::vector<std::set<int> > S4PR::getSiphons(){
	
	std::vector<std::set<int> > D1 = this->D1_Siphons();
	std::vector<std::set<int> > Siphons = this->D1_Siphons(); //1. D:=D1
	//2. Compute de pruning Graph G=(V,E) and the functions S, L, Kg
	std::set<int> Resources=this->getPr(); //los vertices seran los lugares Pr de recurso
	int numV=Resources.size();

	typedef adjacency_list< vecS, vecS, directedS/*,  vertexName*/ > Graph;
        /*		 listS: Store out-edges of each vertex in a std::list
                 vecS: Store vertex set in a std::vector
                 directedS: selects directed edges
                 no_property: vertex properties
                 no_property: edge properties*/
	Graph g(numV);
	for (std::set<int>::iterator r=Resources.begin(); r!=Resources.end(); ++r){
		std::set<int> R;
		R.insert(*r);
		std::set<int>::iterator x;
		for (x=Resources.begin(); x!=Resources.end(); ++x){
				std::set<int> r_post=this->postset(R,'p',0);
				std::set<int> Dr=this-> S(*r);
				std::set<int> Dx=this-> S(*x);
				std::set<int> Trx=this->postset(Dr,'p',0) & this->postset(Dx,'p',0);
				std::set<int> intersec=(this->preset(Trx,'t',0) & Dx & Ps);
				std::set<int> Ur=r_post & Trx & this->postset(intersec,'p',0);
				// es decir Ur= r* & Trx & (*Trx & Dx & Ps)* with Trx= (Dx* & Dr*)
				if (!(Ur.empty())){
				// anadimos un arco entre los recursos r y  x si Ur es vacio
					int vert_r=resource2vertex(*r);
					int vert_x=resource2vertex(*x);
					add_edge(vert_r, vert_x, g);
				}
		}
		R.clear(); //eliminamos el recurso *it_r del conjunto R
	}
	std::vector<int> VertexLabels;
	for (std::set<int>::iterator r=Resources.begin(); r!=Resources.end(); ++r){
		VertexLabels.push_back(*r);
	}
	// Creamos una pareja Gpair que contendra
	// primero el grafo con sus vertices (ej: 0,1,2,3...)
	// segundo un vector en el que se almacenan los recursos (ej:14,15,16...) correspondientes a cada vertice
	std::pair<Graph,std::vector<int> >Gpair;
	Gpair.first=g;
	Gpair.second=VertexLabels;

	//2. Compute th funtions S, L, Kg

	//3. Compute all maximal strongly connected subgraphs
	std::vector<std::pair<Graph, vector<int> > > g_new=ComputeSCC(Gpair);
	//4. g_good=0
	std::vector<std::pair<Graph, vector<int> > > g_good; g_good.clear();

	while (g_new.empty() == false){ //5. while (g_new not empty)
		//6. extract G'=(V',E') from g_new
		std::pair<Graph, vector<int> > pairG_i = *(g_new.begin()); //pairG_ es G'
		//ahora creo nuevo vector temporal pero sin la primera componente que es la que extraigo
		std::vector<std::pair<Graph, vector<int> > > g_new_temp (g_new.begin()+1, g_new.end());
		g_new=g_new_temp;
		//cout<<"g_new size prima "<<g_new.size()<<endl;
		Graph G_i=(pairG_i).first;
		std::vector<int> V_i=(pairG_i).second; // V_i = V' = set of Vertices of the graph G_i from g_new

		//7. if there exists r in V' such that Kg(r)=0
		bool exists_in_Vi=0;
		set<int> R;
		for (std::vector<int>::iterator r=V_i.begin(); r!=V_i.end(); ++r){
			if (KG(pairG_i,*r).empty()==true){
				R.insert(*r);
				exists_in_Vi=1;
			}
		}
		if (exists_in_Vi==1){ //7. if there exists r in V' such that Kg(r)=0
			std::set<int> A;
			std::set<int>::iterator res;
			for (res=R.begin(); res!=R.end(); res++){ // R contiene los recursos que cumplen que Kg(r)=0
				A.insert(*res); //8. Compute A=
			}

			//9. if there is no graph in g_new nor in g_good with vetrices = V'-A
			bool exists_in_g_new=0;
			std::vector<std::pair<Graph, vector<int> > >::iterator it_g;
			for (it_g=g_new.begin(); it_g!=g_new.end(); it_g++){
				vector<int> V1=(*it_g).second;
				vector<int> V2=removeSet_from_v_label(A,V_i);
				if (V1==V2) exists_in_g_new=1;
			}
			bool exists_in_g_good=0;
			for (it_g=g_good.begin(); it_g!=g_good.end(); it_g++){
				vector<int> V1=(*it_g).second;
				vector<int> V2=removeSet_from_v_label(A,V_i);
				if (V1==V2) exists_in_g_good=1;
			}
			//9. if there is no graph in g_new NOR in g_good
			if (!((exists_in_g_new)||(exists_in_g_good))){

				//10. Compute subgraph of G' with V'-A, named G''
				std::pair<Graph,std::vector<int> >pairG_ii;  //grafo
				pairG_ii=removeSet_from_graph_pair(R,pairG_i);
				//11. Compute scc of G''
				std::vector<std::pair<Graph,std::vector<int> > > set_G_iii;  //conjunto de grafos
				set_G_iii=ComputeSCC(pairG_ii);
				std::vector<std::pair<Graph,std::vector<int> > >::iterator G_iii;   //iterador de conjunto de grafos = *grafo
				int numSCC=0;
				for(G_iii=set_G_iii.begin();G_iii!=set_G_iii.end();++G_iii){ //11. for each G'''

					bool less_than_1_node=0;
					if (((*G_iii).second).size()<=1){
						less_than_1_node=1;
					}
					bool contained_in_gnew = 0;
					std::vector<std::pair<Graph, vector<int> > >::iterator it_gnew; //iterador de conjunto de grafos = *grafo
					for (it_gnew=g_new.begin(); ((it_gnew!=g_new.end()) && !contained_in_gnew); it_gnew++){
						if (((*G_iii).second)==((*it_gnew).second)){
							contained_in_gnew=1;
						}
					}
					if((!less_than_1_node)&&(!contained_in_gnew)){
						//Compute the functions S, L and KG to form the PG G'''
						std::vector<int>::iterator it_Giii; //iterador del vector (*G_iii).second del grafo *G_iii
						for (it_Giii=((*G_iii).second).begin();it_Giii!=((*G_iii).second).begin();++it_Giii){
							int res=*it_Giii; //así recorremos todos los lugares de recurso del grafo Giii
							std::set<int> Kgiii=KG(*G_iii,res);
						}
						//add G''' to the g_new
						g_new.push_back(*G_iii);
					}
					numSCC++;
				}
			}//CIERRA IF 9.
		}//CIERRA IF 7.
		else{ //13.
			g_good.push_back(pairG_i); //14. add G' to g_good
			//15. for each r from V'
			for (std::vector<int>::iterator r=V_i.begin(); r!=V_i.end(); ++r){
				std::set<int> R; R.insert(*r);
				//16. if there is no graph in g_new nor in g_good with vetrices = V'-r
				bool exists_in_g_new=0;
				std::vector<std::pair<Graph, vector<int> > >::iterator it_g;
				for (it_g=g_new.begin(); it_g!=g_new.end(); it_g++){
					vector<int> V1=(*it_g).second;
					vector<int> V2=removeSet_from_v_label(R,V_i);
					if (V1==V2) exists_in_g_new=1;
				}
				bool exists_in_g_good=0;
				for (it_g=g_good.begin(); it_g!=g_good.end(); it_g++){
					vector<int> V1=(*it_g).second;
					vector<int> V2=removeSet_from_v_label(R,V_i);
					if (V1==V2) exists_in_g_good=1;
				}
				//if there is no graph in g_new NOR in g_good
				if (!((exists_in_g_new)||(exists_in_g_good))){

					//17.Compute the prunning subgrpah of G' ->  V' - r, named G''
					std::pair<Graph,std::vector<int> >pairG_ii;  //grafo
					pairG_ii=removeSet_from_graph_pair(R,pairG_i);
					//18. Compute scc of G''
					std::vector<std::pair<Graph,std::vector<int> > > set_G_iii;  //conjunto de grafos
					set_G_iii=ComputeSCC(pairG_ii);
					std::vector<std::pair<Graph,std::vector<int> > >::iterator G_iii;   //iterador de conjunto de grafos = *grafo
					int (numSCC)=0;
					for(G_iii=set_G_iii.begin();G_iii!=set_G_iii.end();++G_iii){ //18. for each G'''
						bool less_than_1_node=0;
						if (((*G_iii).second).size()<=1){
							less_than_1_node=1;
						}
						bool contained_in_gnew = 0;
						std::vector<std::pair<Graph, vector<int> > >::iterator it_gnew; //iterador de conjunto de grafos = *grafo
						for (it_gnew=g_new.begin(); ((it_gnew!=g_new.end()) && !contained_in_gnew); it_gnew++){
							if (((*G_iii).second)==((*it_gnew).second)){
								contained_in_gnew=1;
							}
						}
						if((!less_than_1_node)&&(!contained_in_gnew)){
							//Compute the functions S, L and KG to form the PG G'''
							std::vector<int>::iterator it_Giii; //iterador del vector (*G_iii).second del grafo *G_iii
							for (it_Giii=((*G_iii).second).begin();it_Giii!=((*G_iii).second).begin();++it_Giii){
								int res=*it_Giii; //asi recorremos todos los lugares de recurso del grafo Giii
								std::set<int> Kgiii=KG(*G_iii,res);
							}
							//add G''' to the g_new
							g_new.push_back(*G_iii);
						}
						numSCC++;
					}
				}//CIERRA IF 16.
			}//CIERRA FOR 15.
		}//CIERRA ELSE 13.
		//cout<<"g_new size dopo "<<g_new.size()<<endl;
	}//CIERRA WHILE 5.

	//cout<<"stttto"<<endl;
	std::vector<std::pair<Graph, vector<int> > >::iterator it_graph;
	//23. for all G in g_good, if exists another G'
	//satisfying: (1) V' is subset of V (or the same set) and (2) SG' is subset of SG
	std::vector<std::pair<Graph, vector<int> > > graphs_to_remove;
	for (it_graph=g_good.begin();it_graph!=g_good.end()-1;it_graph++){
		vector<int> V = (*it_graph).second;
		set<int> Sg = SG(*it_graph);
		std::vector<std::pair<Graph, vector<int> > >::iterator it_graph_i;
		for (it_graph_i=it_graph+1;it_graph_i!=g_good.end();it_graph_i++){
			vector<int> V_i = (*it_graph_i).second;
			bool condition1=0;
			bool condition2=0;
			if (is_contained(V_i,V)){  //si V_i es subconjunto de V
				condition1=1;
			}
			set<int> Sg_i = SG(*it_graph_i);
			if ((Sg_i+Sg)==Sg){  //si Sg_i es subconjunto de Sg
				condition2=1;
			}
			if (condition1 && condition2){
				graphs_to_remove.push_back(*it_graph_i);
			}
		}
	}
	//cout<<"stttto2"<<endl;
	std::vector<std::pair<Graph, vector<int> > > g_good_temp;
	for (it_graph=g_good.begin();it_graph!=g_good.end();it_graph++){
		vector<int> V = (*it_graph).second;
		bool remove=0;
		std::vector<std::pair<Graph, vector<int> > >::iterator it_graph_i;
		for (it_graph_i=graphs_to_remove.begin();((it_graph_i!=graphs_to_remove.end())&& !remove);it_graph_i++){
			vector<int> V_i = (*it_graph_i).second;
			if (V==V_i){
				remove=1;
			}
		}
		if (!remove){
			g_good_temp.push_back(*it_graph);
		}
	}
	g_good=g_good_temp;
	//cout<<"stttto3"<<endl;
	for (it_graph=g_good.begin();it_graph!=g_good.end();it_graph++){ //24. esto es asi seguro
		std::set<int>newSiphon = this->SG(*it_graph);
		Siphons.push_back(newSiphon);//25. Add to Siphons, D=SG'.
	}
	//cout<<"stttto4"<<endl;
	//ALGORITMO FINITO
	siphons=Siphons;
	return (Siphons);
}




void S4PR::printSiphons(){
	if (siphons.empty()){
		siphons = this->getSiphons();
	}
	std::vector<std::set<int> >::iterator it_sip;
	int num_siphon=0;
	cout<<"The PN's set of siphons is = "<<endl;
	for (it_sip=siphons.begin();it_sip!=siphons.end();++it_sip){
		cout<<*it_sip<<endl;
		num_siphon++;
	}
	//cout<<"There are "<<num_siphon<<" siphons in the Petri Net"<<endl;
}
//std::vector<std::set<int> > S4PR::getBadSiphons(){
//	//obtenemos los sifones y despues comprobamos si en alguno de ellos estï¿½ contenido un p-semiflujo
//	// en cuyo caso ya no sera un sifon malo
//	if (p_semi.isEmpty()){
//		p_semi = this->semiflows(C);
//	}
//	std::vector<std::set<int> > Psemiflows;
//	int rows=p_semi.get_nrows();
//	int cols=p_semi.get_ncols();
//	for (int i=1;i<=rows;i++){
//		std::set<int> semiflowPlaces;
//		for(int j=1;j<=cols;j++){
//			if (p_semi(i,j)==1){
//				semiflowPlaces.insert(j);
//			}
//		}
//		Psemiflows.push_back(semiflowPlaces);
//	}
//
//	if (siphons.empty()){
//		siphons = this->getSiphons();
//	}
//	std::vector<std::set<int> >::iterator it_sip;
//	std::vector<std::set<int> > BadSiphons;
//	for (it_sip=siphons.begin();it_sip!=siphons.end();++it_sip){
//		bool isPsemi=0;
//		std::vector<std::set<int> >::iterator it_semi;
//		for (it_semi=Psemiflows.begin();((it_semi!=Psemiflows.end())&&(isPsemi==0));++it_semi){
//			//SI EL CONJUNTO semi ESTA INCLUIDO EN sip
//			if ((*it_semi + *it_sip)==*it_sip){ // es decir si la union de semi y sip es igual a sip
//				isPsemi=1;
//			}
//		}
//		//comprobar si el marcado del sifon es mayor o igual que el marcado de todos los lugares de espera de procesos
//		//si es asi, el sifon no se puede vaciar
//		bool isBad = 0;
//
//		if(isPsemi==0){
//			BadSiphons.push_back(*it_sip);
//		}
//	}
//	bad_siphons=BadSiphons;
//	return (BadSiphons);
//}

std::vector<std::set<int> > S4PR::getBadSiphons(){
	//obtenemos los sifones y despues comprobamos si en alguno de ellos est� contenido un p-semiflujo
	// en cuyo caso ya no sera un sifon malo
	if (p_semi.isEmpty()){
		p_semi = this->semiflows(C);
	}
	std::vector<std::set<int> > Psemiflows;
	int rows=p_semi.get_nrows();
	int cols=p_semi.get_ncols();
	for (int i=1;i<=rows;i++){
		std::set<int> semiflowPlaces;
		for(int j=1;j<=cols;j++){
			if (p_semi(i,j)==1){
				semiflowPlaces.insert(j);
			}
		}
		Psemiflows.push_back(semiflowPlaces);
	}

//	if (siphons.empty()){
//		siphons = this->getSiphons();
//	}
	std::vector<std::set<int> >::iterator it_sip;
	std::vector<std::set<int> > BadSiphons;
	for (it_sip=siphons.begin();it_sip!=siphons.end();++it_sip){
		bool isPsemi=0;
		std::vector<std::set<int> >::iterator it_semi;
		for (it_semi=Psemiflows.begin();((it_semi!=Psemiflows.end())&&(isPsemi==0));++it_semi){
			//SI EL CONJUNTO semi ESTA INCLUIDO EN sip
			if ((*it_semi + *it_sip)==*it_sip){ // es decir si la union de semi y sip es igual a sip
				isPsemi=1;
			}
		}
		//comprobar si el marcado del sifon es mayor o igual que el marcado de todos los lugares de espera de procesos
		//si es asi, el sifon no se puede vaciar
		//bool isBad = 0;

		if(isPsemi==0){
			BadSiphons.push_back(*it_sip);
		}
	}
	bad_siphons=BadSiphons;
	return (BadSiphons);
}


void S4PR::printBadSiphons(){
	if (bad_siphons.empty()){
		bad_siphons = this->getBadSiphons();
	}
	std::vector<std::set<int> >::iterator it_sip;
	int num_siphon=0;
	cout<<"The PN's set of 'bad' siphons is = "<<endl;
	for (it_sip=bad_siphons.begin();it_sip!=bad_siphons.end();++it_sip){
		cout<<*it_sip<<endl;
		num_siphon++;
	}
	//cout<<"There are "<<num_siphon<<" 'bad' siphons in the Petri Net"<<endl;
}
S4PR S4PR::getControlSiphons(){

	int newnumplaces=this->nplaces;
	Matrix<int> newPre=this->Pre; //this->getPre();
	Matrix<int> newPost=this->Post; //this->getPost();
	std::vector<int> newM0=this->m0;
	int newnumResources=this->numResources;
	int newnumPmonitor=this->numPmonitor;
	std::set<int>newPr=this->Pr;
	std::set<int>newPmonitor=this->Pmonitor;
	if (bad_siphons.empty()){
		bad_siphons = this->getBadSiphons();
	}
	std::vector< std::set<int> >::iterator it;
	for(it=bad_siphons.begin();it!=bad_siphons.end();it++){
		std::set<int> siphon = *it;
		
		std::set<int>::iterator pl;
		int sum_m0 = 0;
		for (pl = siphon.begin(); pl != siphon.end(); ++pl) {
			sum_m0 = sum_m0 + m0[(*pl) - 1];
		}
		if ((sum_m0 - 1) <= (numProcesses-1)) {
			newnumResources++;
			newnumPmonitor++;
			newnumplaces++;
			int pk = newnumplaces;  // el numero del nuevo lugar monitor pk serï¿½ siempre uno mas que el n de lugares existentes
			newM0.push_back(sum_m0 - 1); //newM0[pk]=sum_m0-1;
			newPr.insert(pk);
			newPmonitor.insert(pk); //para cada sifon vamos a anadir un lugar monitor pk
			newPre = newPre.add_row_of_zeros();
			newPost = newPost.add_row_of_zeros();

			for (int tj = 1; tj <= ntrans; tj++) {
				int sum = 0;
				for (pl = siphon.begin(); pl != siphon.end(); ++pl) {
					sum = sum + C(*pl, tj);
				}
				//QUIZA MEJOR METER OTRO BUCLE FOR tj AQUI
				if (sum > 0) { //aniadimos sum a la matriz Post
					newPost(pk, tj) = sum;
					newPre(pk, tj) = 0;
				}
				if (sum < 0) { //aniadimos sum a la matriz Pre
					newPost(pk, tj) = 0;
					newPre(pk, tj) = -1 * sum;
				}
				//IGUAL ESTO SOBRA PORQUE YA HAY CEROS EN ESA FILA
				if (sum == 0) { //ponemos ceros en el resto de columna de tj
					newPost(pk, tj) = 0;
					newPre(pk, tj) = 0;
				}
			}
		}
	}//fin del for S
	cout << "Pmonitor: " << newPmonitor << endl;
	for (int ii = 0; ii < newM0.size(); ++ii) { cout << newM0[ii] << " "; }cout << endl;
	//ahora cambiamos todas las variables internas que quedan de la red de petri
	S4PR newPetriNet;
	newPetriNet.nplaces=newnumplaces;
	newPetriNet.ntrans=this->ntrans;
	newPetriNet.Pre=newPre;
	newPetriNet.Post=newPost;
	newPetriNet.C=newPost-newPre;
	newPetriNet.P0=this->P0;
	newPetriNet.Pmonitor=newPmonitor;
	newPetriNet.Pr=newPr;
	newPetriNet.Ps=this->Ps;
	newPetriNet.m0=newM0;
	newPetriNet.numProcesses=this->numProcesses;
	newPetriNet.numResources=newnumResources;
	newPetriNet.numPmonitor=newnumPmonitor;

	return newPetriNet;
}
S4PR S4PR::getControlledPetriNet(){
	// Devuelve una nueva red de petri con un lugar monitor anadido para cada sifon
	// Hace el proceso de forma iterativa comprobando si hay nuevos sifones
	S4PR newPetriNet;
	newPetriNet.nplaces=this->nplaces;
	newPetriNet.ntrans=this->ntrans;
	newPetriNet.Pre=this->Pre;
	newPetriNet.Post=this->Post;
	newPetriNet.C=this->C;
	newPetriNet.P0=this->P0;
	newPetriNet.Pmonitor=this->Pmonitor;
	newPetriNet.Pr=this->Pr;
	newPetriNet.Ps=this->Ps;
	newPetriNet.m0=this->m0;
	newPetriNet.numProcesses=this->numProcesses;
	newPetriNet.numResources=this->numResources;
	newPetriNet.siphons=this->siphons;
	newPetriNet.bad_siphons=this->bad_siphons;
	if (newPetriNet.bad_siphons.empty()){
		newPetriNet.bad_siphons = this->getBadSiphons();
	}



	bool fin=0;
	while(!fin){ //Hasta que no hayamos eliminado todos los sifones repetiremos el proceso
		newPetriNet=newPetriNet.getControlSiphons();
		//std::cout<<"done"<<std::endl;

		if (newPetriNet.bad_siphons==newPetriNet.getBadSiphons()){
			fin=1;
		}
		else{
			newPetriNet.bad_siphons=newPetriNet.getBadSiphons();
			//newPetriNet.printBadSiphons();
		}
	}
	return newPetriNet;
}
void S4PR::print_PN_parameters(){

	cout << "PARAMETERS OF THE PETRI NET : " <<endl<<endl;
	cout << "Number of places = " << this->getPlaces() <<endl<<endl;
	cout << "Number of transitions = " << this->getTransitions() <<endl<<endl;
	cout << "Incidence matrix: C = "<<endl << this->getC() <<endl<<endl;
	cout<< "Initial marking: m0 = "<< this->getM0()<<endl<<endl;
	cout << "Number of Processes = " << this->getProcessNo() <<endl<<endl;
	cout << "Idle Places: Po = " << this->getP0() <<endl<<endl;
	cout << "Process Places: Ps = " << this->getPs() <<endl<<endl;
	cout<< "Resources places: Pr = "<< this->getPr() <<endl<<endl;
	if(Pmonitor.size()!=0){
		cout<< "Monitor places: Pm = "<< this->getPmonitor() <<endl<<endl;
	}
}
