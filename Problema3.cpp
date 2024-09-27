//Daniela Ramírez de León - 23053
//Javier Alejandro López del Cid - 23415
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>

using namespace std;

// Recursos compartidos
int impresoras = 2;
int escaneres = 1;
int plotters = 1;

// Mutex para cada recurso
pthread_mutex_t mutexImpresoras;
pthread_mutex_t mutexEscaneres;
pthread_mutex_t mutexPlotters;

void usar_recursos(const string& dep, int imp, int esc, int plo) {
    cout << dep << " está usando " << imp << " impresoras, " << esc << " escáneres, " << plo << " plotter." << endl;
    sleep(2);  // Simulamos el tiempo de uso
}

void adquirir_recursos(int imp_necesarias, int esc_necesarios, int plo_necesarios, const string& dep) {
    // Adquirimos impresoras
    pthread_mutex_lock(&mutexImpresoras);
    while (impresoras < imp_necesarias) {
        pthread_mutex_unlock(&mutexImpresoras);
        sleep(1);  // Esperamos un poco y volvemos a intentar
        pthread_mutex_lock(&mutexImpresoras);
    }
    impresoras -= imp_necesarias;
    pthread_mutex_unlock(&mutexImpresoras);

    // Adquirimos escáneres
    pthread_mutex_lock(&mutexEscaneres);
    while (escaneres < esc_necesarios) {
        pthread_mutex_unlock(&mutexEscaneres);
        sleep(1);
        pthread_mutex_lock(&mutexEscaneres);
    }
    escaneres -= esc_necesarios;
    pthread_mutex_unlock(&mutexEscaneres);

    // Adquirimos plotter
    pthread_mutex_lock(&mutexPlotters);
    while (plotters < plo_necesarios) {
        pthread_mutex_unlock(&mutexPlotters);
        sleep(1);
        pthread_mutex_lock(&mutexPlotters);
    }
    plotters -= plo_necesarios;
    pthread_mutex_unlock(&mutexPlotters);

    usar_recursos(dep, imp_necesarias, esc_necesarios, plo_necesarios);
}

void liberar_recursos(int imp_necesarias, int esc_necesarios, int plo_necesarios) {
    // Liberamos impresoras
    pthread_mutex_lock(&mutexImpresoras);
    impresoras += imp_necesarias;
    pthread_mutex_unlock(&mutexImpresoras);

    // Liberamos escáneres
    pthread_mutex_lock(&mutexEscaneres);
    escaneres += esc_necesarios;
    pthread_mutex_unlock(&mutexEscaneres);

    // Liberamos plotter
    pthread_mutex_lock(&mutexPlotters);
    plotters += plo_necesarios; 
    pthread_mutex_unlock(&mutexPlotters);
}

void* departamento(void* arg) {
    string dep = *(string*)arg;

    int imp_necesarias = 1;  // Cada departamento requiere 1 impresora
    int esc_necesarios = 1;  // Cada departamento requiere 1 escáner o plotter
    int plo_necesarios = 1;

    // Intentamos adquirir los recursos
    adquirir_recursos(imp_necesarias, esc_necesarios, plo_necesarios, dep);

    // Liberamos los recursos después de usarlos
    liberar_recursos(imp_necesarias, esc_necesarios, plo_necesarios);

    return nullptr;
}

int main() {
    // Inicialización de mutex
    pthread_mutex_init(&mutexImpresoras, nullptr);
    pthread_mutex_init(&mutexEscaneres, nullptr);
    pthread_mutex_init(&mutexPlotters, nullptr);

    // Creamos 3 departamentos
    pthread_t departamentos[3];
    vector<string> nombres = {"Departamento 1", "Departamento 2", "Departamento 3"};

    for (int i = 0; i < 3; i++) {
        pthread_create(&departamentos[i], nullptr, departamento, (void*)&nombres[i]);
    }

    // Esperamos a que terminen los departamentos
    for (int i = 0; i < 3; i++) {
        pthread_join(departamentos[i], nullptr);
    }

    // Destrucción de mutex
    pthread_mutex_destroy(&mutexImpresoras);
    pthread_mutex_destroy(&mutexEscaneres);
    pthread_mutex_destroy(&mutexPlotters);

    return 0;
}
