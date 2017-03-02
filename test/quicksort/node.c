#include <stdio.h>
#include <stdlib.h>
typedef struct Node {

	struct Node *pre;
	struct Node *next;
}NODE,* pNODE;


typedef struct Data {
	int val;
	NODE node;
}DATA,*pDATA;



/*void create_node_list(pNODE head,int num) {
	
	pNODE pre,next;	
	pDATA p;
	pre=next=head;

	for(i=0;i<num;i++)
	{
		p=(DATA*)malloc(sizeof(DATA));
		scanf("%d",p->val);
	
		p->node->pre=pre;
		p->node->next=head;
		
		pre=p->node;
		pre->next=p->node;
		
	}

}*/

pDATA  contain_of(pNODE pos)
{
	return (pDATA )((unsigned int * )pos-1);
}
void create_data_list(pNODE head,int num)
{
	pNODE pre,p,next;
	pDATA newdata;
	int i=0;
	pre=next=head;
	for(i=0;i<num;i++)
	{
		newdata=(pDATA)malloc(sizeof(DATA));
		scanf("%d%*c",&(newdata->val));

		pre->next=&newdata->node;
		newdata->node.pre=pre;
		pre=&newdata->node;
	}	
	head->pre=&newdata->node;
	newdata->node.next=head;
	
}

void add_data_tail(pNODE head)
{
	pDATA newdata;
	pNODE pre;
	newdata=(pDATA)malloc(sizeof(DATA));
	scanf("%d",&(newdata->val));

	pre=head->pre;
	pre->next=&newdata->node;
	newdata->node.pre=pre;
	newdata->node.next=head;
	head->pre=&newdata->node;
	
}

int del_data_node(pNODE head,int index)
{
	pNODE pre,pos,next;
	pDATA p;
	pos=head->next;	
	int i=1;
	while((pos!=head))
	{
		if(i==index)
			break;
		pos=pos->next;
		i++;
	}
	if(i!=index)
		return -1;

	pre=pos->pre;
	next=pos->next;

	pre->next=next;
	next->pre=pre;
	p=contain_of(pos);
	free(p);
}

int intput_data_node(pNODE head,int index)
{
	pNODE pre,pos,next;
	pDATA newdata;
	pos=head->next;
	int i=1;
	while(pos->next!=head)
	{
		if(i==index)
			break;
		pos=pos->next;
		i++;	
	}
	if(i!=index)
		return -1;

		next=pos->next;
		newdata=(pDATA)malloc(sizeof(DATA));
		scanf("%d",&newdata->val);
		
		pos->next=&newdata->node;
		newdata->node.pre=pos;
		newdata->node.next=next;

}

int input_data_node2(pNODE head,int index)
{
	int i=0;
	pNODE pre,pos,next;
	pos=head;
	while(pos->next!=head)
	{
		if(i==index)
			break;
		pos=pos->next;
		i++;
	}
	if(i!=index)
		return -1;
	next=pos->next;
	pDATA newdata=(pDATA)malloc(sizeof(DATA));
	scanf("%d%*c",&newdata->val);
	pos->next=&newdata->node;
	next->pre=&newdata->node;
	newdata->node.pre=pos;
	newdata->node.next=next;


}

void show_list_val(pNODE head)
{
	pNODE pos=head->next;
	pDATA p=NULL;
	while(pos!=head)
	{
		p=contain_of(pos);
		printf("val=%d\n",p->val);
		pos=pos->next;
	}
}




int main()
{
	NODE head;
	int d=0;
	int ret=0;
	head.next=&head;
	head.pre=&head;

	pDATA a;

	char option;
	//printf("a=%x,a->val=%x,a.node=%x",a,&(a->val),&a->node);
		int num=0;
		printf("create node list,how many node?\n");
		/*scanf("%d%*c",&num);*/
		scanf("%d%*c",&num);
		printf("num=%d\n",num);
		printf("create node list,input val\n");
		create_data_list(&head,num);
		fflush(stdin);

		while(1)
		{
			int index;
			printf("input option\n");
			/*scanf("%c%*c",&option);*/
			/*while((option=getchar())!='\n')*/
			/*option=getchar();*/
			/*option=getchar();
			fflush(stdin);*/
			scanf("%c%*c",&option);
			switch (option)
			{
				case 'p':
					show_list_val(&head);
					break;
				case 'i':
					printf("input a new data index\n");
					scanf("%d%*c",&index);
					input_data_node2(&head,index);
					break;
				case 'd':
					printf("del one node index\n");
					scanf("%d%*c",&index);
					del_data_node(&head,index);
					break;
				default:
					printf("error option\n");

			}
		}

}



