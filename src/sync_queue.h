
//
// sync_queue.h - Blocking queue implemented using POSIX synchronization API
//
// Javier Valcarce García
//

// Since this is a template class the implementation goes in the header file
// there is no cpp file


#pragma once

#include <pthread.h>
#include <queue>


/**
 * @class SyncQueue<T>
 * @classdesc Cola síncronizada usando mecanismos de sincronización POSIX
 *
 * Ejemplo:
 * 
 * SyncQueue<int> q(5);
 *
 * q.Push(1);
 * q.Push(3);
 * q.Push(7);
 *
 * q.Size();        // 3
 * q.Capacity();    // 5
 * 
 * q.Push(9);
 * q.Push(10);
 * q.Push(21);      // Aquí este hilo queda bloqueado a la espera de que la cola se vacie desde otro hilo.
 *                  // Si nadie extrajese algún elemento sufririamos un bloqueo indefinido (deadlock)
 *
 * (supongamos que algún hilo extrae algún elemento ==> el programa continúa...)
 *
 * 
 * int x;
 * 
 * x = q.Front();   // x vale 1
 * x = q.Front();   // x vale 1
 *
 * q.Pop();
 * x = q.Front();   // x vale 3
 * x = q.Front();   // x vale 3
 *
 * q.Pop();
 * x = q.Front();   // x vale 7
 *
 * 
 * q.Pop();   
 * x = q.Front();   // x vale 9
 * q.Pop();  
 * q.Pop();  
 * q.Pop();  // La cola está vacía por lo que aquí este hilo queda bloqueado a la espera de que algún 
 *           // hilo ponga un elemento en la cola 
 *
 */

template <class T> 
class SyncQueue
{        
      std::queue<T>*  m_queue;
      int             m_capacity;
      int             m_count;
      pthread_mutex_t m_lock;
      pthread_cond_t  m_cond_space; // variable de condición sobre la que suspenderse si no hay espacio
      pthread_cond_t  m_cond_items; // variable de condición sobre la que suspenderse si no hay elementos

public:
      
      SyncQueue(int capacity); 
      ~SyncQueue(); 
      
      /** Añade un elemento a la cola */
      int  Push(const T& val);  

      /** Devuelve el primer elemento de la cola, pero no lo extrae */
      T&   Front(); 

      /** Extrae el primer elemento de la cola */
      void Pop();

      /** Número de elementos en la cola */
      int  Size();

      /** Capacidad de la cola (número máximo de elementos que puede albergar) */
      int  Capacity();

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T> SyncQueue<T>::SyncQueue(int capacity) 
{
      m_capacity = capacity;
      m_count = 0;

      pthread_mutex_init(&m_lock, NULL);
      pthread_cond_init(&m_cond_space, NULL);
      pthread_cond_init(&m_cond_items, NULL);

      m_queue = new std::queue<T>(); 
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T> SyncQueue<T>::~SyncQueue() 
{
      delete m_queue; 
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T> int SyncQueue<T>::Push(const T& val)
{
      pthread_mutex_lock(&m_lock);

      while (m_count == m_capacity)
      {
	    pthread_cond_wait(&m_cond_space, &m_lock); 
      }

      m_queue->push(val);  	
      m_count++;
      pthread_cond_signal(&m_cond_items);

      pthread_mutex_unlock(&m_lock);    
      return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T> T& SyncQueue<T>::Front() 
{
      pthread_mutex_lock(&m_lock);

      while (m_count == 0)
      {
	    pthread_cond_wait(&m_cond_items, &m_lock); 
      }
      
      T& val = m_queue->front();
      
      pthread_mutex_unlock(&m_lock);    

      return val; 
}              


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T> void SyncQueue<T>::Pop()
{
      pthread_mutex_lock(&m_lock);

      while (m_count == 0)
      {
	    pthread_cond_wait(&m_cond_items, &m_lock); 
      }

      m_queue->pop();
      m_count--;
      pthread_cond_signal(&m_cond_space);

      pthread_mutex_unlock(&m_lock);    
}


  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T> int SyncQueue<T>::Size() 
{
      unsigned int sz;

      pthread_mutex_lock(&m_lock);

      sz = m_queue->size();

      pthread_mutex_unlock(&m_lock);

      return sz;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T> int SyncQueue<T>::Capacity() 
{
      return m_capacity;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



