
#ifndef CURLSCRIPT_EXPR_H
#define CURLSCRIPT_EXPR_H


using namespace std;

namespace curlscript {

    struct var {
        string var_name;
    };

    struct uri {
        string scheme;
        string host;
        string path;
        string get_uri() { return scheme + host ; }
    };

    struct item {
        struct var;
        struct uri uri;
    };

    struct expr {
        string uid;
        int order;
        string var_name;
        string var_value;
        vector <tuple<vector<item>,string>> statements;
    };
}

#endif //CURLSCRIPT_EXPR_H
