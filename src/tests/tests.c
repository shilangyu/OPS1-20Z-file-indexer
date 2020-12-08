#if TEST

#include "../indexer/indexer.h"
#include <assert.h>

void get_file_type_test() {
    assert(get_file_type("./assets/adobe_crobat.jpg", -1) == INDEX_FILE_TYPE_JPEG);
    assert(get_file_type("./assets/quieres.png", -1) == INDEX_FILE_TYPE_PNG);
    assert(get_file_type("./assets/wojnarowskim.etap1.tar.gz", -1) == INDEX_FILE_TYPE_GZIP);
    assert(get_file_type("./assets/data.zip", -1) == INDEX_FILE_TYPE_ZIP);
    assert(get_file_type("./assets/wahadlo.docx", -1) == INDEX_FILE_TYPE_ZIP);
    assert(get_file_type("./src/main.c", -1) == INDEX_FILE_TYPE_UNKNOWN);
}

int main() {
    get_file_type_test();
}
#endif
