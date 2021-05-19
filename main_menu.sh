#!/bin/bash
	cd ./src
while [ 1 -eq 1 ]
do
	echo "1: Manage your book details"
	echo "2: Send book details to bookstore"
	echo "3: Exit"
	echo "Enter choice: "
	read choice

	if [ $choice -eq 1 ]
	then
		
		./localBookManagement.out
	elif [ $choice -eq 2 ]
	then
	
		./client.out localhost 3000
	elif [ $choice -eq 3 ]
	then
		break
	else
		echo "Invalid choice"
	fi
done