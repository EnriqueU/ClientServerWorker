# ClientServerWorker

* Sin Cola de Tareas:

Cliente envía tarea a servidor,este reparte la tarea a los trabajadores, los trabajadores devuelven la tarea y el servidor retorna el valor el resultado de la operación solicitada por el cliente.

* Con Cola de Tareas:

Clientes envían tareas simultáneamente a servidor,este reparte la tarea a los trabajadores y los agrega a la cola de cada uno, los trabajadores devuelven la tarea y el servidor retorna el valor el resultado de la operación solicitada por el cliente.

## Compile

g++ server.cpp -o server -lpthread --std=c++11
g++ trabajador.cpp -o trabajador -lpthread --std=c++11
g++ client.cpp -o client -lpthread --std=c++11

## Run

./server
./trabajador
./client
