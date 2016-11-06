// Hi Emacs, this is -*- mode: c++; tab-width: 6; indent-tabs-mode: nil; c-basic-offset: 6 -*-

#include "path.h"

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>


using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {

      vector<librest::Path*> p;
      vector<librest::Path*>::iterator i;
      int j;

      for (j = 0; j < 22; j++) {
	    p.push_back(new librest::Path());
      }
      
      
      p[0]->Parse("/hola/que/tal.txt.zip");
      p[1]->Parse("/hola/que/tal.txt.zip/");
      p[2]->Parse("/hola/que/tal.txt.zip.");
      p[3]->Parse("hola..text");
      p[4]->Parse("hola.text");
      p[5]->Parse(".");
      p[6]->Parse(".png");
      p[7]->Parse("sinextension");
      p[8]->Parse("hola/que/tal");
      p[9]->Parse("\\hola\\que\\tal");
      p[10]->Parse("\\HoLa\\QUe\\tal");
      p[12]->Parse("\\hola/que/tal");
      p[13]->Parse("\\hola\\que/tal");
      p[14]->Parse("hola");
      p[15]->Parse("//hola");
      p[16]->Parse("hola\\\\");
      p[17]->Parse("/");
      p[18]->Parse("");
      p[19]->Parse("/a");
      p[20]->Parse("b");
      p[21]->Parse("/458/true/hola/-5695796/TRUE/hola/false/benjamin.curl/3.141516");
      
      bool parse_error;

      for (i = p.begin(); i != p.end(); i++) {
	    
	    cout << "[" << (*i)->Str() << "]" << endl;
	    cout << "[" << (*i)->ToUpper() << "]" << endl;
	    cout << "[" << (*i)->ToLower() << "]" << endl;
	    
	    cout << "     Is valid? = " << (*i)->IsValid() << endl;
	    cout << "     Deph = " << (*i)->Depth() << endl;
	    for (j = 0; j < (*i)->Depth(); j++) {
		  cout << "     Component " << j << " [" << (*i)->Component(j, &parse_error) << "]" << endl;
	    }
	    
	    cout << "     FileTitle     = [" << (*i)->FileTitle() << "]" << endl;
	    cout << "     FileExtension = [" << (*i)->FileExtension() << "]" << endl;
	    cout << "     FileName      = [" << (*i)->FileName() << "]" << endl;
	    cout << "     Start with /  = " << (*i)->HasRoot() << endl;
	    cout << "     Ends  with /  = " << (*i)->HasSeparatorAtEnd() << endl;
      }


      
      
      bool error;
      int intv;
      bool boolv;

      intv = p[21]->ComponentAsInt(0, &error);
      assert(error == false);
      assert(intv == 458);
      
      intv = p[21]->ComponentAsInt(3, &error);
      assert(error == false);
      assert(intv == -5695796);
      
      boolv = p[21]->ComponentAsBool(1, &error);
      assert(error == false);
      assert(boolv == true);

      boolv = p[21]->ComponentAsBool(6, &error);
      assert(error == false);
      assert(boolv == false);

      double d = p[21]->ComponentAsDouble(8, &error);
      assert(error == false);
      assert(d == 3.141516);

      const std::string& cs1 = p[21]->Component(0, 4, &error);
      assert(error == false);
      assert(cs1.compare("458/true/hola/-5695796") == 0);

      const std::string& cs2 = p[21]->Component(1, 4, &error);
      assert(error == false);
      assert(cs2.compare("true/hola/-5695796/TRUE") == 0);
                          
      const std::string& cs3 = p[21]->Component(3, 5, &error);
      assert(error == false);
      assert(cs3.compare("-5695796/TRUE/hola/false/benjamin.curl") == 0);


      const std::string& cs4 = p[21]->FileDirectory();
      assert(cs4.compare("/458/true/hola/-5695796/TRUE/hola/false/benjamin.curl/") == 0);



      return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      
