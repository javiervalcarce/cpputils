// Hi Emacs, this is -*- coding: utf-8; mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#ifndef LIBREST_STR_H_
#define LIBREST_STR_H_

#include <string>
#include <vector>

namespace librest {
namespace str {

      // Utilidades básicas para procesar cadenas (año 2014 y la STL aun no tiene esto, increíble)

      void                      SplitAppendTo       (const std::string& s, char delim, std::vector<std::string>& elems);
      std::vector<std::string>  Split               (const std::string& s, char delim);
      std::vector<std::string>  SplitFirstDelimiter (const std::string& s, char delim);
      std::string&              ToUpper             (std::string& s);
      std::string&              ToLower             (std::string& s);
      std::string&              LTrim               (std::string& s);
      std::string&              RTrim               (std::string& s);
      std::string&              Trim                (std::string& s);
}
}

#endif // LIBREST_STR_H_
