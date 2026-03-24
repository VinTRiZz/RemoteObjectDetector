#! /bin/bash

if (! test -e "$1"); then
    echo "Dir not exist: $1"
    echo "Provide as first argument directory for saves"
    exit 1;
fi

if (! test -d "$1"); then
    echo "Not a directory: $1"
    echo "Provide as first argument directory for saves"
    exit 1;
fi

docker run \
  -p 10001:5432 \
  -v $1:/var/lib/postgresql \
  -e POSTGRES_PASSWORD=12345678 \
  -e POSTGRES_USER=testuser \
  -e POSTGRES_DB=testdb \
  postgres:latest
