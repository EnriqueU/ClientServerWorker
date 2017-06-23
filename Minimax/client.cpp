#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <thread>

using namespace std;

int main(){
	char wait;
	// socket para el servidor de tareas, latencia
	int *sock;
	// limites de integracion
	unsigned int a = 0 , b = 2000000000;
	//estructuras para la coneccion de los servidores
	struct sockaddr_in server;
	// mensaje de envio y mensaje de respuesta
	char message[1024], server_reply[1024];
	//crear socket del servidor de tareas
	sock = new int;
	*sock = socket(AF_INET, SOCK_STREAM, 0);

	if(*sock == -1){
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
	if (connect(*sock,(struct sockaddr *)&server, sizeof(server)) < 0){
		perror("Fallo la coneccion servidor de tareas");
		return -1;
	}else{
	  cout << "Conectado a servidor de tareas\n" << endl;
	}
	while(1){
		// Aqui se envia un mensaje para el servidor
		cout << "Escriba mensaje para el servidor : ";
		cin >> message;
		if(send(*sock, message, strlen(message),0) < 0){
			cout << "no se pudo enviar  " << endl;
			// break;
		}
		cout << "Esperando resultado ... " << endl;
		if(recv(*sock, server_reply, 1024,0)){
			cout << "El resultado es : " << server_reply <<endl;
		}
	}
	cin >> wait;
	free(sock);
}
