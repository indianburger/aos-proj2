#define REQ_Q_KEY "req_key_file" 
#define RESP_Q_KEY "resp_key_file"
#define REQ_SEM_KEY "sem_key_file"

typedef struct diffie_pkt {
    long mtype;
    int inp_x;
    int inp_p;
    int out_result;
} diffie_pkt;


