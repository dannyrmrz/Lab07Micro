#include <pthread.h>
#include <iostream>
#include <queue>
#include <unistd.h> // Para sleep()

// Mutex para controlar el acceso a la pista y los gates
pthread_mutex_t pistaMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gatesMutex = PTHREAD_MUTEX_INITIALIZER;

// Variables de condición para sincronización
pthread_cond_t pistaCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t gatesCond = PTHREAD_COND_INITIALIZER;

// Estado de la pista y los gates
bool pistaLibre = true;
int gatesLibres = 2;

// Logging
void logAccion(const std::string& accion, int avion) {
    std::cout << "Avión " << avion << ": " << accion << std::endl;
}

// Función que simula el comportamiento de cada avión
void* avionFunc(void* arg) {
    int avionId = *(int*)arg;

    // 1. Solicitar permiso para aterrizar
    pthread_mutex_lock(&pistaMutex);
    while (!pistaLibre) {  // Esperar si la pista está ocupada
        logAccion("Esperando para aterrizar", avionId);
        pthread_cond_wait(&pistaCond, &pistaMutex);
    }
    // La pista está libre, el avión puede aterrizar
    pistaLibre = false;
    logAccion("Aterrizando", avionId);
    pthread_mutex_unlock(&pistaMutex);

    sleep(1);  // Simular aterrizaje

    // 2. Buscar un gate disponible para desembarcar
    pthread_mutex_lock(&gatesMutex);
    while (gatesLibres == 0) {  // Esperar si no hay gates libres
        logAccion("Esperando un gate disponible", avionId);
        pthread_cond_wait(&gatesCond, &gatesMutex);
    }
    // Hay un gate disponible, el avión lo ocupa
    gatesLibres--;
    logAccion("Ocupando un gate", avionId);
    pthread_mutex_unlock(&gatesMutex);

    sleep(1);  // Simular desembarque

    // 3. Liberar pista y gate
    pthread_mutex_lock(&pistaMutex);
    pistaLibre = true;
    pthread_cond_signal(&pistaCond);  // Notificar que la pista está libre
    pthread_mutex_unlock(&pistaMutex);

    pthread_mutex_lock(&gatesMutex);
    gatesLibres++;
    pthread_cond_signal(&gatesCond);  // Notificar que un gate está libre
    pthread_mutex_unlock(&gatesMutex);

    logAccion("Desembarcando completo", avionId);
    pthread_exit(NULL);
}

int main() {
    pthread_t aviones[4];  // Cuatro hilos para representar los aviones
    int avionIds[4] = {1, 2, 3, 4};  // IDs para los aviones

    // Crear los hilos (aviones)
    for (int i = 0; i < 4; i++) {
        pthread_create(&aviones[i], NULL, avionFunc, (void*)&avionIds[i]);
    }

    // Esperar que todos los hilos terminen
    for (int i = 0; i < 4; i++) {
        pthread_join(aviones[i], NULL);
    }

    // Limpiar los mutex y variables de condición
    pthread_mutex_destroy(&pistaMutex);
    pthread_mutex_destroy(&gatesMutex);
    pthread_cond_destroy(&pistaCond);
    pthread_cond_destroy(&gatesCond);

    return 0;
}
