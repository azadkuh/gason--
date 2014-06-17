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
        FILE* fp = fopen("../sample-jsons/sample1.json", "r+t");
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
            printf("array[%lu] does not exist.\n", index);

        } else if ( !item.isNumber() ) {
            printf("array[%lu] is not a number.\n", index);

        } else {
            printf("array[%lu] = %d\n", index, item.toInt());
        }


        return true;
    }

    static bool     doTest2() {
        FILE* fp = fopen("../sample-jsons/fathers.json", "r+t");
        if ( fp == 0 ) {
            puts("fathers.json not found!");
            return false;
        }

        // hold json data through the test
        static const size_t KBufferLength = 128*1024;
        char buffer[KBufferLength+1] = {0};
        fread(buffer, KBufferLength, 1, fp);
        fclose(fp);


        // parsing
        gason::JsonValue        root;
        gason::JsonAllocator    allocator;
        gason::JsonParseStatus  status = gason::jsonParse(buffer, root, allocator);
        // buffer will be over-written by jsonParse
        if ( status != gason::JSON_PARSE_OK ) {
            puts("parsing failed!");
            return false;
        }

        bool ok = false;
        gason::JsonIterator it = gason::begin(root("fathers"));
        while ( it.isValid() ) {
            gason::JsonValue father = it->value;

            size_t sonsCount = 0;
            for ( gason::JsonIterator chit = gason::begin(father("sons")); chit.isValid(); chit++ ) {
                sonsCount++;
            }
            size_t daughtersCount = 0;
            for ( gason::JsonIterator chit = gason::begin(father("daughters")); chit.isValid(); chit++ ) {
                daughtersCount++;
            }

            if ( (sonsCount + daughtersCount) >= 4 ) {
                int id           = father("id").toInt(&ok);
                const char* name = father("name").toString(&ok);

                printf("%s (#%d) has %lu children!!\n", name, id,
                       sonsCount + daughtersCount);

            }

            it++;
        }


        return true;
    }

    static bool     doTest3() {
        FILE* fp = fopen("../sample-jsons/servlet.json", "r+t");
        if ( fp == 0 ) {
            puts("servlet.json not found!");
            return false;
        }

        // hold json data through the test
        static const size_t KBufferLength = 4*1024;
        char buffer[KBufferLength+1] = {0};
        fread(buffer, KBufferLength, 1, fp);
        fclose(fp);


        // parsing
        gason::JsonValue        root;
        gason::JsonAllocator    allocator;
        gason::JsonParseStatus  status = gason::jsonParse(buffer, root, allocator);
        // buffer will be over-written by jsonParse
        if ( status != gason::JSON_PARSE_OK ) {
            puts("parsing failed!");
            return false;
        }

        bool ok = false;
        gason::JsonValue deepChild = root
                                     .child("web-app")
                                     .child ("servlet").at(0)
                                     .child("init-param")
                                     .child("maxUrlLength");

        if ( deepChild  &&  deepChild.isNumber() ) {
            int maxUrlLength = deepChild.toInt();
            if ( maxUrlLength != 500 ) {
                printf("maxUrlLength = %d, should be 500!", maxUrlLength);
            } else {
                puts("maxUrlLength = 500");
            }
        } else {
            puts("can not find maxUrlLength in the specified path!");
        }


        return true;
    }
};

///////////////////////////////////////////////////////////////////////////////
int main(int , char **)
{
    JSonGason::doTest1();
    JSonGason::doTest2();
    JSonGason::doTest3();

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
