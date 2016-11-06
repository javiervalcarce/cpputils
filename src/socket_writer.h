// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#ifndef LIBREST_SOCKET_WRITER_H_
#define LIBREST_SOCKET_WRITER_H_

// platform
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
// c++
#include <string>

namespace librest {

/**
 * Escribe en un socket.
 *
 */
class SocketWriter {
public:
      
      enum Error {
            ERROR_NO = 0,         // No error
            ERROR_READ_TIMEOUT,   // el temporizador de escritura de caracteres ha vencido
            ERROR_LINE_TIMEOUT,   // el temporizador de escritura de linea completa ha vencido
            ERROR_LINE_TOO_LONG,  // La linea es demasiado larga y no cabe en el búfer interno
            ERROR_ZERO_FOUND,     // se ha encontrado un caracter '\0'
            ERROR_IO              // Error general en las operaciones de E/S sobre el socket
      };
      

      /**
       * Constructor.
       */
      SocketWriter();
      
      /**
       * Destructor.
       */
      ~SocketWriter();

      /**
       * Reset object's state and assign a new file descriptor.
       *
       * @param socketfs The new file descriptor.
       */
      void AssignNewFileDescriptor(int socketfd);
      
      int FileDescriptor() { return fd_; }
      
      /**
       * Número total de bytes escritos en cualquier socket asignado con AssignNewFileDescriptor().
       */
      uint64_t TotalByteCount() const { return total_byte_count_; }
      
      /**
       * Número de bytes escritos en el socket actualmente asignado con AssignNewFileDescriptor().
       */
      uint64_t FileDescriptorByteCount() const { return fd_byte_count_; }
      
      /**
       * Escribe un bloque de |byte_count| caracteres almacenado en s, en el descriptor de fichero, |bytes_written|
       * es el número de bytes realmente escritos, en caso de error este número puede no ser igual a |byte_count|
       *
       * Esta función tiene sentido porque, además de encapsular la llamada write() específica de POSIX, reintenta
       * si write no pudiese escribir todos los datos, es una función de más alto nivel de write(). En general, la
       * llamada write() no debería usarse directamente porque tiene un nivel muy bajo.
       *
       * @return El código de error, ver la documentación del tipo SocketWriter::Error
       */
      Error WriteData(const char* s, uint64_t byte_count, uint64_t* bytes_written);

      /**
       * Envia inmediatamente los datos actualmente almacenados en el búfer de transmisión, esto lo consigue 
       * desactivando y a continuación volviendo a activar el algoritmo de Naggle en esta conexión TCP
       */
      Error Flush();
      
private:

      int   fd_;       // Descriptor de fichero del socket sobre el que escribo.
      
      uint64_t total_byte_count_;
      uint64_t fd_byte_count_;
      
};

} // librest

#endif // LIBREST_SOCKET_WRITER_H_
