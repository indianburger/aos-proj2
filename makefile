compile: client.c core.h 
	gcc -o client client.c
	touch req_key_file resp_key_file sem_key_file 
	
clean: 
	rm client req_key_file resp_key_file sem_key_file 
