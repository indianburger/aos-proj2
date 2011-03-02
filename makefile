compile: client.c core.h realclient.c server.c
	gcc -o client client.c
	gcc -o server server.c
	gcc -o realclient realclient.c

	touch req_key_file resp_key_file sem_key_file 
	
clean: 
	rm client req_key_file resp_key_file sem_key_file server realclient
