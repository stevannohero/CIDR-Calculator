#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define SERVER "hmif.cf"
#define PORT 9999

void recv_until(int sock, char *buf, char *data) {
   bzero((char *) buf, 512);
   int i = 0;
   while (strstr(buf, data) == NULL) {
	   read(sock, buf+i, 1);
	   i++;
   }
}

void getValidSubnet(char *subnet, char *host) {
   bzero((char *) subnet, 512);
   const char cc = '.';
   char *ret = strchr(host, cc);
   strncpy(subnet, host, (ret-host+1));
   strcat(subnet,"0.0.0/8\n");
}

int getMask(char *address) {
   int i = 0;
   while (address[i] != '/') {
      i++;
   }
   i++;
   char c = address[i]; i++;
   if (address[i] == '\0') {
      return (c - '0');
   } else {
      int x = (c - '0');
      int y = (address[i] - '0');
      return (x*10 + y);
   }
}

void countHosts(char *subnet, char *host) {
   bzero((char *) host, 512);
   
   int y = 32 - getMask(host);
   long long n = 1;

   for (int i=0; i<y; i++){
      n = n*2;
   }
   char temp[10];
   sprintf(temp, "%lld\n", n);
   strcpy(host, temp);
}

int isSubnetValid(char *subnet, char *host) {
   unsigned long bin_mask;
   int c = getMask(subnet);
   char temp_mask[512];
   bzero((char *) temp_mask, 512);    

   if (c == 0) {
      bin_mask = 0;
   } else {
      bin_mask = (0xFFFFFFFF << (32-c) & 0xFFFFFFFF);
   }

   sprintf(temp_mask, "%lu.%lu.%lu.%lu", bin_mask >> 24, (bin_mask >> 16) & 0xFF, (bin_mask >> 8) & 0xFF, bin_mask & 0xFF);

   char temp_subnet[512];
   bzero((char *) temp_subnet, 512);

   int i = 0;
   while (subnet[i] != '/') {
      temp_subnet[i] = subnet[i];
      i++;
   }

   struct sockaddr_in hostaddr, maskaddr, subaddr;

   // string to address in binary
   inet_pton(AF_INET, host, &(hostaddr.sin_addr)); 
   inet_pton(AF_INET, temp_mask, &(maskaddr.sin_addr));
   inet_pton(AF_INET, temp_subnet, &(subaddr.sin_addr)); 
   
   // logical AND
   hostaddr.sin_addr.s_addr = (hostaddr.sin_addr.s_addr & maskaddr.sin_addr.s_addr);
   subaddr.sin_addr.s_addr = (subaddr.sin_addr.s_addr & maskaddr.sin_addr.s_addr);

   char _subnet[512], _host[512];
   bzero((char *) _subnet, 512);
   bzero((char *) _host, 512);

   // address in binary to string
   inet_ntop(AF_INET, &(subaddr.sin_addr), _subnet, INET_ADDRSTRLEN);
   inet_ntop(AF_INET, &(hostaddr.sin_addr), _host, INET_ADDRSTRLEN);

   // compare subnet and host
   int tempc = c;
   int z = 0;
   int count = 0;
   while (tempc > 0){
      if (_subnet[z] == _host[z]){
         count++;
      }
      z++;
      tempc--;
   }
   if (count == c) {
      return 1;
   } else {
      return 0;
   }
}

int main(int argc, char *argv[]) {
   int sockfd, portno, n, i;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   
   char buffer[512];
   
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
	
   server = gethostbyname(SERVER);
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(PORT);
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }

   recv_until(sockfd, buffer, "NIM: ");
   printf("NIM: ");
   scanf("%s", buffer);
   write(sockfd, buffer, strlen(buffer));

   recv_until(sockfd, buffer, "NIM: ");
   printf("Verify NIM: ");
   scanf("%s", buffer);
   write(sockfd, buffer, strlen(buffer));

   recv_until(sockfd, buffer, "\n");
   printf("%s", buffer);

   char host[512];
   char subnet[512];
   
   // Phase 1
   for (i = 0; i < 100; i++) {
      recv_until(sockfd, buffer, "Host: ");
      recv_until(sockfd, host, "\n");
      host[strlen(host)-1] = '\0';
      recv_until(sockfd, buffer, "Subnet: ");
      getValidSubnet(subnet, host);
      write(sockfd, subnet, strlen(subnet));
   }
   recv_until(sockfd, buffer, "\n");
   printf("%s", buffer);
   
   // Phase 2
   for (i = 0; i < 100; i++) {
      recv_until(sockfd, buffer, "Subnet: ");
      recv_until(sockfd, subnet, "\n");
      subnet[strlen(subnet)-1] = '\0';
      recv_until(sockfd, buffer, "Number of Hosts: ");
      countHosts(subnet, host);
      write(sockfd, host, strlen(host));
   }
   recv_until(sockfd, buffer, "\n");
   printf("%s", buffer);
   
   // Phase 3
   for (i = 0; i < 100; i++) {
      recv_until(sockfd, buffer, "Subnet: ");
      recv_until(sockfd, subnet, "\n");
      subnet[strlen(subnet)-1] = '\0';
      recv_until(sockfd, buffer, "Host: ");
      recv_until(sockfd, host, "\n");
      host[strlen(host)-1] = '\0';
      if (isSubnetValid(subnet, host) == 1) {
         write(sockfd, "T\n", 2); 
      } else {
         write(sockfd, "F\n", 2); 
      }
   }
   recv_until(sockfd, buffer, "\n");
   printf("%s", buffer);
   
   close(sockfd);
   return 0;
}
