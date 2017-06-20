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
#define N 100000
#define n 100
int count = 0;
int count2 = 0;
vector<int> socketsT,socketsC;
int t = 0;

int integral(int id){
	clock_t start = clock();
	//cout << "calculando " << endl;
	for(int i = (id-1)*(N/n) ; i < (id)*(N/n) ; i++){
		for(int j = 0 ; j < N ; j++)
			;
	}
	cout <<  " thread 0" << " termino en  " << (clock() - start)/CLOCKS_PER_SEC  << " segundos "<< endl;
	return 1;
}

void check_connection(int* accept_client, int create_client, struct sockaddr_in client, int size){
	*accept_client = accept(create_client, (struct sockaddr*) &client,(socklen_t*)&size);
}

void* connection(void* new_socket){
  int sock = *(int*)new_socket; // socket  del cliente
  int recv_msg; // controla si se recibe mensaje del cliente
  char* message, client_message[1024], mensaje[1024], reply[1024];// mensaje a enviar y a recibir

  message = "Conectado al servidor"; //mensaje de coneccinon

	// Semilla
	srand(time(NULL));

  while(1){
  	if(recv(sock,client_message, 1024, 0)<0)
  		break;
		// mensaje reciido por el cliente
  	if(client_message[0] == 'C'){
  		cout << "cliente : " << client_message << endl;
  		cout << "Enviando mensaje al trabajadores " <<endl;
			//cout << "Len "<< strlen(client_message) << endl;

			//  You can also assign directly to a string.
			string  str = client_message;
			str = str.substr(1,str.length()-1);

			// Cantidad de tareas
			t = (rand()%5+1)*4;
			printf("La Cantidad de Tareas Repartidas son: %d\n",t);

			for(int i =0;i<str.length();i++){
				mensaje[i] = str.at(i);
			}

	    for(int i = 0; i < t ; i++){
				if(write(socketsT[i%4],mensaje,strlen(mensaje)) < 0){
					cout << "No se pudo enviar mensaje " << endl;
					break;
				}
			}
				//thread gg(integral,100);
				//gg.join();
	// mensaje recibido por el trabajador
	}else if(client_message[0] == 't'){
  		cout << "Mensaje del trabajador: " << client_message << endl;
			string  str = client_message;
			str = str.substr(1,str.length()-1);

			std::string res (str);
  		std::string::size_type sz;     // alias of size_t

			double r = std::stod (res,&sz);
			result+=r;
			count++;

			if(count==t){
				std::ostringstream os;
				os << result;
				string str = os.str();

				for(int i =0;i<str.length();i++){
					reply[i] = str.at(i);
				}
				//std::cout << endl<< "La suma es: "<< result << '\n';
				if(write(socketsT[socketsT.size()-1],reply,strlen(reply)) < 0){
		    	break;
	    	}
			}else{
				printf("Hola mundo\n");
			}
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
			}
			for(int i=0; i < n ; i++){
				case1[i].join();
				cout << "Thread " << i << " finished job" << endl;
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
				socketsT.push_back(accept_client);
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
