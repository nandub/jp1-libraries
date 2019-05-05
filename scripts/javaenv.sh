#!/usr/bin/env bash

if which jenv &> /dev/null; then
  export JAVA_HOME=$(jenv prefix)
else
  export JAVA_HOME=$(/usr/libexec/java_home)
fi
export JAVA_INCLUDE="-I${JAVA_HOME}/include -I$JAVA_HOME/include/darwin"
echo $JAVA_INCLUDE
