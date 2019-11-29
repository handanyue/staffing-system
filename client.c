/*************************************************************************
#	 FileName	: server.c
#	 Author		: fengjunhui 
#	 Email		: 18883765905@163.com 
#	 Created	: 2018年12月29日 星期六 13时44分59秒
 ************************************************************************/

#include<stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "common.h"

void user_menu(int sockfd,MSG *msg);
void admin_menu(int sockfd,MSG *msg);

/**************************************
 *函数名：do_query
 *参   数：消息结构体
 *功   能：管理员查询
 ****************************************/
void do_admin_query(int sockfd,MSG *msg)
{
//	printf("------------%s-----------%d.\n",__func__,__LINE__);
	msg->usertype = ADMIN;
	msg->msgtype = ADMIN_QUERY;
	int n;
	char sql[DATALEN] = {0};
	char *errmsg;
	char **result;
	int nrow,ncolumn;
	printf("*************************************************************\n");
	printf("******* 1：员工号 2：电话 3：住址 4:  所有员工 5：退出*******\n");
	printf("*************************************************************\n");
	printf("====================================================================\n");
	printf("请输入查询条件:");
	scanf("%d",&n);
	printf("====================================================================\n");
	switch (n)
	{
		case 1:
			msg->flags = 1;
			
			printf("请输入查询的员工号:");
			scanf("%d",&(msg->info.no));
			send(sockfd, msg, sizeof(MSG), 0);
			recv(sockfd,msg->recvmsg,sizeof(msg->recvmsg),0);
			printf("====================================================================\n");
			printf("工号  类型  姓名  密码  年龄  电话  住址  职位  入职日期  等级  薪资\n");
			printf("====================================================================\n");
			printf("%s\n",msg->recvmsg);
			printf("====================================================================\n");
			break;
		case 2:
			msg->flags = 2;
			printf("请输入查询的员工电话:");
			scanf("%s",msg->info.phone);
			send(sockfd, msg, sizeof(MSG), 0);
			recv(sockfd,msg->recvmsg,sizeof(msg->recvmsg),0);
			printf("====================================================================\n");
			printf("工号  类型  姓名  密码  年龄  电话  住址  职位  入职日期  等级  薪资\n");
			printf("====================================================================\n");
			printf("%s\n",msg->recvmsg);
			printf("====================================================================\n");
			break;
		case 3:
			msg->flags = 2;
			printf("请输入查询的员工住址:");
			scanf("%s",msg->info.addr);
			send(sockfd, msg, sizeof(MSG), 0);
			recv(sockfd,msg->recvmsg,sizeof(msg->recvmsg),0);
			printf("====================================================================\n");
			printf("工号  类型  姓名  密码  年龄  电话  住址  职位  入职日期  等级  薪资\n");
			printf("====================================================================\n");
			printf("%s\n",msg->recvmsg);
			printf("====================================================================\n");
			break;
		case 4:
			msg->flags = 4;
			send(sockfd, msg, sizeof(MSG), 0);
			recv(sockfd,&msg->flags,sizeof(msg->flags),0);
			printf("====================================================================\n");
			printf("工号  类型  姓名  密码  年龄  电话  住址  职位  入职日期  等级  薪资\n");
			printf("====================================================================\n");
			
			int i;
			for(i = 0;i <= msg->flags;i++)
			{
				recv(sockfd,msg->recvmsg,sizeof(msg->recvmsg),0);
				printf("%s\n",msg->recvmsg);
				printf("====================================================================\n");
				send(sockfd,msg->recvmsg,sizeof(msg->recvmsg),0);
				memset(msg->recvmsg,0,sizeof(msg->recvmsg));
			}
			break;
		case 5:
			msg->flags = 5;
			admin_menu(sockfd,msg);
			break;
		default:
			printf("输入有误，请重新输入!\n");
			break;
	}
	//我也不知道为什么这样写可以运行，而写入case 5 ：中会出错；
	if(n == 5)
	{
		send(sockfd, msg, sizeof(MSG), 0);
	}
}


/**************************************
 *函数名：admin_modification
 *参   数：消息结构体
 *功   能：管理员修改
 ****************************************/
void do_admin_modification(int sockfd,MSG *msg)//管理员修改
{
	//printf("------------%s-----------%d.\n",__func__,__LINE__);
	int n;
	msg->usertype = ADMIN;
	msg->msgtype = ADMIN_MODIFY;
	printf("*************************************************************\n");
	printf("**1：管理员密码 2：员工职位 3：员工等级 4：员工薪资 5：退出**\n");
	printf("*************************************************************\n");
	printf("====================================================================\n");
	printf("请输入需要修改的信息:");
	scanf("%d",&n);
	printf("====================================================================\n");
	switch(n)
	{
		case 1:
			msg->flags = 1;
			msg->info.no = 1001;
			printf("请输入修改后的管理员密码:");
			scanf("%s",msg->info.passwd);
			break;
		case 2:
			msg->flags = 2;
			printf("请输入要修改的员工号:");
			scanf("%d",&(msg->info.no));
			printf("请输入修改后的员工职位:");
			scanf("%s",msg->info.work);
			break;
		case 3:
			msg->flags = 3;
			printf("请输入要修改的员工号:");
			scanf("%d",&(msg->info.no));
			printf("请输入修改后的员工等级:");
			scanf("%d",&(msg->info.level));
			break;
		case 4:
			msg->flags = 3;
			printf("请输入要修改的员工号:");
			scanf("%d",&(msg->info.no));
			printf("请输入修改后的员工薪资:");
			scanf("%lf",&(msg->info.salary));
			break;
		case 5:
			msg->flags = 5;
			admin_menu(sockfd,msg);  
			break;
		default:
			printf("请输入正确参数！\n");
			break;
	}
	send(sockfd, msg, sizeof(MSG), 0);
	recv(sockfd, msg, sizeof(MSG), 0);
	printf("msg->recvmsg :%s\n",msg->recvmsg);
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("修改已生效!\n");
		printf("====================================================================\n");
	}else{
		printf("修改失败！请重试!\n");
		printf("====================================================================\n");
	}
}


/**************************************
 *函数名：admin_adduser
 *参   数：消息结构体
 *功   能：管理员创建用户
 ****************************************/

void do_admin_adduser(int sockfd,MSG *msg)//管理员添加用户
{	
	char ans;
	//printf("------------%s-----------%d.\n",__func__,__LINE__);
	msg->usertype = ADMIN;
	msg->msgtype = ADMIN_ADDUSER;
	printf("请输入员工号:");
	scanf("%d",&(msg->info.no));
	printf("请输入员工类型:");
	scanf("%d",&(msg->info.usertype));
	printf("请输入员工姓名:");
	scanf("%s",msg->info.name);
	printf("请输入员工初始密码:");
	scanf("%s",msg->info.passwd);
	printf("请输入员工年龄:");
	scanf("%d",&(msg->info.age));
	printf("请输入员工联系方式:");
	scanf("%s",msg->info.phone);
	printf("请输入员工居住地址:");
	scanf("%s",msg->info.addr);
	printf("请输入员工职位:");
	scanf("%s",msg->info.work);
	printf("请输入员工入职日期:");
	scanf("%s",msg->info.date);
	printf("请输入员工等级:");
	scanf("%d",&(msg->info.level));
	printf("请输入员工薪资:");
	scanf("%lf",&(msg->info.salary));
	getchar();
	printf("您要输入的员工信息为:");
	printf("%d,%d,%s,%s,%d,%s,%s,%s,%s,%d,%lf\n",msg->info.no,msg->info.usertype,msg->info.name,msg->info.passwd,msg->info.age,msg->info.phone,msg->info.addr,msg->info.work,msg->info.date,msg->info.level,msg->info.salary);
	printf("请确认(默认为y):(y/n)");
	ans = getchar();
	if(ans == 'y' || ans == '\n')
	{
	send(sockfd, msg, sizeof(MSG), 0);
	recv(sockfd,msg,sizeof(MSG),0);
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("员工信息已录入!\n");
		printf("====================================================================\n");
	}else{
		printf("录入失败！请重试!\n");
		printf("====================================================================\n");
	}
	}else{
		admin_menu(sockfd,msg);
	}
}


/**************************************
 *函数名：admin_deluser
 *参   数：消息结构体
 *功   能：管理员删除用户
 ****************************************/
void do_admin_deluser(int sockfd,MSG *msg)//管理员删除用户
{
	msg->usertype = ADMIN;
	msg->msgtype = ADMIN_DELUSER;
	//printf("------------%s-----------%d.\n",__func__,__LINE__);
	printf("请输入要删除的工号:");
	scanf("%d",&(msg->info.no));
	printf("====================================================================\n");
	send(sockfd, msg, sizeof(MSG), 0);
	recv(sockfd,msg,sizeof(MSG),0);
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		printf("员工信息已删除!\n");
		printf("====================================================================\n");
	}else{
		printf("查无此人！请重试!\n");
		printf("====================================================================\n");
	}
}



/**************************************
 *函数名：do_history
 *参   数：消息结构体
 *功   能：查看历史记录
 ****************************************/
void do_admin_history (int sockfd,MSG *msg)
{
	//printf("------------%s-----------%d.\n",__func__,__LINE__);
	msg->usertype = ADMIN;
	msg->msgtype = ADMIN_HISTORY;
	send(sockfd, msg, sizeof(MSG), 0);
	recv(sockfd,&msg->flags,sizeof(msg->flags),0);	
	int i;
	for(i = 0;i <= msg->flags;i++)
	{
		recv(sockfd,msg->recvmsg,sizeof(msg->recvmsg),0);
		printf("%s\n",msg->recvmsg);
		printf("====================================================================\n");
		send(sockfd,msg->recvmsg,sizeof(msg->recvmsg),0);
		memset(msg->recvmsg,0,sizeof(msg->recvmsg));
	}
}


/**************************************
 *函数名：admin_menu
 *参   数：套接字、消息结构体
 *功   能：管理员菜单
 ****************************************/
void admin_menu(int sockfd,MSG *msg)
{
	int n;

	while(1)
	{
		printf("*************************************************************\n");
		printf("* 1：查询  2：修改 3：添加用户  4：删除用户  5：查询历史记录*\n");
		printf("* 6：退出													*\n");
		printf("*************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d",&n);
		printf("====================================================================\n");
		getchar();

		switch(n)
		{
			case 1:
				do_admin_query(sockfd,msg);
				break;
			case 2:
				do_admin_modification(sockfd,msg);
				break;
			case 3:
				do_admin_adduser(sockfd,msg);
				break;
			case 4:
				do_admin_deluser(sockfd,msg);
				break;
			case 5:
				do_admin_history(sockfd,msg);
				break;
			case 6:
				msg->msgtype = QUIT;
				send(sockfd, msg, sizeof(MSG), 0);
				close(sockfd);
				exit(0);
			default:
				printf("您输入有误，请重新输入!\n");
				printf("====================================================================\n");
		}
	}
}






/**************************************
 *函数名：do_query
 *参   数：消息结构体
 *功   能：用户查询
 ****************************************/
void do_user_query(int sockfd,MSG *msg)
{
	char **result;
	//printf("------------%s-----------%d.\n",__func__,__LINE__);
	msg->usertype = USER;
	msg->msgtype = USER_QUERY;
	printf("您的姓名是:%s\n",msg->username);
	send(sockfd,msg,sizeof(MSG),0);
	recv(sockfd,msg->recvmsg,sizeof(msg->recvmsg),0);
	printf("====================================================================\n");
	printf("工号  类型  姓名  密码  年龄  电话  住址  职位  入职日期  等级  薪资\n");
	printf("====================================================================\n");
	printf("%s\n",msg->recvmsg);
	printf("====================================================================\n");
}



/**************************************
 *函数名：do_modification
 *参   数：消息结构体
 *功   能：修改
 ****************************************/
void do_user_modification(int sockfd,MSG *msg)
{	
	int n;
	//printf("------------%s-----------%d.\n",__func__,__LINE__);
	msg->usertype = USER;
	msg->msgtype = USER_MODIFY;
	send(sockfd,msg,sizeof(msg),0);
	printf("请确认您的姓名:");
	scanf("%s",msg->info.name);
	send(sockfd,msg->info.name,sizeof(msg->info.name),0);
	recv(sockfd,msg->recvmsg,sizeof(msg->recvmsg),0);
	if(strcmp(msg->recvmsg,"OK") == 0){
		printf("*************************************************************\n");
		printf("************* 1：家庭住址 2：电话 3：密码 4：退出************\n");
		printf("*************************************************************\n");
		printf("请输入需要修改的信息:");
		scanf("%d",&n);
		switch(n)
		{
			case 1:
				msg->flags = 1;
				printf("请输入修改后的地址:");
				scanf("%s",msg->info.addr);
				break;
			case 2:
				msg->flags = 2;
				printf("请输入修改后的电话:");
				scanf("%s",msg->info.phone);
				break;
			case 3:
				msg->flags = 3;
				printf("请输入修改后的密码:");
				scanf("%s",msg->info.passwd);
				break;
			case 4:
				msg->flags = 4;
				user_menu(sockfd,msg);
				break;
			default:
				printf("您输入有误，请重新输入\n");
				break;
		}
		send(sockfd, msg, sizeof(MSG), 0);
		recv(sockfd, msg, sizeof(MSG), 0);
		printf("msg->recvmsg :%s\n",msg->recvmsg);
		if(strncmp(msg->recvmsg, "OK", 2) == 0)
		{
			printf("修改已生效!\n");
		}else{
			printf("修改失败！请重试!\n");
		}
	}else{
		printf("姓名不符！请重试!\n");
	}
}


/**************************************
 *函数名：user_menu
 *参   数：消息结构体
 *功   能：管理员菜单
 ****************************************/
void user_menu(int sockfd,MSG *msg)
{
//	printf("------------%s-----------%d.\n",__func__,__LINE__);
	int n;
	while(1)
	{
		printf("***************************************************************\n");
		printf("****************  1:查询    2:修改    3:退出	***************\n");
		printf("***************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d",&n);
		getchar();

		switch(n)
		{
			case 1:
				do_user_query(sockfd,msg);
				break;
			case 2:
				do_user_modification(sockfd,msg);
				break;
			case 3:
				msg->msgtype = QUIT;
				send(sockfd, msg, sizeof(MSG), 0);
				close(sockfd);
				exit(0);
			default:
				printf("====================================================================\n");
				printf("您输入有误，请重新输入\n");
				printf("====================================================================\n");
				break;
		}
	}
}




int admin_or_user_login(int sockfd,MSG *msg)
{
	//printf("------------%s-----------%d.\n",__func__,__LINE__);
	//输入用户名和密码
	memset(msg->username, 0, NAMELEN);
	printf("请输入用户名：");
	scanf("%s",msg->username);
	getchar();

	memset(msg->passwd, 0, DATALEN);
	printf("====================================================================\n");
	printf("请输入密码（6位）");
	scanf("%s",msg->passwd);
	printf("====================================================================\n");
	getchar();

	//发送登陆请求
	send(sockfd, msg, sizeof(MSG), 0);
	//接受服务器响应
	recv(sockfd, msg, sizeof(MSG), 0);
	printf("msg->recvmsg :%s\n",msg->recvmsg);

	//判断是否登陆成功
	if(strncmp(msg->recvmsg, "OK", 2) == 0)
	{
		if(msg->usertype == ADMIN)
		{
			printf("====================================================================\n");
			printf("亲爱的管理员，欢迎您登陆员工管理系统！\n");
			admin_menu(sockfd,msg);
		}
		else if(msg->usertype == USER)
		{
			printf("====================================================================\n");
			printf("亲爱的用户，欢迎您登陆员工管理系统！\n");
			user_menu(sockfd,msg);
		}
	}
	else
	{
		printf("登陆失败！%s\n", msg->recvmsg);
		return -1;
	}

	return 0;
}


/************************************************
 *函数名：do_login
 *参   数：套接字、消息结构体
 *返回值：是否登陆成功
 *功   能：登陆
 *************************************************/
int do_login(int sockfd)
{	
	int n;
	MSG msg;

	while(1){
		printf("*************************************************************\n");
		printf("*********  1：管理员模式    2：普通用户模式    3：退出**********\n");
		printf("*************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d",&n);
		printf("====================================================================\n");
		getchar();		


		switch(n)
		{
			case 1:
				msg.msgtype  = ADMIN_LOGIN;
				msg.usertype = ADMIN;
				break;
			case 2:
				msg.msgtype =  USER_LOGIN;
				msg.usertype = USER;
				break;
			case 3:
				msg.msgtype = QUIT;
				if(send(sockfd, &msg, sizeof(MSG), 0)<0)
				{
					perror("do_login send");
					return -1;
				}
				close(sockfd);
				exit(0);
			default:
				printf("您的输入有误，请重新输入\n");
				break;
		}
		if(n > 0 && n < 4)
		{
			admin_or_user_login(sockfd,&msg);
		}
	}

}


int main(int argc, const char *argv[])
{
	//socket->填充->绑定->监听->等待连接->数据交互->关闭 
	int sockfd;
	int acceptfd;
//	char addr[32] = {0};
//	char port[32] = {0};
	ssize_t recvbytes,sendbytes;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	socklen_t addrlen = sizeof(serveraddr);
	socklen_t cli_len = sizeof(clientaddr);
	
//	printf("请输入IP地址:");
//	scanf("%s",addr);
//	printf("请输入端口号:");
//	scanf("%s",port);

	//创建网络通信的套接字
	sockfd = socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd == -1){
		perror("socket failed.\n");
		exit(-1);
	}
	printf("sockfd :%d.\n",sockfd); 

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

	if(connect(sockfd,(const struct sockaddr *)&serveraddr,addrlen) == -1){
		perror("connect failed.\n");
		exit(-1);
	}

	do_login(sockfd);

	close(sockfd);

	return 0;
}



