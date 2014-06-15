#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stddef.h>

#include "gason.hpp"

///////////////////////////////////////////////////////////////////////////////
/* sample1.json is:
{
  "array": [
    1,
    2,
    3
  ],
  "boolean": true,
  "null": null,
  "number": 123,
  "object": {
    "a": "b",
    "c": "d",
    "e": "f"
  },
  "string": "Hello World"
}
*/

///////////////////////////////////////////////////////////////////////////////
class JSonGason
{

public:
    static bool     doTest1() {
        FILE* fp = fopen("./sample1.json", "r+t");
        if ( fp == 0 ) {
            puts("sample1.json not found!");
            return false;
        }

        // hold json data through the test
        char buffer[257] = {0};
        fread(buffer, 256, 1, fp);
        fclose(fp);



        // parsing
        gason::JsonAllocator    allocator;
        gason::JsonValue        root;
        // buffer will be over-written by jsonParse
        if ( gason::jsonParse(buffer, root, allocator) != gason::JSON_PARSE_OK ) {
            puts("parsing failed!");
            return false;
        }



        // finding / reading values
        if ( (int)root.child("array").at(1).toNumber() != 2 ) {
            puts("reading array element failed!");
        }
        if ( root("number") != gason::JSON_TAG_NUMBER ) {
            puts("number is not a number!");
        }
        if ( strncmp(root("object")("c").toString(), "d", 1) != 0 ) {
            puts("readinf object.c failed!");
        }
        printf("string is: %s\n", root("string").toString());


        // iteration
        gason::JsonValue childObject = root.child("object");
        gason::JsonIterator it;
        for ( it = gason::begin(childObject);    it != gason::end(childObject);    it++) {
            printf("%s = %s\n",
                   it->key, it->value.toString()
                   );
        }

        return true;
    }
};

///////////////////////////////////////////////////////////////////////////////
int main(int , char **)
{
    JSonGason::doTest1();

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
