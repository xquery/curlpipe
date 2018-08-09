
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
        string port;
        string path;
        string get_uri() {
            string uri;
            uri += scheme;
            uri += host;
            if(!port.empty()){
                uri += ":" + port;
            };
            uri += path;
            return uri;}
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
        vector <tuple<vector<item>,string,vector<item>>> statements;
    };
}

#endif //CURLSCRIPT_EXPR_H
