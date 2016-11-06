// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#ifndef LIBREST_SOCKET_READER_H_
#define LIBREST_SOCKET_READER_H_

// platform
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// c++
#include <string>
#include <stdint.h>


namespace librest {

/**
 * Buffered string reader.
 *
 * Lee cadenas de texto de un descriptor de fichero. El descriptor de fichero (fd: file descriptor) pueder estar
 * asociado a un fichero físico en el disco o a un puerto serie o a un socket de comunicaciones, de acuerdo con 
 * la conocida filosofía UNIX según la cual todo se ve como un fichero.
 * 
 * La tarea parece sencilla usando las funciones de stdio.h pero no lo es: pueden pasar muchas cosas (bloqueos,
 * distntos formatos de fin de línea, etc) que el asunto tiene la suficiente entidad como para escribir una clase 
 * que se ocupe de este problema. No existe nada similar a esta clase en la STL.
 *
 */
class SocketReader {
public:

      /**
       * Errores generados por esta clase
       */
      enum Error {
            ERROR_NO = 0,          // No hay error
            ERROR_EOF,             // Se he llegado a la condición EOF mientras se leían caracteres del tubo
            ERROR_READ_TIMEOUT,    // el temporizador de lectura de caracteres ha vencido
            ERROR_LINE_TIMEOUT,    // el temporizador de lectura de linea completa ha vencido
            ERROR_LINE_TOO_LONG,   // La linea es demasiado larga y no cabe en el búfer interno
            ERROR_ZERO_FOUND,      // se ha encontrado un caracter '\0'
            ERROR_IO               // Error general en las operaciones de E/S sobre el socket
      };
      
      
      /**
       * Ctor.
       *
       * @param max_line_length Tamaño del búfer que se usa para almacenar los caracteres de una línea, determina la 
       * longitud máxima de una línea.
       *
       * @param read_char_timeout Tiempo de espera máximo en una operación read() sobre el socket.
       *
       * @param use_ssl Especifica si este objeto usará SSL_read() en lugar de read() para leer del socket, active esta
       * opción solamente si el socket ha sido previamente configurado con SSL y el handshake ya se ha establecido. Si
       * establece a true esta opción y la opción -DUSE_SSL no ha sido especificada saltará un assert() y terminará el
       * proceso.
       */
      SocketReader(int max_line_length, int read_char_timeout, bool use_ssl = false);

      
      /**
       * Dtor.
       */
      ~SocketReader();

      
      /**
       * Reinicia los contadores de caracteres en el búfer y establece un nuevo socket sobre el que leer. Sirve para
       * recliclar este objeto (en lugar de destruirlo con delete y volver a crearlo con new, que lleva tiempo).
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
       * Si el socket se cierra entonces el indicador eof() devolverá true, en ese caso ya no será posible leer nunca 
       * más del socket 
       */
      bool EndOfFile() const { return eof_; }

      /**
       * BLOQUEANTE con temporizador
       *
       * Lee caracteres del socket hasta encontrar un caracter o una secuencia de caracteres que marquen el fin de línea 
       * "\r\n" o "\r" o "\n". El caracter o secuencia de caracteres de fin de línea no se incluye en la cadena devuelta
       *
       * El caracter nulo no debería aparecer en el socket ya que este es un lector de líneas de texto, si lo hace se 
       * devuelve el error ERROR_ZERO_FOUND aunque la línea se leerá correctamente, si la aplicación no considera esto
       * como un error pues perfecto, a esta clase realmente le da lo mismo.
       *
       * @param s Cadena de texto en la que se almecenará la línea leída
       * @return El código de error
       */
      Error ReadLine(std::string& s);

      /** 
       * BLOQUEANTE con temporizador
       *
       * Lo mismo pero deja los caracteres leídos en un cadena string, el número de bytes leídos se podrá averiguar
       * fácilmente usando std::string::length() 
       */
      Error ReadData(std::string& s, uint64_t byte_count);

      /**
       * Lee un bloque de |byte_count| caracteres, si aparece EOF o un error de E/S entonces |bytes_read| contendrá los
       * bytes que realmente se han leído.
       *
       * @return El código de error
       */
      Error ReadData(char* s, uint64_t byte_count, uint64_t* bytes_read);


      /** 
       * Devuelve un puntero a una secuencia de caracteres sobrantes (podría incluir el '\0') leídos del socket 
       * y almacenados en el búfer interno del objeto SocketReader, a al espera de ser extraidos en la próxima
       * llamda a ReadLine()
       *
       * @param s puntero a la secuencia, asignado por la función
       * @param sz número de caracteres, 0 significa que no hay ningun caracter sobrante
       */
      void GetRemanentChars (char** s, int* sz);

private:

      int   fd_;
      uint64_t total_byte_count_;
      uint64_t fd_byte_count_;
      bool use_ssl_;
      
      char* strbuf_;   // búfer de caracteres
      int   buflen_;   // tamaño máximo del búfer de caracteres
      uint64_t   strlen_;   // número de caracteres que hay en el búfer
      uint64_t   strmax_;   // longitud máxima de una línea de texto
      uint64_t   readsz_;   // número de caracteres a leer en cada operación read()
      bool  eof_;      // flag End Of File/Stream en el socket fd_
      
      fd_set set_;
      struct timeval timeout_;

      void SearchForEndOfLine(char* s, int len, int& delimiter_pos, int& delimiter_len);
      Error ReadAndAppend();
     
};

} // librest

#endif // LIBREST_SOCKET_READER_H_
