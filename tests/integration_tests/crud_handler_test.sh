#!/bin/bash


#=============================
# MUST BE RUN FROM MAIN REPO DIR 
#   (must run the tests in the root directory of the project to simulate the prod environment)
#=============================


#========TEST CREATING FILE WITH POST=============
post_call() { 
    touch post.json
    touch test.json
    
    echo -n '{"Id": 79, "status": 3}' > test.json
    curl -X POST localhost:$1/api/int_test -H "Content-Type: application/json" -d '{"Id": 79, "status": 3}' -o post.json -s
    #use python to access id in json data
    post_json_id=$(python3 -c "import sys, json; f = open('post.json',); print(json.load(f)['id'])")

    diff ../crud/int_test/$post_json_id test.json

    if [ "$?" != "0" ]
    then
        echo "Unsuccessful POST (file was not saved correctly)"
        exit 1
    else
        echo "Successful POST"
    fi
}

#===========TEST RETRIEVING FILE WITH GET============

get_call() {
    touch get.json

    curl localhost:$1/api/int_test/$2 -o get.json -s

    diff ../crud/int_test/$2 get.json

    if [ "$?" != "0" ]
    then
        echo "Unsuccessful GET (file was not retrieved correctly)"
        exit 1
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

    diff ../crud/int_test/$2 put.json

    if [ "$?" != "0" ]
    then
        echo "Unsuccessful PUT (file was not updated correctly)"
        exit 1
    else
        echo "Successful PUT"
    fi
}

#=============TEST GET JSON LIST OF IDS=================

get_list_call () {
    touch get_list.json

    curl localhost:$1/api/int_test -o get_list.json -s
    get_json_list=$(python3 -c "import sys, json; f = open('get_list.json',); arr = json.load(f); [print(i) for i in arr]")
    readarray myarray < <(echo "${get_json_list[@]}")
    length=$(echo "${#myarray[@]}")
    files_in_dir=$(ls -1 ../crud/int_test | wc -l)

    for i in $get_json_list
    do
        if test -f ../crud/int_test/$i; then
            continue
        else
            echo "Unsuccessful GET (list returned included non-existent ID)"
            exit 1
            return
        fi
    done

    if [ "$files_in_dir" == "$length" ]
    then
        echo "Successful GET (list)"
    else
        echo "Unsuccessful GET (list returned different number of IDs than directory contains)"
        exit 1
    fi

}


#=============TEST DELETING FILE WITH DELETE============

delete_call() {
    
    touch delete.txt
    touch delete_test.txt

    echo -n "File /api/int_test/$2 deleted successfully." > delete_test.txt
    curl -X DELETE localhost:$1/api/int_test/$2 -o delete.txt -s

    diff delete.txt delete_test.txt

    if [ "$?" != "0" ]
    then
        echo "Incorrect DELETE response (wrong HTTP code and/or body)"
        exit 1
    else
        echo "Correct DELETE response"
    fi

    if test -f ../crud/int_test/$2; then
        echo "Unsuccessful DELETE (file still exists)"
        exit 1
    else
        echo "Successful DELETE"
    fi
}


clean_up () {
    rm listen.txt post.json test.json get.json put.json delete.txt delete_test.txt get_list.json
    rm -r ../crud/int_test
    kill $(ps -aux | grep "../../build/bin/server $file_name" | awk '{print $2}' | head -n 1)
    kill -9 "$!"
}

touch ./listen.txt

file_name="../../config/server_config"
PORTVALUE="8080"
echo "$PORTVALUE"
echo "$file_name"
../../build/bin/server "$file_name" > ./listen.txt 2>&1 &

post_call "$PORTVALUE"

get_call "$PORTVALUE" "$post_json_id"

put_call "$PORTVALUE" "$post_json_id"

get_list_call "$PORTVALUE"

delete_call "$PORTVALUE" "$post_json_id"

clean_up

exit 0 

