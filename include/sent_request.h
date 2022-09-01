
void list_task_request(uint16_t op_list, char* m_path);
void list_task_replay(char* m_path);

void creat_task_request(char* path, struct timing* des, uint16_t op_creat, char** ARGV,uint32_t c_position, uint32_t argc);
void creat_task_reply(char*);
      
void quit_task_request(uint16_t op_quit,char* path); 

void remove_task_request(char* request_path, uint64_t taskid, uint16_t remove);
void remove_task_repaly(char* replay_path);

void stderror_task_request(char* request_path, uint16_t stderror,  uint64_t taskid);
void stderror_task_replay(char* replay_path);

void getstdout_task_request(char* request_path, uint64_t taskid, uint16_t remove);
void getstdout_task_replay(char* replay_path);

void gettimesexitcode_task_replay(char* replay_path);