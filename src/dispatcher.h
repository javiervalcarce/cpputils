#pragma once

// runtime
#include <pthread.h>
#include <string>

// app
#include "mean_variance.h"
#include "sync_queue.h"
//#include "sync_priority_queue.h"
#include "circular_buffer.h"



/**
 * Representa una tarea
 */
struct Task
{
      typedef int (*TaskFunc)(void*);  // puntero a la función que define la tarea

      Task(std::string myname, TaskFunc myfunc, void* user_data, int mypriority = 0);
      ~Task();

      //pthread_mutex_t lock;          // para garantizar acceso no concurrente a las variables de esta estructura
      std::string   name;              // nombre de la tarea, informativo para depuración
      int           priority;          // prioridad de esta tarea, cuando mayor más prioritario
      TaskFunc      func;              // tarea a ejecutar 
      void*         user_data;         // parámetros de la tarea
      MeanVariance* clk;               // variable aleatoria "tiempo de reloj"
      MeanVariance* cpu;               // variable aleatoria "tiempo de cpu"
      CircularBuffer<double>* clkbuf;  // almacena los últimos N(=25) valores del tiempo de ejecución de reloj
      CircularBuffer<double>* cpubuf;  // almacena los últimos N(=25) valores del tiempo de ejecución de cpu

};



/**
 * Comparador para la priority_queue<Task>
 */
class CompareTask 
{
public:
      bool operator()(Task* t1, Task* t2) // Returns true if t1 is earlier than t2
      {
            return (t1->priority < t2->priority);
      }
};


/**
 * Cola de tareas + hilo procesador
 */   
class Dispatcher
{
public:

      Dispatcher(int capacity);
      
      /** Encola una tarea */
      int  Push(Task* t); 

      /** Número de tareas en la cola */
      int  Size();     

      /** Número máximo de tareas que puede haber en la cola */
      int  Capacity();

      /** Número de tareas procesadas hasta el momento */
      int  ExecutedTaskCount();

      /** Si hay una tarea en ejecución ahora mismo en el recurso */
      bool IsBusy();
 
      /** PID del hilo procesador de tareas */
      int  Pid();


private:

      static void* ThreadFunc(void* arg);

      pthread_t      m_thread;       // id del hilo procesador de tareas     
      pthread_attr_t m_thread_attr;  // atributos del hilo 
      int            m_thread_pid;   // pid del hilo 

      int m_executed;
      bool m_busy;

      SyncQueue<Task*>* m_procq;
      //SyncPriorityQueue<Task*, CompareTask>* m_procq;
};

