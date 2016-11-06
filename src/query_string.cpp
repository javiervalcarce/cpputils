// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-

#include "query_string.h"
#include <cstring>
#include <stdexcept>
#include <map>
#include <inttypes.h>
#include <stdlib.h>
#include <cstdio>
#include <cassert>


using namespace librest;

const char* QueryString::kEmptyString = "";
const char  QueryString::kDelimiter   = '&';


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QueryString::QueryString(int key_count_max, int buffer_size_max, char delimiter) {

      m_delimiter = delimiter; //kDelimiter;
      //m_buf_size = 0; 
      m_key_count = 0;
      m_buf_size_max = buffer_size_max + 1; // 20151222 JVG: + 1 para almacenar el '\0' final
      m_key_count_max = key_count_max;
      
      m_buf = new char  [m_buf_size_max];
      m_key = new char* [m_key_count_max];
      m_val = new char* [m_key_count_max];
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QueryString::~QueryString() {

      delete[] m_val;
      delete[] m_key;
      delete[] m_buf;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void QueryString::Parse(const std::string& query_string) {
      Parse(query_string.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void QueryString::Parse(const char* query_string) {

      char* currtoken;
      char* nexttoken;
      char* p2nullchr; 
      char* key;
      char* val;

      // Reinicio el estado del objeto.
      m_map.clear();
      m_key_count = 0;

      // 20150512 JVG Corrijo bug: Casos límite, puntero nulo y cadena vacía.
      if (query_string == NULL) {
            return;
      }
      if (*query_string == '\0') {
            return;
      }


      // @COPYHERE
      strncpy(m_buf, query_string, m_buf_size_max - 1);
      m_buf[m_buf_size_max - 1] = '\0';

      currtoken = m_buf;
      p2nullchr = m_buf + (int) strlen(m_buf);


      // 1. anular todos los &
      // 2. detectar todos los tokens k=v
      // 3. extraer k y v

      while (currtoken != 0) {
            
	    nexttoken = strchr(currtoken, m_delimiter);
	    if (nexttoken != 0) {
		  *nexttoken = '\0';
		  nexttoken++;
	    }

            // análisis del current token "k=v"
	    key = currtoken;
	    val = strchr(currtoken, '=');

	    if (val != 0) {
                  // el caracter '=' ha sido encontrado
		  *val = '\0';
		  val++;
	    } else {
		  val = p2nullchr;
	    }
	    
	    m_key[m_key_count] = key;
	    m_val[m_key_count] = val;

	    m_key_count++;

	    // add to dictionary
	    m_map[key] = val;

            if (m_key_count >= m_key_count_max) {
                  break;
            }

	    currtoken = nexttoken;
      }
      
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* QueryString::Key(int index) const {

      if (index < 0 || index >= m_key_count) {
	    return kEmptyString;
            //return 0; //throw std::out_of_range("Key doesn't exist");
      }

      return m_key[index];
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* QueryString::Val(int index) const {

      if (index < 0 || index >= m_key_count) {
	    return kEmptyString;
            //return 0; //throw std::out_of_range("Val doesn't exist");
      }

      return m_val[index];
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool QueryString::ParamExist(const char* key) {

      std::map<const char*, char*, cmp_str>::iterator i;
      i = m_map.find(key);
      if (i == m_map.end()) {
	    return false;
      }

      return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool QueryString::ParamHasValue(const char* key) {

      std::map<const char*, char*, cmp_str>::iterator i;
      i = m_map.find(key);
      if (i == m_map.end()) {
	    return false;
      }

      char* s = i->second;
      if (*s == '\0') {
	    return false;
      } 

      return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string QueryString::ParamAsString(const char* key, bool* parse_error) {
      
      std::map<const char*, char*, cmp_str>::iterator i;
      std::string r;

      i = m_map.find(key);
      if (i == m_map.end()) {
	    *parse_error = true;
            r = kEmptyString;
            return r;
      }

      *parse_error = false;
      r = i->second;
      return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int QueryString::ParamAsInt(const char* key, bool* parse_error) {

      int v;
      char* endptr;

      *parse_error = true;

      std::map<const char*, char*, cmp_str>::iterator i;
      i = m_map.find(key);
      if (i == m_map.end()) {
	    return 0;
      }
      
      const char* s = i->second;
      v = (int) strtol(s, &endptr, 10);
      if (*endptr == '\0') {
	    *parse_error = false;
      }

      return v;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool QueryString::ParamAsBool(const char* key, bool* parse_error) {

      *parse_error = true;

      std::map<const char*, char*, cmp_str>::iterator i;
      i = m_map.find(key);
      if (i == m_map.end()) {
	    return 0;
      }

      if ((std::strcmp(i->second, "true") == 0) || (std::strcmp(i->second, "TRUE") == 0)) {
	    *parse_error = false;
	    return true;
      }

      *parse_error = false;
      return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double QueryString::ParamAsDouble(const char* key, bool* parse_error) {

      double v;
      char* endptr;

      std::map<const char*, char*, cmp_str>::iterator i;
      i = m_map.find(key);
      if (i == m_map.end()) {
	    *parse_error = true;
	    return 0.0;
      }

      v = strtod(i->second, &endptr);
      if (*endptr != '\0') {	   
	    *parse_error = true;
	    return 0.0;
      }

      *parse_error = false;
      return v;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
