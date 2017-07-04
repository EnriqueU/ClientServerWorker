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

double result = 0.0;// resultado de la suma de la integral
int I = 10000;// Intervalo de integracion [0,I]
int NH = 100;// Números de hilos
int N = 10;// Números de tareas
int count = 0,count2 = 0;
vector<int> sockets;// sockets de workers y clientes
double times[100][100]; // Matriz de tiempo
int band=0;

//funcion que calcula la integral de la funcion identidad
int integral(int id){
	clock_t start = clock();
	for(int i = (id-1)*(I/NH) ; i < (id)*(I/NH) ; i++){
		for(int j = 0 ; j < I ; j++)
			result = result + i;
	}
	printf("Thread %d terminó en %.7f segundos\n", id, (clock() - start)/CLOCKS_PER_SEC);
	//cout <<  " thread " << id << " termino en  " << (clock() - start)/CLOCKS_PER_SEC  << "segundos " << endl;
	return 1;
}

void matriz(int x, int y,int v){
	times[x][y] = v;
}

void sendTask(){
	// Ordenamiento por la columna de tiempo esperado
	double temp;
	for(int i=0;i<N-1;i++){
		for(int j=i+1;j<N;j++){
			if(times[j][0]>times[i][0]){
				for(int k=1;k<sockets.size();k++){
					temp=times[j][k];
					times[j][k]=times[i][k];
					times[i][k]=temp;
				}
			}
		}
	}
	// Envia tareas  a los worker segun matriz de tiempos
	char mensaje[1024];
	for(int i=0;i<N;i++){
		for(int j=1;j<sockets.size();j++){
			if(times[i][j]==1){
				int ind = j+1;
				if(ind>9){
					int a = ind/10;
					int b = ind - (10*a);
					char na = (char)(a+48);
					char nb = (char)(b+48);
					mensaje[0] = na;
					mensaje[1] = nb;
				}else{
					char n = (char)(ind+48);
					mensaje[0] = n;
				}
				if(write(sockets[j],mensaje,strlen(mensaje)) < 0){
					cout << "No se pudo enviar mensaje " << endl;
					break;
				}
			}
		}
	}
	band=1;
}

void check_connection(int* accept_client, int create_client, struct sockaddr_in client, int size){
	*accept_client = accept(create_client, (struct sockaddr*) &client,(socklen_t*)&size);
}

void* connection(void* new_socket){
  int sock = *(int*)new_socket; // socket  del cliente
  int recv_msg; // controla si se recibe mensaje del cliente
  char* message, client_message[1024], mensaje[1024], reply[1024];// mensaje a enviar y a recibir
  message = "Conectado al servidor"; //mensaje de coneccinon

  while(1){
  	if(recv(sock,client_message, 1024, 0)<0)
  		break;
		// mensaje reciido por el cliente
  	if(client_message[0] == 'C'){
  		cout << "Cliente : " << client_message << endl;
  		cout << "Enviando mensaje al trabajadors " <<endl;
			//------------------------------------------------------------------------
			// calculando el tiempo esperado
			for(int k=0 ; k<N ; k++){
				clock_t s = clock();
				thread esp(integral,k+1);
				esp.join();
				clock_t f = clock();
				thread esperado(matriz , k , 0 , (f-s)/CLOCKS_PER_SEC);
				esperado.join();
			}
			// Se calcula cuales son los mejores workes para cada tarea
	    for(int i = 0; i < sockets.size()-1 ; i++){
				int ind = i+1;
				if(ind>9){
					int a = ind/10;
					int b = ind - (10*a);
					char na = (char)(a+48);
					char nb = (char)(b+48);
					mensaje[0] = na;
					mensaje[1] = nb;
				}else{
					char n = (char)(ind+48);
					mensaje[0] = n;
				}
				if(write(sockets[i],mensaje,strlen(mensaje)) < 0){
					cout << "No se pudo enviar mensaje " << endl;
					break;
				}
			}
			//------------------------------------------------------------------------
		// mensaje reciido por el trabajador
		}else if(client_message[0] == 't'){
			if(band==0){ // Cuando recibe por primera vez de los workers
				cout << "Mensaje del trabajador recibido: " << client_message << endl;
				string  str = client_message;
				str = str.substr(1,strlen(client_message)-1);
				int in = atoi(str.c_str()); // Indice de que llego primero
				if(count2%(sockets.size()-1)==0){
					cout << "[Tarea][Worker]: ["<<count<<"]["<<in<<"]" << endl;
					thread init(matriz,count,in,1);
					init.join();
					count++;
				}
				count2++;
				if(count2==N*(sockets.size()-1)){
					for (int i = 0; i < N; i++) {
						for (int j = 1; j < sockets.size(); j++) {
							printf("%6.2f", times[i][j]);
						}
						printf("\n");
					}
					sendTask();
				}
			}else if(band=1){ // Cuando recibe una vez obtenida la matriz
				cout << "Mensaje del trabajador recibido: " << client_message << endl;
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
	}else{
		cout << "Socket creado " << endl;
	}

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

	//thread send(sendTask);
	//send.join();
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
	}

	if(accept_client < 0){
		perror("FALLO EN ACEPTAR");
		return -1;
	}
	return 0;
}
