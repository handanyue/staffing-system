/*************************************************************************
#	 FileName	: server.c
#	 Author		: fengjunhui 
#	 Email		: 18883765905@163.com 
#	 Created	: 2018年12月29日 星期六 13时44分59秒
 ************************************************************************/

#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <pthread.h>

#include "common.h"



sqlite3 *db;  //仅服务器使用

/**************************************
 *函数名：insert_history   仅服务器获取
 *参   数：
 *功   能：获取当前时间
 **************************************/


int insert_history(int acceptfd,MSG *msg,char *word)
{
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn;
	time_t timep;
	time (&timep);
	char t[32];
	char *status = word;
	
	sprintf(t,"%s",asctime(localtime(&timep)));
	sprintf(sql,"insert into historyinfo values ('%s','%s','%s')",t,msg->username,status);
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
		printf("---****----%s.\n",errmsg);		
	}else{
		printf("ok\n");
	}
}

int process_user_or_admin_login_request(int acceptfd,MSG *msg)
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	//封装sql命令，表中查询用户名和密码－存在－登录成功－发送响应－失败－发送失败响应	
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	char *word = "login";
	int nrow,ncolumn;

	msg->info.usertype =  msg->usertype;
	strcpy(msg->info.name,msg->username);
	strcpy(msg->info.passwd,msg->passwd);
	
	printf("usrtype: %#x-----usrname: %s---passwd: %s.\n",msg->info.usertype,msg->info.name,msg->info.passwd);
	sprintf(sql,"select * from usrinfo where usertype=%d and name='%s' and passwd='%s';",msg->info.usertype,msg->info.name,msg->info.passwd);
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
		printf("---****----%s.\n",errmsg);		
	}else{
		//printf("----nrow-----%d,ncolumn-----%d.\n",nrow,ncolumn);		
		if(nrow == 0){
			strcpy(msg->recvmsg,"name or passwd failed.\n");
			send(acceptfd,msg,sizeof(MSG),0);
			
		}else{
			strcpy(msg->recvmsg,"OK");
			send(acceptfd,msg,sizeof(MSG),0);
			
			//加入历史记录
			insert_history(acceptfd,msg,word);			
		}
	}
	return 0;	
}

int process_user_modify_request(int acceptfd,MSG *msg)
{
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn;
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	memset(msg->recvmsg,0,sizeof(msg->recvmsg));
	memset(msg->info.name,0,sizeof(msg->info.name));
	strcpy(msg->info.name,msg->username);
	switch (msg->flags)
	{
		case 1:
			//拼接sql语句为修改地址信息，以工号为修改条件
			sprintf(sql,"update usrinfo set addr='%s' where name='%s';",msg->info.addr,msg->info.name);
			printf("msg->info.addr = %s\n",msg->info.addr);	      //检测有没有传入修改信息！！！
			printf("msg->info.no = %d\n",msg->info.no);
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
				printf("---****----%s.\n",errmsg);
			}else {
				printf("已修改！\n");  				//检测有没有修改成功！！！
				char *word = "修改地址信息";
				strcpy(msg->recvmsg,"OK");
				send(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
				insert_history(acceptfd,msg,word);
			}
			break;
		case 2:
			//拼接sql语句为修改电话信息，以工号为修改条件
			sprintf(sql,"update usrinfo set phone='%s' where name='%s';",msg->info.phone,msg->info.name);
			printf("msg->info.phone = %s\n",msg->info.phone);	      //检测有没有传入修改信息！！！
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
				printf("---****----%s.\n",errmsg);
			}else {
				printf("已修改！\n");                       //检测有没有修改成功！！！
				char *word = "修改电话信息";
				strcpy(msg->recvmsg,"OK");
				send(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
				insert_history(acceptfd,msg,word);
			}
			break;
		case 3:
			//拼接sql语句为修改密码信息，以工号为修改条件
			sprintf(sql,"update usrinfo set passwd='%s' where name='%s';",msg->info.passwd,msg->info.name);
			printf("msg->info.passwd = %s\n",msg->info.passwd);	      //检测有没有传入修改信息！！！
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
				printf("---****----%s.\n",errmsg);
			}else {
				printf("已修改！\n");                       //检测有没有修改成功！！！
				char *word = "修改密码信息";
				strcpy(msg->recvmsg,"OK");
				send(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
				insert_history(acceptfd,msg,word);
			}
			break;
		case 4:
			process_client_request(acceptfd,msg);
			break;
		default:
			break;
	}
}



int process_user_query_request(int acceptfd,MSG *msg)
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn,i;
		sprintf(sql,"select * from usrinfo where name='%s'",msg->username);
		if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
			printf("---****----%s.\n",errmsg);
		}else {
			for (i = 0; i < nrow; i++) 
			{
				printf("---------this is test mode!----------\n");
				char *word = "查询信息";
				sprintf(msg->recvmsg,"%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",result[ncolumn],\
				result[ncolumn+1],result[ncolumn+2],result[ncolumn+3],\
				result[ncolumn+4],result[ncolumn+5],result[ncolumn+6],\
				result[ncolumn+7],result[ncolumn+8],result[ncolumn+9],result[ncolumn+10]);
				printf("%s\n",msg->recvmsg);
				send(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
				insert_history(acceptfd,msg,word);
			} 
		}
}


int process_admin_modify_request(int acceptfd,MSG *msg)
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn;
	sprintf(sql,"select * from usrinfo where staffno=%d",msg->info.no);
	sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg);
	if(nrow == 0){
		strcpy(msg->recvmsg,"NO");
		send(acceptfd,msg,sizeof(MSG),0);
	}else{
		switch(msg->flags)
		{
			case 1:
				sprintf(sql,"update usrinfo set passwd='%s' where staffno=%d;",msg->info.passwd,msg->info.no);
				printf("msg->info.passwd = %s\n",msg->info.passwd);	      //检测有没有传入修改信息！！！
				if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
					printf("---****----%s.\n",errmsg);
				}else {
					printf("已修改！\n");                       //检测有没有修改成功！！！
					char *word = "修改密码信息";
					strcpy(msg->recvmsg,"OK");
					send(acceptfd,msg,sizeof(MSG),0);
					insert_history(acceptfd,msg,word);
				}
				break;
			case 2:
				sprintf(sql,"update usrinfo set work='%s' where staffno=%d;",msg->info.work,msg->info.no);
				printf("msg->info.work = %s\n",msg->info.work);	      //检测有没有传入修改信息！！！
				if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
					printf("---****----%s.\n",errmsg);
				}else {
					printf("已修改！\n");                       //检测有没有修改成功！！！
					char *word = "修改职位信息";
					strcpy(msg->recvmsg,"OK");
					send(acceptfd,msg,sizeof(MSG),0);
					insert_history(acceptfd,msg,word);
				}
				break;
			case 3:
				sprintf(sql,"update usrinfo set level=%d where staffno=%d;",msg->info.level,msg->info.no);
				printf("msg->info.level = %d\n",msg->info.level);	      //检测有没有传入修改信息！！！
				if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
					printf("---****----%s.\n",errmsg);
				}else {
					printf("已修改！\n");                       //检测有没有修改成功！！！
					char * word = "修改等级信息";
					strcpy(msg->recvmsg,"OK");
					send(acceptfd,msg,sizeof(MSG),0);
					insert_history(acceptfd,msg,word);
				}
				break;
			case 4:
				sprintf(sql,"update usrinfo set salary=%lf where staffno=%d;",msg->info.salary,msg->info.no);
				printf("msg->info.passwd = %lf\n",msg->info.salary);	      //检测有没有传入修改信息！！！
				if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
					printf("---****----%s.\n",errmsg);
				}else {
					printf("已修改！\n");                       //检测有没有修改成功！！！
					char *word = "修改薪资信息";
					strcpy(msg->recvmsg,"OK");
					send(acceptfd,msg,sizeof(MSG),0);
					insert_history(acceptfd,msg,word);
				}
				break;
			case 5:
				process_client_request(acceptfd,msg);
				break;
		}
	}
}


int process_admin_adduser_request(int acceptfd,MSG *msg)
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn;
	sprintf(sql,"insert into usrinfo values(%d,%d,'%s','%s',%d,'%s','%s','%s','%s','%d','%lf');",msg->info.no,msg->info.usertype,msg->info.name,msg->info.passwd,msg->info.age,msg->info.phone,msg->info.addr,msg->info.work,msg->info.date,msg->info.level,msg->info.salary);
	printf("msg->info.level = %d\n",msg->info.level);	      //test
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
		printf("---****----%s.\n",errmsg);
	}else{
		printf("已录入！\n");                       //检测有没有录入成功！！！
		char * word = "录入员工信息";
		strcpy(msg->recvmsg,"OK");
		send(acceptfd,msg,sizeof(MSG),0);
		insert_history(acceptfd,msg,word);
	}
}



int process_admin_deluser_request(int acceptfd,MSG *msg)
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn;
	sprintf(sql,"select * from usrinfo where staffno=%d",msg->info.no);
	sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg);
	if(nrow == 0){
		strcpy(msg->recvmsg,"NO");
		send(acceptfd,msg,sizeof(MSG),0);
	}else{
		sprintf(sql,"delete from usrinfo where staffno=%d;",msg->info.no);
		if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
			printf("---****----%s.\n",errmsg);
		}else{
			printf("已删除！\n");                       //检测有没有成功删除！！！
			char * word = "删除员工信息";
			strcpy(msg->recvmsg,"OK");
			send(acceptfd,msg,sizeof(MSG),0);
			insert_history(acceptfd,msg,word);
		}
	}
}


int process_admin_query_request(int acceptfd,MSG *msg)
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn,i,j;
	switch (msg->flags)
	{
		case 1:
			//根据员工号查询
			sprintf(sql,"select * from usrinfo where staffno=%d",msg->info.no);	     
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
				printf("---****----%s.\n",errmsg);
			}else {
				for (i = 0; i < nrow; i++) 
				{
					printf("---------this is test mode!----------\n");
					sprintf(msg->recvmsg,"%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",result[ncolumn],\
					result[ncolumn+1],result[ncolumn+2],result[ncolumn+3],\
					result[ncolumn+4],result[ncolumn+5],result[ncolumn+6],\
					result[ncolumn+7],result[ncolumn+8],result[ncolumn+9],result[ncolumn+10]);
					printf("%s\n",msg->recvmsg);
					send(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
				} 
			}
			if(nrow == 0){
				memset(msg->recvmsg,0,sizeof(msg->recvmsg));
				strcat(msg->recvmsg,"查无此人!");
				send(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
			}
			break;
		case 2:
		//根据员工电话查询
			sprintf(sql,"select * from usrinfo where phone='%s'",msg->info.phone);	      
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
				printf("---****----%s.\n",errmsg);
			}else { 
				for (i = 0; i < nrow; i++) 
				{
//					printf("---------this is test mode!----------\n");
					sprintf(msg->recvmsg,"%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",result[ncolumn],\
					result[ncolumn+1],result[ncolumn+2],result[ncolumn+3],\
					result[ncolumn+4],result[ncolumn+5],result[ncolumn+6],\
					result[ncolumn+7],result[ncolumn+8],result[ncolumn+9],result[ncolumn+10]);
					printf("%s\n",msg->recvmsg);
					send(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
				} 
			}
			if(nrow == 0){
				memset(msg->recvmsg,0,sizeof(msg->recvmsg));
				strcat(msg->recvmsg,"查无此人!\n");
				send(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
			}
			break;
		case 3:
			//根据员工住址查询
			sprintf(sql,"select * from usrinfo where addr='%s'",msg->info.addr);	      
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
				printf("---****----%s.\n",errmsg);
			}else { 
				for (i = 0; i < nrow; i++) 
				{
//					printf("---------this is test mode!----------\n");
					sprintf(msg->recvmsg,"%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",result[ncolumn],\
					result[ncolumn+1],result[ncolumn+2],result[ncolumn+3],\
					result[ncolumn+4],result[ncolumn+5],result[ncolumn+6],\
					result[ncolumn+7],result[ncolumn+8],result[ncolumn+9],result[ncolumn+10]);
					printf("%s\n",msg->recvmsg);
					send(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
				}
			if(nrow == 0){
				memset(msg->recvmsg,0,sizeof(msg->recvmsg));
				strcat(msg->recvmsg,"查无此人!\n");
				send(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
			}
			break;
			}
		case 4:
		//查找所有
			sprintf(sql,"select * from usrinfo");	      
			if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
				printf("---****----%s.\n",errmsg);
			}else { 
				msg->flags = nrow;
				send(acceptfd,&msg->flags,sizeof(msg->flags),0);
				memset(msg->recvmsg,0,sizeof(msg->recvmsg));
				char * word = "查看员工信息";
				for (i = 0; i <= nrow; i++){
					for(j = 0;j < ncolumn;j++)
					{
						strcat(msg->recvmsg,*result++);
						strcat(msg->recvmsg," ");
					}
					printf("%s  ",msg->recvmsg);
					send(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
					recv(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0); //阻塞直到客户端接收到一条完整的数据
					memset(msg->recvmsg,0,sizeof(msg->recvmsg));
					insert_history(acceptfd,msg,word);
				} 
			break;
		case 5:
			process_client_request(acceptfd,msg);
			break;
			}
	}
}

int process_admin_history_request(int acceptfd,MSG *msg)
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn,i,j;
	sprintf(sql,"select * from historyinfo");	      
	if(sqlite3_get_table(db,sql,&result,&nrow,&ncolumn,&errmsg) != SQLITE_OK){
		printf("---****----%s.\n",errmsg);
	}else { 
		msg->flags = nrow;
		send(acceptfd,&msg->flags,sizeof(msg->flags),0);
		memset(msg->recvmsg,0,sizeof(msg->recvmsg));
		for (i = 0; i <= nrow; i++){
			for(j = 0;j < ncolumn;j++)
			{
				strcat(msg->recvmsg," ");
				strcat(msg->recvmsg,*result++);
			}
			strcat(msg->recvmsg,"\n");
			//printf("\n");
			printf("%s  ",msg->recvmsg);
			send(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
			recv(acceptfd,msg->recvmsg,sizeof(msg->recvmsg),0);
			memset(msg->recvmsg,0,sizeof(msg->recvmsg));
		}
	}		
}


int process_client_quit_request(int acceptfd,MSG *msg)
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	printf("%s已退出系统!\n",msg->username);
	close(acceptfd);
	memset(msg,0,sizeof(msg));
	return 0;
}


int process_client_request(int acceptfd,MSG *msg)
{
	printf("------------%s-----------%d.\n",__func__,__LINE__);
	switch (msg->msgtype)
	{
		case USER_LOGIN:
		case ADMIN_LOGIN:
			process_user_or_admin_login_request(acceptfd,msg);
			break;
		case USER_MODIFY:
			process_user_modify_request(acceptfd,msg);
			break;
		case USER_QUERY:
			process_user_query_request(acceptfd,msg);
			break;
		case ADMIN_MODIFY:
			process_admin_modify_request(acceptfd,msg);
			break;

		case ADMIN_ADDUSER:
			process_admin_adduser_request(acceptfd,msg);
			break;

		case ADMIN_DELUSER:
			process_admin_deluser_request(acceptfd,msg);
			break;
		case ADMIN_QUERY:
			process_admin_query_request(acceptfd,msg);
			break;
		case ADMIN_HISTORY:
			process_admin_history_request(acceptfd,msg);
			break;
		case QUIT:
			process_client_quit_request(acceptfd,msg);
			break;
		default:
			break;
	}

}


int main(int argc, const char *argv[])
{
	//socket->填充->绑定->监听->等待连接->数据交互->关闭 
	int sockfd;
	int acceptfd;
//	char addr[32] = {0};
//	char port[32] = {0};
	ssize_t recvbytes;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	socklen_t addrlen = sizeof(serveraddr);
	socklen_t cli_len = sizeof(clientaddr);

	MSG msg;
	//thread_data_t tid_data;
	char *errmsg;

//	printf("请输入IP地址:");
//	scanf("%s",addr);
//	printf("请输入端口号:");
//	scanf("%s",port);

	if(sqlite3_open(STAFF_DATABASE,&db) != SQLITE_OK){
		printf("%s.\n",sqlite3_errmsg(db));
	}else{
		printf("the database open success.\n");
	}

	if(sqlite3_exec(db,"create table usrinfo(staffno integer,usertype integer,name text,passwd text,age integer,phone text,addr text,work text,date text,level integer,salary REAL);",NULL,NULL,&errmsg)!= SQLITE_OK){
		printf("%s.\n",errmsg);
	}else{
		printf("create usrinfo table success.\n");
	}

	if(sqlite3_exec(db,"create table historyinfo(time text,name text,words text);",NULL,NULL,&errmsg)!= SQLITE_OK){
		printf("%s.\n",errmsg);
	}else{ //华清远见创客学院         嵌入式物联网方向讲师
		printf("create historyinfo table success.\n");
	}

	//创建网络通信的套接字
	sockfd = socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd == -1){
		perror("socket failed.\n");
		exit(-1);
	}
	printf("sockfd :%d.\n",sockfd); 

	
	/*优化4： 允许绑定地址快速重用 */
	int b_reuse = 1;
	setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &b_reuse, sizeof (int));

	//填充网络结构体
	memset(&serveraddr,0,sizeof(serveraddr));
	memset(&clientaddr,0,sizeof(clientaddr));
	serveraddr.sin_family = AF_INET;
//	serveraddr.sin_port   = htons(atoi(argv[2]));
//	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port   = htons(8888);
	serveraddr.sin_addr.s_addr = inet_addr("192.168.1.155");
//	serveraddr.sin_port = htons(atoi(port));
//	serveraddr.sin_addr.s_addr = inet_addr(addr);

	//绑定网络套接字和网络结构体
	if(bind(sockfd, (const struct sockaddr *)&serveraddr,addrlen) == -1){
		printf("bind failed.\n");
		exit(-1);
	}

	//监听套接字，将主动套接字转化为被动套接字
	if(listen(sockfd,10) == -1){
		printf("listen failed.\n");
		exit(-1);
	}

	//定义一张表
	fd_set readfds,tempfds;
	//清空表
	FD_ZERO(&readfds);
	FD_ZERO(&tempfds);
	//添加要监听的事件
	FD_SET(sockfd,&readfds);
	int nfds = sockfd;
	int retval;
	int i = 0;

	while(1){
		tempfds = readfds;
		//记得重新添加
		retval =select(nfds + 1, &tempfds, NULL,NULL,NULL);
		//判断是否是集合里关注的事件
		for(i = 0;i < nfds + 1; i ++){
			if(FD_ISSET(i,&tempfds)){
				if(i == sockfd){
					//数据交互 
					acceptfd = accept(sockfd,(struct sockaddr *)&clientaddr,&cli_len);
					if(acceptfd == -1){
						printf("acceptfd failed.\n");
						exit(-1);
					}
					printf("ip : %s.\n",inet_ntoa(clientaddr.sin_addr));
					FD_SET(acceptfd,&readfds);
					nfds = nfds > acceptfd ? nfds : acceptfd;
				}else{
					recvbytes = recv(i,&msg,sizeof(msg),0);
					printf("msg.type :%#x.\n",msg.msgtype);
					if(recvbytes == -1){
						printf("recv failed.\n");
						continue;
					}else if(recvbytes == 0){
						printf("peer shutdown.\n");
						close(i);
						FD_CLR(i, &readfds);  //删除集合中的i
					}else{
						process_client_request(i,&msg);
					}
				}
			}
		}
	}
	close(sockfd);

	return 0;
}

















#if 0
					//tid_data.acceptfd = acceptfd;   //暂时不使用这种方式
					//tid_data.state	  = 1;
					//tid_data.thread   = thread[tid++];	
					//pthread_create(&tid_data.thread, NULL,client_request_handler,(void *)&tid_data);
#endif 

#if 0
void *client_request_handler(void * args)
{
	thread_data_t *tiddata= (thread_data_t *)args;

	MSG msg;
	int recvbytes;
	printf("tiddata->acceptfd :%d.\n",tiddata->acceptfd);

	while(1){  //可以写到线程里--晚上的作业---- UDP聊天室
		//recv 
		memset(msg,sizeof(msg),0);
		recvbytes = recv(tiddata->acceptfd,&msg,sizeof(msg),0);
		if(recvbytes == -1){
			printf("recv failed.\n");
			close(tiddata->acceptfd);
			pthread_exit(0);
		}else if(recvbytes == 0){
			printf("peer shutdown.\n");
			pthread_exit(0);
		}else{
			printf("msg.recvmsg :%s.\n",msg.recvmsg);
			strcat(buf,"*-*");
			send(tiddata->acceptfd,&msg,sizeof(msg),0);
		}
	}

}

#endif 













