// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#include "url.h"

#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip> 
#include <cctype>
#include <functional>
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace librest;

const string Url::prot_end("://");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Url::Url() {
      is_valid_ = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////       
int Url::Parse(const string& url_s) {

      is_valid_ = false;
      protocol_.clear();
      username_.clear();
      password_.clear();
      hostname_.clear();
      port_ = 0;
      host_.clear();
      path_.clear();
      query_.clear();

      url_ = url_s;

      
      string::const_iterator prot_i = search(url_s.begin(), url_s.end(), prot_end.begin(), prot_end.end());
      protocol_.reserve(distance(url_s.begin(), prot_i));
      transform(url_s.begin(), prot_i, back_inserter(protocol_), ptr_fun<int,int>(tolower));  // protocol is icase

      if (prot_i == url_s.end()) {
            return 1;
      }

      advance(prot_i, prot_end.length());
      string::const_iterator path_i = find(prot_i, url_s.end(), '/');
      host_.reserve(distance(prot_i, path_i));
      transform(prot_i, path_i, back_inserter(host_), ptr_fun<int,int>(tolower));  // host is icase

      string::const_iterator query_i = find(path_i, url_s.end(), '?');
      path_.assign(path_i, query_i);
      if (query_i != url_s.end()) {
            ++query_i;
      }

      query_.assign(query_i, url_s.end());




      

      size_t pos;
      
      // En este punto tenemos la variable host_ es una cadena del tipo "[user:pass@]hostname[:port]"
      // Debemos comprobar si tiene las partes opcionales señaladas

      // ¿ Tiene [user:pass@] ?
      pos = host_.find('@');
      if (pos != string::npos) {

            // Sí
            std::string credentials;

            credentials = host_.substr(0, pos);
            host_ = host_.substr(pos + 1);

            //printf("DEBUG-c: [%s]\n", credentials.c_str());
            //printf("DEBUG-h: [%s]\n", host_.c_str());
            
            // ¿Tiene credenciales user[:pass] ?
            pos = credentials.find(':');
            if (pos != string::npos) {

                  // Sí, tiene nombre de usuario y contraseña separados por ':', por ejemplo
                  // "user:pass@169.254.11.1:554"
                  username_ = credentials.substr(0, pos);
                  password_ = credentials.substr(pos + 1);

                  //printf("DEBUG-c-u: [%s]\n", username_.c_str());
                  //printf("DEBUG-c-p: [%s]\n", password_.c_str());
            } else {

                  // No, tiene nombre de usuario pero no contraseña, por ejemplo: "user@169.254.11.1:554"
                  username_ = credentials;
            }
      }

      
      // ¿ Tiene hostname[:port] ?
      pos = host_.find(':');
      if (pos != string::npos) {

            // Sí
            char* endp;
                  
            hostname_ = host_.substr(0, pos);
            port_     = strtoul(host_.substr(pos + 1).c_str(), &endp, 10);
            
            //printf("DEBUG-d: [%d]\n", port_);
            //printf("DEBUG-s: [%s]\n", host_.substr(pos + 1).c_str());
            //printf("DEBUG-e: [%d]\n", *endp);
            
            if (*endp != '\0') {
                  port_ = 0;
                  is_valid_ = false;
            }
      } else {
            // No hay ':', se omite el puerto
            hostname_ = host_;
            port_ = 0;
      }

      
      is_valid_ = true;
      return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Url::UrlDecode(std::string& s) {
      // stackoverflow
      char* dec = new char[s.length() + 1];
      char* src = (char*) s.c_str();


      char* dst = dec;
      char a;
      char b;

      while (*src) {
            if ((*src == '%') &&
                ((a = src[1]) && (b = src[2])) &&
                (isxdigit(a) && isxdigit(b))) {
                  if (a >= 'a')
                        a -= 'a'-'A';
                  if (a >= 'A')
                        a -= ('A' - 10);
                  else
                        a -= '0';
                  if (b >= 'a')
                        b -= 'a'-'A';
                  if (b >= 'A')
                        b -= ('A' - 10);
                  else
                        b -= '0';
                  *dst++ = 16*a+b;
                  src+=3;
            } else {
                  *dst++ = *src++;
            }
      }

      *dst++ = '\0';
      
      s.assign(dec);
      delete[] dec;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string Url::UrlEncode(const string& value) {
      // stackoverflow
      ostringstream escaped;
      escaped.fill('0');
      escaped << hex;

      for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
            string::value_type c = (*i);

            // Keep alphanumeric and other accepted characters intact
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                  escaped << c;
                  continue;
            }

            // Any other characters are percent-encoded
            escaped << '%' << setw(2) << int((unsigned char) c);
      }

      return escaped.str();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
