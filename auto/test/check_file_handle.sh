#!/bin/bash
lsof -n | grep $1 -c
