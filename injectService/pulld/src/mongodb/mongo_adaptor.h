
#ifndef __MONGO_ADAPTOR_H__
#define __MONGO_ADAPTOR_H__

#include <bson.h>
#include <string>
#include <vector>

using namespace std;

class MongoAdaptor
{
public:
    MongoAdaptor();
    ~MongoAdaptor();
    
    static int init();
    static void *connect(string addr);
    static void connect_close(void *connect);
    static void *collection(void *connect, string &db, string &collection);
    static void collection_release(void *collection);
    static int insert(void *collection, bson_t *query);
    static int update(void *collection, bson_t *query, bson_t *update);
    static int find(void *collection, bson_t *query, vector<string> &vec);
    static int findOne(void *collection, bson_t *query, string &item);
    static int remove(void *collection, bson_t *query);
};
#endif
