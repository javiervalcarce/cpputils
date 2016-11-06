
// runtime
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <ctime>
#include <cfloat>


// app
#include "dispatcher.h"
//#include <libsepwraplog/libsepwraplog.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Task::Task(std::string myname, TaskFunc myfunc, void* myuser_data, int mypriority)
{
      //pthread_mutex_init(&lock, NULL);
      name = myname;
      func = myfunc;
      user_data = myuser_data;
      priority = mypriority;
      clk    = new MeanVariance();
      cpu    = new MeanVariance();
      clkbuf = new CircularBuffer<double>(16);     // capacidad para 16 últimas medidas
      cpubuf = new CircularBuffer<double>(16);     // capacidad para 16 últimas medidas
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Task::~Task()
{
      delete clk;
      delete cpu;
      delete clkbuf;
      delete cpubuf;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Esta función es el hilo procesador
 * @param arg Puntero a un objeto de tipo SynQueue<Task>
 */
void* Dispatcher::ThreadFunc(void* arg)
{
      Dispatcher* obj = static_cast<Dispatcher*>(arg);
      
      struct timeval clk_tick;
      struct timeval clk_tock;
      clock_t cpu_tick;
      clock_t cpu_tock;
      
      obj->m_thread_pid = (int) getpid();

      while (1)
      {
            obj->m_busy = false;

            //----------------------------------------------------------------------------------------------------
            Task* t = obj->m_procq->Front(); // bloqueante...
            
            obj->m_procq->Pop();
            
            // ejecutamos la tarea y medimos su tiempo de ejecución de reloj y de cpu
            //----------------------------------------------------------------------------------------------------
            obj->m_busy = true;
            
            gettimeofday(&clk_tick, 0);
            cpu_tick = clock();

            //miprintf(L_TR, "JVG-Planificador: proc %s <-", t->name.c_str());
            t->func(t->user_data);
           

            gettimeofday(&clk_tock, 0);
            cpu_tock = clock();
            //----------------------------------------------------------------------------------------------------

            double clk_elapsed = (clk_tock.tv_sec * 1000 + clk_tock.tv_usec/1000) - 
                  (clk_tick.tv_sec * 1000 + clk_tick.tv_usec/1000);

            double cpu_elapsed = ((double) (cpu_tock - cpu_tick)  / CLOCKS_PER_SEC) * 1000.0;

            t->clk->Compute(clk_elapsed);
            t->cpu->Compute(cpu_elapsed);
            
            // almacenamos el valor en el búfer circular
            /*
            miprintf(L_TR, "JVG-Planificador: almaceno en CircularBuffer<float>");
            t->clkbuf->Push(clk_elapsed);
            t->cpubuf->Push(cpu_elapsed);
            */

            obj->m_executed++; 
      }

      return 0;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Dispatcher::Dispatcher(int capacity)
{
      //int error;
      size_t stacksize;

      m_executed = 0;
      m_busy = false;

      //const int kQueueCapacity = 50;

      m_procq = new SyncQueue<Task*>(capacity);
      //m_procq = new SyncPriorityQueue<Task*, CompareTask>(capacity);

      pthread_attr_init(&m_thread_attr);
      pthread_attr_setdetachstate(&m_thread_attr, PTHREAD_CREATE_DETACHED);
      pthread_attr_getstacksize(&m_thread_attr, &stacksize);

      //miprintf(L_TR, "JVG-Planificador: Tamaño mínimo de la pila      = %d bytes", PTHREAD_STACK_MIN);
      //miprintf(L_TR, "JVG-Planificador: Tamaño de la pila por defecto = %d bytes = STACKMIN x %.2f", stacksize, (float)stacksize / PTHREAD_STACK_MIN);

      //pthread_attr_setstacksize(&m_thread_attr, 0x20000);

      pthread_attr_getstacksize(&m_thread_attr, &stacksize);
      //miprintf(L_TR, "JVG-Planificador: Tamaño de la pila que usamos  = %d bytes = STACKMIN x %.2f", stacksize, (float)stacksize / PTHREAD_STACK_MIN);

      pthread_create(&m_thread, &m_thread_attr, ThreadFunc, (void*) this);
      
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Dispatcher::Push(Task* t)
{
      m_procq->Push(t);

      return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Dispatcher::Size()
{
      return m_procq->Size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Dispatcher::Capacity()
{
      return m_procq->Capacity();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Dispatcher::ExecutedTaskCount()
{
      return m_executed;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Dispatcher::IsBusy()
{
      return m_busy;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int Dispatcher::Pid()
{
      return m_thread_pid;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
