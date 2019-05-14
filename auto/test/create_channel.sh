#!/bin/bash

    
curl -X POST \
	--connect-timeout 10 \
	 -H "Accept:application/json" \
         -H "Content-Type:application/x-www-form-urlencoded;charset=utf-8" \
	 -d $1 \
         "http://localhost:9000/pull/inject/start" 
