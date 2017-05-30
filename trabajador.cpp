#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <thread>

//particiones 4e9
#define N 100000
#define NT 100
using namespace std;

//function que calcula la integral de la funcion identidad
void integral(int id){
	clock_t start = clock();
	cout << "calculando ... " << endl;
	for(int i = (id-1)*(N/NT) ; i < (id)*(N/NT) ; i++){
		for(int j = 0 ; j < N ; j++)
			;
	}
	cout <<  " thread " << id << " termino en  " << (clock() - start)/CLOCKS_PER_SEC  << "segundos " << endl;
}

//estructura para enviar los argumentos de la funcion al hilo
struct t_args{
  int socket;
  const char message[11] = "Estoy vivo";
};


int main(){
	char wait;
  	int sock, sock2; // socket para el servidor de tareas, latencia
  	int server2_t; // status de la funcion pthread_create

  	int client_id;

  	unsigned int a = 0 , b = 2000000000; // limites de integracion

	struct sockaddr_in server, server2;//estructuras para la coneccion de los servidores

	// mensaje de envio y mensaje de respuesta
	char message[1024], server_reply[1024];

	//hilo para enviar el mensaje al servidor de latencia
	pthread_t server2_thread;

	// argumentos de la funcion para enviar al hilo
	struct t_args socket_args;

	//crear socket del servidor de tareas
	sock = socket(AF_INET, SOCK_STREAM, 0);

	//crear socket del servidor de latencia
	sock2 = socket(AF_INET, SOCK_STREAM, 0);

	thread job[NT];
	// se asigna el valor del socket2
	socket_args.socket = sock2;


	if(sock == -1){
		cout << "No se pudo crear el socket servidor tareas " << endl;
		return -1;
	}

	else{
	  cout << "Socket Servidor de Tareas Creado" << endl;
	}

	if(sock2 == -1){
	  cout << "No se pudo crear el socket servidor de lateancia" << endl;
	  return -1;
	}
	else{
	  cout << "Socket Servidor de Latencia Creado" << endl;
	}

	// Datos del servidor de tareas
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(4000);


	//Datos del servidor de latencia
	server2.sin_addr.s_addr = inet_addr("127.0.0.1");
	server2.sin_family = AF_INET;
	server2.sin_port = htons(5000);

	// Coneccion al sevidor de tareas
	if (connect(sock,(struct sockaddr *)&server, sizeof(server)) < 0){
		perror("Fallo la coneccion servidor de tareas");
		return -1;
	}

	else{
	  cout << "Conectado a servidor de tareas" << endl;
	}


	//coneccion al servidor de latencia
	if(connect(sock2,(struct sockaddr *)&server2,sizeof(server2)) <0){
	  perror("Fallo la coneccion al servidor 2");
	}
	else{
	  cout << "Conectado al sevidor de latencia " << endl;
	}

	//recibir mensaje del servidor de tareas
	while(1){
		cout << "Esperando mensaje del servidor " << endl;
		// reciiendo el mensaje del servidor
		if(recv(sock, server_reply, 1024,0)){
			cout << "Mensaje Recibido : " << server_reply <<endl;
			for(int i = 0 ; i < NT ; i++){
				//cout << "Thread :" << i << " Working " << endl;
				job[i] = thread(integral,i);
			}
			for(int i=0; i < NT ; i++){
				job[i].join();
				// cout << "Thread " << i << " finished job" << endl;
			}
		}else{
			cout << "No se recibio mensaje del servidor" << endl;
		}

		cout << "Enviando resultado al servidor : " << endl;
		message[0] = 't';
		if(send(sock, message, strlen(message),0) < 0){
			cout << "no se pudo enviar  " << endl;
			break;
		}

	}


}
