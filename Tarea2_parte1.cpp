#include <iostream>
#include <vector>
#include <thread>
#include <pthread.h> 
#include <unistd.h>  
#include <cstdlib>

using namespace std;

// clase Monitor qparaa barrera reutilizable
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

