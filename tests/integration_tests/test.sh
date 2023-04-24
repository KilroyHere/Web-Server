#!/bin/bash

touch listen.txt 

../../build/bin/server ../../config/server_config > ./listen.txt 2>&1 &

clean_up () {
    kill "$!"
    rm curl.txt nc.txt listen.txt
}

curl_call() {
    touch curl.txt
    curl localhost:8080 -o curl.txt -s -s
    request=$(cat ./listen.txt | grep "GOOD REQUEST")

    diff curl.txt curl_expected.txt -w -I 'User-Agent: curl/*'

    if [ "$?" != "0" ]
    then
        echo "Incorrect curl"
        clean_up
        exit 1
    fi

    echo "Correct curl"

    if [ "$request" != "GOOD REQUEST!!" ]
    then 
        echo "incorrect response from curl"
        clean_up
        exit 1
    fi

    echo "Correct curl response"

}

nc_call() {
    touch nc.txt
    echo $1 | nc localhost 8080 > ./nc.txt 2>&1
    request=$(cat ./listen.txt | grep "BAD REQUEST")

    diff nc.txt nc_expected.txt -w 
    if [ "$?" != "0" ]
    then
        echo "Incorrect nc"
        clean_up
        exit 1
    fi

    echo "Correct nc"

    if [ "$request" != "BAD REQUEST!!" ]
    then 
        echo "incorrect response from nc"
        clean_up
        exit 1
    fi

    echo "Correct nc response with $1"

}

sleep 2

listen=$(cat ./listen.txt)

if [ "$listen" != "Server listening at Port:8080" ]
then
    echo "Incorrect ping"
    clean_up
    exit 1
fi

echo "Correct ping"

curl_call 

nc_call "String"

clean_up
exit 0


