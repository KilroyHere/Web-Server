#!/bin/bash

clean_up() {
    kill $(ps -aux | grep "../../build/bin/server $DEFAULTVALUE" | awk '{print $2}' | head -n 1)
    kill -9 "$!"
    rm echo.txt listen.txt
}

curl_sleep() {
    #Call Sleep request handler and put it in the background
    #Have output go to echo.txt
    curl localhost:"$1"/sleep -o echo.txt -s &
}

curl_echo() {
    touch echo.txt
    curl localhost:"$1"/echo -o echo.txt -s 
    
    diff echo.txt echo_expected.txt -w -I 'host: *' -I 'User-Agent: curl/*'

    #If diff did not return 0 then incorrect curl

    if [ "$?" != "0" ]
    then
        echo "Incorrect echo"
        clean_up
        exit 1
    fi

    echo "Correct echo"
}

touch ./listen.txt

DEFAULTVALUE="../../config/server_config"

file_name="${1:-$DEFAULTVALUE}"
PORTVALUE="80"
if [ file_name != "$DEFAULTVALUE" ]
then
    PORTVALUE=$(grep -oP 'port\s*\b(\d+)\b;' $file_name | sed 's/port\s*\b\([0-9]\+\)\b;/\1/')
fi

../../build/bin/server "$file_name" > ./listen.txt 2>&1 &

sleep 2

listen=$(cat ./listen.txt | grep -oP "\b$PORTVALUE\b")

if [ "$listen" != "$PORTVALUE" ]
then
    echo "Incorrect port"
    clean_up
    exit 1
fi

echo "Correct port"

curl_sleep "$PORTVALUE"

curl_echo "$PORTVALUE"

sleep 11

diff echo.txt empty.txt

if [ $? != "0" ]
then
    echo "Incorrect sleep"
    clean_up
    exit 1
fi

echo "parallel_test.sh was successful"

clean_up
exit 0