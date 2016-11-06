// Hi Emacs, this is -*- coding: utf-8; mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#ifndef LIBREST_PATH_H_
#define LIBREST_PATH_H_

#include <string>
#include <vector>


namespace librest {

/**
 * Representa una ruta a un fichero, por ejemplo "/home/javier/leeme.txt". Permite inspeccionar fácilmente los 
 * componentes de la ruta, quitar o poner el caracter '/' inicial y final, pasarla a mayúsculas/minúsculas para poder
 * comparar, etc.
 */
class Path {
public:

      /**
       * Construye un objeto de tipo Path vacio, con un path igual a ""
       */
      Path();      

      /**
       * Construye un objecto de tipo de Path con |s| como valor inicial.
       */
      Path(const char* s);
      Path(const std::string& s);

      ~Path();

      /**
       * Parser de un nuevo path. Para poder cambiarlo sin necesidad de crear otro objeto de tipo Path.
       */
      void Parse(const char* s);
      void Parse(const std::string& s);

      /**
       * Inspecciona una ruta y dice si es válida o no.
       * 
       * Este es un tema... complejo. El conjunto de caracteres prohibidos depende tanto del sistema operativo como del
       * sistema de ficheros, por otra parte, aunque ciertos caracteres sean válidos en un sistema de ficheros pueden
       * ser peligrosos si se usan sin cuidado en llamadas como system(), un ejemplo es '|' que en linux sirve para 
       * establecer una tubería entre dos procesos, otro ejemplo es un espacio al final en el nombre de un fichero que
       * provoca errores de acceso al mismo difíciles de detectar.
       *
       * La comprobación que hace esta función, para evitar problemas y asegurar la portabilidad, es muy estricta. Solo 
       * se permite lo siguiente en un path de fichero o directorio:
       * 
       * - se permiten únicamente los caracteres ascii [a-z] [A-Z] [0-9] '.' '-' '_'  '/' '\'. 
       * - El espacio no está permitido, tampoco '*', '?', comillas simples o dobles, el caracter nulo '\0'por supuesto
       *   tampoco.
       *
       * - los caracteres separadores de directorios son '/' y/o '\', los dos son válidos para separar.
       *
       * - no se permite varios caracteres separadores seguidos, ni componentes llamados ".." ni "~", que son una fuente
       *   de problemas de seguridad.
       */
      bool IsValid() const;

      
      // Text Transformations
      const std::string& Sanitize();
      const std::string& ToLower();
      const std::string& ToUpper();
      const std::string& UnRoot();


      // operaciones const 
      const std::string& Str() const;      
      int                Depth() const;

      const std::string& Component(int i, bool* parse_error) const;
      const std::string& Component(int first, int count, bool* parse_error);

      int                ComponentAsInt(int i, bool* parse_error) const;
      bool               ComponentAsBool(int i, bool* parse_error) const; 
      double             ComponentAsDouble(int i, bool* parse_error) const; 


      // 
      const std::string& FileName() const;
      const std::string& FileTitle() const;
      const std::string& FileExtension() const;
      const std::string& FileDirectory() const;


      bool               HasRoot() const;            // ¿empieza con '/'? pej "hola/que/tal.txt" -> false
      bool               HasSeparatorAtEnd() const;  // ¿termina con '/'? pej "/hola/que/tal/" -> true      
      const std::string& DriveLetter() const;        // windows only

private:

      std::string str_;
      std::string sub_;

      bool valid_;
      bool begins_with_separator_;
      bool ends_with_separator_;

      std::vector<std::string> component_;

      std::string drive_letter_;
      std::string file_title_;
      std::string file_extension_;
      std::string file_name_;
      std::string file_directory_;

};

} // librest

#endif // LIBREST_PATH_H_
