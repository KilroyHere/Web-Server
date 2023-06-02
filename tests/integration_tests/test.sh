#!/bin/bash

clean_up () {
    kill "$!"
    rm nc.txt listen.txt curl.txt
    # sed -i "s/localhost:$1/localhost/g" curl_expected.txt
}

curl_call() {
    touch curl.txt
    curl localhost:"$1"/echo -o curl.txt -s
    echo "$1"
    request=$(cat ./listen.txt | grep -oP "\bGOOD REQUEST\b")
    if [ "$1" != "80" ]
    then 
        sed -i "s/localhost/localhost:$1/g" curl_expected.txt
    fi

    diff curl.txt curl_expected.txt -w -I 'User-Agent: curl/*'

    if [ "$?" != "0" ]
    then
        echo "Incorrect curl"
        clean_up
        exit 1
    fi

    echo "Correct curl"

    if [ "$request" != "GOOD REQUEST" ]
    then 
        echo "$request"
        echo "incorrect response from curl"
        clean_up
        exit 1
    fi

    sed -i "s/localhost:$1/localhost/g" curl_expected.txt
    echo "Correct curl response"

}

nc_call() {
    touch nc.txt
    echo $1 | nc localhost "$2" > ./nc.txt 2>&1
    request=$(cat ./listen.txt | grep -oP "\bBAD REQUEST\b")

    diff nc.txt nc_expected.txt -w 
    if [ "$?" != "0" ]
    then
        echo "Incorrect nc"
        clean_up
        exit 1
    fi

    echo "Correct nc"

    if [ "$request" != "BAD REQUEST" ]
    then 
        echo "incorrect response from nc"
        clean_up
        exit 1
    fi

    echo "Correct nc response with $1"

}

touch ./listen.txt

DEFAULTVALUE="../../config/deploy_config"

file_name="${1:-$DEFAULTVALUE}"
PORTVALUE="80"
if [ file_name != "$DEFAULTVALUE" ]
then
    
    PORTVALUE=$(grep -oP 'port\s*\b(\d+)\b;' $file_name | sed 's/port\s*\b\([0-9]\+\)\b;/\1/')
fi
echo "$PORTVALUE"
echo "$file_name"
../../build/bin/server "$file_name" > ./listen.txt 2>&1 &

sleep 2

listen=$(cat ./listen.txt | grep -oP '\b80\b')
echo "$listen"
echo "$listen"

if [ "$listen" != "$PORTVALUE" ]
then
    echo "Incorrect ping"
    clean_up
    exit 1
fi

echo "Correct ping"

curl_call "$PORTVALUE"

nc_call "String" "$PORTVALUE"



echo "test.sh was successful"

clean_up
exit 0 




