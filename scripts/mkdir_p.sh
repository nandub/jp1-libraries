#!/usr/bin/env bash

DIR=$(dirname "$1")
mkdir -p "${DIR}"
touch "${DIR}"/.keep
