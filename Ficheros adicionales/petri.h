#ifndef PETRI_H
#define PETRI_H
#include <sstream>
#include<iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <set>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <utility>       // for std::pair
#include <algorithm>     // for std::for_each
#include <math.h>
#include "matrix.h"
#include "matrix.cpp"
#include "setFunctions.h"
#include <map>
using namespace std;
//Librerias para el grafo
  #include <boost/graph/graph_traits.hpp>
  #include <boost/graph/adjacency_list.hpp>
  #include <boost/graph/dijkstra_shortest_paths.hpp>
  #include <boost/graph/graphviz.hpp>
  #include <boost/property_map/property_map.hpp>
  #include <boost/config.hpp>
  #include <boost/graph/strong_components.hpp>
  #include <boost/graph/graph_utility.hpp>
  using namespace boost;


class PetriNetClass {
    public:
        PetriNetClass();
        ~PetriNetClass();

        std::set<int> postset(std::set<int> nodes, char nodeType, const bool print);
        std::set<int> preset(std::set<int> nodes, char nodeType, const bool print);
        void setValues(const int nplaces, const int ntrans, std::vector<int> m0,Matrix<int> Pre,Matrix<int> Post,Matrix<int> C);
        bool isStateMachine();
        bool isSiphon(std::set<int> SetOfPlaces);
        int getIdle();
		void setIdle(int value);
        Matrix<int> getC();
        Matrix<int> getPre();
        Matrix<int> getPost();
        std::vector<int> getM0();
        Matrix<int> Psemiflows();
        Matrix<int> Tsemiflows();
        Matrix<int> p_semi;
		Matrix<int> t_semi;
		Matrix<int> semiflows(Matrix<int>);

    protected:
        int nplaces;
        int ntrans;
        int IdlePlace; //used for S4PR process net
        std::vector<int> m0;
    	Matrix<int> Pre;
    	Matrix<int> Post;
    	Matrix<int> C;

};

class S4PR : public PetriNetClass {
    public:
        S4PR();
        ~S4PR();

        void read_from_file(FILE *fichero);
        void read_from_file2(int dimP, int dimT, int nProcesses, vector<vector<int> > pre, vector<vector<int> > post, vector<vector<int> > Processes, vector<int> Resources, vector<int> m_0);
        int getPlaces();
        int getTransitions();
        std::set<int> getProcessPlaces(int process);
        PetriNetClass getProcessNet(int process,bool print);
    	int getProcessNo();
    	int getResourceNo();
    	int getPmonitorNo();
    	int getIdlePlace(int process);
    	std::set<int> getP0();
    	std::set<int> getPs();
    	std::set<int> getPr();
    	std::set<int> getPmonitor();
    	bool isCond1();
    	bool isCond2();
    	bool isCond3(bool print);
    	bool isCond4(bool print);
    	bool isCond5(bool print);
    	bool isWellDefinedS4PR();
    	std::vector<std::set<int> > D1_Siphons();
    	std::vector<std::set<int> > getSiphons();
    	void printSiphons();
    	std::vector<std::set<int> > getBadSiphons();
		void printBadSiphons();
    	S4PR getControlSiphons();
    	S4PR getControlledPetriNet();
    	void print_PN_parameters();


    protected://private:
    	int numProcesses;
    	int numResources;
    	int numPmonitor;
    	Matrix<int> matrixPs;
    	std::set<int> P0;
    	std::set<int> Ps;
    	std::set<int> Pr;
    	std::set<int> Pmonitor;
    	std::vector<std::set<int> > siphons;
    	std::vector<std::set<int> > bad_siphons;
    	typedef adjacency_list<vecS,vecS,directedS> Graph;
    	std::set<int> S(int r); 		//Resource Labelling Function
    	std::vector<pair<int,int> >  L(int r, int x); 	//Arc Labelling Function
    	std::set<int> KG(std::pair<Graph,std::vector<int> > Gpair, int r);
    	std::vector<int> remove_i_from_vector(unsigned int index, std::vector<int> V);
    	std::vector<int> removeSet_from_v_label(std::set<int> Set, std::vector<int> V);
    	std::vector<int> remove_indices_from_v(set<int>indices,std::vector<int>V);
    	Graph remove_vertices_from_g(set<int>indices,Graph G_in);
    	std::pair<Graph,std::vector<int> > removeSet_from_graph_pair(std::set<int> Set,std::pair<Graph,std::vector<int> > in_graph);
    	std::vector<std::pair<Graph, vector<int> > > ComputeSCC(std::pair<Graph,std::vector<int> > Gpair);
    	bool is_contained(std::vector<int> V1,std::vector<int> V2);
    	int resource2vertex(int resource);
    	std::set<int> SG(std::pair<Graph,std::vector<int> >in_graph);
};

#endif
