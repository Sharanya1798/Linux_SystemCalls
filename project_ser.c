#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#define PORT 8090
#define PASS_LENGTH 20
#define ERR_EXIT(msg) do{perror(msg);exit(EXIT_FAILURE);}while(0)

struct User
{
	char username[1024];
	char password[1024];
	char account_number[1024];
	int type;
};
struct Account
{
	char account_number[1024];
	double balance;
};
struct Transaction
{
	char transactionDate[1024];
	char account_number[1024];
	double amount;
	char type[1024];
	double remainingBalance;
};
void add(char username[], char password[], int type)
{
	int fd = open("User.txt", O_CREAT|O_RDWR, 0744);
	int emptySpace = 0;
	struct User user1;
	while(read(fd, &user1, sizeof(user1)) != 0)
	{
		if(user1.type == 5)
		{
			emptySpace = 1;
			break;
		}
	}

	struct User user;
	strcpy(user.username, username);
	strcpy(user.password, password);
	char acnt_no[1024];
	memset(acnt_no, 0, sizeof acnt_no);
	struct Account acnt;
	strcat(acnt_no, "SBI-");
	strcat(acnt_no, username);
	strcat(acnt_no, "-14154");
	strcpy(user.account_number, acnt_no);
	user.type = type;
	if(emptySpace == 1)
	{
		lseek(fd, -1*sizeof(user1), SEEK_CUR);
		write(fd, &user, sizeof(user));
	}
	 else {
	 	lseek(fd, 0, SEEK_END);
		write(fd, &user, sizeof(user));
	}
	close(fd);
	
	fd = open("Account.txt", O_CREAT|O_RDWR, 0744);
	int emptySpace1 = 0;
	struct Account acnt1;
	while(read(fd, &acnt1, sizeof(acnt1)) != 0)
	{
		if(strcmp(acnt1.account_number, "00000") == 0)
		{
			emptySpace1 = 1;
			break;
		}
	}
	
	strcpy(acnt.account_number, acnt_no);
	acnt.balance = 0.0;
	if(emptySpace1 == 1)
	{
		lseek(fd, -1*sizeof(acnt), SEEK_CUR);
		write(fd, &acnt, sizeof(acnt));
	}
	 else {
	 	lseek(fd, 0, SEEK_END);
		write(fd, &acnt, sizeof(acnt));
	}
	close(fd);
}		
void addJointUser(char username1[], char password1[], char username2[], char password2[])
{
	char acnt_no[1024];
	memset(acnt_no, 0, sizeof acnt_no);
	struct Account acnt;
	strcat(acnt_no, "SBI-");
	strcat(acnt_no, username1);
	strcat(acnt_no, "-14154");
	
	int fd = open("User.txt", O_CREAT|O_RDWR, 0744);
	lseek(fd, 0, SEEK_END);
	struct User user;
	strcpy(user.username, username1);
	strcpy(user.password, password1);
	strcpy(user.account_number, acnt_no);
	user.type = 2;
	write(fd, &user, sizeof(user));
	close(fd);
	
	fd = open("User.txt", O_CREAT|O_RDWR, 0744);
	lseek(fd, 0, SEEK_END);
	struct User user1;
	strcpy(user1.username, username2);
	strcpy(user1.password, password2);
	strcpy(user1.account_number, acnt_no);
	user1.type = 2;
	write(fd, &user1, sizeof(user1));
	close(fd);
	
	fd = open("Account.txt", O_CREAT|O_RDWR, 0744);
	lseek(fd, 0, SEEK_END);
	
	strcpy(acnt.account_number, acnt_no);
	acnt.balance = 0;
	write(fd, &acnt, sizeof(acnt));
	close(fd);
}
void addTransaction(char type[], struct Account acnt, double amount)
{
	int fd2 = open("Transactions.txt", O_CREAT|O_RDWR, 0744);
	struct Transaction trans;
	time_t t; 
    	time(&t);
    	strcpy(trans.type, type);
	strcpy(trans.account_number, acnt.account_number);
	strcpy(trans.transactionDate, ctime(&t));
	trans.amount = amount;
	trans.remainingBalance = acnt.balance;
	lseek(fd2, 0, SEEK_END);
	write(fd2, &trans, sizeof(trans));
	close(fd2);
}
int doOperations(int type, int socket, char acnt_nmbr[], int opt)
{
	int fd, fd1, fd2, success = 0, returnValue = -1;
	double depositAmount, withdrawAmount;
	fd1 = open("Account.txt", O_RDWR, 0744);
	struct Account acnt;
	while(read(fd1, &acnt, sizeof(acnt)) != 0)
	{
		if(strcmp(acnt.account_number, acnt_nmbr) == 0)
		{
			struct flock lock;
			if(type == 2)
			{
				lock.l_start = (-1)*sizeof(struct Account);
				lock.l_len = sizeof(struct Account);
				lock.l_whence = SEEK_CUR;
				lock.l_pid = getpid();
			}
			switch(opt){
				case 1 : {		
					if(type == 2)
					{
						lock.l_type = F_WRLCK;
						printf("write lock on deposit, before lock");
						fcntl(fd1, F_SETLKW, &lock);
					}
					printf("baby\n");
					read(socket, &depositAmount, sizeof(depositAmount));
					success = 1;
					acnt.balance += depositAmount;
					lseek(fd1, -1 * sizeof(struct Account), SEEK_CUR);
					write(fd1, &acnt, sizeof(acnt));
					addTransaction("Credit", acnt, depositAmount);
					printf("%lf", acnt.balance);
					write(socket, &success, sizeof(success));	
					returnValue = 1;
					break;
				}
				case 2 : {
					if(type == 2)
					{
						lock.l_type = F_WRLCK;
						printf("write lock on withdraw, before lock");
						fcntl(fd1, F_SETLKW, &lock);
					}
					read(socket, &withdrawAmount, sizeof(withdrawAmount));	
					if(acnt.balance >= withdrawAmount)
					{
						acnt.balance -= withdrawAmount;
						lseek(fd1, -1 * sizeof(struct Account), SEEK_CUR);
						write(fd1, &acnt, sizeof(acnt));
						success = 1;
						addTransaction("Dedit", acnt, withdrawAmount);	
					} else { success = 0; }
					write(socket, &success, sizeof(success));	
					returnValue = 2;
					break;
				}
				case 4 : {
					if(type == 2)
					{
						lock.l_type = F_RDLCK;
						printf("read lock on balance enquiry, before lock\n");
						fcntl(fd1, F_SETLKW, &lock);
					}
					double blnc;
					blnc = acnt.balance;
					success = 1;
					write(socket, &success, sizeof(success));	
					write(socket, &blnc, sizeof(blnc));
					printf("%lf", acnt.balance);
					returnValue = 4;
					break;
				}
				case 5 : {
					if(type == 2)
					{
						lock.l_type = F_RDLCK;
						printf("write lock on mini statement, before lock\n");
						fcntl(fd1, F_SETLKW, &lock);
					}
					char acnt_nmbr[1024];
					success = 1;
					strcpy(acnt_nmbr, acnt.account_number);
					write(socket, &acnt_nmbr, sizeof(acnt_nmbr));
					write(socket, &success, sizeof(success));	
					returnValue = 5;
					break;
				}
				case 6 : {
					returnValue = -1;
					break;
				}
			}
			if(type == 2)
			{
				lock.l_type = F_UNLCK;

				sleep(10);  //to ensure transactions are not conflict
				fcntl(fd1, F_SETLK, &lock);
				printf("unlocked\n");
			}
			return returnValue;
		}	
	}
}
int menu1(int socket, char username[], int type)
{
	int op_id;
	double depositAmount, withdrawAmount;
	read(socket, &op_id, sizeof(op_id));
	int fd, fd1, fd2, success = 0;
	struct User customer;
	fd = open("User.txt", O_RDWR);
	while(read(fd, &customer, sizeof(customer)) != 0)
	{
		if(strcmp(customer.username, username) == 0)
		{ 
			if( op_id != 3) 
			{
				char acnt_no[1024];
				strcpy(acnt_no, customer.account_number);
				return doOperations(type, socket, acnt_no, op_id);
			} else { 
				char pswd[1024];
				read(socket, &pswd, sizeof(pswd));
				success = 1;
				if(strcmp(customer.username, username) == 0)
					strcpy(customer.password, pswd);
				lseek(fd, -1 * sizeof(customer), SEEK_CUR);
				write(fd, &customer, sizeof(customer));
				write(socket, &success, sizeof(success));	
				return -1;
			}
		}	
	}
}
int menu2(int socket, char username[])
{
	int opt, fd;
	read(socket, &opt, sizeof(opt));
	switch(opt)
	{
		case 1 : {
			char newuser[1024], pswd[1024], newuser2[1024], pswd2[1024];
			int present = 0, success = 1, cust_type;
			read(socket, &cust_type, sizeof(cust_type));
			while(1) {
				fd = open("User.txt", O_RDWR);
				struct User customer;
				read(socket, &newuser, sizeof(newuser));
				while(read(fd, &customer, sizeof(customer)) != 0)
				{
					if(strcmp(customer.username, newuser) == 0)
					{	present = 1; break; }
				}
				if(present == 1) 
				{    success = 0;
					write(socket, &success, sizeof(success));
				} else {
					write(socket, &success, sizeof(success));
					read(socket, &pswd, sizeof(pswd));
					break;
				}
				close(fd);		
			}
			if(cust_type == 2)
			{
				while(1) {
					fd = open("User.txt", O_RDWR);
					struct User customer2;
					read(socket, &newuser2, sizeof(newuser2));
					while(read(fd, &customer2, sizeof(customer2)) != 0)
					{
						if(strcmp(customer2.username, newuser2) == 0)
						{	present = 1; break; }
					}
					if(present == 1) 
					{    success = 0;
						write(socket, &success, sizeof(success));
					} else {
						write(socket, &success, sizeof(success));
						read(socket, &pswd2, sizeof(pswd2));
						close(fd);
						addJointUser(newuser, pswd, newuser2, pswd2);
						return 1;
					}
				close(fd);		
				}
			}
			add(newuser, pswd, cust_type);
			return 1;
		}
		case 2 : {
			int delete, success = 0;
			read(socket, &delete, sizeof(delete));
			if(delete == 1) {
			while(1) {
				char acnt_no[1024];
				fd = open("Account.txt", O_RDWR);
				struct Account acnt;
				read(socket, &acnt_no, sizeof(acnt_no));
				while(read(fd, &acnt, sizeof(acnt)) != 0)
				{
					if(strcmp(acnt.account_number, acnt_no) == 0)
					{	
						success = 1;
						int prompt = 0;
						write(socket, &success, sizeof(success));	
						if(acnt.balance > 0)
							prompt = 1;
						write(socket, &prompt, sizeof(prompt));
						read(socket, &prompt, sizeof(prompt));
						if(prompt == 0)
						{
							int fd1 = open("User.txt", O_RDWR);
							struct User user1;
							while(read(fd1, &user1, sizeof(user1)) != 0)
							{
								if(strcmp(acnt.account_number, user1.account_number) == 0)
								{
									user1.type = 5;
									lseek(fd1, -1*sizeof(struct User), SEEK_CUR);
									write(fd1, &user1, sizeof(user1));	
									break;
								}	
							}
							strcpy(acnt.account_number, "00000");
							lseek(fd, -1*sizeof(struct Account), SEEK_CUR);
							write(fd, &acnt, sizeof(acnt));	
							close(fd1);		
						} else { return 2; }
						close(fd);
						return 2; 
					 }
				}
				if(success == 0)
					write(socket, &success, sizeof(success));				
				close(fd);		
			} }
			else if(delete == 2)
			{
				while(1) {
				char username[1024];
				fd = open("User.txt", O_RDWR);
				struct User user;
				read(socket, &username, sizeof(username));
				while(read(fd, &user, sizeof(user)) != 0)
				{
					if(strcmp(user.username, username) == 0)
					{	
						success = 1;
						int prompt = 0;
						write(socket, &success, sizeof(success));	
						if(user.type == 2)
						{
							write(socket, &prompt, sizeof(prompt));
							read(socket, &prompt, sizeof(prompt));
							user.type = 5;
							lseek(fd, -1*sizeof(struct User), SEEK_CUR);
							write(fd, &user, sizeof(user));
							struct User user1;
							int fd1 = open("User.txt", O_RDWR);
							while(read(fd1, &user1, sizeof(user1)) != 0)
							{
								if(strcmp(user.account_number, user1.account_number) == 0 && user1.type == 2)
								{		
									user1.type = 1;
									lseek(fd1, -1*sizeof(struct User), SEEK_CUR);
									write(fd1, &user1, sizeof(user1));
								
								}
							}
						} else if(user.type == 1)
						{ 
							int prompt = 0;
							int fd1 = open("Account.txt", O_RDWR);
							struct Account acnt;
							while(read(fd1, &acnt, sizeof(acnt)) != 0)
							{
								if(strcmp(user.account_number, acnt.account_number) == 0)
								{
									if(acnt.balance > 0)
										prompt = 1;
									write(socket, &prompt, sizeof(prompt));
									read(socket, &prompt, sizeof(prompt));
									if(prompt == 0)
									{
										user.type = 5;
										lseek(fd, -1*sizeof(struct User), SEEK_CUR);
										write(fd, &user, sizeof(user));
										
										strcpy(acnt.account_number, "00000");
										lseek(fd1, -1*sizeof(struct Account), SEEK_CUR);
										write(fd1, &acnt, sizeof(acnt));	
										close(fd1);		
									} else { return 2; }
									return 2;
								}
							}
						close(fd);
						return 2; 
					 }
				}}
				if(success == 0)
					write(socket, &success, sizeof(success));				
				close(fd);		
			}	
			}
		return 2;
		}
		case 3 : {
			int choice, success=0;
			read(socket, &choice, sizeof(choice));
			char usrname[1024];
			while(1) {
				read(socket, &usrname, sizeof(usrname));
				struct User customer;
				fd = open("User.txt", O_RDWR);
				while(read(fd, &customer, sizeof(customer)) != 0)
				{
					if(strcmp(customer.username, usrname) == 0)
					{
						char pswd[1024];
						success = 1;
						write(socket, &success, sizeof(success));
						if(choice == 1)
						{
							read(socket, &pswd, sizeof(pswd));
							strcpy(customer.password, pswd);
							lseek(fd, -1*sizeof(customer), SEEK_CUR);
							write(fd, &customer, sizeof(customer));
						} else if(choice == 2)
						{
							char newUsr[1024];
							struct User newCustomer;
							customer.type = 2; 
							lseek(fd, -1*sizeof(customer), SEEK_CUR);
							write(fd, &customer, sizeof(customer));
							read(socket, &newUsr, sizeof(newUsr));
							read(socket, &pswd, sizeof(pswd));
							strcpy(newCustomer.username, newUsr);
							strcpy(newCustomer.password, pswd);
							strcpy(newCustomer.account_number, customer.account_number);
							newCustomer.type = 2; 
							lseek(fd, 0, SEEK_END);
							write(fd, &newCustomer, sizeof(newCustomer));
							close(fd);
						} 
						return 3;
					}
				}
				if(success == 0)
					write(socket, &success, sizeof(success));
				close(fd);
			}
			return 3;
		}
		case 4 : {
			char user[1024];
			int success = 0;
			read(socket, &user, sizeof(user));
			struct User customer;
			fd = open("User.txt", O_RDWR);
			while(read(fd, &customer, sizeof(customer)) != 0)
			{
				if(strcmp(customer.username, user) == 0)
				{
					char acnt_no[1024];
					int type1;
					success = 1;
					write(socket, &success, sizeof(success));
					strcpy(acnt_no, customer.account_number);
					write(socket, &acnt_no, sizeof(acnt_no));
					type1 = customer.type;
					write(socket, &type1, sizeof(type1));
					break;
				} 
			}
			if(success == 0)
				write(socket, &success, sizeof(success));
			return 4;
		}
		case 5 : {
			return -1;
		}
	}
	
	//return -1;
}
void service_cli(int sock){

	printf("Client [%d] connected\n", sock);
	
	int type, fd, valid=1, invalid=0, login_success=0;
	char username[1024], password[1024];
	read(sock, &username, sizeof(username));
	read(sock, &password, sizeof(password));
	fd = open("User.txt", O_RDWR, 0744);
	struct User checking;
	while(read(fd, &checking, sizeof(checking)) != 0)
	{
		if(strcmp(checking.username, username)==0 && strcmp(checking.password, password)==0)
		{
		login_success = 1;
		type = checking.type;
		break;
		}
	}
	write(sock, &login_success, sizeof(login_success));
	write(sock, &type, sizeof(type));

	if(type != 0)
		while(-1!=menu1(sock, username, type));
	else
		while(-1!=menu2(sock, username));
	close(fd);
	close(sock);
	printf("Client [%d] disconnected\n", sock);
}
int main(){

	printf("Initializing connection...\n");
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd==-1) {
		printf("socket creation failed\n");
		ERR_EXIT("socket()");
	}
	int optval = 1;
	int optlen = sizeof(optval);
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, optlen)==-1){
		printf("set socket options failed\n");
		ERR_EXIT("setsockopt()");
	}
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(PORT);
	printf("Binding socket...\n");
	if(bind(sockfd, (struct sockaddr *)&sa, sizeof(sa))==-1){
		printf("binding port failed\n");
		ERR_EXIT("bind()");
	}
	if(listen(sockfd, 100)==-1){
		printf("listen failed\n");
		ERR_EXIT("listen()");
	}
	printf("Listening...\n");
	while(1){
		int connectedfd;
		if((connectedfd = accept(sockfd, (struct sockaddr *)NULL, NULL))==-1){
			printf("connection error\n");
			ERR_EXIT("accept()");
		}
		if(fork()==0){
			service_cli(connectedfd);
			exit(1);
		}
	}
	close(sockfd);
	printf("Connection closed!\n");
	return 0;
}
