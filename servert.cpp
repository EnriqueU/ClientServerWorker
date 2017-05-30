#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <pthread.h>
#include <thread>
#include <vector>

using namespace std;

#define N 100000
#define n 100

vector<int> sockets,socketst;

int integral(int id){
	clock_t start = clock();
	//cout << "calculando " << endl;
	for(int i = (id-1)*(N/n) ; i < (id)*(N/n) ; i++){
		for(int j = 0 ; j < N ; j++)
			;
	}
	//cout <<  " thread " << id << " termino en  " << (clock() - start)/CLOCKS_PER_SEC  << " segundos "<< endl;
	return 1;
}

int doJob(){
	thread case1[n];
	for(int i = 0 ; i < n ; i++){
		cout << "Thread :" << i << " Working " << endl;
		case1[i] = thread(integral,i);
	}
	for(int i=0; i < n ; i++){
		case1[i].join();
	}
}

void check_connection(int* accept_client, int create_client, struct sockaddr_in client, int size){
	*accept_client = accept(create_client, (struct sockaddr*) &client,(socklen_t*)&size);
}

void* connection(void* new_socket){
  int sock = *(int*)new_socket; // socket  del cliente
  int recv_msg; // controla si se recibe mensaje del cliente
  char* message, client_message[1024], mensaje[1024];// mensaje a enviar y a recibir

  sockets.push_back(sock);
  message = "Conectado al servidor"; //mensaje de coneccinon

	//
  while(1){
  	if(recv(sock,client_message, 1024, 0)<0)
  		break;
		// mensaje reciido por el cliente
  	if(client_message[0] == 'C'){
  		cout << "cliente : " << client_message << endl;
  		cout << "Enviando mensaje al trabajadors " <<endl;
			cout << "Len "<< strlen(client_message) << endl;

			//  You can also assign directly to a string.
			string  str = client_message;
			str = str.substr(1,str.length()-1);
			//std::string str = client_message.str();
			//client_message = client_message.substr(1,client_message.length()-1);
	    //client_message[0] = '1';
			for(int i =0;i<str.length();i++){
				mensaje[i] = str.at(i);
			}
			cout << "Enviando: " << mensaje<< endl;
	    for(int i = 0; i < sockets.size() -2 ; i++)
		    if(write(sockets[i],mensaje,strlen(mensaje)) < 0){
					cout << "No se pudo enviar mensaje " << endl;
					break;
				}
				thread gg(doJob);
				gg.join();
		// mensaje reciido por el trabajador
	}else if(client_message[0] == 't'){
  		cout << "trabajador: " << client_message << endl;
  		cout << "Mensaje del trabajador recibido: " ;
		  client_message[0] = 'a';

	    if(write(sockets[sockets.size() - 1],client_message,strlen(client_message)) < 0){
	    	break;
    	}

  	}


  }
  // cuando muere el cliente
  if(recv_msg == 0){
    cout << "Cliente Desconectado" << endl;
    fflush(stdout);
  }
  else if(recv_msg == -1){
    perror("fallo en recibir mensaje");
  }

  free(new_socket);

  return 0;

}


int main(){
	clock_t start;


	//opciones
	char option;

	// hilos caso1
	thread case1[n];

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

	cout << " Tipo de trabajo : " ;
	cin >> option;

	switch(option){
		case '1':
			for(int i = 0 ; i < n ; i++){
				cout << "Thread :" << i << " Working " << endl;
				case1[i] = thread(integral,i);

}			for(int i=0; i < n ; i++){
				case1[i].join();
				// cout << "Thread " << i << " finished job" << endl;
			}
			break;
		case '2':
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
			break;
		default:
		break;

	}


	return 0;
}
