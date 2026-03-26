#! /bin/bash

if [[ "$1" == "" || "$2" == "" || "$3" == "" || "$4" == "" ]]; then
    echo "Usage: setup_database.sh [DB HOST] [DB PORT] [DB USER] [DB DATABASE]"
    exit 1
fi

read -s -p "Enter password: " PGPASSWORD
export PGPASSWORD

LOCAL_scriptDir=$(realpath $(dirname "$0"))

for envScript in "$LOCAL_scriptDir"/*.sql; do
    echo "Executing script: " $envScript
    psql -h "$1" -p "$2" -U "$3" -d "$4" -f "$envScript"
done
