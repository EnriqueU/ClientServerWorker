#include <iostream>
#include <sstream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <thread>
#include <vector>

//particiones 4e9
#define I 1000
#define NH 100
#define N 10
using namespace std;
double result = 1234.0;
vector<int> tareas;
//function que calcula la integral de la funcion identidad
//function que calcula la integral de la funcion identidad
int integral(int id){
	clock_t start = clock();
	//cout << "calculando ... " << endl;
	for(int i = (id-1)*(I/NH) ; i < (id)*(I/NH) ; i++){
		for(int j = 0 ; j < I ; j++)
			result = result + i;
	}
	cout <<  " thread " << id << " termino en  " << (clock() - start)/CLOCKS_PER_SEC  << "segundos " << endl;
	return 1;
}

//estructura para enviar los argumentos de la funcion al hilo
struct t_args{
  int socket;
  const char message[11] = "Estoy vivo";
};


int main(){
	char wait;
	// socket para el servidor de tareas, latencia
	int sock, sock2;
	// status de la funcion pthread_create
	int server2_t;
	int client_id;
	// limites de integracion
	unsigned int a = 0 , b = 2000000000;
	//estructuras para la coneccion de los servidores
	struct sockaddr_in server, server2;
	// mensaje de envio y mensaje de respuesta
	char message[1024], server_reply[1024], reply[1024];
	//hilo para enviar el mensaje al servidor de latencia
	pthread_t server2_thread;
	// argumentos de la funcion para enviar al hilo
	struct t_args socket_args;
	//crear socket del servidor de tareas
	sock = socket(AF_INET, SOCK_STREAM, 0);
	thread job[NH];

	if(sock == -1){
		cout << "No se pudo crear el socket servidor tareas " << endl;
		return -1;
	}else{
	  cout << "Socket Servidor de Tareas Creado" << endl;
	}

	// Datos del servidor de tareas
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(4000);


	// Coneccion al sevidor de tareas
	if (connect(sock,(struct sockaddr *)&server, sizeof(server)) < 0){
		perror("Fallo la coneccion servidor de tareas");
		return -1;
	}else{
	  cout << "Conectado a servidor de tareas" << endl;
	}

	//recibir mensaje del servidor de tareas
	while(1){
		cout << "Esperando mensaje del servidor " << endl;
		// reciiendo el mensaje del servidor
		if(recv(sock, server_reply, 1024,0)){
			cout << "Mensaje Recibido : " << server_reply <<endl;
			for(int l=0;l<N;l++)
				tareas.push_back(100);

			while(!tareas.empty()){
				for(int i = 0 ; i < NH ; i++){
					job[i] = thread(integral,i+1);
				}
				for(int i=0; i < NH ; i++){
					job[i].join();
				}
				cout << "Enviando resultado al servidor : " << endl;
				string str =  server_reply;
				str = "t" + str;

				for(int i =0;i<str.length();i++){
					reply[i] = str.at(i);
				}
				if(send(sock, reply, strlen(reply),0) < 0){
					cout << "no se pudo enviar  " << endl;
					break;
				}
				tareas.pop_back();
			}
		}else{
			cout << "No se recibio mensaje del servidor" << endl;
		}
	}
}
