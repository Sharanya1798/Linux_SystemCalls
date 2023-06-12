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

char *files[] = {"Account.txt", "User.txt"};
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
void add(char username[], char password[], int type, double amount)
{
	int fd = open("User.txt", O_CREAT|O_RDWR, 0744);
	lseek(fd, 0, SEEK_END);
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
	write(fd, &user, sizeof(user));
	close(fd);
	
	fd = open("Account.txt", O_CREAT|O_RDWR, 0744);
	lseek(fd, 0, SEEK_END);
	
	strcpy(acnt.account_number, acnt_no);
	acnt.balance = amount;
	write(fd, &acnt, sizeof(acnt));
	close(fd);

}		
void addJointUser(char username1[], char password1[], char username2[], char password2[], double amount)
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
	acnt.balance = amount;
	write(fd, &acnt, sizeof(acnt));
	close(fd);
}

int main()
{
	/*addJointUser("user1", "user1@1798", "user2", "user2@1798", 400.0);
	addJointUser("user3", "user3@1798", "user4", "user4@1798", 500.0);
	addJointUser("user5", "user5@1798", "user6", "user6@1798", 600.0);
	addJointUser("user7", "user7@1798", "user8", "user8@1798", 700.0);
	addJointUser("user9", "user9@1798", "user10", "user10@1798", 800.0);
	
	add("admin", "admin@1798", 0, 1000.0);
	add("customer1", "customer1@1798", 1, 2000.0);
	add("customer2", "customer2@1798", 1, 3000.0);
	add("customer3", "customer3@1798", 1, 4000.0);
	add("customer4", "customer4@1798", 1, 5000.0);*/
	
	int fd = open(files[0], O_RDWR); 
	struct Account acnt;
	printf("Account Number\t Balance\n");
	while(read(fd, &acnt, sizeof(acnt)) != 0)
	{
		printf("%s\t%lf\n", acnt.account_number, acnt.balance);
	}
	close(fd);
	printf("\n");
	
	fd = open(files[1], O_RDWR); 
	struct User users;
	printf("User\t\t Password\t\tAccount number\t\t\tType\n");
	while(read(fd, &users, sizeof(users)) != 0)
	{
		printf("%s\t%s\t%s\t\t\t%d\n", users.username, users.password, users.account_number, users.type);
	}
	close(fd);
	printf("\n");	
	/*
	fd = open("Transactions.txt", O_RDWR);
	struct Transaction trans;
	printf("Account number \t Type \t  Amount      Remaining amount\t\tDate\n\n");
	while(read(fd, &trans, sizeof(trans)) != 0)
	{
		printf("%s\t%s\t%0.2lf\t\t%0.2lf\t%s", trans.account_number,  trans.type, trans.amount, trans.remainingBalance, trans.transactionDate);
	} 
	close(fd);*/
}
	
	
