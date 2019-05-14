
#include "mongo_adaptor.h"
#include <mongoc.h>
#include <bson.h>
#include "common.h"
#include "global.h"

MongoAdaptor::MongoAdaptor()
{
}

MongoAdaptor::~MongoAdaptor()
{
}

int MongoAdaptor::init()
{
    mongoc_init();
    
    return 0;
}

void *MongoAdaptor::connect(string addr)
{
    void *ins = static_cast<void *>(mongoc_client_new(addr.c_str()));

    assert(ins != NULL);

    if(ins == NULL) {

		rec_log(LOG_INFO, "mongodb connect failed");
    }

    return ins;
}

void MongoAdaptor::connect_close(void *c)
{
    mongoc_client_t *client = static_cast<mongoc_client_t *>(c);

    if(NULL != client) {
  	
		mongoc_client_destroy(client);
    }
}

void *MongoAdaptor::collection(void *connect, string &db, string &collection)
{
    assert(connect != NULL);

    mongoc_client_t *client = static_cast<mongoc_client_t *>(connect);

    if(NULL == client) {
	rec_log(LOG_INFO, "get mongodb client failed");
		return NULL;
    }

    void *ins = (void*)(mongoc_client_get_collection(client, db.c_str(), collection.c_str()));

    assert(ins != NULL);

    if(ins == NULL) {

		rec_log(LOG_INFO, "mongodb get collection failed");
    }

    return ins;
}

void MongoAdaptor::collection_release(void *collection)
{
    mongoc_collection_t *collec = static_cast<mongoc_collection_t *>(collection);
    
    if(NULL != collec) {

    	mongoc_collection_destroy(collec);
    }
}

int MongoAdaptor::insert(void *collection, bson_t *query)
{
    bson_error_t error;

    assert(collection != NULL);

    mongoc_collection_t *collect = static_cast<mongoc_collection_t *>(collection);

    if (NULL == collect) {
	
		return REC_ERROR_GET_COLLECTION_FAILED;
    }

    return mongoc_collection_insert(collect, MONGOC_INSERT_NONE, query, NULL, &error);
}

int MongoAdaptor::update(void *collection, bson_t *query, bson_t *update)
{
    bson_error_t error;

    assert(collection != NULL);

    mongoc_collection_t *collect = static_cast<mongoc_collection_t *>(collection);
    
    if(NULL == collect) {

	rec_log(LOG_INFO, "get mongodb collection failed");

        return REC_ERROR_GET_COLLECTION_FAILED;
    }

    return mongoc_collection_update(collect, MONGOC_UPDATE_NONE, query, update, NULL, &error);
}

int MongoAdaptor::find(void *collection, bson_t *query, vector<string> &list)
{
    const bson_t *doc;
    char *str;
    vector<string> set;

    assert(collection != NULL);

    mongoc_collection_t *collect = static_cast<mongoc_collection_t *>(collection);

    if(NULL == collect) {

	rec_log(LOG_INFO, "get mongodb collection failed");

        return REC_ERROR_GET_COLLECTION_FAILED;
    }

    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts (collect, query, NULL, NULL);
    
    while (mongoc_cursor_next(cursor, &doc)) {

		str = bson_as_json (doc, NULL);
		
		list.push_back(str);

		bson_free (str);
    }

    return 0;
}

int MongoAdaptor::findOne(void *collection, bson_t *query, string &item)
{
    const bson_t *doc;
    char *str;

    assert(collection != NULL);

    mongoc_collection_t *collect = static_cast<mongoc_collection_t *>(collection);

    if(NULL == collect) {
	
	rec_log(LOG_INFO, "get mongodb collection failed");

        return REC_ERROR_GET_COLLECTION_FAILED;
    }

    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts (collect, query, NULL, NULL);

    while (mongoc_cursor_next(cursor, &doc)) {

        str = bson_as_json (doc, NULL);

        item = str;

        bson_free (str);

		return REC_ERROR_OK;
    }

    return REC_ERROR_PORCESS_INFO_NONE;
}

int MongoAdaptor::remove(void *collection, bson_t *query)
{
    bson_error_t error;

    assert(collection != NULL);

    mongoc_collection_t *collect = static_cast<mongoc_collection_t *>(collection);

    if(NULL == collect) {

	rec_log(LOG_INFO, "get mongodb collection failed");

		return REC_ERROR_GET_COLLECTION_FAILED;
    }

    return mongoc_collection_remove(collect, MONGOC_REMOVE_SINGLE_REMOVE, query, NULL, &error);
}
