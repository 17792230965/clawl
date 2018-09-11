#include<string>
#include<curl/curl.h>
#include<unistd.h>
#include<boost/regex.hpp>
#include<boost/algorithm/string.hpp>


size_t function(char *ptr, size_t size, size_t nmemb, void* output)
{
    std::string* html = (std::string*) output;
    html->append(ptr, size*nmemb);
    return size*nmemb;
}

bool passage_Info(const std::string& input, std::string* output)
{
    const char* url = input.c_str();
    CURL* curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output);
    
    CURLcode ret = curl_easy_perform(curl);
    if(ret != CURLE_OK){
         
        curl_easy_cleanup(curl);
        return false;
    }
    curl_easy_cleanup(curl);
    return true;
}

void getMainPassage(const std::string& output, std::vector<std::string>* url_list)
{
    boost::regex reg("/read/\\S+html");
    std::string::const_iterator cur = output.begin();
    std::string::const_iterator end = output.end();
    boost::smatch result;
    while(boost::regex_search(cur, end, result, reg)){
        url_list->push_back("http://www.shengxu6.com" + result[0]);
        cur = result[0].second;
    }

}

void get_body(const std::string& output, std::string* content)
{
    std::string beg_flag = "<div class=\"panel-body content-body content-ext\">";
    size_t beg = output.find(beg_flag);
    if(beg == std::string::npos){
        fprintf(stderr, "找不到标记！\n");
        return;
    }
    beg += beg_flag.size();
    std::string end_flag = "<script>_drgd200();</script>";
    size_t end = output.find(end_flag);
    if(end == std::string::npos){
        fprintf(stderr, "找不到标记！\n");
        return;
    }
    if(beg > end){
        fprintf(stderr, "标记有问题！\n");
        return;
    }
    *content = output.substr(beg, end-beg);
    boost::algorithm::replace_all(*content, "&nbsp;", " ");
    boost::algorithm::replace_all(*content, "<br />", "\n");
    return;
}



void Test1()
{
    std::string output;
    passage_Info("http://www.shengxu6.com/book/2967.html", &output);
    std::vector<std::string> url_list;
    getMainPassage(output, &url_list);
    for(int i = 0; i < url_list.size(); ++i){
        //printf("%s\n", url_list[i].c_str());
        passage_Info(url_list[i], &output);
        std::string content;
        get_body(output, &content);
        printf("%s\n", content.c_str());

    }
}

int main()
{
    Test1();
    return 0;
}
