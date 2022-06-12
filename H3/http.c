#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IPSTR "124.71.221.251"
//#define IPSTR "172.16.10.189"
#define PORT 80
#define BUFSIZE 1024

int get_boad_serial(uint8_t *serial)
{
		FILE *pfile = NULL;
		/*get disk serial as board serial*/
		pfile = popen("cat /proc/cpuinfo | grep Serial | awk {'print     $3'}", "r");
		if(NULL == pfile)
				return -1;
		fgets(serial,50,pfile);

		return 0;
}


int http_post_data(int counter,int power)
{
		int sockfd, ret, i, h;
		struct sockaddr_in servaddr;
		char str1[4096], str2[4096], buf[BUFSIZE], *str;
		socklen_t len;
		fd_set   t_set1;
		struct timeval  tv;
		uint8_t serial[64];

		if(get_boad_serial(serial)){
		//		printf("get boad serial fail!\n");
				return -1;
		}else{
				printf("boad serial:%s\n",serial);
		}

		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
				printf("创建网络连接失败,本线程即将终止---socket error!\n");
				exit(0);
		};

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(PORT);
		if (inet_pton(AF_INET, IPSTR, &servaddr.sin_addr) <= 0 ){
				printf("创建网络连接失败,本线程即将终止--inet_pton error!\n");
				exit(0);
		};

		if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
				printf("连接到服务器失败,connect error!\n");
				exit(0);
		}
		printf("与远端建立了连接\n");

		//发送数据
		memset(str2, 0, 4096);
		//strcat(str2, "{\"counter\":\"183\",\"power\":\"78\"}");
		sprintf(str2,"{\"board_serial\":\"%s\",\"counter\":\"%d\",\"power\":\"%d\"}",serial,counter,power);
		str=(char *)malloc(128);
		len = strlen(str2);
		sprintf(str, "%d", len);

		memset(str1, 0, 4096);
		strcat(str1, "POST /data HTTP/1.1\r\n");
		strcat(str1, "Host: 124.71.221.251:80\r\n");
		strcat(str1, "Accept: */*\r\n");
		strcat(str1, "Content-Type: application/json\r\n");
		strcat(str1, "Content-Length: ");
		strcat(str1, str);
		strcat(str1, "\r\n\r\n");

		strcat(str1, str2);
		strcat(str1, "\r\n\r\n");
		printf("%s\n",str1);

		ret = write(sockfd,str1,strlen(str1));
		if (ret < 0) {
				printf("发送失败！错误代码是%d，错误信息是'%s'\n",errno, strerror(errno));
				exit(0);
		}else{
				printf("消息发送成功，共发送了%d个字节！\n\n", ret);
		}

		FD_ZERO(&t_set1);
		FD_SET(sockfd, &t_set1);

		while(1){
				//sleep(2);
				tv.tv_sec= 20;
				tv.tv_usec= 0;
				h= 0;
				h= select(sockfd +1, &t_set1, NULL, NULL, &tv);

				if (h == 0) 
				{
						printf("timeout\n");
						continue;
				}
				if (h < 0) {
						close(sockfd);
						printf("在读取数据报文时SELECT检测到异常，该异常导致线程终止！\n");
						return -1;
				};

				if (h > 0){
						memset(buf, 0, 4096);
						i= read(sockfd, buf, 4095);
						if (i==0){
								close(sockfd);
								printf("读取数据报文时发现远端关闭，该线程终止！\n");
								return -1;
						}

						printf("%s\n", buf);
				}
		}
		close(sockfd);


		return 0;
}
