// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#include "socket_reader.h"

// C++
#include <vector>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <locale>         // std::locale, std::toupper

/* According to POSIX.1-2001 */
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef USE_SSL
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif


using namespace librest;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* TODO: int read_line_timeout*/
SocketReader::SocketReader(int max_line_length, int read_char_timeout, bool use_ssl) {
      
      fd_     = -1;                        // socket (solo lectura)
      total_byte_count_ = 0;
      fd_byte_count_ = 0;
      readsz_ = 128;                       // tamaño predeterminado del bloque de lectura sobre el socket
      strmax_ = max_line_length + 2;       // tamaño máximo de la línea, +2 para almacenar '\r\n'
      buflen_ = strmax_ + readsz_;         // tamaño del búfer de caracteres
      strbuf_ = new char[buflen_];         // búfer
      strlen_ = 0;                         // número de caracteres en el búfer
      eof_    = false;                     // EOF del socket

      // Timeout para la operación de lectura
      timeout_.tv_sec  = (read_char_timeout / 1000); 
      timeout_.tv_usec = (read_char_timeout % 1000) * 1000;

      use_ssl_ = use_ssl;
      
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SocketReader::~SocketReader() {
      delete[] strbuf_;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SocketReader::AssignNewFileDescriptor(int socketfd) {

      fd_ = socketfd;
      fd_byte_count_ = 0;
      strlen_ = 0;               // número de caracteres en el búfer
      eof_    = false;           // EOF del socket
      
      FD_ZERO(&set_);            // clear the set 
      FD_SET(fd_, &set_);        // add our file descriptor to the set 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** 
 * Busca un delimitador de final de línea en un búfer de caracteres, desde la posición 0 a len-2, el último caracter no
 * se puede examinar, cuidado con esto.
 *
 * @param s Bufer de bytes/caracteres, puede tener caracteres '\0' en medio
 * @param len Longitud de la cadena de caracteres, este parámetro es necesario porque a no está terminada en '\0'
 * @return delimiter_pos Posición del primer caracter de la marca de fin de línea
 * @return delimiter_len Longitud en caracteres de la marca de fin de línea
 *
 */
void SocketReader::SearchForEndOfLine(char* s, int len, int& delimiter_pos, int& delimiter_len) {
      // búsqueda de "\r\n" o "\n" o "\r"
      int i;
      for (i = 0; i < len - 1; i++) {

            if (s[i] == '\r') {
                  if (s[i + 1] == '\n') {
                        delimiter_pos = i;
                        delimiter_len = 2;
                        return;
                  } else {
                        delimiter_pos = i;
                        delimiter_len = 1;
                        return;
                  }
            }

            if (s[i] == '\n') {
                  delimiter_pos = i;
                  delimiter_len = 1;
                  return;
            } 
      }

      delimiter_pos = -1;
      delimiter_len =  0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Lee bytes del tubo (usando select-read, con temporizador para evitar bloqueos permanentes) y los deposita en strbuf_
 * @param eof Si se devuelve true significa que no hemos leido nada ni podremos leer nada en el futuro (EOF)
 */
SocketReader::Error SocketReader::ReadAndAppend() {

      int rv;
      int rdlen;
      struct timeval this_timeout;

      
      this_timeout = timeout_;
      rv = select(fd_ + 1, &set_, NULL, NULL, &this_timeout);
      if (rv == -1) {
            //printf("SocketReader::ReadAndAppend() ERROR_IO!\n");
            return ERROR_IO;
      } else if (rv == 0) {
            //printf("SocketReader::ReadAndAppend() TIMEOUT!\n");
            return ERROR_READ_TIMEOUT;
      }

      // En este punto tenemos garantizado que read() no se bloqueará, retornará inmediatamente
      rdlen = (int) read(fd_, (void*) (strbuf_ + strlen_), readsz_);
      if (rdlen == -1) {
            //printf("SocketReader::ReadAndAppend() read() error\n");
            return ERROR_IO;
      }
      
      if (rdlen == 0) { 
            eof_ = true;
	    return ERROR_EOF;
      }
      
      fd_byte_count_ += rdlen;
      total_byte_count_ += rdlen;
      strlen_ += rdlen;
      return ERROR_NO;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SocketReader::Error SocketReader::ReadLine(std::string& s) {
      int delimiter_pos;
      int delimiter_len;
      Error error;

      s.clear();
      
      if (eof_) {
            return ERROR_EOF;
      }
      
      // el siguiente algoritmo de extracción de caracteres no es trivial, hace falta leerlo con detenimientos, hacer
      // unos dibujos con lápiz y pale e interiorizarlo
      //
      // es un algoritmo "buffered reader" bastante clásico
      // 
      while (1) {
            // buscamos en el búfer de caracteres (que inicialmente estará vacío) una marca de fin de línea
            // las variables (por referencia) delimiter_pos y delimitar_len localizan dicha marca en el búfer
            SearchForEndOfLine(strbuf_, strlen_, delimiter_pos, delimiter_len);
            
            if (delimiter_pos < 0) {
                  // mmm, no se ha encontrado una marca de fin de línea

                  // ¿hay más de STRMAX_ caracteres en el búfer?
                  if (strlen_ > strmax_) {                        
                       
                        // devolvemos los primeros strmax_ caracteres
                        s.assign(strbuf_, strmax_);

                        // movemos los caracteres sobrantes más allá de strmax_ al principio del búfer
                        memmove(strbuf_, strbuf_ + strmax_, strlen_ - strmax_);

                        // y calculamos la nueva longitud de la cadena
                        strlen_ -= strmax_;

                        // se ha excedido la longitud máxima permitida para una línea, truncamos
                        return ERROR_LINE_TOO_LONG;
                  }

                  // no hemos llegado aún al número máximo de caracteres que puede haber en el búfer
                  // seguimos extrayendo caracteres del tubo

		  error = ReadAndAppend();
		  if (error == ERROR_READ_TIMEOUT) {
                        // el temporizador ha vencido, devuelvo una cadena con los caracteres que tenga y salgo
                        s.assign(strbuf_, strlen_);
                        strlen_ = 0;
                        return ERROR_READ_TIMEOUT;
                  }            

                  if (error == ERROR_EOF) {
                        // ya no hay más caracteres en el tubo, devuelvo una cadena con los restantes y salgo
                        s.assign(strbuf_, strlen_);
                        strlen_ = 0;
                        return ERROR_EOF;
                  }
  
                  if (error != ERROR_NO) {
                        strlen_ = 0;                               
                        return error;
                  }

	    } else  {
                  // hemos encontrado una marca de fin de línea

                  // devolvemos los primeros strmax_ caracteres
                  s.assign(strbuf_, delimiter_pos);
                  
                  // y al resto los movemos al principio del búfer (no es muy eficiente pero simplifica enormemente 
                  // el algoritmo)
                  memmove(strbuf_, strbuf_ + delimiter_pos + delimiter_len, strlen_ - delimiter_len);
                  
                  // y calculamos la nueva longitud de la cadena restante en el búfer
                  strlen_ -= (delimiter_pos + delimiter_len);
                  
                  break;
            }
      }

      return ERROR_NO;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SocketReader::Error SocketReader::ReadData(std::string& s, uint64_t byte_count) {

      // 1- leo primero los caracteres que hayan quedado remanentes en el bufer de lectura,
      // 2- después sigo extrayendo más caracteres del tubo si hiciese falta
      s.clear();

      if (strlen_ > 0) {
            // hay caracteres remanentes en el buffer
            if (byte_count <= strlen_) {
                  // Hay caracteres de sobra el búfer, no necesitamos hacer read()
                  s.assign(strbuf_, byte_count);
                  strlen_ -= byte_count;
                  return ERROR_NO;
            }
            
            s.assign(strbuf_, strlen_);
            byte_count -= strlen_;
            strlen_ = 0;
            // y seguimos...            
      }

      int er;
      int rd_count = 0;      
      int rd_chunk = 0;
      struct timeval this_timeout;

      // ...seguimos extrayendo caracteres del tubo
      while (byte_count > 0) {
            this_timeout = timeout_;
            er = select(fd_ + 1, &set_, NULL, NULL, &this_timeout);
            if (er == -1) {
                  //printf("SocketReader::ReadData(std::string&) select ERROR_IO\n");
                  return ERROR_IO;
            }
            if (er == 0) {
                  //printf("SocketReader::ReadData(std::string&) select TIMEOUT\n");
                  return ERROR_READ_TIMEOUT;
            }
            
            rd_chunk = readsz_ > byte_count ? byte_count : readsz_;
            rd_count = (int) read(fd_, (void*) (strbuf_), rd_chunk);
            if (rd_count == -1) {
                  //perror("SocketReader::ReadData(std::string&)");
                  return ERROR_IO;
            }
            
            if (rd_count == 0) { 
                  //printf("EOF!\n");
                  //perror("SocketReader::ReadData(std::string&)");
                  eof_ = true;
                  return ERROR_NO;
            }
            
            //rd_count > 0
            byte_count -= rd_count;
            s.append(strbuf_, rd_count);
            
            fd_byte_count_ += rd_count;
            total_byte_count_ += rd_count;
      }

      return ERROR_NO;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SocketReader::Error SocketReader::ReadData(char* s, uint64_t byte_count, uint64_t* bytes_read) {

      // 1- leo primero los caracteres que hayan quedado remanentes en el bufer de lectura,
      // 2- después sigo extrayendo más caracteres del tubo si hiciese falta

      *bytes_read = 0;

      if (strlen_ > 0) {
            // hay caracteres remanentes en el buffer
            if (byte_count <= strlen_) {
                  // hay caracteres de sobre el búfer, no necesitamos hacer read()
                  memcpy(s, strbuf_, byte_count);
                  strlen_ -= byte_count;
                  *bytes_read += byte_count;
                  return ERROR_NO;
            }
            
            memcpy(s, strbuf_, strlen_);
            *bytes_read += strlen_;
            strlen_ = 0;
            // y seguimos...            
      }

      int er;
      int rd_count = 0;      
      int rd_chunk = 0;

      struct timeval this_timeout;

      // ...seguimos extrayendo caracteres del tubo
      while (*bytes_read < byte_count) {

            // ojo, timeout es modificado por select(), hay que asignarlo siempre antes de llamar de nuevo a select
            this_timeout = timeout_;
            er = select(fd_ + 1, &set_, NULL, NULL, &this_timeout);
            if (er == -1) {
                  //printf("SocketReader::ReadData(char* s) select ERROR_IO\n");
                  //perror("SocketReader::ReadData(std::string&) select");
                  return ERROR_IO;
            }
            if (er == 0) {
                  //printf("SocketReader::ReadData(char* s) select TIMEOUT\n");
                  //perror("SocketReader::ReadData(std::string&) select");
                  return ERROR_READ_TIMEOUT;
            }
            
            rd_chunk = (byte_count - *bytes_read) < readsz_ ? (byte_count - *bytes_read) : readsz_;

            rd_count = (int) read(fd_, (void*) strbuf_, rd_chunk);

            if (rd_count == -1) {
                  //printf("SocketReader::ReadData(char* s) read ERROR_IO\n");
                  //perror("SocketReader::ReadData(char* s) read");
                  return ERROR_IO;
            }

            if (rd_count == 0) { 
                  //printf("SocketReader::ReadData(char* s) read EOF\n");
                  //perror("SocketReader::ReadData(char* s) read");
                  eof_ = true;
                  return ERROR_NO;
            }

            //rd_count > 0
            memcpy(s + *bytes_read, strbuf_, rd_count);
            *bytes_read += rd_count;
            fd_byte_count_ += rd_count;
            total_byte_count_ += rd_count;
      }

      return ERROR_NO;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SocketReader::GetRemanentChars (char** s, int* n) {
      *s = strbuf_;
      *n = strlen_;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
