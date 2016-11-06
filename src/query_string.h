// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#ifndef LIBREST_QUERY_STRING_H_
#define LIBREST_QUERY_STRING_H_

#include <map>
#include <string>
#include <cstring>
#include <functional>

namespace librest {


/**
 * Analizador léxico de cadenas con formato "clave1=valor1&clave2=valor2&clave2=valor2&..." especificadas en RFC3986.
 *
 * Permite averiguar qué parámetros están presentes en la cadena y leer sus valores convertidos a los diferentes tipos 
 * de datos primitivos del lenguaje c++.
 *
 */
class QueryString {
public:

      /**
       * Constructor.
       *
       * @param key_count_max Máximo número de pares (clave, valor) que pueden almacenarse. Durante el análisis de la 
       * cadena QUERY_STRING ejecutado mediante Parse() todos los pares k=v por encima de este número se descartan.
       *
       * @param buffer_size_max El tamaño máximo que podrá tener la cadena QUERY_STRING que se va a analizar. Todos los
       * caracteres que exedan este tamaño serán descartados.
       */
      QueryString(int key_count_max, int buffer_size_max, char delimiter = '&');

      /**
       * Destructor.
       */
      ~QueryString();
  

      /**
       * Analiza una cadena de caracteres y extrae los parámetros y sus valores.
       *
       * La cadena deberá tener el formato "param1=value1&param2=value2&param3=value3&..." (sin el caracter '?' inicial)
       *
       * Una vez llamada esta función ya podemos obtener la información cómodamente mediante Count(), Key(), Val(), 
       * ParamExist(), ParamHasValue(), ParamAsString(), ParamAsInt(), ParamAsBool(), ParamAsDouble(), etc.
       *
       * @param query La cadena query que se va a analizar.
       */
      void Parse(const char* query);

      /**
       * Lo mismo pero para cadenas de tipo std::string.
       */
      void Parse(const std::string& query);

      /**
       * Obtiene la clave del par |index|-ésimo. Sirve para acceder a una clave por su posición en la cadena, si lo
       * que quiere es acceder a ella por su nombre use ParamExist(), ParamHasValue() y ParamAsXXX().
       *
       * @param index Index of the key.
       *
       * @return Puntero a la cadena terminada en '\0' con el nombre del parámetro de índice |index|, el búfer interno
       * es propiedad de este objeto y no debe ser liberada por el usuario. Si el índice está fuera de rango se devuelve
       * un puntero a una cadena vacía (""), es decir, el puntero devuelto nunca será NULL.
       */
      const char* Key(int index) const;

      /**
       * Obtiene el valor del par i-ésimo. Sirve para acceder un valor por su posición en la cadena, si lo
       * que quiere es acceder a ella por el nombre de la clave use ParamExist(), ParamHasValue() y ParamAsXXX().
       *
       * @param index Index of the key
       *
       * @return Puntero a la cadena terminada en '\0' con el valor del parámetro de índice |index|, el búfer interno es
       * propiedad de este objeto y no debe ser liberada por el usuario. Si el índice está fuera de rango se devuelve un
       * puntero a una cadena vacía (""), es decir, el puntero devuelto nunca será NULL.
       */
      const char* Val(int index) const;

      /**
       * Número de parámetros contenidos en la cadena que acaba de ser analizada mediante Parse(), si el 
       * número de parámetros es superior a Capacity() entonces se trunca a ese valor.
       */
      int Count() const { return m_key_count;     }

      /**
       * Número máximo de parámetros que este objeto es capaz de procesar mediante Parse(), este número 
       * se especificó en el constructor de la clase.
       */
      int Capacity() const { return m_key_count_max; }
      
      /**
       * Devuelve true si la clave [key] existe, false en caso contrario.
       * Nuevo en la versión 0.2
       */
      bool ParamExist(const char* key);

      /**
       * Devuelve true si la clave [key] tiene asociado un valor (=value), false en caso contrario
       * Nuevo en la versión 0.2
       */
      bool ParamHasValue(const char* key);

      /**
       * Obtiene el valor asociado a la clave [key] y lo devuelve como tipo std::string.
       */
      std::string ParamAsString(const char* key, bool* parse_error);

      /**
       * Obtiene el valor asociado a la clave [key] y lo devuelve como tipo int.
       * Nuevo en la versión 0.2
       */
      int ParamAsInt(const char* key, bool* parse_error);

      /**
       * Obtiene el valor asociado a la clave [key] y lo devuelve como tipo bool.
       * Nuevo en la versión 0.2
       */
      bool ParamAsBool(const char* key, bool* parse_error);

      /** 
       * Obtiene el valor asociado a la clave [key] y lo devuelve como tipo double.
       * Nuevo en la versión 0.2
       */
      double ParamAsDouble(const char* key, bool* parse_error);


private:

      static const char* kEmptyString;
      static const char  kDelimiter;

      
      char   m_delimiter;
      
      char*  m_buf;
      int    m_buf_size_max;
      char** m_key;
      char** m_val;

      int    m_key_count;        // number of pairs (key, val) stored
      int    m_key_count_max;    // maximun number of pairs (key, val)

      // Sort function
      struct cmp_str : public std::binary_function<const char*, const char*, bool> {
	    bool operator()(const char* a, const char* b) const {
		  return std::strcmp(a, b) < 0;
	    }
      };
      
      std::map<const char*, char*, cmp_str> m_map;

};

} // librest
#endif // LIBREST_QUERY_STRING_H_
