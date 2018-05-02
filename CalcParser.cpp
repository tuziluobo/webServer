#include <assert.h>
#include "CalcParser.hpp"

CalcInstruction CalcParser::parse(std::string insstr)
{
	CalcInstruction ret;

	string opera = "";
    string url = "";
    string version = "";
    string key = "";
    string value = "";
    string info = "invalid";
    string filetype;
    uint i = 0;
    string endsign = "\r\n";
    if(insstr==endsign){
        info = "RESET";
        ret.info = info;
        return ret;
    }
            
    for(;i<insstr.size()&&insstr[i]!='\r';i++){   
        if(insstr[i]==' '){       //If first meet space, treat as initial line
            info = "initial";
            ret.info = info;
            break;
        }
        else if(insstr[i]==':'){    //If first meet : ,treat as key line
            info = "key-value";
            ret.info = "key-value";
            break;
        }
        opera = opera+insstr[i];
    }
    if(info == "invalid"){
        ret.info = info;
        return ret;
    }
    i++;
    if(info == "initial"){
        if(opera!="GET"){        //If operation is not GET
    		info = "invalid";
    		ret.info = info;
    		return ret;
    	}
        ret.key = opera;
        for(;i<insstr.size()&&insstr[i]!='\r';i++){
            if(insstr[i]==' '){
                break;
            }
            url = url+insstr[i];
        }
        if(url=="/"){
            filetype = "text/html";
            url = url+"index.html";
        }
        else if(url.size()==0){
            info = "invalid";
            ret.info = info;
            return ret;
        }
        else if(url[0]!='/'){
        	info = "invalid";
        	ret.info = info;
        	return ret;
        }
        else if(url.size()>=4&&url.substr(url.size()-4,4)==".jpg"){
            filetype = "image/jpeg";
        }
        else if(url.size()>=4&&url.substr(url.size()-4,4)==".png"){
            filetype = "image/png";
        }
        else if(url.size()>=4&&url.substr(url.size()-5,5)==".html"){
        	filetype = "text/html";
        } 
        ret.url = url;
        ret.filetype = filetype;
        i++;
        for(;i<insstr.size()&&insstr[i]!='\r';i++){
            version = version+insstr[i];
        }
        if(version!="HTTP/1.1"){
            info = "invalid";
            ret.info = info;
            return ret;
        }
        ret.version = version;
    }
   	else if(info == "key-value"){
        if(opera==""){                  //If key is null
        	ret.info = "invalid";
        	return ret;
        }
        while(insstr[i]==' ')
        	i++;
        for(;i<insstr.size()&&insstr[i]!='\r'&&insstr[i]!=' ';i++){
            value = value+insstr[i];
        }
        ret.key = opera;
        ret.value = value;
    }
    else{
        ret.info = "invalid";
    	ret.info = info;
    	return ret;
    }

    return ret;
}
