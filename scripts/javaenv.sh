#!/usr/bin/env bash

if which jenv &> /dev/null; then
  export JAVA_HOME=$(jenv prefix)
else
  export JAVA_HOME=$(/usr/libexec/java_home)
fi
export JAVA_INCLUDE_PATH=${JAVA_HOME}/include
echo -I$JAVA_INCLUDE_PATH -I$JAVA_INCLUDE_PATH/darwin
