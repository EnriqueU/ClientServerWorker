#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <pthread.h>
#include <thread>
#include <vector>

using namespace std;
double result = 0.0;
#define I 1000
#define NH 1
#define N 10
int count = 0,count2 = 0;
vector<int> sockets;

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

void check_connection(int* accept_client, int create_client, struct sockaddr_in client, int size){
	*accept_client = accept(create_client, (struct sockaddr*) &client,(socklen_t*)&size);
}

void* connection(void* new_socket){
  int sock = *(int*)new_socket; // socket  del cliente
  int recv_msg; // controla si se recibe mensaje del cliente
  char* message, client_message[1024], mensaje[1024], reply[1024];// mensaje a enviar y a recibir
	double times[N][5];
  message = "Conectado al servidor"; //mensaje de coneccinon

	//
  while(1){
  	if(recv(sock,client_message, 1024, 0)<0)
  		break;
		// mensaje reciido por el cliente
  	if(client_message[0] == 'C'){
  		cout << "cliente : " << client_message << endl;
  		cout << "Enviando mensaje al trabajadors " <<endl;

			//  You can also assign directly to a string.
			/*string  str = client_message;
			str = str.substr(1,str.length()-1); // Nos quedamos con solo el número
			for(int i =1;i<=str.length();i++)
				mensaje[i] = str.at(i-1);*/
	    for(int i = 0; i < sockets.size()-1 ; i++){
				int ind = i+1;
				char n = (char)(ind+48);
				mensaje[0] = n;
				if(write(sockets[i],mensaje,strlen(mensaje)) < 0){
					cout << "No se pudo enviar mensaje " << endl;
					break;
				}
			}
			thread gg(integral,1);
			gg.join();
		// mensaje reciido por el trabajador
	}else if(client_message[0] == 't'){
  		cout << "Mensaje del trabajador recibido: " << client_message << endl;
			string  str = client_message;
			str = str.substr(1,2);
			int in = atoi(str.c_str()); // Indice de que llego primero
			if(count2%(sockets.size()-1)==0){
				cout << "Valor de count" << count << endl;
				cout << "Indice es: " << in<<endl;
				times[count][in]=1;
				count++;
			}
			count2++;
			if(count2==N*(sockets.size()-1)){
				for (int i = 0; i < N; i++) {
					for (int j = 0; j < 5; j++) {
						printf("%6.2f", times[i][j]);
					}
					printf("\n");
				}
			}

			/*if(count==N){
				std::ostringstream os;
				os << result;
				string str = os.str();

				for(int i =0;i<str.length();i++){
					reply[i] = str.at(i);
				}
				//std::cout << endl<< "La suma es: "<< result << '\n';
				if(write(sockets[sockets.size()-1],reply,strlen(reply)) < 0){
		    	break;
	    	}
			}*/
  	}
  }
  // cuando muere el cliente
  if(recv_msg == 0){
    cout << "Cliente Desconectado" << endl;
    fflush(stdout);
  } else if(recv_msg == -1){
    perror("fallo en recibir mensaje");
  }
  free(new_socket);
  return 0;
}


int main(){
	clock_t start;
	//opciones
	char option;
	// socket del client
	int create_client, accept_client, read;
	//id del cliente
	int client_id = 0;
	//tamanio de la estructura sockaddr_in
	int size;
	//puntero al socket para enviar al hilo
	int *new_socket;
	//mensaje del cliente
	char client_message[1024];
	// datos del servidor y cliente
	struct sockaddr_in server,client;
	//limite de la cola de espera
	int queueLimit = 3;
	//socket del cliente
	create_client = socket(AF_INET, SOCK_STREAM, 0);
	if(create_client == -1){
		cout << "No se pudo crear el socket" << endl;
		return -1;
	}
	cout << " Socket creado " << endl;

	// datos del servidor
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(4000);

	// enlace al cliente
	if((bind(create_client, (struct sockaddr*)&server,sizeof(server)))<0){
		cout << "Error bind" << endl;
		return -1;
	}

	cout << "Enlace Creado" << endl;
	// esuchar conecciones
	listen(create_client, queueLimit);
	// tamanio de la estructura
	size = sizeof(struct sockaddr_in);

	// aceptar conecciones
	while(accept_client = accept(create_client, (struct sockaddr*)&client,(socklen_t*)&size)){
		cout << "Se establecio coneccion con el servidor" << endl;
		pthread_t client_thread;
		new_socket = new int;
		*new_socket = accept_client;
		// crear hilo para el nuevo servidor
		sockets.push_back(accept_client);
		if(pthread_create(&client_thread,NULL,connection,(void*)new_socket)<0){
			perror( "No se pudo crear el hilo" );
			return 1;
		}
		cout << "Nueva coneccion cliente " << client_id++ << endl;
		//write(create_client, client_message,strlen(client_message));
	}

	if(accept_client < 0){
		perror("FALLO EN ACEPTAR");
		return -1;
	}
	return 0;
}
