#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <iostream>
#include <assert.h>
#include "CalcFramer.hpp"
#include "CalcParser.hpp"
#include "sstream"
#include "string"
#include <fstream>
#include <iostream> 
#include<time.h>  
#include<sys/types.h>  
#include<sys/stat.h> 
#include<sys/sendfile.h>
#include <fcntl.h> 
#include<unistd.h>  
#include <cstring>
#include <errno.h>
#include <limits.h>
using namespace std;
using std::ofstream;
using std::ifstream;
#define RECVSIZE 2000
string serve = "Server: HTTPCandy\r\n";
string lasttime = "Last-Modified: ";
string Contenttype = "Content-Type: ";
string Contentlen = "Content-Length: ";
string version = "HTTP/1.1";
string servconn = "Connection: close";
void send200OK(int clntSocket, string time, bool status, string filetype, ssize_t filesize);
void send400CE(int clntSocket);
void send404NF(int clntSocket);
void send403FORBIDDEN(int clntSocket);
string getGMT(time_t t);
void HandleTCPClient(int clntSocket, string doc_root)
{
    CalcFramer calcframer;
    CalcParser calcparser;
    char echoBuffer[RECVSIZE];
    ssize_t recvMsgsize;
    ssize_t filesize;
    string url="";
    string path;
    string filetype;
    bool status = false;
    recvMsgsize = recv(clntSocket,echoBuffer,RECVSIZE,0);
    int hostnum = 0;
    int infosize = 0;


    if(recvMsgsize<0&&errno == EWOULDBLOCK){
        send400CE(clntSocket);
        close(clntSocket);
        return;
    }


    while(recvMsgsize>0){
        string msgorigin = echoBuffer;
        string msg = msgorigin.substr(0,recvMsgsize);
        calcframer.append(msg);
        string top = calcframer.topMessage();
        while(calcframer.hasMessage()){
            string submess = calcframer.topMessage(); 
            calcframer.popMessage();
            CalcInstruction instruction = calcparser.parse(submess);
            string info = instruction.info;
            if(info == "key-value"){        //get key line
                infosize = infosize+1;
                string key = instruction.key;
                string value = instruction.value;
                if(key=="Connection"&&value == "close"){
                    status = true;          //check connection infomation
                }
                if(key=="Host")
                    hostnum = hostnum+1;    //check host number
            }
            else if(info == "initial"){     // get initial line
                infosize = infosize+1;
                url = instruction.url;
                path = doc_root+url;
                filetype = instruction.filetype;   
                    
            }
            else if(info=="RESET"&&infosize>0){    //at the end of header
                string sendstring;
                struct stat stat_buf;
                const char *realPath = (char *)path.c_str();
                char filepath[200];
                char *res = realpath(realPath,filepath);
                string resovledpath = filepath;
                if(url==""||hostnum!=1){          //header not send host
                	send400CE(clntSocket);
                }
                if(!res||resovledpath.find(doc_root)==string::npos||stat(filepath, &stat_buf) != 0){  //escape root or can't find file
                    send404NF(clntSocket);

                }
                else if(!(stat_buf.st_mode & S_IROTH)){
                    send403FORBIDDEN(clntSocket);
                }
                else{
                    filesize = stat_buf.st_size;
                    time_t t= stat_buf.st_mtime;
                    string time = getGMT(t);
                    int fq = open(filepath, O_RDONLY); 
                    send200OK(clntSocket,time,status,filetype,filesize);
                    off_t offset = 0;
                    ssize_t len = 0;
                    while((len=sendfile(clntSocket,fq,&offset,RECVSIZE)>0)&&filesize-len>0){
                        filesize = filesize - len;
                    }
                    close(fq);
                    if(status){      //If received connection:close, close socket
                        close(clntSocket);
                        return;
                    }

                }
                url = "";                //after deal with one header, reset all infomation
                hostnum = 0;
                infosize = 0;
            }
            else if(info == "invalid"){   //got invalid header
                infosize = 0;
                send400CE(clntSocket);
                while(calcframer.hasMessage()){      //throw all remained lines in this header
                    string submess = calcframer.topMessage(); 
                    calcframer.popMessage();
                    CalcInstruction instruction = calcparser.parse(submess);
                    string info = instruction.info;
                    if(info=="key-value"){
                        if(instruction.key=="Connection"&&instruction.value=="close"){
                            close(clntSocket);
                        }
                    }
                    if(info=="RESET"){
                        break;
                    }
                }
            }
	   }
       recvMsgsize = recv(clntSocket,echoBuffer,RECVSIZE,0);
       if(recvMsgsize<0&&errno == EWOULDBLOCK){
            send400CE(clntSocket);
            close(clntSocket);
            return;
       }
    }  
    close(clntSocket);    /* Close client socket */
}


void send200OK(int clntSocket, string time, bool status, string filetype, ssize_t filesize){
    string responsecode = version+" 200 OK"+'\r'+'\n';
    filetype = Contenttype + filetype +'\r'+'\n';
    string Content_len = Contentlen + to_string(filesize);
    Content_len = Content_len+'\r'+'\n';
    string moditime = lasttime+time;
    string sendstring;
    if(!status){
        sendstring = responsecode +serve+ moditime+ filetype + Content_len+'\r'+'\n';
    }
    else{
        sendstring = responsecode +serve+ moditime+ filetype + Content_len+ servconn+ '\r'+'\n'+'\r'+'\n';
    }
    send(clntSocket,sendstring.data(),sendstring.length(),0);
}
void send400CE(int clntSocket){
    string responsecode = " 400 Client Error\r\n";
    string sendstring = version+responsecode;
    sendstring = sendstring+serve+'\r'+'\n';
    send(clntSocket,sendstring.data(),sendstring.length(),0);
    //return sendstring;
}
void send404NF(int clntSocket){
    string responsecode = " 404 NOT FOUND\r\n";
    string sendstring = version+responsecode;
    sendstring = sendstring+serve+'\r'+'\n';
    send(clntSocket,sendstring.data(),sendstring.length(),0);
    //return sendstring;
}
void send403FORBIDDEN(int clntSocket){
    string responsecode = " 403 Forbidden\r\n";
    string sendstring = version+responsecode;
    sendstring = sendstring+serve+'\r'+'\n';
    send(clntSocket,sendstring.data(),sendstring.length(),0);
    //return sendstring;
}
string getGMT(time_t t){
	struct tm * tmp = gmtime(&t);
	string weekday[]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
	int month = tmp->tm_mon+1;
	int day = tmp->tm_mday;
	int year = tmp->tm_year+1900;
	int wday = tmp->tm_wday;
	int hour = tmp->tm_hour;
	int min = tmp->tm_min;
	int sec = tmp->tm_sec;
	cout<<day<<endl;
	cout<<month<<endl;
	cout<<year<<endl;
	string gmttime = weekday[wday]+","+" "+to_string(day)+" "+to_string(month)+" "+to_string(year)+" "+to_string(hour)+":"+to_string(min)+":"+to_string(sec)+" GMT\r\n";
	return gmttime;
}


