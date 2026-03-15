#!/usr/bin/env bash
# Benchmark ESP web server with autocannon: 10 clients, 20s
npx autocannon -c 10 -d 20 http://192.168.0.148/
