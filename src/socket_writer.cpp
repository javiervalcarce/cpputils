// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#include "socket_writer.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <cstdio>
#include <cerrno>
#include <cassert>

using namespace librest;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SocketWriter::SocketWriter() {
      fd_ = -1;
      total_byte_count_ = 0;
      fd_byte_count_ = 0;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SocketWriter::~SocketWriter() {
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SocketWriter::AssignNewFileDescriptor(int socketfd) {
      fd_ = socketfd;

#ifdef __MACH__
      // Esto es para Mac OS X y otros UNIX pero no Linux
      int optval = 1;     
      // SO_NOSIGPIPE evita que al hacer write() sobre un socket de una conexión cerrada el proceso reciba la señal
      // SIGPIPE que aborta el proceso si no se trata adecuadamente. Como esto es una librería no es recomendable andar
      // instalando manejadores de señal, esto es cosa a nivel de aplicación, así que lo hago así.
      setsockopt(fd_, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval));
#endif


      fd_byte_count_ = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Lee un bloque de |byte_count| caracteres, el caracter nulo ('\0') no debe aparece, si lo hacer se devuelve el error
 * ERROR_ZERO_FOUND.
 * @return El código de error
 */
SocketWriter::Error SocketWriter::WriteData(const char* s, uint64_t byte_count, uint64_t* bytes_written) {
      
      assert(s != NULL);
      assert(bytes_written != NULL);
      
      uint64_t a;
      int w;

      //printf("SocketWriter::WriteData %d\n", byte_count);
      
      a = 0;
      while (a < byte_count) {

#ifdef __MACH__           
            // Mac OS X y otros UNIX
            // desde el año 2011 write() nunca devuelve 0 en POSIX
            w = (int) write(fd_, s + a, byte_count - a); 
#else
            // Linux, MSG_NOSIGNAL sirve para que si la conexión tcp está cerrada o se ha roto, el proceso entero no reciba
            // la señal SIGPIPE, cuya acción por defecto es terminar el proceso.
            w = (int) send(fd_, s + a, byte_count - a, MSG_NOSIGNAL); 
#endif

            if (w == -1) {

            // Si fd_ es un fichero físico en disco y si se ha compilado sin la macro 
            // -D_FILE_OFFSET_BITS=64 entonces el tamaño max del fichero es 2GB, devolviendo EFBIG 27 
            //
            //printf("SocketWriter::WriteData ERROR_IO en write() %d\n", (int) errno);
            //perror("SocketWriter::WriteData");
                *bytes_written = a;
                fd_byte_count_ += a;
                total_byte_count_ += a;
                return ERROR_IO;
            }

            a += w;
      }

      *bytes_written = a;
      fd_byte_count_ += a;
      total_byte_count_ += a;
      return ERROR_NO;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SocketWriter::Error SocketWriter::Flush() {
      int flag;

      // deshabilita el algoritmo de Naggle, esto provoca que salgan todos los datos actualmente almacenados en el búfer
      // de tx
      flag = 1; 
      setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

      // vuelve a habilitarlo
      flag = 0; 
      setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
      return ERROR_NO;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
