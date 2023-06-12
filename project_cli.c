#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define PORT 8090
#define PASS_LENGTH 20
#define ERR_EXIT(msg) do{perror(msg);exit(EXIT_FAILURE);}while(0)

int doBanking(int sock);
int menu(int sock, int type);
int admin_actions(int sock, int action);
int customer_Actions(int sock, int opt);
void view_booking(int sock);

struct Transaction
{
	char transactionDate[1024];
	char account_number[1024];
	double amount;
	char type[1024];
	double remainingBalance;
};

int main(int argc, char * argv[]){
	char *ip = "127.0.0.1";
	if(argc==2){
		ip = argv[1];
	}
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(client_fd == -1){
		printf("socket creation failed\n");
		exit(0);
	}
	struct sockaddr_in ca;
	ca.sin_family=AF_INET;
	ca.sin_port= htons(PORT);
	ca.sin_addr.s_addr = inet_addr(ip);
	if(connect(client_fd, (struct sockaddr *)&ca, sizeof(ca))==-1){
		printf("connect failed\n");
		exit(0);
	}
	printf("connection established\n");

	while(doBanking(client_fd)!=3);

	close(client_fd);

	return 0;
}

int doBanking(int sock){
	int type;
	char username[1024], password[1024];
	system("clear");
	printf("-----------------------------WELCOME TO BANKING SYSTEM--------------------------\n");

	printf("------PLEASE LOGIN------\nEnter Your Username: ");
	scanf("%s", username);
	strcpy(password,getpass("Enter password: "));

	//write(sock, &type, sizeof(type));
	write(sock, &username, sizeof(username));
	write(sock, &password, sizeof(password));

	int valid_login;
	read(sock, &valid_login, sizeof(valid_login));
	read(sock, &type, sizeof(type));
	if(valid_login == 1){
		system("clear");
		while(menu(sock, type)!=-1);
		return 3;
	}
	else{
		printf("Login Failed\n");
		return 3;
	}
}

int menu(int sock, int type){
	int opt;
	if(type != 0){
		printf("\n----- OPTIONS -----\n");
		printf("1. Deposit\n");
		printf("2. Withdraw\n");
		printf("3. Password change\n");
		printf("4. Balance enquiry\n");
		printf("5. View statement\n");
		printf("6. Logout\n");
		printf("Enter Your Choice: ");
		scanf("%d", &opt);
		
		return customer_Actions(sock, opt);
		return -1;
	}
	else
	{
		printf("\n----- OPTIONS -----\n");
		printf("1. Add an Account\n");
		printf("2. Delete an Account\n");
		printf("3. Modify an Account\n");
		printf("4. Search for a Customer\n");
		printf("5. Logout\n");
		printf("Enter Your Choice: ");
		scanf("%d", &opt);
		
		return admin_actions(sock, opt);
		return -1;
	}
}
int admin_actions(int sock, int opt)
{
	system("clear");
	write(sock, &opt, sizeof(opt));
	switch(opt)
	{
		case 1 : {
			char username[1024], password[1024], username2[1024], password2[1024];
			int success, cust_type;
			printf("1. Normal account\n2. Joint account\nEnter your choice: ");
			scanf("%d", &cust_type);
			write(sock, &cust_type, sizeof(cust_type));
			while(1)
			{
				printf("Enter a new username: ");
				scanf("%s", username);
				write(sock, &username, sizeof(username));
				read(sock, &success, sizeof(success));
				if(success == 1)
				{
					strcpy(password,getpass("Enter password: "));
					write(sock, &password, sizeof(password));
					break;
				} else {
					printf("Usename already exists, Try another\n");
				}
			} 
			if(cust_type == 2)
			{
			while(1){
				printf("Enter the second username: ");	
				scanf("%s", username2);
				write(sock, &username2, sizeof(username2));
				read(sock, &success, sizeof(success));
				if(success == 1)
				{
					strcpy(password2,getpass("Enter second user password: "));
					write(sock, &password2, sizeof(password2));
					break;
				} else {
					printf("Usename already exists, Try another\n");
				}
			} }
			printf("Account added Successfully!!");
			return 1;
		}
		case 2 : {
			int delete, success = 0;
			printf("1. Delete Account\n2. Delete Customer\nEnter your choice: ");
			scanf("%d", &delete);
			write(sock, &delete, sizeof(delete));
			if(delete == 1)
			{
				char acntNo[1024];
				while(1) {
					printf("Enter account number: ");
					scanf("%s", acntNo);
					write(sock, &acntNo, sizeof(acntNo));
					read(sock, &success, sizeof(success));
					if(success == 1)
					{
						int prompt, yes = 0;
						read(sock, &prompt, sizeof(prompt));
						if(prompt == 1)
						{
							printf("Account holds some Amount, Do you still want to delete ?\n1.YES\n2.NO\nEnter your choice: ");
							scanf("%d", &yes);
							if(yes == 1)
							{
								prompt = 0;
								printf("Deletion successful!!\n");
							} else { printf("Deletion Aborted\n"); }
						}
						write(sock, &prompt, sizeof(prompt));
						break;
					} else 
						printf("Account Number not found, Try another\n");
				}
			} 
			else if(delete == 2)
			{
				char username[1024];
				while(1)
				{	printf("Enter Username of Customer: ");
					scanf("%s", username);	
					write(sock, &username, sizeof(username));
					read(sock, &success, sizeof(success));
					if(success == 1)
					{
						int prompt, yes = 0;
						read(sock, &prompt, sizeof(prompt));
						if(prompt == 1)
						{
							printf("Account holds some Amount, Do you still want to delete ?\n1.YES\n2.NO\nEnter your choice: ");
							scanf("%d", &yes);
							if(yes == 1)
							{
								prompt = 0;
								printf("Deletion successful!!\n");
							} else { printf("Deletion Aborted\n"); }
						}else  { printf("Deletion successful!!!\n"); }
						write(sock, &prompt, sizeof(prompt));
						break;
					} else 
						printf("Username not found, Try another\n");
				}
			}
			return 2;
		}
		case 3 : {
			int modify, success;
			printf("Which of the follwing you want to modify ?\n");
			printf("1. Modify any user password\n2.Convert to Joint Customer\nEnter your choice: ");
			scanf("%d", &modify);
			write(sock, &modify, sizeof(modify));
			if(modify == 1)
			{
				char usrname[1024], pswd[1024];
				while(1) {
				printf("Enter customer username: ");
				scanf("%s", usrname);
				write(sock, &usrname, sizeof(usrname));
				read(sock, &success, sizeof(success));
				if(success == 1)
				{
					strcpy(pswd,getpass("Enter new password: "));
					write(sock, &pswd, sizeof(pswd));
					break;
				} else 
					printf("Username not found, Try another\n");
				}
				printf("Password updated successfully!!\n");
			} else if(modify == 2)
			{
				char usernme[1024], newUsr[1024], pswd[1024];
				while(1) {
				printf("Enter Username of customer you want to convert\n");
				scanf("%s", usernme);
				write(sock, &usernme, sizeof(usernme));
				read(sock, &success, sizeof(success));
				if(success == 1)
				{
					printf("Enter new customer username: ");
					scanf("%s", newUsr);
					strcpy(pswd,getpass("Enter new customer password: "));
					write(sock, &newUsr, sizeof(newUsr));
					write(sock, &pswd, sizeof(pswd));
					break;
				} else 
					printf("Username not found, Try another\n");
				}
				printf("Joint Account updated successfully!!\n");	
			} else {
				printf("Invalid choice made\n");
				return 3;
			}
			return 3;
		}
		case 4 : {
			char user[1024];
			int success;
			printf("Enter username: ");
			scanf("%s", user);
			write(sock, &user, sizeof(user));
			read(sock, &success, sizeof(success));
			if(success == 1)
			{
				char acnt_number[1024];
				int type1;
				printf("Username: %s\n", user);
				read(sock, &acnt_number, sizeof(acnt_number));
				printf("Account number: %s\n", acnt_number);
				read(sock, &type1, sizeof(type1));
				if(type1 == 0)
					printf("User type: Admin\n");
				else if(type1 == 1)
					printf("User type: Normal Customer\n");
				else if(type1 == 2)
					printf("User type: Joint Customer\n");
			}
			else
				printf("Username not found\n");
			return 4;
		}
		case 5 : {
			printf("Logged out successfully\n");
			return -1;
		}
	}
}
int customer_Actions(int sock, int opt)
{
	system("clear");
	write(sock, &opt, sizeof(opt));
	printf("%d", opt);
	double amount;
	int success;
	switch(opt)
	{
		case 1: {
			printf("Enter the amount to be deposited: \n");
			scanf("%lf", &amount);
			write(sock, &amount, sizeof(amount));
			read(sock, &success, sizeof(success));
			if(success == 1)
				printf("Amount deposited successfully\n");
			else
				printf("Sorry, there was an issue, please try again\n");
			return 1;
		}
		case 2: {
			printf("Enter the amount to be withdrawn: \n");
			scanf("%lf", &amount);
			write(sock, &amount, sizeof(amount));
			read(sock, &success, sizeof(success));
			if(success == 1)
				printf("Amount Withdrawal successful\n");
			else
				printf("Sorry, You don't have enough money to withdraw\n");
			return 2;
		}
		case 3: {
			char pswd[1024];
			strcpy(pswd,getpass("Enter new password: "));
			write(sock, &pswd, sizeof(pswd));
			read(sock, &success, sizeof(success));
			if(success == 1)
				printf("Password updated successfully\nLogin again to continue.................");
			else
				printf("Sorry, there was an issue, please try again\n");
			return -1;
		}
		case 4: {
			double balance;
			read(sock, &success, sizeof(success));
			read(sock, &balance, sizeof(balance));
			if(success == 1)
				printf("Your Available Balance is : %lf\n", balance);
			else
				printf("Sorry, there was an issue, please try again\n");
			return 4;
		}
		case 5: {
			char acnt_nmbr[1024];
			read(sock, &acnt_nmbr, sizeof(acnt_nmbr));
			read(sock, &success, sizeof(success));
			//printf("%s\n", acnt_nmbr);
			int fd = open("Transactions.txt", O_RDWR);
			struct Transaction trans;
			printf("Account number \t Type \t  Amount      Remaining amount\t\tDate\n\n");
			while(read(fd, &trans, sizeof(trans)) != 0)
			{
				//printf("%s\n", trans.account_number);
				if(strcmp(acnt_nmbr, trans.account_number) == 0)
					printf("%s\t%s\t%0.2lf\t\t%0.2lf\t%s", trans.account_number, trans.type, trans.amount, trans.remainingBalance, trans.transactionDate);
			} 
			close(fd);
			return 5;
		}
		case 6: {
			printf("Logged out successfully\n");
			return -1;
		}
	}
}

/*
int do_admin_action(int sock, int opt){
	switch(opt){
		case 1:{
			int tno;
			char tname[20];
			write(sock, &opt, sizeof(opt));
			printf("Enter Train Name: ");scanf("%s", tname);
			printf("Enter Train No. : ");scanf("%d", &tno);
			write(sock, &tname, sizeof(tname));
			write(sock, &tno, sizeof(tno));
			read(sock, &opt, sizeof(opt));
			if(opt == 1 ) printf("Train Added Successfully.\n");
			while(getchar()!='\n');
			getchar();
			return opt;
			break;
		}
		case 2:{
			int no_of_trains;
			write(sock, &opt, sizeof(opt));
			read(sock, &no_of_trains, sizeof(int));
			while(no_of_trains>0){
				int tid, tno;
				char tname[20];
				read(sock, &tid, sizeof(tid));
				read(sock, &tname, sizeof(tname));
				read(sock, &tno, sizeof(tno));
				if(!strcmp(tname, "deleted"));else
				printf("%d.\t%d\t%s\n", tid+1, tno, tname);
				no_of_trains--;
			}
			printf("Enter 0 to cancel.\nEnter the train ID to delete: "); scanf("%d", &no_of_trains);
			write(sock, &no_of_trains, sizeof(int));
			read(sock, &opt, sizeof(opt));

			if(opt == 0)printf("Operation aborted");	//cancel option chose
			else printf("Train deleted successfully\n");	//other than cancel

			while(getchar()!='\n');
			getchar();
			return opt;
			break;
		}
		case 3:{
			int no_of_trains,is_valid;
			write(sock, &opt, sizeof(opt));
			read(sock, &no_of_trains, sizeof(int));
			while(no_of_trains>0){
				int tid, tno;
				char tname[20];
				read(sock, &tid, sizeof(tid));
				read(sock, &tname, sizeof(tname));
				read(sock, &tno, sizeof(tno));
				if(!strcmp(tname, "deleted"));else
				printf("%d.\t%d\t%s\n", tid+1, tno, tname);
				no_of_trains--;
			}
			printf("Enter 0 to cancel.\nEnter the train ID to modify: "); scanf("%d", &no_of_trains);
			write(sock, &no_of_trains, sizeof(int));
			read(sock,&is_valid,sizeof(is_valid));
			if(is_valid == 0){
				printf("Operation aborted!\n");
			}
			else if(is_valid != 0){
				printf("Which parameter do you want to modify?\n1. Train Name\n2. Available Seats\n");
				printf("Your Choice: ");scanf("%d", &no_of_trains);
				write(sock, &no_of_trains, sizeof(int));
				// if(no_of_trains == 2 || no_of_trains == 3){
				if(no_of_trains == 2 ){

					read(sock, &no_of_trains, sizeof(int));
					printf("Current Value: %d\n", no_of_trains);
					printf("Enter Value: ");scanf("%d", &no_of_trains);
					write(sock, &no_of_trains, sizeof(int));
				}
				else if(no_of_trains == 1){
					char name[20];
					read(sock, &name, sizeof(name));
					printf("Current Value: %s\n", name);
					printf("Enter Value: ");scanf("%s", name);
					write(sock, &name, sizeof(name));
				}
				read(sock, &opt, sizeof(opt));
				if(opt == no_of_trains) printf("Train Data Modified Successfully\n"); //if modified
			}
			while(getchar()!='\n');
			getchar();
			return opt;
			break;
		}
		case 4:{
			write(sock, &opt, sizeof(opt));
			char pass[PASS_LENGTH],name[10];
			printf("Enter the name: ");scanf("%s", name);
			strcpy(pass, getpass("Enter a password for the ADMIN: "));
			write(sock, &name, sizeof(name));
			write(sock, &pass, sizeof(pass));
			read(sock, &opt, sizeof(opt));
			printf("The Account Number for this ADMIN is: %d\n", opt);
			read(sock, &opt, sizeof(opt));
			if(opt == 4)printf("Successfully created ADMIN\n");
			while(getchar()!='\n');
			getchar();
			return opt;
			break;
		}
		case 5: {
			int choice, users, id;
			write(sock, &opt, sizeof(opt));
			printf("What kind of account do you want to delete?\n");
			printf("1. Customer\n2. Agent\n3. Admin\n");
			printf("Your Choice: ");
			scanf("%d", &choice);
			while(choice <1 || choice >3){
				printf("Invalid Choice!\n");
				printf("1  to Customer\n2 to Agent\n3 to Admin\n");
				printf("Your Choice: ");
				scanf("%d", &choice);
			}
			write(sock, &choice, sizeof(choice));
			read(sock, &users, sizeof(users));
			while(users--){
				char name[10];
				read(sock, &id, sizeof(id));
				read(sock, &name, sizeof(name));
				if(strcmp(name, "deleted")!=0)
				printf("%d\t%s\n", id, name);
			}
			printf("Enter 0 to cancel\nEnter the ID to delete: ");scanf("%d", &id);
			write(sock, &id, sizeof(id));
			read(sock, &opt, sizeof(opt));
			if(opt == 0)
			{
				printf("Operation aborted!\n");
			}
			else{
				printf("Successfully deleted user\n");
			}
			while(getchar()!='\n');
			getchar();
			return opt;
		}
		case 6: {
			write(sock, &opt, sizeof(opt));
			read(sock, &opt, sizeof(opt));
			if(opt==6) printf("Logged out successfully.\n");
			while(getchar()!='\n');
			getchar();
			return -1;
			break;}
		case 7:
		{
			//search users
			write(sock, &opt, sizeof(opt));
			char name[20];
			int entries;
			printf("Enter the name of user to search\n");
			scanf("%s", name);

			write(sock, &name, sizeof(name));

			read(sock, &entries, sizeof(entries));

			if(entries<=0)
			printf("No user found\n");
			else
			while(entries--){
				int id, type;
				read(sock, &id, sizeof(id));
				read(sock, &type, sizeof(type));
				printf("ID: %d\tTYPE: %d\n", id, type);
			}

			read(sock, &opt, sizeof(opt));
			while(getchar()!='\n');
			getchar();
			return opt;
			break;
		}
		case 8:
		{
			write(sock, &opt, sizeof(opt));

			char name[20];
			int entries;
			printf("Enter the name of train to search: \n");
			scanf("%s", name);

			write(sock, &name, sizeof(name));

			read(sock, &entries, sizeof(entries));

			if(entries<=0)
			printf("No train found\n");
			else
			while(entries--)
			{
				int id, type, av_seats;
				read(sock, &id, sizeof(id));
				read(sock, &type, sizeof(type));
				read(sock, &av_seats, sizeof(av_seats));
				printf("ID: %d\tTRAIN NO: %d\tAVAILABLE SEATS: %d\n", id, type, av_seats);
			}

			read(sock, &opt, sizeof(opt));
			while(getchar()!='\n');
			getchar();
			return opt;
			break;
		}
		default: return -1;
	}
}

int do_action(int sock, int opt){
	write(sock, &opt, sizeof(opt));
	switch(opt){
		case 1:{
			//book a ticket
			int trains, trainid, trainavseats, trainno, required_seats,last_seatno_used,lastTrainId,valid = 0;
			int t_cnt;
			char trainname[20];
			read(sock, &trains, sizeof(trains));
			// struct train av_trains[trains];
			printf("ID\tT_NO\tAV_SEAT\tTRAIN NAME\n");

			// t_cnt=0;
			while(trains--){
				read(sock, &trainid, sizeof(trainid));
				read(sock, &trainno, sizeof(trainno));
				read(sock, &trainavseats, sizeof(trainavseats));
				read(sock, &trainname, sizeof(trainname));
				read(sock, &last_seatno_used, sizeof(last_seatno_used));


				if(strcmp(trainname, "deleted")!=0){
					printf("%d\t%d\t%d\t%s\n", trainid+1, trainno, trainavseats, trainname);
				}
			}
			lastTrainId = trainid;
			printf("Enter the train ID: "); scanf("%d", &trainid);

			write(sock, &trainid, sizeof(trainid));
			read(sock,&valid,sizeof(valid));

			if(valid){

				read(sock, &trainavseats, sizeof(trainavseats));
				printf("Available seats : %d\n",trainavseats);
				printf("Enter the number of seats: "); scanf("%d", &required_seats);
				if(trainavseats>=required_seats && required_seats>0)
					write(sock, &required_seats, sizeof(required_seats));
				else{
					required_seats = -1;
					write(sock, &required_seats, sizeof(required_seats));
				}
				read(sock, &opt, sizeof(opt));

				if(opt == 1) printf("Tickets booked successfully\n");
				else printf("Tickets were not booked. Please try again.\n");
			}
			else{
				printf("Operation Aborded\n");
			}
			printf("Press any key to continue...\n");
			while(getchar()!='\n');
			getchar();
			while(!getchar());
			return 1;
		}
		case 2:{
			//View your bookings
			view_booking(sock);
			read(sock, &opt, sizeof(opt));
			return 2;
		}
		case 3:{
			//update bookings
			int val;
			view_booking(sock);
			printf("Enter the booking id to be updated: "); scanf("%d", &val);
			write(sock, &val, sizeof(int));	//Booking ID
			printf("What information do you want to update:\n1 to Increase No of Seats\n2 to Decrease No of Seats\nYour Choice: ");
			scanf("%d", &val);
			write(sock, &val, sizeof(int));	//Increase or Decrease
			if(val == 1){
				printf("How many tickets do you want to increase: ");scanf("%d",&val);
				write(sock, &val, sizeof(int));	//No of Seats
			}else if(val == 2){
				printf("How many tickets do you want to decrease: ");scanf("%d",&val);
				write(sock, &val, sizeof(int));	//No of Seats
			}
			read(sock, &opt, sizeof(opt));
			if(opt == -2)
				printf("Operation failed. No more available seats\n");
			else printf("Operation succeded.\n");
			while(getchar()!='\n');
			getchar();
			return 3;
		}
		case 4:{
			//cancel booking
			int bid;
			view_booking(sock);
			printf("Enter the booking id to be Cancelled: "); scanf("%d", &bid);
			write(sock, &bid, sizeof(int));	//Booking ID
			read(sock,&opt,sizeof(int));
			if(opt){
				printf("Cancelled Succesfully\n");
			}
			else{
				printf("Cancellation failed!\n");
			}
			getchar();
			getchar();
			return 4;
		}
		case 5: {

			read(sock, &opt, sizeof(opt));
			if(opt == 5) printf("Logged out successfully.\n");
			while(getchar()!='\n');
			getchar();
			return -1;
			break;
		}
		default: return -1;
	}
}

void view_booking(int sock){
	int entries;
	read(sock, &entries, sizeof(int));
	if(entries == 0) printf("No records found.\n");
	else printf("Your recent %d bookings are :\n", entries);
	while(!getchar());
	while(entries--){
		int bid, bks_seat, bke_seat, cancelled,tno;
		// char trainname[20];
		read(sock,&bid, sizeof(bid));
		read(sock,&tno, sizeof(int));
		read(sock,&bks_seat, sizeof(int));
		read(sock,&bke_seat, sizeof(int));
		read(sock,&cancelled, sizeof(int));
		if(!cancelled)
		printf("BookingID: %d\t1st Ticket: %d\tLast Ticket: %d\tTrainNo. :%d\n", bid+1, bks_seat, bke_seat, tno);
	}
	printf("Press any key to continue...\n");
	while(getchar()!='\n');
	getchar();
}*/
