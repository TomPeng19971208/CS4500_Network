#include <iostream>
#include <sys/time.h>
#include "schema.h"
#include "dataframe.h"
int main() {
    time_t start, end;
    start = time(NULL);
    String* s1 = new String("s1");
    String* s2 = new String("s2");
    Schema* schema = new Schema("IIIISSBB");
    DataFrame* df = new DataFrame(*schema);

    for(int i=0; i<2000000; i++) {
        schema->add_row(s1);
        Row* r = new Row(*schema);
        r->set(0, 20);
        r->set(1, 2);
        r->set(2, 3);
        r->set(3, 4);
        r->set(4,s1);
        r->set(5, s2);
        r->set(6, true);
        r->set(7, false);
        df ->add_row(*r);
        int j = df->get_int(0, i);
    }
    SumRower* sumRower = new SumRower();
    df->pmap(*sumRower);
    end = time(NULL);
    printf("time cost is %ds \n", end-start);
    size_t size = (sizeof(int) * 4 + sizeof(String)* 2 + sizeof(bool) * 2) * 2000000 / 1000000;
    printf("size of dataset we used for this test is %dmb", size);
    return 0;
}