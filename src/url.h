// Hi Emacs, this is -*- coding: utf-8; mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#ifndef LIBREST_URL_H_
#define LIBREST_URL_H_    

#include <string>

namespace librest {

/**
 * TODO: Ampliar y mejorar este parser de URLs.
 *
 * Implementación PARCIAL de un parser de cadenas URI, no acepta todos los posibles formatos especificados en RFC2396
 * pero sí lo más usuales. Esta cadena la parsea corectamente:
 * 
 * "schema://host:port/path?query_string"
 *
 * El hostame es "host:port"
 *
 * Esta otra cadena NO la reconoce
 * "schema://user:pass@hostname:puerto/path?query_string#fragment"
 *
 */
class Url {
public:

      /**
       * Ctor.
       */
      Url();


      /**
       * Parsea la cadena |url_s| y extrae sus distintos componentes, que podrán obtenerse con el resto de funciones
       * miembro de la clase.
       *
       * @return Si el parseo es correcto devolverá 0 y además IsValid() será true. En caso contrario, si el parseo no es correcto devolverá 1 y además IsValid() devolverá false.
       */
      int Parse(const std::string& url_s);

      /**
       * Si he podido o no parsear correctamente la cadena suministrada a Parse()
       */
      bool IsValid()                const { return is_valid_; }

      
      /** En la cadena "schema://user:pass@host:port/path?query_string" devuelve todo, la misma cadena */
      const std::string& Str()      const { return url_;      }

      /** En la cadena "schema://user:pass@host:port/path?query_string" devuelve "schema" */
      const std::string& Schema()   const { return protocol_; }

      /** En la cadena "schema://user:pass@host:port/path?query_string" devuelve "hostname:port" */
      const std::string& Host()     const { return host_;     }

      /** En la cadena "schema://user:pass@host:port/path?query_string" devuelve "hostname" */
      const std::string& Hostname() const { return hostname_; }

      /** En la cadena "schema://user:pass@host:port/path?query_string" devuelve port como número entero */
      int                Port()     const { return port_;     }
      
      /** En la cadena "schema://user:pass@host:port/path?query_string" devuelve "path" */
      const std::string& Path()     const { return path_;     }

      /** En la cadena "schema://user:pass@host:port/path?query_string" devuelve "query_string" */
      const std::string& Query()    const { return query_;    }

      /** En la cadena "schema://user:pass@host:port/path?query_string" devuelve "user" */
      const std::string& Username() const { return username_; }

      /** En la cadena "schema://user:pass@host:port/path?query_string" devuelve "pass" */
      const std::string& Password() const { return password_; }


      /**
       * Decodifica la URL enviada según RFCXXX. El resultado de la decodificación REEMPLAZA al contenido original de
       * |s|, si desea conservar el valor previo haga una copia de seguridad de la misma.
       */
      static void        UrlDecode(std::string& s);

      /**
       * Codifica la URL enviada según RFCXXX. 
       */
      static std::string UrlEncode(const std::string& s);
      
private:

      // cadena "://"
      static const std::string prot_end;
      
      bool is_valid_;

      std::string url_;
      std::string protocol_;
      std::string username_;
      std::string password_;
      std::string host_;

      std::string hostname_;
      int port_;
      
      std::string path_;
      std::string query_;

};

} // librest

#endif // LIBREST_URL_H_ 
