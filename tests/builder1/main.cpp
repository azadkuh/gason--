#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "jsonbuilder.hpp"

///////////////////////////////////////////////////////////////////////////////
/* sample1.json is going to be made.
{
  "array": [
    0,
    1,
    2
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
        char buffer[257] = {0};
        return build(buffer, 256, "./sample1-w.json");
    }

    static bool     doTestOverflow() {
        char buffer[65] = {0};
        return build(buffer, 64, "./sample1-w-overflow.json");
    }

protected:
    static bool     build(char *buffer, size_t capacity,
                          const char* fileName) {
        gason::JSonBuilder doc(buffer, capacity);

        // this sample JSon at least requires a buffer with 119 bytes.
        doc.startObject()
                .startArray("array")
                .addValue(0)
                .addValue(1)
                .addValue(2)
                .endArray()

           .addValue("boolean", true)
           .addNull("null")
           .addValue("number", 123)
           .startObject("object")
                .addValue("a", "b")
                .addValue("c", "d")
                .addValue("e", "f")
           .endObject()

           .addValue("string", "Hello World")
           .endObject();

        if ( !doc.isBufferAdequate() ) {
            puts("warning: the buffer is small and the output json is not valid.");
        }


        FILE* fp = fopen(fileName, "w+t");
        if ( fp == 0 ) {
            puts("failed opening sample1-w.json to write");
            return false;
        }

        // hold json data through the test
        fwrite(buffer, strlen(buffer), 1, fp);
        fclose(fp);

        return true;
    }
};

///////////////////////////////////////////////////////////////////////////////
int main(int , char **)
{
    JSonGason::doTest1();
    JSonGason::doTestOverflow();

    return 0;
}
///////////////////////////////////////////////////////////////////////////////
