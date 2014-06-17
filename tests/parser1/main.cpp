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
        gason::JsonParseStatus  status = gason::jsonParse(buffer, root, allocator);
        // buffer will be over-written by jsonParse
        if ( status != gason::JSON_PARSE_OK ) {
            puts("parsing failed!");
            return false;
        }

        // finding / reading values
        bool ok = false;
        if ( root.child("array").at(5).toInt(&ok) != 2 ) {
            puts("reading array element failed!");
        }
        if ( !root("number").isNumber() ) {
            puts("number is not a number!");
        }
        if ( strncmp(root("object")("c").toString(), "d", 1) != 0 ) {
            puts("readinf object.c failed!");
        }
        printf("string is: %s\n", root("string").toString());


        // iteration
        gason::JsonIterator it = gason::begin( root.child("object") );
        while ( it.isValid() ) {
            printf("%s = %s\n", it->key, it->value.toString());

            it++;
        }


        // invalid object / index
        size_t index = 7;
        gason::JsonValue item = root("array")[index];
        if ( !item ) {
            printf("array[%u] does not exist.\n", index);

        } else if ( !item.isNumber() ) {
            printf("array[%u] is not a number.\n", index);

        } else {
            printf("array[%u] = %d\n", index, item.toInt());
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
