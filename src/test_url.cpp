// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-
#include "url.h"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <assert.h>

using namespace std;
using librest::Url;


#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void test_assert(double n, bool expr) {
      printf("Test number %08.3f: ", n);
      if (expr) {
            printf(GREEN "ok\n" RESET);
      } else {
            printf(RED "failed\n" RESET);
      }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

      Url  o;
      string s;
     
      test_assert(1.0, o.IsValid() == false);
            
      test_assert(2.0, o.Parse("http://host") == 0);
      test_assert(2.1, o.Path() == "");
      test_assert(2.2, o.Parse("http://host/") == 0);
      test_assert(2.3, o.Path() == "/");

      
      test_assert(3.0, o.Parse("http://user:pass@host") == 0);
      test_assert(3.1, o.Username() == "user");
      test_assert(3.2, o.Password() == "pass");
      test_assert(3.3, o.Path() == "");
      
      test_assert(4.0, o.Parse("http://user:pass@host/") == 0);
      test_assert(4.1, o.Username() == "user");
      test_assert(4.2, o.Password() == "pass");
      test_assert(4.3, o.Host() == "host");

      test_assert(5.0, o.Parse("http://@host/") == 0);
      test_assert(5.1, o.Username() == "");
      test_assert(5.2, o.Password() == "");
      test_assert(5.3, o.Host() == "host");
      test_assert(5.4, o.Port() == 0);
      
      test_assert(6.0, o.Parse("http://user:pass@host:554/") == 0);
      test_assert(6.1, o.Username() == "user");
      test_assert(6.2, o.Password() == "pass");
      test_assert(6.3, o.Host() == "host:554");
      test_assert(6.4, o.Hostname() == "host");
      test_assert(6.5, o.Path() == "/");
      test_assert(6.6, o.Port() == 554);


      test_assert(7.0, o.Parse("http://user:@host:554") == 0);
      test_assert(7.1, o.Username() == "user");
      test_assert(7.2, o.Password() == "");
      test_assert(7.3, o.Host() == "host:554");
      test_assert(7.4, o.Hostname() == "host");
      test_assert(7.5, o.Path() == "");
      test_assert(7.6, o.Port() == 554);

      test_assert(8.0, o.Parse("http://user:pass@host:8080/path1/path2/index.html") == 0);
      test_assert(8.1, o.Username() == "user");
      test_assert(8.2, o.Password() == "pass");
      test_assert(8.3, o.Port() == 8080);
      test_assert(8.4, o.Path() == "/path1/path2/index.html");
      test_assert(8.5, o.Query() == "");

      test_assert(8.0, o.Parse("http://:@host:8080/path1/path2/index.html") == 0);
      test_assert(8.1, o.Username() == "");
      test_assert(8.2, o.Password() == "");
      test_assert(8.3, o.Host() == "host:8080");
      test_assert(8.4, o.Port() == 8080);
      test_assert(8.5, o.Path() == "/path1/path2/index.html");
      test_assert(8.6, o.Query() == "");
      
      return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
