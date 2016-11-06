// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-

#include "str.h"
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>

using namespace librest;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void str::SplitAppendTo(const std::string& s, char delim, std::vector<std::string>& elems) {
      std::stringstream ss(s);
      std::string item;

      while (std::getline(ss, item, delim)) {
            elems.push_back(item);
      }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> str::Split(const std::string& s, char delim) {
      std::vector<std::string> elems;
      SplitAppendTo(s, delim, elems);
      return elems;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> str::SplitFirstDelimiter(const std::string& s, char delim) {
      std::vector<std::string> elems;

      std::size_t pos = s.find(delim);
      
      if (pos != std::string::npos) {
            // El caracter delimitador existe, rompo la cadena en dos partes por el primer caracter delimitador.
            elems.push_back(s.substr(0, pos));
            elems.push_back(s.substr(pos + 1, std::string::npos));
      } else {
            elems.push_back(s);
      }
      
      return elems;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string& str::ToUpper(std::string& s) {
      std::locale loc;
      std::string::size_type i;

      for (i = 0; i < s.length(); ++i) {
            s[i] = std::toupper(s[i], loc);
      }
      return s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string& str::ToLower(std::string& s) {
      std::locale loc;
      std::string::size_type i;

      for (i = 0; i < s.length(); ++i) {
            s[i] = std::tolower(s[i], loc);
      }
      return s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// trim from start
std::string& str::LTrim(std::string& s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
      return s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// trim from end
std::string& str::RTrim(std::string& s) {
      s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
      return s;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// trim from both ends
std::string& str::Trim(std::string& s) {
      return LTrim(RTrim(s));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
