#include "matrix.h"

// Constructor por defecto
template<typename T>
Matrix<T>::Matrix()
{
	m_dim_matrix=0;
    m_rows = 0;
    m_cols = 0;
    m_matrix = NULL;
}

template<typename T>
Matrix<T>::Matrix(const int rows, const int cols)
{
	m_dim_matrix=0;
    m_rows = rows;
    m_cols = cols;
    m_matrix = new T*[m_rows];

    for (int i = 0; i < m_rows; i++) {
        m_matrix[i] = new T[m_cols];

        for (int j = 0; j < m_cols; j++){
        	m_matrix[i][j] = 0;
        }
    }
}

// Constructor copia
template<typename T>
Matrix<T>::Matrix(const Matrix &m)
{
    *this = m;
}

// Suma de matrices con sobrecarga de operadores
template<typename T>
Matrix<T> Matrix<T>::operator+ (const Matrix &matrix_2)
{
    Matrix matrix_result(m_rows, m_cols);
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            matrix_result.m_matrix[i][j] = m_matrix[i][j] + matrix_2.m_matrix[i][j];
        }
    }
    return matrix_result;
}

// Resta de matrices con sobrecarga de operadores
template<typename T>
Matrix<T> Matrix<T>::operator- (const Matrix &matrix_2)
{
    Matrix<T> matrix_result(m_rows,m_cols);
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            matrix_result.m_matrix[i][j] = m_matrix[i][j] - matrix_2.m_matrix[i][j];
        }
    }
    return matrix_result;
}

// Multiplicaci�n de matrices con sobrecarga de operadores
template<typename T>
Matrix<T> Matrix<T>::operator* (const Matrix &matrix_2)
{
    Matrix matrix_result(m_rows, matrix_2.m_cols);
    T total;

    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < matrix_2.m_cols; j++) {
            for (int k = 0; k < m_cols; k++) {
                total += (m_matrix[i][k] * matrix_2.m_matrix[k][j]);
            }
            matrix_result.m_matrix[i][j] = total;

            // Limpiar el total sumado arriba
            total = 0;
        }
    }
    return matrix_result;
}

//devolver el elemento (i,j)
template<typename T>
T& Matrix<T>::operator()(const int row, const int col) {
    if (m_matrix != NULL && row > 0 && row <= m_rows && col > 0 && col <= m_cols)
    {
      return m_matrix[row-1][col-1];
    }
    else
    {
      throw Exception("Subscript out of range");
    }
}


// Verificar si una Matriz es sim�trica
template<typename T>
bool Matrix<T>::isSymmetric()
{
    if (m_rows != m_cols) {
        return false;
    }

    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            if (m_matrix[i][j] != m_matrix[j][i]) {
                return false;
            }
        }
    }
    return true;
}

// Verificar si una Matriz es identidad
template<typename T>
bool Matrix<T>::isIdentity()
{
    if (m_rows != m_cols) {
        return false;
    }

    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            if (i == j) {
                if (m_matrix[i][j] != 1)
                    return false;
            } else {
                if (m_matrix[i][j] != 0)
                    return false;
            }
        }
    }
    return true;
}

// Obtener el mayor de la Matriz
template<typename T>
T Matrix<T>::get_max()
{
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            if (m_matrix[i][j] > m_max) {
                m_max = m_matrix[i][j];
            }
        }
    }
    return m_max;
}

// Obtener el menor de la Matriz
template<typename T>
T Matrix<T>::get_min()
{
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            if (m_matrix[i][j] < m_min) {
                m_min = m_matrix[i][j];
            }
        }
    }
    return m_min;
}

// Obtener la moda de la Matriz
template<typename T>
T Matrix<T>::get_mode()
{
    // Creo una Matrix auxiliar
    Matrix matrix_aux(m_rows, m_cols);

    // Lleno la Matriz con ceros
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            matrix_aux.m_matrix[i][j] = 0;
        }
    }

    m_dim_matrix = m_rows * m_cols;

    // Para retener una fila n veces
    int y = 0;

    // Para retener una columna n veces
    int z = 0;

    // Empiezo a comparar cada elemento n veces
    for (int x = 0; x < m_dim_matrix; x++) {
        for (int i = 0; i < m_rows; i++) {
            for (int j = 0; j < m_cols; j++) {
                if (m_matrix[y][z] == m_matrix[i][j]) {
                    matrix_aux.m_matrix[i][j]++;
                }
            }
        }

        // Pasar a la siguiente columna despues de n comparaciones
        z++;

        /* Empiezo a comparar con la siguiente fila
           despues empiezo nuevamente en la 1era columna
           Y luego paso a la siguiente fila */
        if (z == m_cols) {
            z = 0;
            y++;
        }
    }

    // Obtengo el mayor valor de la Matriz
    m_max = matrix_aux.get_max();

    // Si ningun valor se ha repetido m�s de una vez, entonces no hay moda
    if (m_max == 1) {
        return -1;
    } else {
        for (int i = 0; i < m_rows; i++) {
            for (int j = 0; j < m_cols; j++) {
                if (matrix_aux.m_matrix[i][j] == m_max) {
                    m_mode = m_matrix[i][j];
                }
            }
        }
    }

    return m_mode;
}

template<typename T>
int Matrix<T>::get_nrows(){return m_rows;}
template<typename T>
int Matrix<T>::get_ncols(){return m_cols;}
template<typename T>
T** Matrix<T>::get_matrix(){return m_matrix;}

template<typename T>
void Matrix<T>::delete_matrix()
{
    for (int i = 0; i < m_rows; i++) {
        delete[] m_matrix[i];
    }
    delete[] m_matrix;
}


// Llenar una Matriz desde teclado
template<typename T>
void Matrix<T>::fill_by_user()
{
    for (int i = 0; i < m_rows; i++) {
        cout << "Fila " << i + 1 << endl;
        for (int j = 0; j < m_cols; j++) {
            cout << "Ingresa el elemento " << j + 1 << endl;
            cin >> m_ele;
            m_matrix[i][j] = m_ele;
        }
        cout << endl;
    }
    m_max = m_matrix[0][0];
    m_min = m_matrix[0][0];
}
// Llenar una Matriz desde fichero
template<typename T>
void Matrix<T>::fill_from_file(FILE *fichero, const int rows, const int cols)
{
	m_rows=rows;
	m_cols=cols;

    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
        	fscanf(fichero,"%d", &m_matrix[i][j]);
        }
    }
    m_max = m_matrix[0][0];
    m_min = m_matrix[0][0];
}


template<typename T>
void Matrix<T>::fill_from_file2(vector<vector<int> > mat)
{
	m_rows=mat.size();
	m_cols=mat[0].size();

    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
        	m_matrix[i][j] = mat[i][j];
        }
    }
    m_max = m_matrix[0][0];
    m_min = m_matrix[0][0];
}

// Llenar aleatoriamente una Matriz
template<typename T>
void Matrix<T>::fill_random()
{
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            m_matrix[i][j] = rand() % 30;
        }
    }

    m_max = m_matrix[0][0];
    m_min = m_matrix[0][0];
    srand(time(NULL));
}

// Obtener la transpuesta de una Matriz
template<typename T>
Matrix<T> Matrix<T>::get_transpose()
{
    Matrix<T> matrix_result(m_cols,m_rows);

    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            matrix_result.m_matrix[j][i]= m_matrix[i][j];
        }
    }
    return matrix_result;
}

// Multiplicar a una Matriz por un escalar
template<typename T>
void Matrix<T>::multiply_by_scalar(T scalar)
{
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            m_matrix[i][j] = m_matrix[i][j] * scalar;
        }
    }
    cout << "La matriz ha sido multiplicada por el escalar " << scalar << endl;
}

// Imprimir Matriz
template<typename T>
void Matrix<T>::print_matrix()
{
    for (int i = 0; i < m_rows; i++) {
        for (int j = 0; j < m_cols; j++) {
            cout << m_matrix[i][j] << " ";
        }
        cout << endl << endl;
    }
    cout << endl << endl;
}

// Intercambiar dos columnas en una Matriz
template<typename T>
void Matrix<T>::swap_cols(int col_1, int col_2)
{
    if (col_1 > m_cols || col_2 > m_cols) {
        cout << "Esa columna se encuentra fuera de rango." << endl;
    } else {
        T temp;
        col_1--;
        col_2--;

      for (int i = 0; i < m_rows; i++) {
          temp = m_matrix[i][col_1];
          m_matrix[i][col_1] = m_matrix[i][col_2];
          m_matrix[i][col_2] = temp;
      }
      cout << "Se intercambiaron las columnas " << col_1 + 1 << " y " << col_2 + 1 << endl;
    }
}

// Intercambiar dos filas en una Matriz
template<typename T>
void Matrix<T>::swap_rows(int row_1, int row_2)
{
    if (row_1 > m_rows || row_2 > m_rows) {
        cout << "Esa fila se encuentra fuera de rango." << endl;
    } else {
        T temp;
        row_1--;
        row_2--;

        for (int i = 0; i < m_cols; i++) {
            temp = m_matrix[row_1][i];
            m_matrix[row_1][i] =  m_matrix[row_2][i];
            m_matrix[row_2][i] = temp;
        }
        cout << "Se intercambiaron las filas: " << row_1 + 1 << " y " << row_2 + 1 << endl;
    }
}

template<typename T>
bool  Matrix<T>::isEmpty(){
	return (m_matrix == NULL);
}


//Devuelve un vector creado a partir de la fila de una matriz
template<typename T>
std::vector<int> Matrix<T>::rowFromMatrix(int row){

	std::vector<int> vectorRow(m_cols,0);
	for (int i=0;i<m_cols;i++){
		vectorRow[i]=m_matrix[row-1][i];
	}
	return vectorRow;
}
//Devuelve una set creada a partir de la fila de una matriz
//pero excluyendo los -1 y los negativos
template<typename T>
std::set<int> Matrix<T>::setFromMatrix(int row){

	std::set<int> vectorRow;
	for (int i=0;i<m_cols;i++){
		if (m_matrix[row-1][i]>=0){
			vectorRow.insert(m_matrix[row-1][i]);
		}
	}
	return vectorRow;
}

//Devuelve la matriz compuesta solo por las filas y las columnas correspondientes a los parametrois de entrada
template<typename T>
Matrix<T> Matrix<T>::subMatrix(std::set<int> rows,std::set<int> cols){
	Matrix<T> matrix_result(rows.size(),cols.size());
	std::set<int>::iterator it1,it2;
	int i=0;
	for (it1=rows.begin(); it1!=rows.end(); ++it1){
		int j = 0;
		for (it2=cols.begin(); it2!=cols.end(); ++it2){
			matrix_result.m_matrix[i][j++] = this->m_matrix[*it1-1][*it2-1];
		}
		i++;
	}
	return matrix_result;
}

template<typename T>
Matrix<T> Matrix<T>::add_row_of_zeros(){
	int nrows=this->get_nrows();
	int ncols=this->get_ncols();
	Matrix<T> matrix_result(nrows+1,ncols);
	for (int i=0;i<nrows;i++){
		for (int j=0;j<ncols;j++){
			matrix_result.m_matrix[i][j]=this->m_matrix[i][j];
		}
	}

	for (int k=0;k<ncols;k++){
		matrix_result.m_matrix[nrows][k]=0;
	}

	return matrix_result;
}
template<typename T>
Matrix<T> Matrix<T>::add_col_of_zeros(){
	int nrows=this->get_nrows();
	int ncols=this->get_ncols();
	Matrix<T> matrix_result(nrows,ncols+1);
	for (int i=0;i<nrows;i++){
		for (int j=0;j<ncols;j++){
			matrix_result.m_matrix[i][j]=this->m_matrix[i][j];
		}
	}
	for (int k=0;k<nrows;k++){
		matrix_result.m_matrix[k][ncols]=0;
	}
	return matrix_result;
}

//Llenar una matriz a partir de las rutas de los robots

template<typename T>
void Matrix<T>::fill_from_paths(std::vector<std::vector<std::vector<float>>> paths)
{
	m_rows = 0;
	m_cols = 0;

	for (unsigned int i = 0; i < paths.size(); ++i) {
		std::vector<std::vector<float>> temp_path = paths[i];
		for (unsigned int j = 0; j < temp_path.size(); ++j) {
			m_rows += 1;
			m_cols += 1;
		}
	}
	std::cout << m_cols;

	for (int i = 0; i < m_rows; i++) {
		for (int j = 0; j < m_cols; j++) {
			if (i == j) {
				m_matrix[i][j] = m_cols;
			}
			else m_matrix[i][j] = 0;
		}
	}
	m_max = m_matrix[0][0];
	m_min = m_matrix[0][0];
	
}

template<typename T>
Matrix<T>::~Matrix() {}
