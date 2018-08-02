
#ifndef CURLSCRIPT_EXPR_H
#define CURLSCRIPT_EXPR_H


using namespace std;

namespace curlscript {

    struct item {
        string scheme;
        string host;
        string path;

        string get_uri() { return scheme + host + "/" + path; }
    };

    struct expr {
        string identifier;
        vector <item> items1;
        string op;
        vector <item> items2;
    };
}

#endif //CURLSCRIPT_EXPR_H
