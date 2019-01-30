#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <set>

using namespace std;

class Exception
{
public:
  const char* msg;
  Exception(const char* arg)
   : msg(arg)
  {
  }
};


template <class T>
class Matrix {
    public:
        Matrix();
        Matrix(const Matrix &m);
        Matrix(const int rows, const int cols);
        ~Matrix();

        Matrix<T> operator+ (const Matrix &matrix_2);
        Matrix<T> operator- (const Matrix &matrix_2);
        Matrix<T> operator* (const Matrix &matrix_2);
        T& operator()(const int row, const int col);


        bool isSymmetric();
        bool isIdentity();

        T get_max();
        T get_min();
        T get_mode();

        int get_nrows();
        int get_ncols();
        T** get_matrix();

        void delete_matrix();
        void fill_by_user();
        void fill_from_file(FILE *fichero, const int rows, const int cols);
        void fill_from_file2(vector<vector<int> > mat);
        void fill_random();
		void fill_from_paths(std::vector<std::vector<std::vector<float>>> paths);
        Matrix<T> get_transpose();   // 1-8-16
        void multiply_by_scalar(T);
        void print_matrix();
        void swap_cols(int, int);
        void swap_rows(int, int);
        bool isEmpty();
        std::vector<int> rowFromMatrix(int row);
        std::set<int> setFromMatrix(int row);     // 30/8/16
        Matrix<T> subMatrix(std::set<int> rows,std::set<int> cols);
        Matrix<T> add_row_of_zeros(); //12-11-16
        Matrix<T> add_col_of_zeros();

        protected:
        T m_ele;
        T m_max;
        T m_min;
        T m_mode;
        T **m_matrix;

        int m_dim_matrix;
        int m_cols;
        int m_rows;
};


#endif // MATRIX_H
