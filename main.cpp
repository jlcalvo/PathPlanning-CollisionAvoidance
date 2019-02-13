#include<stdlib.h>
#include<string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <string>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/types_c.h>
#include <opencv/cv.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/aruco.hpp>
#include <aruco/cvdrawingutils.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include"Process.cpp"
#include <time.h>
#include"Eng.cpp"
#include "../cpp_petri_net/petri.h"
#include "../cpp_petri_net/Petri_Net.h"
#include "../cpp/obstacles_filter.cpp"


typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned int UNINT32;

using namespace cv;
using namespace cv::aruco;
using namespace std;

std::vector<std::vector<double> > point_mem;
std::vector <int> vect_i_pos_act;
std::vector <int> Robot_esclusiva;
std::vector <int> Robot_fermare;
std::vector<std::vector <int> > Robot_in;
std::vector<std::vector <int> > Robot_out;
std::vector<vector<vector <double> > > paths_metri;
std::vector<std::vector<std::vector<double> > > saved_lugares_pre;
std::vector<std::vector<std::vector<double> > > saved_lugares_post;
std::vector<int> marcat_new_monitor;

bool go = false;
int counttt = 0; // contatore per inviare una sola volta tutte le traiettorie
bool aver = false;
const int NUM_SECONDS = 2;

int ancho = 640, alto = 480;	//Resolución de la cámara  1280 x 720 pixeles or 1920 x 1080 pixeles 
unsigned int ncamara=0;		//Número de la cámara que se va a utilizar
int tiempocap=40;	//Tiempo en milisegundos de la frecuencia con la que se ejecuta el programa
double anchop=2.34;		// Ancho de la pista en metros
double largop=3.32;		//Largo de la pista en metros

VideoCapture Capturadevideo;
Mat Imagen;
vector<vector<Point2f> >  esquinafinal;
vector<vector<Point2f> >  esquina;
vector<Point3f> Robotmundo;
vector<Point2f> Robotm;
vector<Point2f> punto1mundo;
vector<Point2f> punto2mundo;
vector<vector<vector<Point2f> > > point_color_struct;


double theta;

std::vector<vector<vector<double> > > obstaculo;
double area_minima=300;		//aproximadamente el area del robot (pi*r²)

std::vector<int>m_0;
std::vector<std::vector<int>>pre;
std::vector<std::vector<int>>post; 
std::vector<int> resources;
std::vector<std::vector<std::vector <double> > > paths_to_send;
std::vector<bool> control_access;
int correccion_eliminacion = 0;
int it_while = 1;
std::vector<int> control;
		
std::vector<std::string> modo = {"active","active","active"};

int hh =0;
std::vector<std::vector<std::vector <double> > > identificacion;
int width = (int)((largop *100)-28-15); //larghezza in cm eliminando i bordi (tratti non percorribili)
int height = (int)((anchop *100)-14-15); //altezza in cm eliminando i bordi (tratti non percorribili)
std::ofstream fout3;
int USB;

int setUSB(){

	int USB = open( "/dev/ttyUSB0", O_RDWR| O_NONBLOCK | O_NDELAY );
	if ( USB < 0 )
	{
	cout << "Error " << errno << " opening " << "/dev/ttyUSB0" << ": " << strerror (errno) << endl;
	}

	struct termios tty;
	memset (&tty, 0, sizeof tty);

	/* Error Handling */
	if ( tcgetattr ( USB, &tty ) != 0 )
	{
	cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << endl;
	}

	/* Set Baud Rate */
	cfsetospeed (&tty, B9600);
	cfsetispeed (&tty, B9600);

	/* Setting other Port Stuff */
	tty.c_cflag     &=  ~PARENB;        // Make 8n1
	tty.c_cflag     &=  ~CSTOPB;
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     |=  CS8;
	tty.c_cflag     &=  ~CRTSCTS;       // no flow control
	tty.c_lflag     =   0;          // no signaling chars, no echo, no canonical processing
	tty.c_oflag     =   0;                  // no remapping, no delays
	tty.c_cc[VMIN]      =   0;                  // read doesn't block
	tty.c_cc[VTIME]     =   0.05;                  // 0.5 seconds read timeout

	tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines
	tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);// turn off s/w flow ctrl
	tty.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG); // make raw
	tty.c_oflag     &=  ~OPOST;              // make raw

	/* Flush Port, then applies attributes */
	tcflush( USB, TCIFLUSH );

	if ( tcsetattr ( USB, TCSANOW, &tty ) != 0)
	{
	cout << "Error " << errno << " from tcsetattr" << endl;
	}
	else {
		//cout << "Ok" << endl;
	}
	return USB;
}

void send_seq_cell_occ_WiFi(std::vector<int> seq_celle_occ){

	//"S" (grande) come segnale di inizio - tutte le celle - "F" come simbolo di fine path
	string protocollo;
	protocollo = "S";
	for(unsigned int i = 0; i < seq_celle_occ.size(); i++){
		int tmp = seq_celle_occ[i];
		protocollo = protocollo + "l";
		protocollo = protocollo + static_cast<ostringstream*>( &(ostringstream() << tmp) )->str();
		protocollo = protocollo + "k";
	}
	protocollo = protocollo + "F";


#ifdef WIN32
	//Serial port configuration in windows machines

	// Declare variables and structures
	HANDLE hSerial;
	DCB dcbSerialParams = {0};
	COMMTIMEOUTS timeouts = {0};

	// Open the highest available serial port number
	//fprintf(stderr, "Opening serial port...");
	hSerial = CreateFile("\\\\.\\COM3", GENERIC_READ|GENERIC_WRITE, 0, NULL,
	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Error\n");
		return ;
	}
	else ;//fprintf(stderr, "OK\n");
	// Set device parameters (9600 baud, 1 start bit,
	// 1 stop bit, no parity)
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (GetCommState(hSerial, &dcbSerialParams) == 0)
	{
		fprintf(stderr, "Error getting device state\n");
		CloseHandle(hSerial);
		return ;
	}
	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if(SetCommState(hSerial, &dcbSerialParams) == 0)
	{
		fprintf(stderr, "Error setting device parameters\n");
		CloseHandle(hSerial);
		return ;
	}
	// Set COM port timeout settings
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if(SetCommTimeouts(hSerial, &timeouts) == 0)
	{
		fprintf(stderr, "Error setting timeouts\n");
		CloseHandle(hSerial);
		return ;
	}

	// Send specified text (remaining command line arguments)
	DWORD bytes_written = 0;
	for(unsigned int i = 0; i < protocollo.length(); i++){
	/* Inizio Test */
		char ok[1];
		ok[0] = protocollo.at(i);
		if(!WriteFile(hSerial, ok, 1, &bytes_written, NULL))
		{
			fprintf(stderr, "Error\n");
			CloseHandle(hSerial);
			return ;
		}
		usleep(70000);//WARNING
		//std::wcout<<protocollo.at(i)<<std::endl;
	}


	std::wcout<<protocollo.c_str()<<std::endl;

	// Close serial port
	//fprintf(stderr, "Closing serial port...");
	if (CloseHandle(hSerial) == 0)
	{
		fprintf(stderr, "Error\n");
		return ;
	}
	//fprintf(stderr, "OK\n");



#else
	//Serial port configuration in Linux machines

	int USB = setUSB();


	// Send specified text (remaining command line arguments)
	DWORD bytes_written = 0;
	for(unsigned int i = 0; i < protocollo.length(); i++){
	/* Inizio Test */
		char ok[1];
		ok[0] = protocollo.at(i);
		if(!write(USB, ok, 1))
		{
			fprintf(stderr, "Error\n");
			close(USB);
			return ;
		}
		usleep(70000);//WARNING
		//std::wcout<<protocollo.at(i)<<std::endl;
	}


	std::wcout<<protocollo.c_str()<<std::endl;

	// Close serial port
	//fprintf(stderr, "Closing serial port...");
	if (close(USB))
	{
		fprintf(stderr, "Error\n");
		return ;
	}
	//fprintf(stderr, "OK\n");

#endif
	// exit normally
	return ;
}

void send_destinos_WiFi(std::vector<std::vector<double> > Targets){
	//"D" (grande) come segnale di inizio path - Coche - tutte le coppie x-y - "F" come simbolo di fine path
	string protocollo;
	protocollo = "D";
	for(int i = 0; i < Targets.size(); i++){
		double tmp1 = Targets[i][0];
		double tmp2 = Targets[i][1];
		protocollo = protocollo + "x";
		protocollo = protocollo + static_cast<ostringstream*>( &(ostringstream() << tmp1) )->str();
		protocollo = protocollo + "y";
		protocollo = protocollo + static_cast<ostringstream*>( &(ostringstream() << tmp2) )->str();
	}
	protocollo = protocollo + "F";


#ifdef WIN32
	//Serial port configuration in windows machines

	// Declare variables and structures
	HANDLE hSerial;
	DCB dcbSerialParams = {0};
	COMMTIMEOUTS timeouts = {0};

	// Open the highest available serial port number
	//fprintf(stderr, "Opening serial port...");
	hSerial = CreateFile("\\\\.\\COM3", GENERIC_READ|GENERIC_WRITE, 0, NULL,
	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Error\n");
		return ;
	}
	else ;//fprintf(stderr, "OK\n");
	// Set device parameters (9600 baud, 1 start bit,
	// 1 stop bit, no parity)
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (GetCommState(hSerial, &dcbSerialParams) == 0)
	{
		fprintf(stderr, "Error getting device state\n");
		CloseHandle(hSerial);
		return ;
	}
	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if(SetCommState(hSerial, &dcbSerialParams) == 0)
	{
		fprintf(stderr, "Error setting device parameters\n");
		CloseHandle(hSerial);
		return ;
	}
	// Set COM port timeout settings
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if(SetCommTimeouts(hSerial, &timeouts) == 0)
	{
		fprintf(stderr, "Error setting timeouts\n");
		CloseHandle(hSerial);
		return ;
	}

	// Send specified text (remaining command line arguments)
	DWORD bytes_written = 0;
	for(unsigned int i = 0; i < protocollo.length(); i++){
	/* Inizio Test */
		char ok[1];
		ok[0] = protocollo.at(i);
		if(!WriteFile(hSerial, ok, 1, &bytes_written, NULL))
		{
			fprintf(stderr, "Error\n");
			CloseHandle(hSerial);
			return ;
		}
		usleep(70000);//WARNING
		//std::wcout<<protocollo.at(i)<<std::endl;
	}


	std::wcout<<protocollo.c_str()<<std::endl;

	// Close serial port
	//fprintf(stderr, "Closing serial port...");
	if (CloseHandle(hSerial) == 0)
	{
		fprintf(stderr, "Error\n");
		return ;
	}
	//fprintf(stderr, "OK\n");



#else
	//Serial port configuration in Linux machines

	int USB = setUSB();


	// Send specified text (remaining command line arguments)
	DWORD bytes_written = 0;
	for(unsigned int i = 0; i < protocollo.length(); i++){
	/* Inizio Test */
		char ok[1];
		ok[0] = protocollo.at(i);
		if(!write(USB, ok, 1))
		{
			fprintf(stderr, "Error\n");
			close(USB);
			return ;
		}
		usleep(70000);//WARNING
		//std::wcout<<protocollo.at(i)<<std::endl;
	}


	std::wcout<<protocollo.c_str()<<std::endl;

	// Close serial port
	//fprintf(stderr, "Closing serial port...");
	if (close(USB))
	{
		fprintf(stderr, "Error\n");
		return ;
	}
	//fprintf(stderr, "OK\n");

#endif
	// exit normally
	return ;
}

void send_path_WiFi(std::vector<std::vector<double> > path, int num_coche){

	std::wcout<<"Robot "<< num_coche<<std::endl;
	//"P" (grande) come segnale di inizio path - Coche - tutte le coppie x-y - "F" come simbolo di fine path
	string protocollo;
	protocollo = "P";
	protocollo = protocollo + "c"; //coche
	protocollo = protocollo + static_cast<ostringstream*>( &(ostringstream() << num_coche) )->str();
	for(unsigned int i = 0; i < path.size(); i++){
		double tmp1 = path[i][0];
		double tmp2 = path[i][1];
		protocollo = protocollo + "x";
		protocollo = protocollo + static_cast<ostringstream*>( &(ostringstream() << tmp1) )->str();
		protocollo = protocollo + "y";
		protocollo = protocollo + static_cast<ostringstream*>( &(ostringstream() << tmp2) )->str();
	}
	protocollo = protocollo + "F";


#ifdef WIN32
	//Serial port configuration in windows machines

	// Declare variables and structures
	HANDLE hSerial;
	DCB dcbSerialParams = {0};
	COMMTIMEOUTS timeouts = {0};

	// Open the highest available serial port number
	//fprintf(stderr, "Opening serial port...");
	hSerial = CreateFile("\\\\.\\COM3", GENERIC_READ|GENERIC_WRITE, 0, NULL,
	OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Error\n");
		return ;
	}
	else ;//fprintf(stderr, "OK\n");
	// Set device parameters (9600 baud, 1 start bit,
	// 1 stop bit, no parity)
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (GetCommState(hSerial, &dcbSerialParams) == 0)
	{
		fprintf(stderr, "Error getting device state\n");
		CloseHandle(hSerial);
		return ;
	}
	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if(SetCommState(hSerial, &dcbSerialParams) == 0)
	{
		fprintf(stderr, "Error setting device parameters\n");
		CloseHandle(hSerial);
		return ;
	}
	// Set COM port timeout settings
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if(SetCommTimeouts(hSerial, &timeouts) == 0)
	{
		fprintf(stderr, "Error setting timeouts\n");
		CloseHandle(hSerial);
		return ;
	}

	// Send specified text (remaining command line arguments)
	DWORD bytes_written = 0;
	for(unsigned int i = 0; i < protocollo.length(); i++){
	/* Inizio Test */
		char ok[1];
		ok[0] = protocollo.at(i);
		if(!WriteFile(hSerial, ok, 1, &bytes_written, NULL))
		{
			fprintf(stderr, "Error\n");
			CloseHandle(hSerial);
			return ;
		}
		usleep(70000);//WARNING
		//std::wcout<<protocollo.at(i)<<std::endl;
	}


	std::wcout<<protocollo.c_str()<<std::endl;

	// Close serial port
	//fprintf(stderr, "Closing serial port...");
	if (CloseHandle(hSerial) == 0)
	{
		fprintf(stderr, "Error\n");
		return ;
	}
	//fprintf(stderr, "OK\n");



#else
	//Serial port configuration in Linux machines

	int USB = setUSB();


	// Send specified text (remaining command line arguments)
	DWORD bytes_written = 0;
	for(unsigned int i = 0; i < protocollo.length(); i++){
	/* Inizio Test */
		char ok[1];
		ok[0] = protocollo.at(i);
		if(!write(USB, ok, 1))
		{
			fprintf(stderr, "Error\n");
			close(USB);
			return ;
		}
		usleep(70000);//WARNING
		//std::wcout<<protocollo.at(i)<<std::endl;
	}


	std::wcout<<protocollo.c_str()<<std::endl;

	// Close serial port
	//fprintf(stderr, "Closing serial port...");
	if (close(USB))
	{
		fprintf(stderr, "Error\n");
		return ;
	}
	//fprintf(stderr, "OK\n");

#endif
	// exit normally
	return ;
}

void send_packet_WiFi(double x_act, double y_act, double theta_act, int num_coche){
	//"c" num_coche - la triade x,y,theta
	string protocollo;
	protocollo = protocollo + "c"; //coche
	protocollo = protocollo + static_cast<ostringstream*>( &(ostringstream() << num_coche) )->str();
	protocollo = protocollo + "x";
	protocollo = protocollo + static_cast<ostringstream*>( &(ostringstream() << x_act) )->str();
	protocollo = protocollo + "y";
	protocollo = protocollo + static_cast<ostringstream*>( &(ostringstream() << y_act) )->str();
	protocollo = protocollo + "T";
	protocollo = protocollo + static_cast<ostringstream*>( &(ostringstream() << theta_act) )->str();

#ifdef WIN32
	 	//WINDOWS
	// Declare variables and structures
	HANDLE hSerial;
	DCB dcbSerialParams = {0};
	COMMTIMEOUTS timeouts = {0};

	// Open the highest available serial port number
	//fprintf(stderr, "Opening serial port...");
	hSerial = CreateFile("\\\\.\\COM3", GENERIC_READ|GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	 Sleep(60);
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "Error\n");
		return ;
	}
	else ;//fprintf(stderr, "OK\n");

	 	// Set device parameters (9600 baud, 1 start bit,
	// 1 stop bit, no parity)
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (GetCommState(hSerial, &dcbSerialParams) == 0)
	{
		fprintf(stderr, "Error getting device state\n");
		CloseHandle(hSerial);
		return ;
	}

	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if(SetCommState(hSerial, &dcbSerialParams) == 0)
	{
		fprintf(stderr, "Error setting device parameters\n");
		CloseHandle(hSerial);
		return ;
	}

	// Set COM port timeout settings
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if(SetCommTimeouts(hSerial, &timeouts) == 0)
	{
		fprintf(stderr, "Error setting timeouts\n");
		CloseHandle(hSerial);
		return ;
	}

	// Send specified text (remaining command line arguments)
	DWORD bytes_written = 0;

	/* Inizio Test */

	// La stringa e' ...
	//std::cout<<"La stringa e' ..."<<"\n";
	//std::cout<<protocollo.c_str()<<"\n";

	if(!WriteFile(hSerial, protocollo.c_str(), protocollo.length(), &bytes_written, NULL))
	{
		fprintf(stderr, "Error\n");
		CloseHandle(hSerial);
		return ;
	}

	// Close serial port
	//fprintf(stderr, "Closing serial port...");
	if (CloseHandle(hSerial) == 0)
	{
		fprintf(stderr, "Error\n");
		return ;
	}
	//fprintf(stderr, "OK\n");

#else

	//LINUX
	 usleep(60000);
	int USB = setUSB();


	// Send specified text (remaining command line arguments)
	DWORD bytes_written = 0;

	/* Inizio Test */

	// La stringa e' ...
	//std::cout<<"La stringa e' ..."<<"\n";
	//std::cout<<protocollo.c_str()<<"\n";

	if(!write(USB, protocollo.c_str(), protocollo.length()))
	{
		fprintf(stderr, "Error\n");
		close(USB);
		return ;
	}

	// Close serial port
	//fprintf(stderr, "Closing serial port...");
	if (close(USB))
	{
		fprintf(stderr, "Error\n");
		return ;
	}
	//fprintf(stderr, "OK\n");

	// exit normally
#endif
	return ;
}

bool is_shared_resource(std::vector<int> vec) {
	bool b = false;
	int count = 0;
	for (unsigned int i = 0; i < vec.size(); ++i) {
		if (vec[i] > 0) { ++count; }
	}
	if (count > 1) { b = true; }
	return b;
}

std::vector<int> search_in_matrix(int tj, std::vector<std::vector<int>> pre, std::vector<int> r) {
	std::vector<int> resources_involved;
	for (unsigned int i = 0; i < r.size(); ++i) {
		if (pre[r[i] - 1][tj] == 1) {
			if (is_shared_resource(pre[r[i]-1])) {
				resources_involved.push_back(r[i]);
			}
		}
	}
	return resources_involved;
}

void pick_token(std::vector<int> x, std::vector<int> *capacities) {
	for (unsigned int i = 0; i < x.size(); ++i) {
		capacities->at(x[i]-1) = ((*capacities)[x[i]-1] - 1);
	}
}

void return_token(std::vector<int> y, std::vector<int> *capacities) {
	for (unsigned int i = 0; i < y.size(); ++i) {
		capacities->at(y[i]-1) = ((*capacities)[y[i]-1] + 1);
	}
}

bool resource_available(std::vector<int> z, std::vector<int> *capacities) {
	bool available = true;
	for (unsigned int i = 0; i < z.size();++i) {
		if ((*capacities)[z[i] - 1] == 0) { available = false; }
	}
	return available;
}


bool distancia (std::vector<double> pos_actual, std::vector<double> pos_objetivo, double dist_min) {
	double distancia = sqrt ( pow(pos_objetivo[0]-pos_actual[0]/100,2) + pow (pos_objetivo[1]-pos_actual[1]/100,2));
	if (distancia > dist_min) { return false;}
	else return true;
}

int main(int argc,char **argv)
{
	// lista dei punti di destinazione (uno per ogni robot)
	// e' ovvio che il numero di robot e target points devono essere uguali...
	std::vector<vector<double>> Targets;
	vector<double> target_rob;
	int nrobots = 0;
	cout << "Enter the number of Robots: ";
	cin >> nrobots;

	for(int i = 0; i < nrobots; i++){
		cout << "Enter Target Point (in cm) for Robot "<<i+1<<"\n";
		cout << "Enter x-coordinate\n";
		int x_coordinate;
		cin >> 	x_coordinate;
		target_rob.push_back(x_coordinate);
		cout << "Enter y-coordinate\n";
		int y_coordinate;
		cin >> 	y_coordinate;
		target_rob.push_back(y_coordinate);

		Targets.push_back(target_rob);
		target_rob.clear();
	}
	
	//SETTAGGIO COLORE OSTACOLI
	Eng::set_Points(point_color_struct);
	cout<<"setPoints";


    try
    {
    	//Cámara
    	Capturadevideo.open(ncamara); // Capturadevideo.open(número de la cámara)
    	Capturadevideo.set(CV_CAP_PROP_FRAME_WIDTH, ancho);
    	Capturadevideo.set(CV_CAP_PROP_FRAME_HEIGHT, alto);
    	double Brightness;
	Capturadevideo.set(CV_CAP_PROP_BRIGHTNESS,Brightness);
	Capturadevideo.set(CV_CAP_PROP_FPS, 60);

    	usleep(1000000);
    	//Comprobar si el video se abre
        if (!Capturadevideo.isOpened())
        {
            cerr<<"No se puede abrir el video"<<endl;
            return -1;
        }


        char key=0;

        double time_counter = 0;

		clock_t this_time = clock();
		clock_t last_time = this_time;
//		Capturadevideo.retrieve(Imagen);
	do{
		this_time = clock();
		time_counter += (double)(this_time - last_time);
		last_time = this_time;
		Capturadevideo.read(Imagen);
		Mat im = Imagen.clone();
	
		Eng::Eng_Process(nrobots, anchop, largop, im, aver, go, esquinafinal, esquina, Robotmundo, Robotm, punto1mundo, punto2mundo, obstaculo, point_color_struct);
		
    		std::vector<vector<double> > robotcm; // lista de robots
    		// Cada robot tendrá un vector de tres elementos: x, y, theta (orientación)
    		// el par x, y representa el punto de inicio de cada robot
    		std::vector<double> rob_tempVar;
		if (aver){
				
		 for( int i=0;i<nrobots;i++)
			{
				theta=(atan2((punto2mundo[i].y-punto1mundo[i].y),(punto2mundo[i].x-punto1mundo[i].x)));
				rob_tempVar.push_back((int)(Robotm[i].x*100));
				rob_tempVar.push_back((int)(Robotm[i].y*100));
				rob_tempVar.push_back(theta);
				robotcm.push_back(rob_tempVar);
				
				rob_tempVar.clear();
			}		
		
            	std::vector<vector<vector <double> > > paths;
            	std::vector<int> seq_celle_occ;

            	/*CALCULO DE TRAYECTORIAS */
            	if(counttt == 0){   //Primera vez que se ejecuta el programa (calculo de trayectorias y envio al robot)
			cout<<"Salida del modulo de vision"<< endl;
			for( int i=0;i<nrobots;i++){
				cout<<"Robot["<<i+1<<"]=("<<robotcm[i][0]<<","<<robotcm[i][1]<<","<<robotcm[i][2]<<") -- (x,y,theta)"<<endl;
			}
			obstacles_filter(&obstaculo,area_minima);

			std::ofstream fout2("obstacles.txt");
			fout2<<"[";
			for(int ii=0;ii<obstaculo.size();++ii){
				fout2<<"x:\n";
				std::vector<std::vector<double>> temp_obst = obstaculo[ii];
				for (int j = 0; j < temp_obst.size(); ++j) {
					fout2<< temp_obst[j][0]<< "  ";
				}
				fout2<< "\n";
			}
			for(int ii=0;ii<obstaculo.size();++ii){
				fout2<<"y:\n";
				std::vector<std::vector<double>> temp_obst = obstaculo[ii];
				for (int j = 0; j < temp_obst.size(); ++j) {
					fout2<< temp_obst[j][1]<< "  ";
				}
				fout2<< "\n";
			}
			fout2<<"\n";
			fout2.close();
		
			std::cout<<"Algoritmo de planificacion para calcular trayectorias:"<< endl <<"1: celdas rectangulares"<< endl << "2: Voronoi"<< endl;
			int x;
			std::cin>>x;

          		if (x==1){  //calcular trayectorias con particion en cuadrados
	    		paths = Process::process(width, height, obstaculo, robotcm, Targets, seq_celle_occ);
            		// stampa dei punti trovati, "re_sizati" e centrati
            		for (unsigned int j = 0; j < paths.size(); j++)
            		{                   			
            			std::vector<std::vector <double> > path = paths[j];
            			std::vector<std::vector <double> > path_metri;       			
            			for (unsigned int k = 0; k < path.size(); k++)
            			{
            				vector<double> tempory;
            				double conversion = (double)((int)(((path[k][0] + 1) * 25) - 12.5))/100;
            				double conversion2 = (double)((int)(((path[k][1] + 1) * 25) - 12.5))/100;

            				 //controllo per permettere, qualora la distanza dal primo punto
            				 //del path rispetto alla posizione attuale risulti essere minore
            				 //di 25 cm, di escludere quest'ultimo dal percorso da inviare al robot
            				if(k == 0){
            					double dist_new = std::sqrt((((conversion) - (robotcm[j][0]/100)) * ((conversion) - (robotcm[j][0]/100))) + (((conversion2) - (robotcm[j][1]/100)) * 							((conversion2) - (robotcm[j][1]/100))));
            					if(dist_new < 0.25)
            						std::wcout << "Escluso "<<conversion<<" e "<<conversion2<<std::endl;
            					else{
            						tempory.push_back(conversion);
            						tempory.push_back(conversion2);
            						path_metri.push_back(tempory);
            						tempory.clear();
            					}
            				}
            				else{
            					tempory.push_back(conversion);
            					tempory.push_back(conversion2);
							//	per evitare che vadano fuori pista--->
								if(tempory[0] <= 0.12)
									tempory[0] = 0.30;
								if(tempory[1] <= 0.12)
									tempory[1] = 0.30;

            					path_metri.push_back(tempory);
            					tempory.clear();
            				}
            			}
            			paths_metri.push_back(path_metri);

            		}
			/* - FIRST POINT HAS TO BE THE CURRENT POSITION (NORMALIZED IF NECESSARY) FOR EACH ROBOT OBTAINED BY THE CAMERA.*/
            		for(int i = 0; i < nrobots; i++){
				vector<double> first_point_current_pos;
				double conv_uno = double(robotcm[i][0])/100;
				double conv_due = double(robotcm[i][1])/100;
				conv_uno = (int)(conv_uno*100)/25;
				conv_uno = (double)((int)(((conv_uno + 1) * 25) - 12.5))/100;            				
				conv_due = (int)(conv_due*100)/25;
				conv_due = (double)((int)(((conv_due + 1) * 25) - 12.5))/100;  
				first_point_current_pos.push_back(conv_uno);
				first_point_current_pos.push_back(conv_due);
						
				paths_metri[i].insert(paths_metri[i].begin(),first_point_current_pos);
			}
			}// end if x==1

			

			if (x==2) //cálculo de trayectorias utilziando diagramas de Voronoi
			{
				paths = Process::processVoronoi((int)(largop*100),(int)(anchop*100), obstaculo, robotcm, Targets);
            			// Impresión de los puntos encontrados
            			for (unsigned int j = 0; j < paths.size(); j++)
            			{                   			
            				std::vector<std::vector <double> > path = paths[j];
            				std::vector<std::vector <double> > path_metri;       			
            				for (unsigned int k = 0; k < path.size(); k++)
            				{
            					vector<double> tempory;
            					double conversion = (double)((int) path[k][0])/100;
            					double conversion2 = (double)((int) path[k][1])/100;
						tempory.push_back(conversion);
            					tempory.push_back(conversion2);
						path_metri.push_back(tempory);
            					tempory.clear();
            				}
            				paths_metri.push_back(path_metri);
            			}
				vector<vector <double> > temp_path=paths[0];
				/*for(int ii=0;ii<temp_path.size();++ii){
					cout<< temp_path[ii][0]<< " , " << temp_path[ii][1] << "; ";

				}
				cout<< endl;*/

				std::ofstream fout("paths.txt");
				fout<<"[";
				for(int ii=0;ii<paths.size();++ii){
					std::vector<std::vector <double> > temppp = paths[ii];
					for (int jj=0;jj<temppp.size();++jj){
					fout<< temppp[jj][0]<< " , " << temppp[jj][1] << "; "<<"\n";
					}
					fout<<"]";
				}
				fout<<"\n";
				fout.close();
			} // END IF X==2

		// PRINT COORDINATES OF THE ELEMENTS INVOLVED

		cout<<"Robot positions (x(cm), y(cm), rad):"<< endl;
      		for( int i=0;i<nrobots;i++)
		{
			theta=(atan2((punto2mundo[i].y-punto1mundo[i].y),(punto2mundo[i].x-punto1mundo[i].x)));
			Robotmundo[i]=Point3f(Robotm[i].x,Robotm[i].y,theta);
			cout<<"Robot["<<i+1<<"]=("<<robotcm[i][0]<<","<<robotcm[i][1]<<","<<robotcm[i][2]<<")"<<endl;
		}
		cout<<"Obstacles positions (cm):"<< endl;
		for(unsigned int i = 0; i < obstaculo.size(); i++ ){
			vector<vector<double>> mm = obstaculo[i];
			for(unsigned int j = 0; j < mm.size(); j++ ){
				cout<<"obstaculo["<<i+1<<"] esquina["<<j+1<<"]: "<<mm[j][0]<<" "<<mm[j][1]<<endl;
			}
			cout<<endl;
		}
		cout<<"Platform boundaries (cm):"<< endl;
		std::wcout<<"width: "<<largop*100<<std::endl;
		std::wcout<<"height: "<<anchop*100<<std::endl;

		cout<<"Paths (cm):"<< endl;
		for(unsigned int i = 0; i < nrobots; i++ ){
			vector<vector<double>> mm = paths[i];
			cout<<"Robot "<<i<<": "<<endl;
			for(unsigned int j = 0; j < mm.size(); j++ ){
				cout<<mm[j][0]<<" , "<<mm[j][1]<<" / ";
			}
			cout<<endl;
		}
		

		
		// PETRI NET S4PR
		if (nrobots>1){
					Input_S4PR net(paths_metri);
					m_0 = net.get_m_0();
					pre = net.get_Pre();
					post = net.get_Post();
					resources = net.get_resources();
					int numProcesses = net.get_nprocesses();
					int numPlaces = net.get_dimP();
					int numTrans = net.get_dimT();
					std::vector<std::vector<int>> processes = net.get_processes();

					S4PR PetriNet;
					PetriNet.read_from_file2(numPlaces, numTrans, numProcesses, pre, post, processes, resources, m_0);

					S4PR newPetriNet;
					newPetriNet = PetriNet.getControlledPetriNet();
		
					std::set<int> monitor_places= newPetriNet.getPmonitor();
					resources.pop_back(); //remove (-1): see Input_S4PR.

					for (std::set<int>::iterator it = monitor_places.begin(); it != monitor_places.end(); ++it) {
						std::vector<int> vec_pre = newPetriNet.getPre().rowFromMatrix(*it);
						pre.push_back(vec_pre);
						std::vector<int> vec_post = newPetriNet.getPost().rowFromMatrix(*it);
						post.push_back(vec_post);
						resources.push_back(*it);
					}
					m_0 = newPetriNet.getM0();

					control.push_back(1);
					control.push_back(1 + paths_metri[0].size());
			
					//SEND PATHS (in m)
					//SEND IDLE PLACES
					for (unsigned int i = 0; i < paths_metri.size(); ++i) {
						std::vector<std::vector<double>> temp_path = paths_metri[i];
						std::vector<std::vector<double>> temp_path_to_send;
						for (unsigned int j = 0; j < 1; ++j) {
							std::vector<double> temp;
							
							temp.push_back(temp_path[j][0]);
							temp.push_back(temp_path[j][1]);
							temp_path_to_send.push_back(temp);
							//cout<<"R"<<i<<" path to send: "<< temp_path_to_send[j][0] <<" , "<< temp_path_to_send[j][1] << endl;
						}
						paths_to_send.push_back(temp_path_to_send);
						control_access.push_back(true);
					}
		} else {
					cout<<"transmitting path to R1:";
            				for(unsigned int i = 0; i < paths_metri.size(); i++){
            					send_path_WiFi(paths_metri[i], i+1);
            					//usleep(4000000);
            				}
		

            				for(int kkk = 0; kkk < 2; kkk++){
						for(unsigned int i=0;i<nrobots;i++){								
							send_packet_WiFi((robotcm[i][0]/100),(robotcm[i][1]/100),robotcm[i][2], i+1);
							cout<<"Sending updated position ("<< robotcm[i][0]/100 << "," << robotcm[i][1]/100 << "," << robotcm[i][2] << ") to robot "<< i+1 <<endl;
							//usleep(500000);
						}
					}
            	}
		
            		
		//Check if during the normalization phase there are equal initial points (two robots in the same position, which is not allowed) 
		for(int i = 0; i < nrobots; i++){
			vector<double> first_point_i = paths_metri[i][0];
			for(int j = 0; j < nrobots; j++){
				if(i != j){
				//If the starting points are the same ...
					if(first_point_i[0] == paths_metri[j][0][0] and first_point_i[1] == paths_metri[j][0][1]){
					double tt = (robotcm[i][0]/100) - (robotcm[j][0]/100);
					if( (robotcm[j][0]/100) - (robotcm[i][0]/100)  > 0.10)
						paths_metri[j][0][0] = paths_metri[j][0][0] + 0.25;
					else if((robotcm[j][1]/100) - (robotcm[i][1]/100)  > 0.10)
						paths_metri[j][0][1] = paths_metri[j][0][1] + 0.25;
					}
				}								
			}				
		}
					
					
		/*for(int i = 0; i < nrobots; i++){
				std::wcout << "Initial point of robot "<<i+1<<" = " << paths_metri[i][0][0] << " " << paths_metri[i][0][1] << " (m) " << std::endl;
		}*/

            	counttt++; // Incremento por lo que solo envío las trayectorias una vez
            	//for(int i = 0 ; i < nrobots; i++)
            	//	std::wcout<<"Enviando el primer paquete de robot "<<i+1<<" : "<<(robotcm[i][0]/100)<<" "<<(robotcm[i][1]/100)<<" "<<(robotcm[i][2])<<std::endl;
            	
/*		cout<<"END SETTING UP, CONTINUARE? (INVIO)"<<endl;
            	cin.get();
            	//cin.get();*/
		
		
		fout3.open("robotcm.txt", std::ofstream::out | std::ofstream::app);
		fout3<<"[";							
		fout3<< robotcm[0][0]<< " , " << robotcm[0][0]<< "; ";							
		
		fout3.close();
		
            	}// END IF COUNT==0
    				
    				
  		
            	if(counttt != 0){ //cout<<" count!=0" << endl;

		    if(time_counter > (double)(NUM_SECONDS * CLOCKS_PER_SEC)){
            				//cout<<"time_counter: " << time_counter << endl;
					//cout<<"num_sec " << NUM_SECONDS * CLOCKS_PER_SEC << endl;
				
			if (nrobots>1){
				int it = 0;
				cout<<"NUEVA ITERACION: "<<endl;
				for (unsigned int i = 0; i < paths_metri.size(); ++i) {
					std::cout << "R" << i+1 <<  std::endl;
					if (control_access[i]) {
						std::vector<std::vector<double>> temp_path = paths_metri[i];
						std::vector<std::vector<double>> temp_path_to_send = paths_to_send[i];
						paths_to_send[i].clear();
						cout<<"*******************************"<<endl;
						int j = control[i];
						std::vector<double> temp;
						std::vector<int> resources_involved = search_in_matrix(j - 1, pre, resources);

							if (!resources_involved.empty()){
								if (resource_available(resources_involved, &m_0)) {
									pick_token(resources_involved, &m_0);
									if (it_while != 1) {
										temp.push_back(temp_path[j - it - correccion_eliminacion - 1][0]);
										temp.push_back(temp_path[j - it - correccion_eliminacion - 1][1]);
										temp_path_to_send.push_back(temp);
										temp.clear();
									}
									temp.push_back(temp_path[j - it - correccion_eliminacion][0]);
									temp.push_back(temp_path[j - it - correccion_eliminacion][1]);
									temp_path_to_send.push_back(temp);
									modo[i] = "active";
									cout << "R" << i+1 << " modo ACTIVO. Asignados "<<resources_involved.size()<<" recursos compartidos: "<<resources_involved[0]<<endl;
									
									temp.clear();
								}
								else { std::cout << "R" << i+1 << " modo espera" << std::endl;
								modo[i] = "wait"; }
							}
							else { modo[i] = "active";
								std::cout << "R" << i+1 << " modo ACTIVO. Calculando trayectoria hasta proxima region compartida" << std::endl;
								int ittt=1;

							 	while (resources_involved.empty()){
									if (ittt!=1) {
										control.at(i) = j + 1;
										j = control[i];
									}
							if (it_while != 1) {
										temp.push_back(temp_path[j - it - correccion_eliminacion - 1][0]);
										temp.push_back(temp_path[j - it - correccion_eliminacion - 1][1]);
										temp_path_to_send.push_back(temp);
										temp.clear();
									}	

									temp.push_back(temp_path[j - it - correccion_eliminacion][0]);
									temp.push_back(temp_path[j - it - correccion_eliminacion][1]);

									temp_path_to_send.push_back(temp);


									
									resources_involved = search_in_matrix(j - 1 + 1, pre, resources);
									temp.clear();
									++ittt;
								}
							}
						
						paths_to_send.at(i) = temp_path_to_send;
					}
					it = paths_metri[i].size();
				}

				/*std::cout << "Marcado inicial  ";
				for (unsigned int i = 0; i < m_0.size(); ++i) {
					std::cout << m_0[i] << " ";
				}
				std::cout << std::endl;*/
				cout<<"Paths to send (cm):"<< endl;
				for(unsigned int i = 0; i < paths_to_send.size(); i++ ){
					vector<vector<double>> mm = paths_to_send[i];
					cout<<"Robot "<<i<<": "<<endl;
					for(unsigned int j = 0; j < mm.size(); j++ ){
						cout<<mm[j][0]<<" , "<<mm[j][1]<<" / ";
					}
					cout<<endl;
				}
				cout<<"**************"<<endl;
				//for (unsigned int i = 0; i < paths_to_send.size(); ++i) {
				
					if (!paths_to_send[0].empty()) {
						cout<< "transmitting path to R1"<<endl;
						send_path_WiFi(paths_to_send[0], 0+1);
					} else cout<< "no path to be transmitted R1"<<endl;
					if (!paths_to_send[0].empty()) {
						cout<< "transmitting path to R1"<<endl;
						send_path_WiFi(paths_to_send[0], 0+1);
					}
					
					
					if (!paths_to_send[1].empty()) {
						cout<< "transmitting path to R2"<<endl;
						send_path_WiFi(paths_to_send[1], 1+1);
					} else cout<< "no path to be transmitted R2"<<endl;
					if (!paths_to_send[1].empty()) {
						cout<< "transmitting path to R2"<<endl;
						send_path_WiFi(paths_to_send[1], 1+1);
					}
					
					
				//}
				cout<<"**************"<<endl;
				int control_posicion=0;
				for (unsigned int ii = 0; ii < paths_to_send.size(); ++ii) {
					cout<<"R "<<ii<<":"<<endl;
					std::vector<std::vector<double>> temp_paths_metri = paths_metri[ii];
					cout<<"control de region: "<< control[ii]-control_posicion<<endl<<"-----"<<temp_paths_metri[control[ii]-control_posicion][0]<<" , "<<temp_paths_metri[control[ii]-control_posicion][1]<<endl;
					if (distancia(robotcm[ii],temp_paths_metri[control[ii]-control_posicion],0.20)){
						cout<<"Robot en area de destino, se permite acceso a la generacion de la nueva secuencia de regiones"<<endl;
						std::vector<int> resources_involved = search_in_matrix(control[ii], post, resources);
						return_token(resources_involved, &m_0);
						int xx = control[ii];
						control.at(ii) = xx + 1;
						paths_to_send[ii].clear();
						control_access[ii] = true;

					} else if (modo[ii] == "wait") { //Robot en modo espera.
						cout<<"El robot se encuentra en modo espera."<<endl;
						control_access[ii] = true;
						paths_to_send[ii].clear();
						
					} else {				//El robot todavía no ha llegado al destino de la secuencia enviada.

						control_access[ii] = false;
						paths_to_send[ii].clear();
						cout<<"El robot todavia no ha llegado al destino de la secuencia enviada."<<endl;

					}
					control_posicion += paths_metri[ii].size();
					
				}
				cout<<"**************"<<endl;
				/*std::cout << "Marcado inicial  ";
				for (unsigned int i = 0; i < m_0.size(); ++i) {
					std::cout << m_0[i] << " ";
				}
				std::cout << std::endl;

				for (unsigned int t=0; t < control.size(); ++t) {
					std::cout << "control " << t << ": " << control[t] << std::endl;
				}*/

				int travleled_path = 0;
			
				for (unsigned int jj = 0; jj < control.size(); ++jj) {
					travleled_path = travleled_path + paths_metri[jj].size() + correccion_eliminacion;
					//std::cout << "paths_metri.size: " << paths_metri[jj].size() << endl;
					//std::cout << "traveled_path: " << travleled_path << endl;
					//std::cout << "control[jj]: " << control[jj] << endl;
					if (control[jj] > (travleled_path - 1)) { 
						correccion_eliminacion = (paths_metri.size() - jj - 1)*paths_metri[jj].size();
						paths_metri.erase(paths_metri.begin() + jj);
						paths_to_send.erase(paths_to_send.begin() + jj);
						control.erase(control.begin() + jj);
					}
				}
				//std::cout << "control size: " << control.size() << endl;
				++it_while;
				cout<<"**************"<<endl;

			}//fin nrobots>1
					
						cout<<"-----ACTUALIZACION DE LA POSICION------"<<endl<<endl;
						for(int m = 0; m<=1; m++){
							
							
							for(unsigned int i=0;i<nrobots;i++){
								
							// NON INVIARE PACCHETTI SE AL ROBOT I-ESIMO SE QUEST'ULTIMO E' CONTENUTO NEL VETTORE ROBOT_FERMARE 	   								
								
								
									
									//if (!(robotcm[i][0]==1) && !(robotcm[i][1]==239) && !(robotcm[i][2]==0)){//Punto falso, marcador de la esquina sup izq
									
									send_packet_WiFi((robotcm[i][0]/100),(robotcm[i][1]/100),robotcm[i][2], i+1);
									cout<<"Robot["<<i+1<<"]=("<<robotcm[i][0]<<","<<robotcm[i][1]<<","<<robotcm[i][2]<<") -- (x,y,theta)"<<endl;
									cout<<"sending updated position to robot "<<i +1 <<endl;
									//usleep(2000000);
									fout3.open("robotcm.txt",std::ofstream::out | std::ofstream::app);
									fout3<< robotcm[i][0]<< " , " << robotcm[i][0]<< "; ";
									fout3.close();
									//}
									
									
								
									
							}
						}
			}//end time_counter 
				
            		    		
            				counttt = 2;
            				

            	} // END COUNT != 0
            	
            } // END AVER
            
            
             
		imshow("marcadores detectados",Imagen);
		key=cv::waitKey(tiempocap);//wait for key to be pressed
		if(key == 10){
			go = true;
		}

        }while(key!=27 && Capturadevideo.grab());// END DO

    } catch (std::exception &ex)

    {
        cout<<"Exception :"<<ex.what()<<endl;
    }
 // END
}



