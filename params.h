typedef struct _param {
	struct _param *next;
	char *id;
	char *value;
} param;

void add_param(param *head, char *pid, char *pvalue) {
	while(head->next != NULL) head = head->next;
	param *p = malloc(sizeof(param));
	p->id = pid;
	p->value = pvalue;
	p->next = NULL;
	head->next = p;
}

