#!/bin/bash

finalExit=0

#MUST BE RUN FROM MAIN REPO DIR 
#   (must run the tests in the root directory of the project to simulate the prod environment)

#========TEST CREATING FILE WITH POST=============
post_call() { 
    touch post.json
    touch test.json
    
    echo -n '{"Id": 79, "status": 3}' > test.json
    curl -X POST localhost:$1/api/int_test -H "Content-Type: application/json" -d '{"Id": 79, "status": 3}' -o post.json -s
    #use python to access id in json data
    post_json_id=$(python3 -c "import sys, json; f = open('post.json',); print(json.load(f)['id'])")

    diff ../folder4/int_test/$post_json_id test.json

    if [ "$?" != "0" ]
    then
        echo "Unsuccessful POST (file was not saved correctly)"
        finalExit=1
    else
        echo "Successful POST"
    fi
}

#===========TEST RETRIEVING FILE WITH GET============

get_call() {
    touch get.json

    curl localhost:$1/api/int_test/$2 -o get.json -s

    diff ../folder4/int_test/$2 get.json

    if [ "$?" != "0" ]
    then
        echo "Unsuccessful GET (file was not retrieved correctly)"
        finalExit=1
    else
        echo "Successful GET"
    fi
}

#===========TEST UPDATING FILE WITH PUT============

put_call() { 
    touch put.json

    echo -n '{"Id": 80, "status": 4}' > put.json
    curl -X PUT localhost:$1/api/int_test/$2 -H "Content-Type: application/json" -d '{"Id": 80, "status": 4}' -s
    #use python to access id in json data

    diff ../folder4/int_test/$2 put.json

    if [ "$?" != "0" ]
    then
        echo "Unsuccessful PUT (file was not updated correctly)"
        finalExit=1
    else
        echo "Successful PUT"
    fi
}

delete_call() {
    
    touch delete.txt
    touch delete_test.txt

    echo -n "File /api/int_test/$2 deleted successfully." > delete_test.txt
    curl -X DELETE localhost:$1/api/int_test/$2 -o delete.txt -s

    diff delete.txt delete_test.txt

    if [ "$?" != "0" ]
    then
        echo "Incorrect DELETE response (wrong HTTP code and/or body)"
        finalExit=1
    else
        echo "Correct DELETE response"
    fi

    if test -f ../folder4/int_test/$2; then
        echo "Unsuccessful DELETE (file still exists)"
        finalExit=1
    else
        echo "Successful DELETE"
    fi
}


clean_up () {
    rm listen.txt post.json test.json get.json put.json delete.txt delete_test.txt
    rm -r ../folder4/int_test
}

touch ./listen.txt

file_name="config/server_config"
PORTVALUE="8080"
echo "$PORTVALUE"
echo "$file_name"
./build/bin/server "$file_name" > ./listen.txt 2>&1 &
webserverPID=$!

post_call "$PORTVALUE"

get_call "$PORTVALUE" "$post_json_id"

put_call "$PORTVALUE" "$post_json_id"

delete_call "$PORTVALUE" "$post_json_id"

clean_up

kill $webserverPID

exit $finalExit

