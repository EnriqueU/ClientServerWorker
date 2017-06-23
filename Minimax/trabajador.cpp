#include <iostream>
#include <sstream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include<bits/stdc++.h>
#include <string>

using namespace std;
//particiones 4e9
#define I 100000
#define NH 4
double result = 0.0;

int n=0,h=0;

// Valores Iniciales de Aplha y Beta
const int MAX = 1000;
const int MIN = -1000;

int AplphaBetaPruning(int depth, int nodeIndex, bool maximizingPlayer, int values[], int alpha, int beta, int h){
    // Condición de terminación. Es decir, el nodo hoja es alcanzado
    if (depth == h)
        return values[nodeIndex];

    if (maximizingPlayer){
        int best = MIN;
        // Recorre los nodos hijos de izquierda a derecha
        for (int i=0; i<2; i++){
            int val = AplphaBetaPruning(depth+1, nodeIndex*2+i,false, values, alpha, beta, h);
            best = max(best, val);
            alpha = max(alpha, best);
            // Alpha Beta Pruning
            if (beta <= alpha)
                break;
        }
        return best;
    }else{
        int best = MAX;
        // Recorre los nodos hijos de izquierda a derecha
        for (int i=0; i<2; i++){
            int val = AplphaBetaPruning(depth+1, nodeIndex*2+i,true, values, alpha, beta, h);
            best = min(best, val);
            beta = min(beta, best);
            // Alpha Beta Pruning
            if (beta <= alpha)
                break;
        }
        return best;
    }
}

int Minimax(int depth, int nodeIndex, bool isMax, vector<int> scores){
    // Condición de terminación. Es decir, el nodo hoja es alcanzado
    if (depth == h)
        return scores[nodeIndex];

    // Si el desplazamiento actual es maximizar, se busca el valor máximo alcanzable
    if (isMax)
       return max(Minimax(depth+1, nodeIndex*2, false, scores),
            Minimax(depth+1, nodeIndex*2 + 1, false, scores));

    // Si el movimiento actual es Minimizar, se busca el valor mínimo alcanzable
    else
        return min(Minimax(depth+1, nodeIndex*2, true, scores),
            Minimax(depth+1, nodeIndex*2 + 1, true, scores));
}

void funcion(int depth, int nodeIndex, bool isMax, vector<int> scores){
	result = Minimax(depth,nodeIndex,isMax,scores);
}

vector<int> split(string str, char delimiter) {
  vector<int> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
		std::string res (tok);
		std::string::size_type sz;     // alias of size_t
		int r = std::stod (res,&sz);
    internal.push_back(r);
  }
  return internal;
}

//function que calcula la integral de la funcion identidad
void integral(int id){
	clock_t start = clock();
	//cout << "calculando ... " << endl;
	for(int i = (id-1)*(I/NH) ; i < (id)*(I/NH) ; i++){
		for(int j = 0 ; j < I ; j++)
			result = result + i;
	}
	cout <<  " thread " << id << " termino en  " << (clock() - start)/CLOCKS_PER_SEC  << "segundos " << endl;
}

void runtask(thread job[],char reply[],int sock, vector<int> scores){
	clock_t init = clock();
	//----------------------------
	for(int i = 0 ; i < NH ; i++){
		job[i] = thread(funcion,0,0,true,scores);
		//job[i] = thread(integral,i+1);
	}
	for(int i=0; i < NH ; i++){
		job[i].join();
	}
	//----------------------------
	cout << "Enviando resultado al servidor : " << endl;
	std::ostringstream os;
	os << result;
	string str = os.str();
	std::cout << "La suma es: " << result<< endl;

	str =  "t" + str;

	for(int i =0;i<str.length();i++){
		reply[i] = str.at(i);
	}
	if(send(sock, reply, strlen(reply),0) < 0){
		cout << "no se pudo enviar  " << endl;
	}
	result = 0.0;
}

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
			vector<int> scores = split(server_reply, ',');
			n = scores.size();
	    h = log2(n);
			runtask(job,reply,sock,scores);
		}else{
			cout << "No se recibio mensaje del servidor" << endl;
		}
	}
}
