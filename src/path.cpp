// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-

#include "path.h"
#include "str.h"
#include <cstdlib>
#include <cstring>

using namespace librest;
using namespace std;

const std::string kEmptyString = "";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Path::Path() {
      Parse("");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Path::Path(const char* s) {
      Parse(s);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Path::Path(const std::string& s) {
      Parse(s);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Path::~Path() {
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Path::Parse(const string& s) {
      Parse(s.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Path::Parse(const char* s) {

      str_ = s;
      valid_ = false;
      
      drive_letter_ = "";
      file_title_ = "";
      file_extension_ = "";
      file_name_ = "";
      file_directory_ = "";

      component_.clear();

      begins_with_separator_ = false;
      ends_with_separator_ = false;
      
      
      /*
      if (strcmp(s, "/api1/echo") == 0) {
            //printf("see\n");
            component_.push_back("api1");
            component_.push_back("echo");
            return;
      }
      */  
          
      // Paso 1 - Si la longitud es cero la ruta no es válida
      // ---------------------------------------------------------------------------------------------------------------
      if (*s == '\0') {
	    //printf("**salgo en 0\n");
            return;
      }

      
      
      // Paso 2 - Compruebo que no hay caracteres prohibidos
      // ---------------------------------------------------------------------------------------------------------------
      for (const char* c = s; *c != '\0'; c++) {
            if (false
                || (*c >= 'a' && *c <= 'z') 
                || (*c >= 'A' && *c <= 'Z') 
                || (*c >= '0' && *c <= '9')
                || (*c == '_') 
                || (*c == '-')
                || (*c == '.')
                || (*c == '/')
                || (*c == '\\')) {
                  // caracter válido, ok
            } else {
                  // caracter prohibido, salimos
                  //printf("**salgo en 1\n");
                  return;
            }
      }
      

      // Paso 3 - Compruebo que no hay dos o más caracteres separador ('/', '\') seguidos      
      // ---------------------------------------------------------------------------------------------------------------
      if (true 
          && (str_.find("//")    == string::npos) 
          && (str_.find("\\\\")  == string::npos)
          && (str_.find("/\\")   == string::npos)
          && (str_.find("\\/")   == string::npos)) {
            // ok
      } else {
            //printf("**salgo en 2\n");
            return;
      }

      //printf("**ok\n");
      // Paso 4 - La ruta es buena, extraigo las distintas partes de la misma
      // ---------------------------------------------------------------------------------------------------------------
      valid_ = true;

      // el caso "/" lo resulve bien
      begins_with_separator_ = (str_[0]                 == '/' || str_[0]                 == '\\');
      ends_with_separator_   = (str_[str_.length() - 1] == '/' || str_[str_.length() - 1] == '\\');

      if (str_.length() > 1) {

            // la ruta tiene al menos 2 caracteres, con esta certeza uso el siguiente algoritmo
            string::size_type n;
            string::size_type r;

            r = (begins_with_separator_) ? 1 : 0;
            do {
                  n = str_.find_first_of("/\\", r);
                  if (n != string::npos) {
                        component_.push_back(str_.substr(r, n - r));
                  } else {
                        component_.push_back(str_.substr(r, string::npos));
                  }
                  r = n + 1;
            } while ((n != string::npos) && (n != str_.length() - 1));
      }


      // Paso 5 - Extraigo los nombres
      // ---------------------------------------------------------------------------------------------------------------
      if ((component_.size() > 0) && (ends_with_separator_ == false)) {
	    string& f = component_.back(); // el nombre del fichero es el último componente de la ruta
	    string::size_type n = f.rfind('.');
	    if (n != string::npos) {
		  file_name_      = f;
		  file_title_     = f.substr(0, n);
		  file_extension_ = f.substr(n + 1, std::string::npos);
	    } else {
		  file_title_ = f;
		  file_name_ = f;
	    }
      }

      // Si termina en "/" entonces es la cadena completa
      // Si no termina en "/" entonces son todos los componentes menos el último
      string::size_type n = str_.rfind('/');
      if (n == string::npos) {
            file_directory_ = "";
      } else {
            file_directory_ = str_.substr(0, n + 1);
      }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Path::IsValid() const {
      return valid_;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      
// operaciones que cambian el objeto
const string& Path::Sanitize() {
      // to be implemented
      return str_;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const string& Path::ToLower() {

      str::ToLower(str_);
      return str_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const string& Path::ToUpper() {
      str::ToUpper(str_);
      return str_;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const string& Path::UnRoot() {
      if (str_[0] == '/' || str_[0] == '\\') {
            str_ = str_.erase(0, 1); // le quitamos el primer caracter, components_ queda igual
      }
      
      return str_;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const string& Path::Str() const {
      return str_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Path::Depth() const {
      return (int) component_.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const string& Path::Component(int i, bool* parse_error) const {

      if ((i < 0) || (i >= (int) component_.size())) { 
            *parse_error = true;
            return kEmptyString;
      }

      *parse_error = false;
      return component_[i];
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const string& Path::Component(int first, int count, bool* parse_error)  {

      int i;

      if (first < 0) { 
            *parse_error = true;
            return kEmptyString;
      }

      if (first + count > (int) component_.size()) { 
            *parse_error = true;
            return kEmptyString;
      }

      // Subpath components are separated by '/', but not in the extremes.
      sub_ = "";

      for (i = first; i < first + count; i++) {
            sub_.append(component_[i]);
            if (i < first + count - 1) {
                  sub_.append("/");
            }
      }

      return sub_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Path::ComponentAsInt(int i, bool* parse_error) const {

      if ((i < 0) || (i >= (int) component_.size())) { 
            *parse_error = true;
            return 0;
      }

      int num;
      char* endptr;

      if (i >= (int) component_.size()) { 
            *parse_error = true;
            return 0;
      }

      // 20151222 JVG: Conversión de tipo y base 0 (decimal y hexadecimal)
      num = static_cast<int>(strtol(component_[i].c_str(), &endptr, 0));
      if (*endptr != '\0') {
	    *parse_error = true;
          return 0;
      }

      *parse_error = false;
      return num;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Path::ComponentAsBool(int i, bool* parse_error) const {
      
      if ((i < 0) || (i >= (int) component_.size())) { 
            *parse_error = true;
            return false;
      }

      const char* s = component_[i].c_str();

      *parse_error = false;
      if (strcmp(s, "true")  == 0) return true;
      if (strcmp(s, "TRUE")  == 0) return true;
      if (strcmp(s, "false") == 0) return false;
      if (strcmp(s, "FALSE") == 0) return false;

      *parse_error = true;
      return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double Path::ComponentAsDouble(int i, bool* parse_error) const {

      if ((i < 0) || (i >= (int) component_.size())) { 
            *parse_error = true;
            return false;
      }

      double v;
      char* endptr;

      v = strtod(component_[i].c_str(), &endptr);
      if (*endptr != '\0') {	   
	    *parse_error = true;
	    return 0.0;
      }

      *parse_error = false;
      return v;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const string& Path::FileName() const {
      return file_name_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const string& Path::FileTitle() const {
      return file_title_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const string& Path::FileExtension() const {
      return file_extension_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const string& Path::FileDirectory() const {
      return file_directory_;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Path::HasRoot() const {
      return begins_with_separator_;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Path::HasSeparatorAtEnd() const {
      return ends_with_separator_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const string& Path::DriveLetter() const {
      return drive_letter_;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      
      
