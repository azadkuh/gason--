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
    gason::JsonAllocator    iallocator;

public:
    bool     doTest_sample1() {
        puts("\n /---- sample1.json ---\\");
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
        gason::JsonValue        root;
        gason::JsonParseStatus  status = gason::jsonParse(buffer, root, iallocator);
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

    bool     doTest_fathers() {
        puts("\n /---- fathers.json ---\\");
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
        gason::JsonParseStatus  status = gason::jsonParse(buffer, root, iallocator);
        // buffer will be over-written by jsonParse
        if ( status != gason::JSON_PARSE_OK ) {
            puts("parsing failed!");
            return false;
        }

        size_t childrenStat[10] = {0};

        gason::JsonIterator it = gason::begin(root("fathers"));
        while ( it.isValid() ) {
            gason::JsonValue father = it->value;

            size_t childrenCount = 0;
            for ( gason::JsonIterator chit = gason::begin(father("sons")); chit.isValid(); chit++ ) {
                childrenCount++;
            }
            for ( gason::JsonIterator chit = gason::begin(father("daughters")); chit.isValid(); chit++ ) {
                childrenCount++;
            }

            childrenStat[childrenCount] = childrenStat[childrenCount] + 1;

            it++;
        }

        for ( size_t i = 0;    i < 10;    i++ ) {
            printf("%02lu father(s) has/have %lu child/children.\n", childrenStat[i], i);
        }


        return true;
    }

    bool     doTest_servlet() {
        puts("\n /---- servlet.json ---\\");
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
        gason::JsonParseStatus  status = gason::jsonParse(buffer, root, iallocator);
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
            int maxUrlLength = deepChild.toInt(&ok);
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

    bool     doTest_makefile() {
        puts("\n /---- makefile.json ---\\");
        FILE* fp = fopen("../sample-jsons/makefile.json", "r+t");
        if ( fp == 0 ) {
            puts("makefile.json not found!");
            return false;
        }

        // hold json data through the test
        static const size_t KBufferLength = 32*1024;
        char buffer[KBufferLength+1] = {0};
        fread(buffer, KBufferLength, 1, fp);
        fclose(fp);


        // parsing
        gason::JsonValue        root;
        gason::JsonParseStatus  status = gason::jsonParse(buffer, root, iallocator);
        // buffer will be over-written by jsonParse
        if ( status != gason::JSON_PARSE_OK ) {
            puts("parsing failed!");
            return false;
        }

        gason::JsonValue  cmd  = root("command");
        gason::JsonValue  mak  = root("data");

        printf("command is: %s\n",
               (cmd.isString()) ? cmd.toString() : "!!! not found" );
        printf("data is: %lu bytes.\n",
               (mak.isString()) ? strlen(mak.toString()) : 0 );


        return true;
    }
};

///////////////////////////////////////////////////////////////////////////////
int main(int , char **)
{
    JSonGason   tester;

    for ( size_t i = 0;    i < 2;    i++ ) {
        printf("\n\n---------------------> test iteration %03lu\n", i+1);

        tester.doTest_makefile();
        tester.doTest_sample1();
        tester.doTest_servlet();
        tester.doTest_fathers();
    }

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
