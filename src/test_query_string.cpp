// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#include "query_string.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <assert.h>

using namespace std;
using namespace librest;

QueryString qs(5, 1024);


void Print(QueryString& q);

void Interactive001();
bool Test001();
bool Test002();
bool Test003();

typedef bool (*TestFunc)();
typedef void (*UserFunc)();


// Interactive tests
UserFunc user[] = {
      Interactive001
};


// Automatic unit tests
TestFunc test[] = {
      Test001,
      Test002,
      Test003
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Print(QueryString& q) {
      int i;
      for (i = 0; i < q.Count(); i++) {
            cout << "      key[" << i << "] = " << q.Key(i) << " val[" << i << "] = " << q.Val(i) << endl;
      }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Interactive001() {

      const char* qsarray[6] = {
            "&&&&",
            "&p1&p2&p3",
            "p1&p2&p3&",
            "p1&p2&p3",
            "&p1=v1&p2==v2&&p3=",
            "&=v1&==&=4&="
      };
      
      const char* str;
      int j;

      for (j = 0; j < (int) (sizeof(qsarray) / sizeof(char*)); j++) {
            str = qsarray[j];
            qs.Parse((char*) str);
            cout << str << endl;
            Print(qs);
      }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Test001() {

      bool r = true;
      QueryString qs(5, 1024);

      qs.Parse((char*) "param1=value1&param2=value2&param3");

      r &= qs.ParamExist("param1") == true;
      r &= qs.ParamExist("param2") == true;
      r &= qs.ParamExist("param3") == true;
      r &= qs.ParamExist("paramX") == false;
      r &= qs.ParamHasValue("param1") == true;
      r &= qs.ParamHasValue("param2") == true;
      r &= qs.ParamHasValue("param3") == false;

      return r;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Test002() {

      bool r = true;
      bool error = true;
      
      QueryString qs(5, 1024);
      qs.Parse((char*) "hola=javi");

      std::string v = qs.ParamAsString("hola", &error);

      if (error == true) r = false;
      if (v != "javi") r = false;


      std::string w = qs.ParamAsString("eoo", &error);
      if (!(error == true)) r = false;

      qs.Parse((char*) "p1=v1&p2&p3=v3");
      std::string z = qs.ParamAsString("p3", &error);
      if (!(error == false)) r = false;
      if (z != "v3") r = false;

      return r;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Comprobación de conversión de cadenas a enteros, booleanos, etc.
bool Test003() {

      bool r = true;
      bool error = true;
      
      QueryString qs(5, 1024);

      qs.Parse((char*) "p1=332&p2=2.8432&p3=true&p4=TRUE&p5=cadena");

      int v1 = qs.ParamAsInt("p1", &error);
      if (!(error == false)) r = false;
      if (v1 != 332) r = false;


      double v2 = qs.ParamAsDouble("p2", &error);
      if (!(error == false)) r = false;
      if (v2 != 2.8432) r = false;


      bool v3 = qs.ParamAsBool("p3", &error);
      if (!(error == false)) r = false;
      if (v3 != true) r = false;

      bool v4 = qs.ParamAsBool("p4", &error);
      if (!(error == false)) r = false;
      if (v4 != true) r = false;

      std::string v5 = qs.ParamAsString("p5", &error);
      if (!(error == false)) r = false;
      if (v5 != "cadena") r = false;

      return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

      unsigned i;

      // user interactive tests
      for (i = 0; i < sizeof(user) / sizeof(void*); i++) {
	    printf("- Interactive test number %03d", i);
	    user[i]();
      }

      // automatic tests
      for (i = 0; i < sizeof(test) / sizeof(void*); i++) {

	    printf("- Automatic test number %03d   : ", i);
	    if (test[i]()) printf("OK\n"); else printf("Failed\n");
      }

      printf("\n");
      
      return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
