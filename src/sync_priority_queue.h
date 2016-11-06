
//
// sync_priority_queue.h - Blocking priority queue implemented using POSIX synchronization API
//
// Javier Valcarce García
//

// Since this is a template class the implementation goes in the header file
// there is no cpp file

#pragma once

#include <pthread.h>
#include <queue>
#include <vector>

#include <libsepwraplog/libsepwraplog.h>


template <class T, class C> 
class SyncPriorityQueue
{        
      std::priority_queue<T, std::vector<T>, C>*  m_queue;
      int             m_capacity;
      int             m_count;
      pthread_mutex_t m_lock;
      pthread_cond_t  m_cond_space; // variable de condición sobre la que suspenderse si no hay espacio
      pthread_cond_t  m_cond_items; // variable de condición sobre la que suspenderse si no hay elementos

public:
      
      SyncPriorityQueue(int capacity); 
      ~SyncPriorityQueue(); 
      
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
template<class T, class C> SyncPriorityQueue<T, C>::SyncPriorityQueue(int capacity) 
{
      int error;
      m_capacity = capacity;
      m_count = 0;

      error = pthread_mutex_init(&m_lock, NULL);
      if (error)
      {
            miprintf(L_ERR, "pthread_mutex_init() error");
      }

      error = pthread_cond_init(&m_cond_space, NULL);
      if (error)
      {
            miprintf(L_ERR, "pthread_cond_init(&m_cond_space) error");
      }

      error = pthread_cond_init(&m_cond_items, NULL);
      if (error)
      {
            miprintf(L_ERR, "pthread_cond_init(&m_cond_items) error");
      }

      m_queue = new std::priority_queue<T, std::vector<T>, C>(); 
      if (m_queue == NULL)
      {
            miprintf(L_ERR, "SyncPriorityQueue::SyncPriorityQueue() bad_alloc");
      }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T, class C> SyncPriorityQueue<T, C>::~SyncPriorityQueue() 
{
      
      delete m_queue; 

      pthread_mutex_destroy(&m_lock);
      pthread_cond_destroy(&m_cond_space);
      pthread_cond_destroy(&m_cond_items);

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T, class C> int SyncPriorityQueue<T, C>::Push(const T& val)
{
      int error;

      //miprintf(L_TR, "JVG-Push: lock");
      error = pthread_mutex_lock(&m_lock);
      if (error)
      {
            miprintf(L_ERR, "SyncPriorityQueue::Push pthread_mutex_lock() error");
      }

      while (m_count == m_capacity)
      {
            //miprintf(L_TR, "JVG-Push: wait");

	    error = pthread_cond_wait(&m_cond_space, &m_lock); 

            if (error)
            {
                  miprintf(L_ERR, "SyncPriorityQueue::Push pthread_cond_wait() error");
            }
      }

      m_queue->push(val);  	
      m_count++;
      
      //miprintf(L_TR, "JVG-Push: signal");
      error = pthread_cond_signal(&m_cond_items);
      if (error)
      {
            miprintf(L_ERR, "SyncPriorityQueue::Push pthread_cond_signal() error");
      }

      //miprintf(L_TR, "JVG-Push: unlock");
      error = pthread_mutex_unlock(&m_lock);    
      if (error)
      {
            miprintf(L_ERR, "SyncPriorityQueue::Push pthread_mutex_unlock() error");
      }


      return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T, class C> T& SyncPriorityQueue<T, C>::Front() 
{
      int error;

      //miprintf(L_TR, "JVG-Front: lock");

      error = pthread_mutex_lock(&m_lock);
      if (error)
      {
            miprintf(L_ERR, "SyncPriorityQueue::Front pthread_mutex_lock() error");
      }

      while (m_count == 0)
      {
            //miprintf(L_TR, "JVG-Front: wait");// poblema, no se despierta jamás

	    error = pthread_cond_wait(&m_cond_items, &m_lock); 
            if (error)
            {
                  miprintf(L_ERR, "SyncPriorityQueue::Front pthread_cond_wait() error");
            }

            //miprintf(L_TR, "JVG-Front: despierto del wait!");
      }
      
      
      // una priority_queue no tiene front(), tiene top()
      //miprintf(L_TR, "JVG-Front: top");
      T& val = (T&) m_queue->top(); 
      
      
      //miprintf(L_TR, "JVG-Front: unlock");
      error = pthread_mutex_unlock(&m_lock);    
      if (error)
      {
            miprintf(L_ERR, "SyncPriorityQueue::Front pthread_mutex_unlock() error");
      }
      

      return val; 
}              


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T, class C> void SyncPriorityQueue<T, C>::Pop()
{
      int error;

      //miprintf(L_TR, "JVG-Pop: lock");

      error = pthread_mutex_lock(&m_lock);
      if (error)
      {
            miprintf(L_ERR, "SyncPriorityQueue::Pop pthread_mutex_lock() error");
      }


      while (m_count == 0)
      {
            //miprintf(L_TR, "JVG-Pop: wait");

	    error = pthread_cond_wait(&m_cond_items, &m_lock); 
            if (error)
            {
                  miprintf(L_ERR, "SyncPriorityQueue::Pop pthread_cond_wait() error");
            }
      }

      //miprintf(L_TR, "JVG-Pop: pop"); // se queda aquí ?

      m_queue->pop();
      m_count--;

      //miprintf(L_TR, "JVG-Pop: signal");
      error = pthread_cond_signal(&m_cond_space);//jvg after
      if (error)
      {
            miprintf(L_ERR, "SyncPriorityQueue::Pop pthread_cond_signal() error");
      }


      //miprintf(L_TR, "JVG-Pop: unlock");      
      error = pthread_mutex_unlock(&m_lock);    
      if (error)
      {
            miprintf(L_ERR, "SyncPriorityQueue::Pop pthread_mutex_unlock() error");
      }


}
  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T, class C> int SyncPriorityQueue<T, C>::Size() 
{
      int error;
      unsigned int sz;

      error = pthread_mutex_lock(&m_lock);
      if (error)
      {
            miprintf(L_ERR, "SyncPriorityQueue::Size pthread_mutex_lock() error");
      }

      sz = m_queue->size();
      
      error = pthread_mutex_unlock(&m_lock);
      if (error)
      {
            miprintf(L_ERR, "SyncPriorityQueue::Size pthread_mutex_unlock() error");
      }

      return sz;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T, class C> int SyncPriorityQueue<T, C>::Capacity() 
{
      return m_capacity;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



