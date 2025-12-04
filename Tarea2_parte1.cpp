#include <iostream>
#include <vector>
#include <thread>
#include <pthread.h> 
#include <unistd.h>  
#include <cstdlib>

using namespace std;

// Clase Monitor qparaa barrera reutilizable
class Monitor_Barrera
{
public:
    // inicializar barrera con N hebras
    Monitor_Barrera(int N_hebras) : N(N_hebras), count(0), etapa(0)
    {
        // inicializar mutex y variable estilo POSIX
        pthread_mutex_init(&lock, nullptr);
        pthread_cond_init(&cond, nullptr);
    }

    ~Monitor_Barrera()
    {
        pthread_mutex_destroy(&lock);
        pthread_cond_destroy(&cond);
    }

    
    void wait()
    {
        pthread_mutex_lock(&lock); // 1. LOCK

        int mi_etapa = etapa; 
        
        count++; // llego una hebra mas

        if (count == N)
        {
            // si es la ultima hebra:
            count = 0; // resetea elcontador
            etapa++;     // avanza de etapa
            pthread_cond_broadcast(&cond); // despierta a todas las hebras
        }
        else
        {
            //mientras la etapa global no cambie sigue esperando.
        
            while (mi_etapa == etapa)
            {
                pthread_cond_wait(&cond, &lock);
            }
        }

        pthread_mutex_unlock(&lock); // UNLOCK
    }

private:
    int N;              // numero de hebras requeridas
    int count;          // hebras que han llegado
    int etapa;          // etapa actual
    pthread_mutex_t lock; // mutex POSIX
    pthread_cond_t cond;  // variable de condicion POSIX
};

// FunciON HEBRA para VERIFICACION
void hebra_trabajadora(Monitor_Barrera &barrera, int id, int etapas_totales)
{
    for (int e = 0; e < etapas_totales; ++e)
    {
        // a) trabajo simulado
        int tiempo = rand() % 500 + 100; 
        usleep(tiempo * 1000); 

        // b) imprime antes de esperar
        printf("[Hebra %d] esperando en etapa %d\n", id, e);

        // c) llama a wait() 
        barrera.wait();

        // d) imprime despues de pasar
        printf("[Hebra %d] paso barrera en etapa %d\n", id, e);
    }
}

int main(int argc, char *argv[])
{
    srand(time(nullptr));

    // valores por defecto
    int N = 5; // numero hebras
    int E = 4; // numero etapas

    // leer parametros : ./barrera -n 5 -e 4
    if (argc >= 5) {

        N = atoi(argv[2]); 
        E = atoi(argv[4]); 
    } else {
        cout << "USO SUGERIDO: " << argv[0] << " -n <num_hebras> -e <etapas>" << endl;
        cout << "USANDO VALORES DEFECTO: N=5, E=4" << endl;
    }

    Monitor_Barrera barrera(N);
    vector<thread> hebras;

    cout << "INICIA VERIFICACION" << endl;

    // crea N hebras
    for (int i = 0; i < N; ++i)
    {
        hebras.push_back(thread(hebra_trabajadora, ref(barrera), i, E));
    }

    // espera que termine
    for (auto &h : hebras)
    {
        h.join();
    }

    cout << "TERMINO DE LA EJECUCION" << endl;
    return 0;
}
