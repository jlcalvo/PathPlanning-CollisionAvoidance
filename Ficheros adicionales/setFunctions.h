#ifndef SETFUNCTIONS_H
#define SETFUNCTIONS_H

#include "matrix.h"
#include <vector>
#include <set>
#include <iostream>
#include <iterator>
#include <algorithm>
using namespace std;

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
	// imprimir un conjunto vector v de entrada sobreescribiendo el operador <<
	if ( !v.empty() ) {
		out << '[';
		std::copy (v.begin(), v.end()-1, std::ostream_iterator<T>(out, ","));
		std::copy (v.end()-1, v.end(), std::ostream_iterator<T>(out, "]"));
	}
	return out;
}

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::set<T>& v) {
// imprimir un conjunto set v de entrada sobreescribiendo el operador <<
	std::set<int>::iterator it;

	if ( !v.empty() ) {
		out << '{';
		for (it=v.begin(); it!=v.end(); it++){
			if (it==v.begin())
				{out << *it;}
			else
				{out << ','<< *it;}
		}
	  out <<"}";
  }
  return out;
}

template <typename T>
void operator+=(std::set<T>& s1, const std::set<T>& s2) {
	// Hace la union entre los dos conjuntos de entrada, s1 y s2 y la devuelve en s1
	std::set<int>::iterator it;
	for (it=s2.begin(); it!=s2.end(); ++it){
		s1.insert(*it);
	}
}

template <typename T>
std::set<T> operator+(const std::set<T>& s1, const std::set<T>& s2) {
	// Hace la union entre los dos conjuntos de entrada, s1 y s2
	// rtn es el conjunto en el que se almacena la union
	std::set<T> rtn(s1);
    rtn += s2;
    return rtn;
}


template <typename T>
std::set<T> operator&(const std::set<T>& s1, const std::set<T>& s2) {
	// Hace la interseccion entre los dos conjuntos de entrada, s1 y s2
	// ret es el conjunto en el que se almacena la interseccion y el cual se actualiza
	std::set<int> ret;

	std::vector<int> common_data;
	set_intersection(s1.begin(),s1.end(),s2.begin(),s2.end(), std::back_inserter(common_data));

	ret.insert(common_data.begin(), common_data.end());
	return ret;
}

template <typename T>
std::set<T> operator!(const std::vector<T>& v) {
	//se calcula el soporte del vector de entrada v
	//el resultado se devuelve en un conjunto set
	std::set<int> ret;
	for(unsigned int i=0;i<v.size();i++)
		if (v[i]!=0)
			ret.insert(i+1);
	return ret;
}

template <typename T>
std::set<T> operator-(const std::set<T>& s1, const std::set<T>& s2) {
	//se devuelve el conjunto ret que almacena el conjunto s1,
	//excluyendo s2
	std::set<int> ret;

	std::vector<int> diff_data;
	set_difference(s1.begin(),s1.end(),s2.begin(),s2.end(), std::back_inserter(diff_data));

	ret.insert(diff_data.begin(), diff_data.end());
	return ret;
}


template <typename T>
bool operator==(const std::set<T>& s1, const std::set<T>& s2) {
	//se devuelve true si los dos conjuntos son iguales

	if (s1.size() != s2.size()) return 0;

	if (((s1-s2).size()!=0) || ((s2-s1).size()!=0)) return 0; // s1 y s2 son iguales sii s1\s2 = s2\s1 = \emptyset
	return 1;
}

template <typename T>
ostream &operator<< (ostream &out,Matrix<T> ob) {
	// imprimir la matriz sobreescribiendo el operador <<
	out << '[';
	for(int i=0;i<ob.get_nrows();i++){
		for(int j=0;j<ob.get_ncols()-1;j++){
			out <<(ob.get_matrix())[i][j]<<",";
		}
		out <<(ob.get_matrix())[i][ob.get_ncols()-1]<<";"<<endl;
	}
	out<<"];";
	return out;
}


#endif
