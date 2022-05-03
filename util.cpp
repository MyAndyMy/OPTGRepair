//
//  util.cpp
//  OPTGRSRTR5.0
//
//  Created by Andy on 2022/3/14.
//

#include "util.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "limits.h"
#include <unordered_map>
#include "string.h"
#include "stdlib.h"
#include <algorithm>
/*--------------------以下是预设权值--------------------*/
double lambda = 2.3;
double w_conflict=50000;
double w_adjacency=-1;
double w_association=0;
/*--------------------以上是预设权值--------------------*/

/*--------------------以下打印tuple信息--------------------*/
void Tuple::print_all_info(){
    cout<<"attrs_value's size:"<<attrs_value.size();
    if(attrs_value.size()>0){
        cout<<";";
        for(int i=0;i<attrs_value.size();++i){
            cout<<attrs_value[i]<<" ";
        }
        cout<<endl;
    }
    else{
        cout<<endl;
    }
    cout<<"tuple_id:"<<tuple_id<<endl;
    cout<<"s_time's size:"<<s_time.size();
    if(s_time.size()>0){
        cout<<";";
        cout<<s_time<<endl;
    }
    else{
        cout<<endl;
    }
    cout<<"e_time's size:"<<e_time.size();
    if(e_time.size()>0){
        cout<<";";
        cout<<e_time<<endl;
    }
    else{
        cout<<endl;
    }
    cout<<"tuple_weight:"<<tuple_weight<<endl;
    cout<<"break_point's size:"<<break_point.size();
    if(break_point.size()>0){
        cout<<";";
        for(auto it:break_point){
            cout<<it.first<<" ";
        }
        cout<<endl;
    }
    else{
        cout<<endl;
    }
    cout<<"in_conflict:"<<in_conflict<<endl;
    cout<<"x:"<<x<<endl;
    cout<<"xr:"<<xr<<endl;
}
/*--------------------以上打印tuple信息--------------------*/

/*--------------------以下打印FD信息--------------------*/
void FD::print_all_info(){
    cout<<"lhs'size:"<<lhs.size();
    if(lhs.size()>0){
        cout<<";";
        for(int i=0;i<lhs.size();++i){
            cout<<lhs[i]<<" ";
        }
        cout<<endl;
    }
    else{
        cout<<endl;
    }
    cout<<"rhs'size:"<<rhs.size();
    if(rhs.size()>0){
        cout<<";";
        for(int i=0;i<rhs.size();++i){
            cout<<rhs[i]<<" ";
        }
        cout<<endl;
    }
    else{
        cout<<endl;
    }
    cout<<"fd_id:"<<fd_id<<endl;
    cout<<"fd_weight:"<<fd_weight<<endl;
}
/*--------------------以上打印FD信息--------------------*/

/*--------------------以下打印relationship信息--------------------*/
void Relationship::print_all_info(){
    cout<<"rel's first tuple_id:"<<rel.first.tuple_id;
    cout<<";";
    cout<<"rel's second tuple_id:"<<rel.second.tuple_id<<endl;
    cout<<"rel_id:"<<rel_id<<endl;
    cout<<"rel_weight:"<<rel_weight<<endl;
    cout<<"rel_type:"<<rel_type<<endl;
    cout<<"y:"<<y<<endl;
    cout<<"yr:"<<yr<<endl;
}
void Generator::print_every_rel(vector<Relationship> relationship){
    for(int i=0;i<relationship.size();++i){
        relationship[i].print_all_info();
    }
}
/*--------------------以上打印relationship信息--------------------*/

/*--------------------以下去掉首尾特殊字符--------------------*/
string Trim(string& str){
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
    return str;
}
/*--------------------以上去掉首尾特殊字符--------------------*/

/*--------------------以下加载数据--------------------*/
Generator::Generator(char* data_file_path, char* data_type){
    if(!strcmp(data_type,"r")||!strcmp(data_type,"t")){
        load_source_data(data_file_path, data_type);
    }
    else{
        std::cout<<"data type error!"<<endl;
    }
}
void Generator::load_source_data(char* data_file_path, char* data_type){
    ifstream fin(data_file_path);
    if(!fin){
        cout<<data_file_path<<" doesn't exist"<<endl;
    }
    else{
        cout<<data_file_path<<" exists"<<endl;
    }
    if(!fin.is_open()){
        cout<<data_file_path<<" open fail"<<endl;
    }
    else{
        cout<<data_file_path<<" open success"<<endl;
    }
    string line;
    getline(fin, line);
    istringstream sin(line);
    vector<string> objs;
    string obj;
    while (getline(sin, obj, ',')){
        attrs_name.push_back(Trim(obj));
    }
    size_t t_id=source_tuple.size();//记录元组的tuple_id
    while (getline(fin, line)){
        istringstream sin(line);
        vector<string> fields;//所有属性值
        string field;//属性值
        while(getline(sin, field, ','))
            fields.push_back(field);
        Tuple temp;
        temp.tuple_id = t_id;
        for(int i = 0; i<attrs_name.size(); i++){
            if(i<attrs_name.size()-2){
                temp.attrs_value.push_back(Trim(fields[i]));//把一个元组的每个属性值存入element数组
            }
            else{
                if(!strcmp(data_type, "t")){
                    Trim(fields[i]);
                    auto it=fields[i].begin()+6;
                    if(*it=='/'){
                        it--;
                        fields[i].insert(it,'0');
                    }
                    it=fields[i].end()-2;
                    if(*it=='/'){
                        it++;
                        fields[i].insert(it,'0');
                    }
                    it=fields[i].begin();
                    while(it!=fields[i].end()){
                        if(*it=='/'){
                            fields[i].erase(it);
                            it=fields[i].begin();
                        }
                        ++it;
                    }
                    temp.attrs_value.push_back(Trim(fields[i]));
                    if(i==attrs_name.size()-2){
                        temp.s_time=Trim(fields[i]);
                    }
                    else if(i==attrs_name.size()-1){
                        temp.e_time=Trim(fields[i]);
                    }
                }
                else if(!strcmp(data_type, "r")){
                    //cout<<fields[i]<<endl;
                    //cout<<Trim(fields[i])<<endl;
                    temp.attrs_value.push_back(Trim(fields[i]));
                    if(i==attrs_name.size()-2){
                        temp.s_time="-";
                    }
                    else if(i==attrs_name.size()-1){
                        temp.e_time="-";
                    }
                }
            }
        }
        source_tuple.push_back(temp);
        t_id++;
    }
}
void Generator::load_fd_file(char* fd_file_path){
    ifstream fin(fd_file_path);
    if(!fin){
        cout<<fd_file_path<<" doesn't exist"<<endl;
    }
    else{
        cout<<fd_file_path<<" exists"<<endl;
    }
    if(!fin.is_open()){
        cout<<fd_file_path<<" open fail"<<endl;
    }
    else{
        cout<<fd_file_path<<" open success"<<endl;
    }
    string line;
    size_t fd_id=fun_denp.size();
    while (getline(fin, line)){
        istringstream sin(line);
        vector<string> objs1;//一个FDstring删除了”-“，比如，由“A，B-C，D”变成了“A，B“和”C，D”
        string obj1;
        while (getline(sin, obj1, '-'))
            objs1.push_back(obj1);
        string ll = Trim(objs1[0]), rr = Trim(objs1[1]);
        FD temp_fd;
        temp_fd.fd_id=fd_id;
        fd_id++;
        istringstream sin1(ll);
        vector<string> objs2;//“A"，"B“
        string obj2;
        while(getline(sin1, obj2, ','))
            objs2.push_back(obj2);
        for(int i = 0; i<objs2.size(); i++){
            string temp_str=Trim(objs2[i]);
            int temp_int=temp_str[0]-'0';
            for(int i=1;i<temp_str.size();++i){
                temp_int=temp_str[1]-'0'+temp_int*10;
            }
            temp_fd.lhs.push_back(temp_int);
        }
            
        istringstream sin2(rr);
        vector<string> objs3;//“C"，"D“
        string obj3;
        while(getline(sin2, obj3, ','))
            objs3.push_back(obj3);
        for(int i = 0; i<objs3.size(); i++){
            string temp_str=Trim(objs3[i]);
            int temp_int=temp_str[0]-'0';
            for(int i=1;i<temp_str.size();++i){
                temp_int=temp_str[1]-'0'+temp_int*10;
            }
            temp_fd.rhs.push_back(temp_int);
        }
        fun_denp.push_back(temp_fd);
    }
}
namespace std{
    template <class Type>
    Type stringToNum(const string& str){
        istringstream iss(str);
        Type num;
        iss >> num;
        return num;
    }
}
void Generator::load_tuple_weight(char* data_weight_file_path,vector<Tuple> &tuple){
    //
    if(!strcmp(data_weight_file_path,"")){
        for(int i=0;i<tuple.size();++i){
            tuple[i].tuple_weight=lambda;
        }
    }
    else{
        ifstream fin(data_weight_file_path);
        if(!fin){
            cout<<data_weight_file_path<<" doesn't exist"<<endl;
        }
        else{
            cout<<data_weight_file_path<<" exists"<<endl;
        }
        if(!fin.is_open()){
            cout<<data_weight_file_path<<" open fail"<<endl;
        }
        else{
            cout<<data_weight_file_path<<" open success"<<endl;
        }
        string line;
        //getline(fin, line);
        //istringstream sin(line);
        //vector<string> objs;
        //string obj;
        //while (getline(sin, obj, ',')){}
        //cout<<"in function:"<<endl;
        size_t w_id=0;//记录元组的tuple_id
        while (getline(fin, line)){
            //istringstream sin(line);
            //vector<string> fields;//所有属性值
            //string field;//属性值
            //while(getline(sin, field, ','))
                //fields.push_back(field);
            //cout<<"in function:"<<line<<endl;
            source_tuple[w_id].tuple_weight=std::stringToNum<double>(Trim(line));
            w_id++;
        }
    }
}
void Generator::load_fd_weight(char* fd_weight_file_path,vector<FD> &fds){
    if(!strcmp(fd_weight_file_path,"")){
        for(int i=0;i<fds.size();++i){
            fds[i].fd_weight=w_conflict;
        }
    }
    else{
        ifstream fin(fd_weight_file_path);
        if(!fin){
            cout<<fd_weight_file_path<<" doesn't exist"<<endl;
        }
        else{
            cout<<fd_weight_file_path<<" exists"<<endl;
        }
        if(!fin.is_open()){
            cout<<fd_weight_file_path<<" open fail"<<endl;
        }
        else{
            cout<<fd_weight_file_path<<" open success"<<endl;
        }
        string line;
        size_t fd_id=0;
        while (getline(fin, line)){
            istringstream sin(line);
            vector<string> objs1;
            string obj1;
            while (getline(sin, obj1, ',')){
                objs1.push_back(obj1);
            }
            fun_denp[fd_id].fd_weight=std::stringToNum<double>(Trim(objs1[0]));
            ++fd_id;
        }
    }
}
/*--------------------以上加载数据--------------------*/

/*--------------------以下打印属性名称--------------------*/
void Generator::print_attrs_name(){
    cout<<"一共"<<attrs_name.size()<<"个属性: ";
    for(int i=0;i<attrs_name.size()-1; i++){
        cout<<attrs_name[i]<<",";
    }
    cout<<attrs_name[attrs_name.size()-1]<<endl;
}
/*--------------------以上打印属性名称--------------------*/

/*--------------------以下打印元组数量--------------------*/
size_t Generator::number_of_tuples(vector<Tuple> &tuple){
    return tuple.size();
}
/*--------------------以上打印元组数量--------------------*/

/*--------------------以下打印元组属性--------------------*/
void Generator::print_every_tuple(vector<Tuple> &tuple){
    for(int i=0;i<tuple.size();++i){
        cout<<"第"<<i<<"个: ";
        int j=0;
        for(j=0;j<attrs_name.size()-1;++j){
            cout<<tuple[i].attrs_value[j]<<", ";
        }
        cout<<tuple[i].attrs_value[j]<<endl;
    }
}
/*--------------------以上打印元组属性--------------------*/

/*--------------------以下打印FD数量--------------------*/
size_t Generator::number_of_fds(){
    return fun_denp.size();
}
/*--------------------以上打印FD数量--------------------*/

/*--------------------以下打印每个FD--------------------*/
void Generator::print_every_fds(){
    for(int i=0;i<fun_denp.size();++i){
        cout<<"第"<<fun_denp[i].fd_id<<"个FD: ";
        for(int j=0;j<fun_denp[i].lhs.size();++j){
            cout<<fun_denp[i].lhs[j]<<" ";
        }
        cout<<"-> ";
        for(int j=0;j<fun_denp[i].rhs.size();++j){
            cout<<fun_denp[i].rhs[j]<<" ";
        }
        cout<<" "<<"weight:"<<fun_denp[i].fd_weight;
        cout<<endl;
    }
}
/*--------------------以上打印每个FD--------------------*/

/*--------------------以下构建repair中元组的冲突对--------------------*/
void Generator::construct_left_conflict(vector<Tuple> &tuple, vector<Relationship> &relationship){
    unordered_map<pair<int,int>,int> visited;//标记两个元组是否为第一次冲突
    int con_id=(int)relationship.size();
    for(int p=0;p<fun_denp.size();++p){
        for (int i = 0; i<tuple.size(); i++){
            for (int j = i + 1; j<tuple.size(); j++){
                int lhs_equal=0;
                int rhs_equal=0;
                for (int k = 0; k<fun_denp[p].lhs.size(); k++){
                    if (tuple[i].attrs_value[fun_denp[p].lhs[k]] != tuple[j].attrs_value[fun_denp[p].lhs[k]]){
                        lhs_equal = 1;//lhs不相等
                        break;
                    }
                }
                if (lhs_equal == 0){//lhs都相等了，再去看rhs
                    for (int k = 0; k<fun_denp[p].rhs.size(); k++){
                        if (tuple[i].attrs_value[fun_denp[p].rhs[k]] != tuple[j].attrs_value[fun_denp[p].rhs[k]]){
                            rhs_equal = 1;//rhs不相等
                            break;
                        }
                    }
                    if (rhs_equal == 1){//rhs不相等，有冲突了，下面看看时间区间
                        if(visited[make_pair(i, j)]==0){//如果tuple i和tuple j第一次冲突，（i，j）被记录到relationship中
                            Relationship temp_r(tuple[i],tuple[j]);
                            temp_r.rel_type=1;//conflict
                            tuple[i].in_conflict++;
                            tuple[j].in_conflict++;
                            temp_r.rel_id=con_id;
                            for_weight_soft[make_pair(tuple[i].tuple_id, tuple[j].tuple_id)]=fun_denp[p].fd_weight;
                            for_weight_soft[make_pair(tuple[i].tuple_id, tuple[j].tuple_id)]=min(for_weight_soft[make_pair(tuple[i].tuple_id, tuple[j].tuple_id)],w_conflict);
                            relationship.push_back(temp_r);
                            con_id++;
                        }
                        else{//如果tuple i和tuple j不是第一次冲突，（i，j）已经被记录relationship中了
                            for_weight_soft[make_pair(tuple[i].tuple_id, tuple[j].tuple_id)]+=fun_denp[p].fd_weight;
                            for_weight_soft[make_pair(tuple[i].tuple_id, tuple[j].tuple_id)]=min(for_weight_soft[make_pair(tuple[i].tuple_id, tuple[j].tuple_id)],w_conflict);
                        }
                        visited[make_pair(i, j)]++;
                    }
                }
            }
        }
    }
    for(int i=0;i<relationship.size();++i){
        relationship[i].rel_weight=for_weight_soft[make_pair(relationship[i].rel.first.tuple_id, relationship[i].rel.second.tuple_id)];
    }
}
/*--------------------以上构建repair中元组的冲突对--------------------*/

/*--------------------以下计算repair中留下的元组冲突对加权和--------------------*/
double Generator::weighted_left_conflicts(vector<Relationship> &relationship){
    double res=0.0;
    for(int i=0;i<relationship.size();++i){
        res+=relationship[i].rel_weight;
    }
    return res;
}
/*--------------------以上计算repair中留下的元组冲突对加权和--------------------*/

/*--------------------以下构建元组的冲突对--------------------*/
void Generator::construct_conflict(vector<Tuple> &tuple,vector<Relationship> &relationship,char* con_type){
    unordered_map<pair<int,int>,int> visited;//标记两个元组是否为第一次冲突
    int con_id=(int)relationship.size();
    for(int p=0;p<fun_denp.size();++p){
        for (int i = 0; i<tuple.size(); i++){
            for (int j = i + 1; j<tuple.size(); j++){
                int lhs_equal=0;
                int rhs_equal=0;
                int time_ovlp=0;
                for (int k = 0; k<fun_denp[p].lhs.size(); k++){
                    if (tuple[i].attrs_value[fun_denp[p].lhs[k]] != tuple[j].attrs_value[fun_denp[p].lhs[k]]){
                        lhs_equal = 1;//lhs不相等
                        break;
                    }
                }
                if (lhs_equal == 0){//lhs都相等了，再去看rhs
                    for (int k = 0; k<fun_denp[p].rhs.size(); k++){
                        if (tuple[i].attrs_value[fun_denp[p].rhs[k]] != tuple[j].attrs_value[fun_denp[p].rhs[k]]){
                            rhs_equal = 1;//rhs不相等
                            break;
                        }
                    }
                    if (rhs_equal == 1){//rhs不相等，有冲突了，下面看看时间区间
                        //如果是temporal data
                        if(!strcmp(con_type, "t")){
                            if (min(tuple[i].e_time, tuple[j].e_time)>max(tuple[i].s_time, tuple[j].s_time))
                                time_ovlp=1;;//时间区间有重叠
                            if (time_ovlp == 1){//时间区间有重叠了
                                //cout<<"tupleiiii"<<i<<" and tuple"<<j<<" 关于FD"<<p<<" 冲突"<<endl;
                                //cout<<tuple[i].attrs_value[0]<<" "<<tuple[i].attrs_value[1]<<" "<<tuple[i].attrs_value[2]<<" "<<tuple[i].attrs_value[3]<<" "<<tuple[i].s_time<<"-"<<tuple[i].e_time<<endl;
                                //cout<<tuple[j].attrs_value[0]<<" "<<tuple[j].attrs_value[1]<<" "<<tuple[j].attrs_value[2]<<" "<<tuple[j].attrs_value[3]<<" "<<tuple[j].s_time<<"-"<<tuple[j].e_time<<endl;
                                if(visited[make_pair(i, j)]==0){//如果tuple i和tuple j第一次冲突，（i，j）被记录到relationship中
                                    if(tuple[i].s_time<=tuple[j].s_time){//保证rel.first.s_time<=rel.second.s_time
                                        Relationship temp_r(tuple[i],tuple[j]);
                                        temp_r.rel_type=1;//conflict
                                        tuple[i].in_conflict++;
                                        tuple[j].in_conflict++;
                                        temp_r.rel_id=con_id;
                                        for_weight[make_pair(i, j)]=fun_denp[p].fd_weight;
                                        for_weight[make_pair(i, j)]=min(for_weight[make_pair(i, j)],w_conflict);
                                        relationship.push_back(temp_r);
                                        visited[make_pair(i, j)]++;
                                    }
                                    else{
                                        Relationship temp_r(tuple[j],tuple[i]);
                                        temp_r.rel_type=1;//conflict
                                        tuple[i].in_conflict++;
                                        tuple[j].in_conflict++;
                                        temp_r.rel_id=con_id;
                                        for_weight[make_pair(j, i)]=fun_denp[p].fd_weight;
                                        for_weight[make_pair(j, i)]=min(for_weight[make_pair(j, i)],w_conflict);
                                        relationship.push_back(temp_r);
                                        visited[make_pair(j, i)]++;
                                    }
                                    con_id++;
                                }
                                else{//如果tuple i和tuple j不是第一次冲突，（i，j）已经被记录relationship中了
                                    if(tuple[i].s_time<=tuple[j].s_time){
                                        for_weight[make_pair(i, j)]+=fun_denp[p].fd_weight;
                                        for_weight[make_pair(i, j)]=min(for_weight[make_pair(i, j)],w_conflict);
                                        visited[make_pair(i, j)]++;
                                    }
                                    else{
                                        for_weight[make_pair(j, i)]+=fun_denp[p].fd_weight;
                                        for_weight[make_pair(j, i)]=min(for_weight[make_pair(j, i)],w_conflict);
                                        visited[make_pair(j, i)]++;
                                    }
                                }
                            }
                            else{//时间区间没有重叠，不构成冲突
                                //cout<<"tuple"<<i<<" and tuple"<<j<<" 关于FD"<<p<<" 不冲突"<<endl;
                                //cout<<tuple[i].attrs_value[0]<<" "<<tuple[i].attrs_value[1]<<" "<<tuple[i].attrs_value[2]<<" "<<tuple[i].attrs_value[3]<<" "<<tuple[i].s_time<<"-"<<tuple[i].e_time<<endl;
                                //cout<<tuple[j].attrs_value[0]<<" "<<tuple[j].attrs_value[1]<<" "<<tuple[j].attrs_value[2]<<" "<<tuple[j].attrs_value[3]<<" "<<tuple[j].s_time<<"-"<<tuple[j].e_time<<endl;
                            }
                        }
                        else if(!strcmp(con_type, "r")){
                            //如果是relational data
                            int element_id=(int)U.size();
                            Element temp_ele(tuple[i],tuple[j],fun_denp[p]);
                            temp_ele.ele_id=element_id;
                            U.push_back(temp_ele);
                            if(visited[make_pair(i, j)]==0){//如果tuple i和tuple j第一次冲突，（i，j）被记录到relationship中
                                Relationship temp_r(tuple[i],tuple[j]);
                                temp_r.rel_type=1;//conflict
                                tuple[i].in_conflict++;
                                tuple[j].in_conflict++;
                                temp_r.rel_id=con_id;
                                for_weight[make_pair(i, j)]=fun_denp[p].fd_weight;
                                for_weight[make_pair(i, j)]=min(for_weight[make_pair(i, j)],w_conflict);
                                relationship.push_back(temp_r);
                                con_id++;
                            }
                                //如果tuple i和tuple j不是第一次冲突，（i，j）已经被记录relationship中了
                            else{
                                for_weight[make_pair(i, j)]+=fun_denp[p].fd_weight;
                                for_weight[make_pair(i, j)]=min(for_weight[make_pair(i, j)],w_conflict);
                                //加权值
                                //for(int h=0;h<relationship.size();++h){
                                //if(relationship[h].rel.first.tuple_id==i&&relationship[h].rel.second.tuple_id==j){
                                //relationship[h].rel_weight=min(w_conflict,relationship[h].rel_weight+fun_denp//[p].fd_weight);
                                //}
                                //}
                            }
                            visited[make_pair(i, j)]++;
                        }
                        else{
                            cout<<"con_type error!"<<endl;
                        }
                    }
                }
            }
        }
    }
    for(int i=0;i<relationship.size();++i){
        if(relationship[i].rel_type==1){
            relationship[i].rel_weight=for_weight[make_pair(relationship[i].rel.first.tuple_id, relationship[i].rel.second.tuple_id)];
        }
    }
}
/*--------------------以上构建元组的冲突对--------------------*/

/*--------------------以下打印参与冲突的元组个数--------------------*/
size_t Generator::number_of_tuples_in_conflict(vector<Tuple> &tuple){
    int res=0;
    for(int i=0;i<tuple.size();++i){
        if(tuple[i].in_conflict>0){
            res++;
        }
    }
    return res;
}
/*--------------------以上打印参与冲突的元组个数--------------------*/

/*--------------------以下打印冲突个数--------------------*/
size_t Generator::number_of_conflicts(vector<Relationship> relationship){
    size_t res=0;
    for(int i=0;i<relationship.size();++i){
        if(relationship[i].rel_type==1){
            res++;
        }
    }
    return res;
}
/*--------------------以上打印冲突个数--------------------*/

/*--------------------以下normalization1----------------*/
void Generator::normalization1(){//对每个冲突对暴力分段，for循环每个冲突对
    for(int i=0;i<relationship_S.size()&&relationship_S[i].rel_type==1;++i){
        if(relationship_S[i].rel.first.s_time==relationship_S[i].rel.second.s_time){//如果起始时间相等
            if(relationship_S[i].rel.first.e_time==relationship_S[i].rel.second.e_time){}//且终止时间相等
            else if(relationship_S[i].rel.first.e_time<relationship_S[i].rel.second.e_time){
                //
                size_t jj=relationship_S[i].rel.second.tuple_id;
                source_tuple[jj].break_point[relationship_S[i].rel.first.s_time]++;
                source_tuple[jj].break_point[relationship_S[i].rel.first.e_time]++;
                source_tuple[jj].break_point[relationship_S[i].rel.second.e_time]++;
            }
            else if(relationship_S[i].rel.first.e_time>relationship_S[i].rel.second.e_time){
                //
                size_t ii=relationship_S[i].rel.first.tuple_id;
                source_tuple[ii].break_point[relationship_S[i].rel.first.s_time]++;
                source_tuple[ii].break_point[relationship_S[i].rel.second.e_time]++;
                source_tuple[ii].break_point[relationship_S[i].rel.first.e_time]++;
            }
        }
        else if(relationship_S[i].rel.first.s_time<relationship_S[i].rel.second.s_time){//如果起始时间不相等
            if(relationship_S[i].rel.first.e_time==relationship_S[i].rel.second.e_time){
                size_t ii=relationship_S[i].rel.first.tuple_id;
                source_tuple[ii].break_point[relationship_S[i].rel.first.s_time]++;
                source_tuple[ii].break_point[relationship_S[i].rel.second.s_time]++;
                source_tuple[ii].break_point[relationship_S[i].rel.first.e_time]++;
            }
            else if(relationship_S[i].rel.first.e_time<relationship_S[i].rel.second.e_time){
                size_t ii=relationship_S[i].rel.first.tuple_id;
                source_tuple[ii].break_point[relationship_S[i].rel.first.s_time]++;
                source_tuple[ii].break_point[relationship_S[i].rel.second.s_time]++;
                source_tuple[ii].break_point[relationship_S[i].rel.first.e_time]++;
                size_t jj=relationship_S[i].rel.second.tuple_id;
                source_tuple[jj].break_point[relationship_S[i].rel.second.s_time]++;
                source_tuple[jj].break_point[relationship_S[i].rel.first.e_time]++;
                source_tuple[jj].break_point[relationship_S[i].rel.second.e_time]++;
            }
            else if(relationship_S[i].rel.first.e_time>relationship_S[i].rel.second.e_time){//包含关系
                size_t ii=relationship_S[i].rel.first.tuple_id;
                source_tuple[ii].break_point[relationship_S[i].rel.first.s_time]++;
                source_tuple[ii].break_point[relationship_S[i].rel.second.s_time]++;
                source_tuple[ii].break_point[relationship_S[i].rel.second.e_time]++;
                source_tuple[ii].break_point[relationship_S[i].rel.first.e_time]++;
            }
        }
    }
    int t_id=(int)normalized_tuple.size();
    for(int i=0;i<source_tuple.size();++i){
        if(source_tuple[i].break_point.size()==0){
            Tuple nor_temp;
            nor_temp.attrs_value=source_tuple[i].attrs_value;
            size_t num_attr=nor_temp.attrs_value.size();
            nor_temp.s_time=source_tuple[i].attrs_value[num_attr-2];
            nor_temp.e_time=source_tuple[i].attrs_value[num_attr-1];
            nor_temp.tuple_id=t_id;
            nor_temp.tuple_weight=source_tuple[i].tuple_weight;
            nor_temp.in_conflict=source_tuple[i].in_conflict;
            nor_temp.source_id=source_tuple[i].tuple_id;
            t_id++;
            normalized_tuple.push_back(nor_temp);
        }
        else{
            auto it = source_tuple[i].break_point.begin();
            for(int j=0;j<source_tuple[i].break_point.size()-1;j++){
                Tuple nor_temp;
                nor_temp.attrs_value=source_tuple[i].attrs_value;
                nor_temp.tuple_id=t_id;
                nor_temp.tuple_weight=source_tuple[i].tuple_weight;
                nor_temp.in_conflict=source_tuple[i].in_conflict;
                nor_temp.source_id=source_tuple[i].tuple_id;
                size_t num_attr=nor_temp.attrs_value.size();
                nor_temp.s_time=it->first;
                nor_temp.attrs_value[num_attr-2]=it->first;
                it++;
                nor_temp.e_time=it->first;
                nor_temp.attrs_value[num_attr-1]=it->first;
                normalized_tuple.push_back(nor_temp);
                t_id++;
            }
        }
    }
}
/*--------------------以上normalization1----------------*/

/*--------------------以下为元组构建相邻对----------------*/
void Generator::construct_adjacency(vector<Tuple> &tuple, vector<Relationship> &relationship){
    size_t adj_id=relationship.size();
    if(tuple.size()==0){}
    else{
    //cout<<"relationship.size() is"<<relationship.size()<<endl;
    for(int i=0;i<tuple.size()-1;++i){
        if(tuple[i].e_time==tuple[i+1].s_time&&tuple[i].source_id==tuple[i+1].source_id){
            Relationship temp_rel(tuple[i],tuple[i+1]);
            temp_rel.rel_weight=w_adjacency;
            temp_rel.rel_id=adj_id;
            adj_id++;
            temp_rel.rel_type=2;
            relationship.push_back(temp_rel);
        }
    }
    }
}
/*--------------------以上为元组构建相邻对----------------*/

/*--------------------以下打印相邻个数----------------*/
size_t Generator::number_of_adjacency(vector<Relationship> relationship){
    size_t res=0;
    for(int i=0;i<relationship.size();++i){
        if(relationship[i].rel_type==2){
            res++;
        }
    }
    return res;
}
/*--------------------以上打印相邻个数----------------*/

/*--------------------以下打印元组关联对个数----------------*/
size_t Generator::number_of_association(vector<Relationship> relationship){
    size_t res=0;
    for(int i=0;i<relationship.size();++i){
        if(relationship[i].rel_type==3){
            res++;
        }
    }
    return res;
}
/*--------------------以上打印元组关联对个数----------------*/

/*--------------------以下S-repair----------------*/
void Generator::lp_solver_S_repair(vector<Tuple> &tuple, vector<Relationship> &relationship){
    int tuples = (int)tuple.size();
    int conflicts = (int)number_of_conflicts(relationship);
initialize:
    glp_prob *lp;
    lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MIN);
auxiliary_variables_rows:
    glp_add_rows(lp, conflicts);//row是辅助变量
    for (int i = 1; i <= conflicts; i++) glp_set_row_bnds(lp, i, GLP_LO, 1.0, 0.0);
variables_columns:
    glp_add_cols(lp, tuples);//column是原有变量
    for (int i = 1; i <= tuples; i++) glp_set_col_bnds(lp, i, GLP_DB, 0.0, 1.0);//每个变量范围为[0，1]
to_minimize:
    for (int i = 1; i <= tuples; i++) glp_set_obj_coef(lp, i, tuple[i-1].tuple_weight);//x
constrant_matrix:
    long long size = conflicts*tuples;
    if(size>500000000){
        cout<<"size is too large!"<<endl;
        exit(0);
    }
    int *ia = new int[size + 1];
    int *ja = new int[size + 1];
    double *ar = new double[size + 1];
    /*xi+xj-yij>=0 conflict*/
    for (int i = 1; i <= conflicts; i++) {
        for (int j = 1; j <= tuples; j++) {//原有变量个数
            int k = (i - 1)*tuples + j;
            ia[k] = i;//第i个辅助变量
            ja[k] = j;//第j个原有变量
            if(j<=tuples){
                if(relationship[i-1].rel.first.tuple_id==tuple[j-1].tuple_id||relationship[i-1].rel.second.tuple_id==tuple[j-1].tuple_id) ar[k] = 1.0;
                else ar[k] = 0.0;
            }
            else ar[k] = 0.0;
        }
    }
    glp_load_matrix(lp, size, ia, ja, ar);
calculate:
    glp_simplex(lp, NULL);
output:
    for (int i = 1; i <= tuples; i++){
        tuple[i-1].x=glp_get_col_prim(lp, i);
    }
cleanup:
    delete[] ia;
    delete[] ja;
    delete[] ar;
    glp_delete_prob(lp);
}
/*--------------------以上S-reoair----------------*/
/*--------------------以下舍入分数最优解----------------*/
void Generator::rounding(vector<Tuple> &tuple, vector<Relationship> &relationship){
    for(size_t i=0;i<tuple.size();++i){
        if(tuple[i].x==0.5){
            //cout<<"x有0.5"<<endl;
            tuple[i].xr=1;
        }
        else tuple[i].xr=tuple[i].x;
    }
    for(size_t i=0;i<relationship.size();++i){
        if(relationship[i].y==0.5){
            //cout<<"y有0.5"<<endl;
            relationship[i].yr=1;
        }
        else relationship[i].yr=relationship[i].y;
    }
}
/*--------------------以上舍入分数最优解----------------*/

/*--------------------以下计算G-repair分数最优解----------------*/
double Generator::calculate_opt_f_G(vector<Tuple> &tuple,vector<Relationship> &relationship){
    double res=0.0;
    for(size_t i=0;i<tuple.size();++i){
        res+=tuple[i].tuple_weight*tuple[i].x;
    }
    for(size_t j=0;j<relationship.size();++j){
        res+=relationship[j].rel_weight*(1-relationship[j].y);
    }
    return res;
}
/*--------------------以上计算G-repair分数最优解----------------*/

/*--------------------以下计算G-repair整数近似解----------------*/
double Generator::calculate_opt_r_G(vector<Tuple> &tuple,vector<Relationship> &relationship){
    double res=0.0;
    for(size_t i=0;i<tuple.size();++i){
        res+=tuple[i].tuple_weight*tuple[i].xr;
    }
    for(size_t j=0;j<relationship.size();++j){
        res+=relationship[j].rel_weight*(1-relationship[j].yr);
    }
    return res;
}
/*--------------------以上计算G-repair整数近似解----------------*/

/*--------------------以下计算S-repair分数最优解----------------*/
double Generator::calculate_opt_f_S(vector<Tuple> &tuple){
    double res=0.0;
    for(size_t i=0;i<tuple.size();++i){
        res+=tuple[i].tuple_weight*tuple[i].x;
    }
    return res;
}
/*--------------------以上计算S-repair分数优解----------------*/

/*--------------------以下计算S-repair整数近似解----------------*/
double Generator::calculate_opt_r_S(vector<Tuple> &tuple){
    double res=0.0;
    for(size_t i=0;i<tuple.size();++i){
        res+=tuple[i].tuple_weight*tuple[i].xr;
    }
    return res;
}
/*--------------------以上计算S-repair整数近似解----------------*/


/*--------------------以下G-repair----------------*/
void Generator::lp_solver_G_repair(vector<Tuple> &tuple, vector<Relationship> &relationship)
{
    int tuples = (int)tuple.size();
    int conflicts = (int)number_of_conflicts(relationship);
    int adjacency = (int)number_of_adjacency(relationship);

    /*---------------常规操作---------------*/
initialize:
    glp_prob *lp;
    lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MIN);
auxiliary_variables_rows:
    glp_add_rows(lp, conflicts+2*adjacency);//row是辅助变量
    for (int i = 1; i <= conflicts + 2*adjacency; i++) glp_set_row_bnds(lp, i, GLP_LO, 0.0, 0.0);
variables_columns:
    glp_add_cols(lp, tuples + conflicts + adjacency);//column是原有变量
    for (int i = 1; i <= tuples + conflicts + adjacency; i++) glp_set_col_bnds(lp, i, GLP_DB, 0.0, 1.0);//每个变量范围为[0，1]
to_minimize:
    for (int i = 1; i <= tuples; i++){
        glp_set_obj_coef(lp, i, tuple[i-1].tuple_weight);//x
    }
    for (int i = tuples + 1; i <= tuples + adjacency; i++){
        glp_set_obj_coef(lp, i, -relationship_G[i-tuples-1].rel_weight);//y(adjacency)
        //cout<<"adj"<<relationship_G[i-tuples-1].rel_weight<<endl;
    }
    for (int i = tuples + adjacency + 1; i <= tuples + conflicts + adjacency; i++){
        glp_set_obj_coef(lp, i, -relationship_G[i-tuples-1].rel_weight);
        //glp_set_obj_coef(lp, i, -for_weight[make_pair(relationship_G[i-tuples-1].rel.first.tuple_id, relationship_G[i-tuples-1].rel.second.tuple_id)]);//y(conflict)
        //cout<<"con"<<for_weight[make_pair(relationship_G[i-tuples-1].rel.first.tuple_id, relationship_G[i-tuples-1].rel.second.tuple_id)]<<endl;
    }
constrant_matrix:
    long long size = (conflicts +  2 * adjacency)*(tuples + conflicts + adjacency);
    if(size>500000000){
        cout<<"size is too large!"<<endl;
        exit(0);
    }
    int *ia = new int[size + 1]();
    int *ja = new int[size + 1]();
    double *ar = new double[size + 1]();
    //yij-xi>=0
    for (int i = 1; i <= adjacency; i++) {//c是相邻对儿数，对于每对儿相邻元组，都有一个辅助变量
        for (int j = 1; j <= tuples + conflicts + adjacency; j++) {//a + b + c是原有变量个数
            int k = (i - 1)*(tuples + conflicts + adjacency) + j;
            ia[k] = i;//第i个辅助变量
            ja[k] = j;//第j个原有变量
            
            if(j<=tuples){
                if(relationship[i-1].rel.first.tuple_id==tuple[j-1].tuple_id)
                    ar[k] = -1.0;
                else if(relationship[i-1].rel.second.tuple_id==tuple[j-1].tuple_id)
                    ar[k] = 0.0;
                else ar[k] = 0.0;
            }
            else if(j>tuples&&j<=tuples+adjacency){
                if(j-tuples==i) ar[k]=1.0;
                else ar[k]=0.0;
            }
            else ar[k] = 0.0;
        }
    }
    //yij-xj>=0
    for (int i = adjacency+1; i <= 2*adjacency; i++) {//c是相邻对儿数，对于每对儿相邻元组，都有一个辅助变量
        for (int j = 1; j <= tuples + conflicts + adjacency; j++) {//a + b + c是原有变量个数
            int k = (i - 1)*(tuples + conflicts + adjacency) + j;
            ia[k] = i;//第i个辅助变量
            ja[k] = j;//第j个原有变量
            
            if(j<=tuples){
                if(relationship[i-adjacency-1].rel.first.tuple_id==tuple[j-1].tuple_id)
                    ar[k] = 0.0;
                else if(relationship[i-adjacency-1].rel.second.tuple_id==tuple[j-1].tuple_id)
                    ar[k] = -1.0;
                else ar[k] = 0.0;
            }
            else if(j>tuples&&j<=tuples+adjacency){
                if(j-tuples==i-adjacency) ar[k]=1.0;
                else ar[k]=0.0;
            }
            else ar[k] = 0.0;
        }
    }
    //xi+xj-yij>=0 conflict
    for (int i = 2*adjacency+1; i <= conflicts+2*adjacency; i++) {
        
        for (int j = 1; j <= tuples + conflicts + adjacency; j++) {//原有变量个数
            
            int k = (i - 1)*(tuples + conflicts + adjacency) + j;
            
            ia[k] = i;//第i个辅助变量
            
            ja[k] = j;//第j个原有变量
            //ar[k] = 0.0;
            if(j<=tuples){
                if(relationship[i-adjacency-1].rel.first.tuple_id==tuple[j-1].tuple_id||relationship[i-adjacency-1].rel.second.tuple_id==tuple[j-1].tuple_id) ar[k] = 1.0;
                else ar[k] = 0.0;
                
            }
            else if(j>tuples&&j<=tuples + adjacency){
                ar[k] = 0.0;
                
            }
            else{
                if(j-tuples-adjacency==i-2*adjacency) ar[k] = -1.0;
                else ar[k] = 0.0;
                
            }
        }
    }
    glp_load_matrix(lp, size, ia, ja, ar);
calculate:
    glp_simplex(lp, NULL);
output:
    for (int i = 1; i <= (tuples + conflicts + adjacency); i++)
    {
        if (i <= tuples) {
            tuple[i-1].x=glp_get_col_prim(lp, i);
            //cout<<"x is111:"<<glp_get_col_prim(lp, i)<<endl;
        }
        else if (i > tuples&&i <= tuples + adjacency) {
            relationship[i-tuples-1].y=glp_get_col_prim(lp, i);
        }
        else if (i > tuples + adjacency&&i <= tuples + conflicts + adjacency) {
            //cout<<"qqqqqq"<<i-tuples-conflicts-1<<endl;
            relationship[i-tuples-1].y=glp_get_col_prim(lp, i);
            //cout<<glp_get_col_prim(lp, i)<<endl;
            //if(relationship[i-tuples-1].rel_type==2) cout<<"vec"<<endl;
        }
    }
cleanup:
    delete[] ia;
    delete[] ja;
    delete[] ar;
    glp_delete_prob(lp);
}
/*--------------------以上G-repair----------------*/

/*--------------------以下验证G-repair解的有效性----------------*/
bool Generator::effectiveness_of_solution_G(vector<Relationship> &relationship){
    for(int i=0;i<relationship.size();++i){
        if(relationship[i].rel_type==1){
            if(relationship[i].y!=min(relationship_G[i].rel.first.x+relationship_G[i].rel.second.x,1.0)){
                return false;
            }
        }
        else if(relationship[i].rel_type==2){
            double t=max(relationship_G[i].rel.first.x,relationship_G[i].rel.second.x);
            if(relationship[i].y!=t){
                return false;
            }
        }
    }
    return true;
}
/*--------------------以上验证G-repair解的有效性----------------*/

/*--------------------以下计算解的时间跨度----------------*/
int Generator::span(vector<Tuple> &tuple){
    map<string,int> time;
    for(int i=0;i<tuple.size();++i){
        if(tuple[i].xr==0){
            string temp=tuple[i].s_time;
            while(temp<tuple[i].e_time){
                time[temp]++;
                if(temp[3]<'9'){
                    temp[3]=temp[3]+1;
                }
                else if(temp[3]=='9'){
                    temp[3]='0';
                    if(temp[2]<'9'){
                        temp[2]=temp[2]+1;
                    }
                    else if(temp[2]=='9'){
                        temp[2]='0';
                        if(temp[1]<9){
                            temp[1]=temp[1]+1;
                        }
                        else if(temp[1]=='9'){
                            temp[1]='0';
                            temp[0]=temp[0]+1;
                        }
                    }
                }
            }
        }
    }
    return (int)time.size();
}
/*--------------------以上计算解的时间跨度----------------*/

/*--------------------以下计算set cover问题的sets----------------*/
void Generator::construct_sets(){
    for(int i=0;i<source_tuple.size();++i){
        Set1 set1(source_tuple[i]);
        set1.set_type=1;
        set1.set_weight=source_tuple[i].tuple_weight;
        //cout<<"tuple_weight:"<<source_tuple[i].tuple_weight<<endl;
        for(int j=0;j<U.size();++j){
            if(U[j].t1.tuple_id==source_tuple[i].tuple_id||U[j].t2.tuple_id==source_tuple[i].tuple_id){
                set1.element.push_back(U[j]);
            }
        }
        S1.push_back(set1);
    }
    /*for(int i=0;i<U.size();++i){
        Set2 set2(U[i].t1,U[i].t2,U[i].fd);
        for(int j=0;j<U.size();++j){
            if(U[j].t1.tuple_id==U[i].t1.tuple_id&&U[j].t2.tuple_id==U[i].t2.tuple_id){
                set2.element.push_back(U[j]);
            }
        }
        S2.push_back(set2);
    }*/
    //上下代码等价的
    for(int i=0;i<U.size();++i){
        Set2 set2(U[i].t1,U[i].t2,U[i].fd);
        set2.set_type=2;
        set2.element.push_back(U[i]);
        set2.set_weight=U[i].fd.fd_weight;
        //cout<<"U[i].fd.fd_weight:"<<U[i].fd.fd_weight<<endl;
        S2.push_back(set2);
    }
}
/*--------------------以上计算set cover问题的sets----------------*/

/*--------------------以下打印set cover问题的输入U和S----------------*/
void Generator::print_all_info_sc(){
    cout<<"一共有"<<U.size()<<"个元素"<<endl;
    cout<<"一共有"<<S1.size()+S2.size()<<"个集合"<<endl;
    for(int i=0;i<U.size();++i){
        cout<<"element number:"<<U[i].ele_id<<" ";
        cout<<"由元组 "<<U[i].t1.tuple_id<<"和元组 "<<U[i].t2.tuple_id<<"以及函数依赖 "<<U[i].fd.fd_id<<"组成"<<endl;
    }
    for(int i=0;i<S1.size();++i){
        cout<<"set number:"<<i<<" ";
        cout<<"由element ";
        for(int j=0;j<S1[i].element.size()-1;++j){
            cout<<S1[i].element[j].ele_id<<"和element";
        }
        cout<<S1[i].element[S1[i].element.size()-1].ele_id<<"组成, ";
        cout<<"its weight is : "<<S1[i].set_weight<<endl;
    }
    for(int i=0;i<S2.size();++i){
        cout<<"set number:"<<i+S1.size()<<" ";
        cout<<"由element ";
        for(int j=0;j<S2[i].element.size()-1;++j){
            cout<<S2[i].element[j].ele_id<<"和element";
        }
        cout<<S2[i].element[S2[i].element.size()-1].ele_id<<"组成, ";
        cout<<"its weight is : "<<S2[i].set_weight<<endl;
    }
}
/*--------------------以上打印set cover问题的输入U和S----------------*/

/*--------------------以下将elements和sets构建成vector<int>和vector<vector<int>>----------------*/
void Generator::construct_sc(){
    for(int i=0;i<U.size();++i){
        UU.push_back(U[i].ele_id);
        UU_lp.push_back(U[i].ele_id);
    }
    vector<int> temp;
    for(int i=0;i<S1.size();++i){
        for(int j=0;j<S1[i].element.size();++j){
            temp.push_back(S1[i].element[j].ele_id);
            //cout<<"constructing set1ing:"<<S1[i].element[j].ele_id<<endl;
        }
        C.push_back(temp);
        C_lp.push_back(temp);
        temp.clear();
        weight_bef.push_back(make_pair(S1[i].set_weight, 0));
        weight_bef_lp.push_back(make_pair(S1[i].set_weight, 0));
        weight_aft.push_back(make_pair(S1[i].set_weight, 0));
        //weight.push_back(S1[i].set_weight);
    }
    for(int i=0;i<S2.size();++i){
        for(int j=0;j<S2[i].element.size();++j){
            temp.push_back(S2[i].element[j].ele_id);
        }
        C.push_back(temp);
        C_lp.push_back(temp);
        temp.clear();
        weight_bef.push_back(make_pair(S2[i].set_weight, 1));
        weight_bef_lp.push_back(make_pair(S2[i].set_weight, 1));
        weight_aft.push_back(make_pair(S2[i].set_weight, 1));
        //weight.push_back(S2[i].set_weight);
    }
}
/*--------------------以上将elements和sets构建成vector<int>和vector<vector<int>>----------------*/

/*--------------------以下打印set cover问题的vector<int>输入UU和vector<vector<int>>输入C----------------*/
void Generator::print_all_info_sc_int(){
    cout<<"一共有"<<UU.size()<<"个元素"<<endl;
    cout<<"一共有"<<C.size()<<"个集合"<<endl;
    cout<<"element = {";
    for(int i=0;i<UU.size()-1;++i){
        cout<<UU[i]<<", ";
    }
    cout<<UU[UU.size()-1]<<"}"<<endl;
    for(int i=0;i<C.size();++i){
        cout<<"set "<<i<<" = {";
        for(int j=0;j<C[i].size()-1;++j){
            cout<<C[i][j]<<", ";
        }
        cout<<C[i][C[i].size()-1]<<"}"<<", ";
    }
}
/*--------------------以上打印set cover问题的vector<int>输入UU和vector<vector<int>>输入C----------------*/

/*--------------------以下是set cover问题的贪心算法----------------*/
vector<int> Generator::v_difference(vector<int> &va, vector<int> &vb) {
    vector<int> vc(va.size() + vb.size());
    vector<int>::iterator it;
    std::sort(va.begin(), va.end());
    std::sort(vb.begin(), vb.end());
    it = std::set_difference(va.begin(), va.end(), vb.begin(), vb.end(), vc.begin());
    vc.resize(it - vc.begin());
    return vc;
}

vector<int> Generator::v_intersection(vector<int> &va, vector<int> &vb) {
    vector<int> vc(va.size() + vb.size());
    vector<int>::iterator it;
    std::sort(va.begin(), va.end());
    std::sort(vb.begin(), vb.end());
    it = std::set_intersection(va.begin(), va.end(), vb.begin(), vb.end(), vc.begin());
    vc.resize(it - vc.begin());
    return vc;
}
int Generator::max_union(vector<int> &U, vector<vector<int> > &F) {
    double minsize = INT_MAX, index = 0;
    /*cout<<"U=";
    for(int i=0;i<U.size();++i) cout<<U[i]<<", ";
    cout<<endl;
    cout<<"F=";
    for(int i=0;i<F.size();++i){
        for(int j=0;j<F[i].size();++j){
            cout<<F[i][j]<<", ";
        }
        cout<<endl;
    }*/
    for (int i = 0; i < F.size(); i++) {
        vector<int> uf = v_intersection(U, F[i]);//返回交集
        //cout<<"i="<<i<<endl;
        if(uf.size()!=0){
            //cout<<"交集的size:"<<uf.size()<<endl;
            size_t len=uf.size();
            if(len!=0){
                double t = 0.0;
                /*if(i<S1.size()){
                    t=(double)S1[i].set_weight/len;
                    cout<<"set的weight:"<<S1[i].set_weight<<endl;
                    cout<<"S1[i].set_weight/len:"<<t<<endl;
                    cout<<"minsize:"<<minsize<<endl;
                }
                if(i>=S1.size()&&i<S1.size()+S2.size()){
                    t=(double)S2[i-S1.size()].set_weight/len;
                    cout<<"set的weight:"<<S2[i-S1.size()].set_weight/len<<endl;
                    cout<<"S2[i-S1.size()].set_weight/len:"<<t<<endl;
                    cout<<"minsize:"<<minsize<<endl;
                }*/
                t=(double)weight_aft[i].first/len;
                //cout<<"set的weight:"<<weight_aft[i].first<<endl;
                //cout<<"eight_bef[i].first/len:"<<t<<endl;
                //cout<<"minsize:"<<minsize<<endl;
                if (t < minsize) {
                    //cout<<"该换minsize了"<<endl;
                    minsize = t;
                    index = i;
                    //cout<<"minsize:"<<minsize<<endl;
                }
            }
        }
    }
    //cout<<"max_union:"<<index<<endl;
    return index;
}
vector<int> Generator::greedy_sc(vector<int> &X, vector<vector<int> > &F) {
    vector<int> U = X;
    vector<int> C;//F[C[i]]是解
    while (U.size() != 0) {
        // std::cout << U.size() << std::endl;
        int S = max_union(U, F);//F[S]和U的交集最大
        U = v_difference(U, F[S]);//U把F[S]里的元素都剪减掉了
        /*cout<<"v_difference:";
        for(int i=0;i<U.size();++i){
            cout<<U[i]<<", ";
        }
        cout<<endl;*/
        
        auto it=F.begin();
        for(int i=0;i<S;++i){
            it++;
        }
        F.erase(it);
        greedy_opt=greedy_opt+weight_aft[S].first;
        auto itt=weight_aft.begin();
        for(int i=0;i<S;++i){
            itt++;
        }
        weight_aft.erase(itt);
        /*cout<<"把F输出看看:";
        for(int i=0;i<F.size();++i){
            for(int j=0;j<F[i].size();++j){
                cout<<F[i][j]<<", ";
            }
            cout<<endl;
        }
        cout<<endl;*/

        C.push_back(S);//F[S]以int S的方式记录在C中
        
    }
    
    return C;
}
/*--------------------以上是set cover问题的贪心算法----------------*/

/*--------------------以下构建留下的normalized元组集合----------------*/
void Generator::construct_left_tuple(vector<Tuple> &tuple){
    for(int i=0;i<tuple.size();++i){
        if(tuple[i].xr==0){
            left_tuple.push_back(tuple[i]);
        }
    }
}
/*--------------------以上构建留下的normalized元组集合----------------*/

/*--------------------以下对left_tuple做coalescing----------------*/
void Generator::coalescing(vector<Tuple> &tuple){
    if(tuple.size()==0){}
    else{
    vector<Tuple>::iterator last=tuple.begin();
    while(last!=tuple.end()-1){
        vector<Tuple>::iterator next=last+1;
        if(last->e_time==next->s_time&&last->source_id==next->source_id){
            last->e_time=next->e_time;
            last->attrs_value[last->attrs_value.size()-1]=last->e_time;
            tuple.erase(next);
        }
        else{
            last++;
        }
    }
    }
}
/*--------------------以上对left_tuple做coalescing----------------*/

/*--------------------以下计算贪心算法整数近似解----------------*/
double Generator::calculate_opt_r_Greedy(){
    double res=0.0;
    for(int i=0;i<source_tuple.size();++i){
        if(source_tuple[i].xr==1){
            res+=source_tuple[i].tuple_weight;
        }
    }
    construct_left_tuple(source_tuple);
    construct_left_conflict(left_tuple, relationship_S);
    res=res+weighted_left_conflicts(relationship_S);
    return res;
}
/*--------------------以上计算贪心算法整数近似解----------------*/

/*--------------------以下将所有source_tuple的x和xr置0----------------*/
void Generator::clear_source_tuple(){
    for(int i=0;i<source_tuple.size();++i){
        source_tuple[i].x=0;
        source_tuple[i].xr=0;
    }
}
/*--------------------以上将所有source_tuple的x和xr置0----------------*/

/*--------------------以下输出data的错误率----------------*/
double Generator::print_error_percentage(vector<Tuple> &tuple){
    size_t tuple_nums=tuple.size();
    size_t error_nums=number_of_tuples_in_conflict(tuple);
    return (double)error_nums/tuple_nums;
}
/*--------------------以上输出data的错误率----------------*/

/*--------------------以下pollute数据----------------*/
vector<int> is_dirty;
void Generator::pollute_flight(vector<Tuple> &tuple,int size,double err_rate,string result_path,int nums_FD){
    FILE *fp;
    FILE *fp_weight;
    size_t nums=attrs_name.size();
    //char result_file_name[]="/Users/andy/Documents/Data/Flight/flight";
    //char weight_file_name[]="/Users/andy/Documents/Data/Flight/flight";
    string postfix=".csv";
    string weight_postfix=".txt";
    string jmy="jmy";
    //char middle[6];
    //char back[10];
    //char w[]="weight";
    string middle=to_string(size);
    string back=to_string(err_rate);
    //sprintf(back,"%f",err_rate);
    //sprintf(middle,"%d",size);
    string result_tuple_path=result_path+middle+jmy+back+postfix;
    //strcat(result_file_name,middle);
    //strcat(result_file_name,jmy);
    //strcat(result_file_name,back);
    //strcat(result_file_name,postfix);
    cout<<"result_tuple_path:"<<result_tuple_path<<endl;
    string result_weight_path=result_path+middle+jmy+back+"weight"+weight_postfix;
    cout<<"result_weight_path"<<result_weight_path<<endl;
    
    fp = fopen(result_tuple_path.data(), "w");
    fp_weight = fopen(result_weight_path.data(), "w");
    if(fp==NULL) cout<<"大失败！"<<endl;
    else cout<<"大成功！"<<endl;
    vector<double> xiaoshu={0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
    int turn=0;
    int n=1;
    for(n=1;n<nums-1;++n){
        //cout<<attrs_name[n].c_str()<<endl;
        fprintf(fp,"%s ,",attrs_name[n].c_str());
    }
    cout<<attrs_name[n].c_str()<<endl;
    fprintf(fp,"%s\n",attrs_name[n].c_str());
    //fclose (fp);
    int err_tuples=size*err_rate;
    err_tuples=err_tuples/2;
    //cout<<"err_tuples:"<<err_tuples<<endl;
    //int err=0;
    unordered_map<string, int> hm;//hm中存的是前size个元组中出现的flight名称
    unordered_map<string, string> locate_scheduled_dept;//记录flight对应的scheduled_dept
    unordered_map<string, string> locate_actual_dept;//记录flight对应的actual_dept
    unordered_map<string, string> locate_scheduled_arrival;//记录flight对应的scheduled_arrival
    unordered_map<string, string> locate_actual_arrival;//记录flight对应的actual_arrival
    unordered_map<string, string> locate_dept_gate;//记录flight对应的actual_arrival
    unordered_map<string, string> locate_arrival_gate;//记录flight对应的actual_arrival
    
    for(int y=0;y<50000;++y){
        hm[Trim(tuple[y].attrs_value[1]).substr(11,24)]++;
        hm[Trim(tuple[y].attrs_value[1]).substr(11,24)+"r"]++;
        if(Trim(tuple[y].attrs_value[2])!=""){
            locate_scheduled_dept[Trim(tuple[y].attrs_value[1])]=Trim(tuple[y].attrs_value[2]);
        }
        if(Trim(tuple[y].attrs_value[3])!=""){
            locate_actual_dept[Trim(tuple[y].attrs_value[1])]=Trim(tuple[y].attrs_value[3]);
        }
        if(Trim(tuple[y].attrs_value[4])!=""){
            locate_dept_gate[Trim(tuple[y].attrs_value[1])]=Trim(tuple[y].attrs_value[4]);
        }
        if(Trim(tuple[y].attrs_value[5])!=""){
            locate_scheduled_arrival[Trim(tuple[y].attrs_value[1])]=Trim(tuple[y].attrs_value[5]);
        }
        if(Trim(tuple[y].attrs_value[6])!=""){
            locate_actual_arrival[Trim(tuple[y].attrs_value[1])]=Trim(tuple[y].attrs_value[6]);
        }
        if(Trim(tuple[y].attrs_value[7])!=""){
            locate_arrival_gate[Trim(tuple[y].attrs_value[1])]=Trim(tuple[y].attrs_value[7]);
        }
    }
    
    vector<string> locate_airline={"United Airlines","Delta Air Lines","Southwest","Alask Airlines","Spirit Airlines","Allegiant Air LLC","JetBlue Airways","Denver Air Connection","American Airlines","Cape Air","Frontier Airlines","Boutique Air","Grant Aviation","Avelo Airlines","Breeze Airways","Hawaiian Airlines","Sun Country Airlines","Air Wisconsin","American Eagle","CommutAir","Contour Airlines","Elite Airways","Endeavor Air","Envoy Air","GoJet Airlines","Horizon Air","Mesa Airlines","Piedmont Airlines","PSA Airlines","Republic Airways","Silver Airways","SkyWest Airlines","Advanced Air","Air Choice One","Air Sunshine","Bering Air","Boutique Air","Everts Air","Grand Canyon Airlines","Hageland Aviation Services","Island Airways","JSX","Kenmore Air","Key Lime Air","Mokulele Airlines","San Juan Airlines","Servant Air","Surf Air","Taquan Air","Utah Airways"};
    
    vector<string> locate_destination={"AL","AK","AZ","AR","CA","CO","CT","DE","FL","GA","HI","ID","IL","IN","IA","KS","KY","LA","ME","MD","MA","MI","MN","MS","MO","MT","NE","NV","NH","NJ","NM","NY","NC","ND","OH","OK","OR","PA","RI","SC","SD","TN","TX","UT","VT","VA","WA","WV","WI","WY","DC","PR","VI"};
    
    size_t nums_scheduled_dept=locate_scheduled_dept.size();
    size_t nums_actual_dept=locate_actual_dept.size();
    size_t nums_scheduled_arrival=locate_scheduled_arrival.size();
    size_t nums_actual_arrival=locate_actual_arrival.size();
    size_t nums_dept_gate=locate_dept_gate.size();
    size_t nums_arrival_gate=locate_arrival_gate.size();
    size_t nums_locate_airline=locate_airline.size();
    size_t nums_locate_destination=locate_destination.size();
    //输出locate和hm看看
    bool flag=true;
    auto it_for_flight=hm.begin();
    cout<<"hm.size():"<<hm.size()<<endl;
    /*for(auto it:hm){
        cout<<"test:"<<it.first<<endl;
        cout<<"test:"<<it.second<<endl;
    }*/
    int start_date=20210100;
    char str[9];
    sprintf(str,"%d",start_date);
    
    vector<vector<string> > normal_tuple;
    //char str[]="20210101";
    for(int i=0;i<size-err_tuples;i++){
        //flag=true;
        //str=str+'1';
        vector<string> temp_normal_tuple;
        int j=0;
        for(j=0;j<nums-1;++j){
            if(j==0){//flight
                if(it_for_flight!=hm.end()){
                    if(strcmp(str,"20210930")<0){
                        if(str[5]-'0'==1||str[5]-'0'==3||str[5]-'0'==5||str[5]-'0'==7||str[5]-'0'==8){
                            if((str[6]-'0')*10+(str[7]-'0')>30){
                                str[5]=str[5]+1;
                                str[6]='0';
                                str[7]='1';
                                start_date=20210000+(str[5]-'0')*100+(str[6]-'0')*10+(str[7]-'0');
                            }
                            else{
                                start_date++;
                                sprintf(str,"%d",start_date);
                            }
                        }
                        else if(str[5]-'0'==2){
                            if((str[6]-'0')*10+(str[7]-'0')>27){
                                str[5]=str[5]+1;
                                str[6]='0';
                                str[7]='1';
                                start_date=20210000+(str[5]-'0')*100+(str[6]-'0')*10+(str[7]-'0');
                            }
                            else{
                                start_date++;
                                sprintf(str,"%d",start_date);
                            }
                        }
                        else if(str[5]-'0'==4||str[5]-'0'==6||str[5]-'0'==9){
                            if((str[6]-'0')*10+(str[7]-'0')>29){
                                str[5]=str[5]+1;
                                str[6]='0';
                                str[7]='1';
                                start_date=20210000+(str[5]-'0')*100+(str[6]-'0')*10+(str[7]-'0');
                            }
                            else{
                                start_date++;
                                sprintf(str,"%d",start_date);
                            }
                        }
                    }
                    else if(strcmp(str,"20210930")==0){
                        start_date=20211001;
                        sprintf(str,"%d",start_date);
                    }
                    else if(strcmp(str,"20210930")>0){
                        if(str[5]-'0'==0||str[5]-'0'==2){
                            if((str[6]-'0')*10+(str[7]-'0')>30){
                                str[5]=str[5]+1;
                                str[6]='0';
                                str[7]='1';
                                start_date=20211000+(str[5]-'0')*100+(str[6]-'0')*10+(str[7]-'0');
                            }
                            else{
                                start_date++;
                                sprintf(str,"%d",start_date);
                            }
                        }
                        else if(str[5]-'0'==1){
                            if((str[6]-'0')*10+(str[7]-'0')>29){
                                str[5]=str[5]+1;
                                str[6]='0';
                                str[7]='1';
                                start_date=20211000+(str[5]-'0')*100+(str[6]-'0')*10+(str[7]-'0');
                            }
                            else{
                                start_date++;
                                sprintf(str,"%d",start_date);
                            }
                        }
                    }
                    if(strcmp(str,"20211301")==0){
                        it_for_flight++;
                        start_date=20210101;
                        sprintf(str,"%d",start_date);
                    }
                    fprintf(fp,"%s ,",(str+it_for_flight->first).c_str());
                    temp_normal_tuple.push_back((str+it_for_flight->first).c_str());
                }
                else{
                    fprintf(fp,"%s ,","sss");
                    temp_normal_tuple.push_back("sss");
                }
            }
            else if(j==1){//scheduled_dept
                //srand((int)time(0));
                int j=rand();//生成随机数[0,nums_scheduled_dept-1]
                auto it=locate_scheduled_dept.begin();
                for(int h=0;h<j%nums_scheduled_dept;h++){
                    it++;
                }
                fprintf(fp,"%s ,",it->second.c_str());
                temp_normal_tuple.push_back(it->second.c_str());
            }
            else if(j==2){//actual_dept
                int j=rand();//生成随机数[0,nums_actual_dept-1]
                auto it=locate_actual_dept.begin();
                for(int h=0;h<j%nums_actual_dept;h++){
                    it++;
                }
                fprintf(fp,"%s ,",it->second.c_str());
                temp_normal_tuple.push_back(it->second.c_str());
            }
            else if(j==3){//dept_gate
                int j=rand();//生成随机数[0,nums_dept_gate-1]
                auto it=locate_dept_gate.begin();
                for(int h=0;h<j%nums_dept_gate;h++){
                    it++;
                }
                fprintf(fp,"%s ,",it->second.c_str());
                temp_normal_tuple.push_back(it->second.c_str());
            }
            else if(j==4){//scheduled_arrival
                int j=rand();//生成随机数[0,nums_scheduled_arrival-1]
                auto it=locate_scheduled_arrival.begin();
                for(int h=0;h<j%nums_scheduled_arrival;h++){
                    it++;
                }
                fprintf(fp,"%s ,",it->second.c_str());
                temp_normal_tuple.push_back(it->second.c_str());
            }
            else if(j==5){//actual_arrival
                int j=rand();//生成随机数[0,nums_actual_arrival-1]
                auto it=locate_actual_arrival.begin();
                for(int h=0;h<j%nums_actual_arrival;h++){
                    it++;
                }
                fprintf(fp,"%s ,",it->second.c_str());
                temp_normal_tuple.push_back(it->second.c_str());
            }
            else if(j==6){//arrival_gate
                int j=rand();//生成随机数[0,nums_arrival_gate-1]
                auto it=locate_arrival_gate.begin();
                for(int h=0;h<j%nums_arrival_gate;h++){
                    it++;
                }
                fprintf(fp,"%s ,",it->second.c_str());
                temp_normal_tuple.push_back(it->second.c_str());
            }
            else if(j==7){//airline
                int j=rand();//生成随机数[0,nums_locate_airline-1]
                auto it=locate_airline.begin();
                for(int h=0;h<j%nums_locate_airline;h++){
                    it++;
                }
                fprintf(fp,"%s ,",(*it).c_str());
                temp_normal_tuple.push_back((*it).c_str());
            }
            else if(j==8){//destination
                int j=rand();//生成随机数[0,nums_locate_destination-1]
                auto it=locate_destination.begin();
                for(int h=0;h<j%nums_locate_destination;h++){
                    it++;
                }
                fprintf(fp,"%s\n",(*it).c_str());
                temp_normal_tuple.push_back((*it).c_str());
            }
        }
        //hm[tuple[i].attrs_value[1]]--;
        int r=rand();//生成随机数[3,9]
        double t=r%7+3+xiaoshu[turn];
        if(t>=10||t<0) fprintf(fp_weight,"%.1f\n",5.0);
        else fprintf(fp_weight,"%.1f\n",r%7+3+xiaoshu[turn]);
        turn++;
        if(turn>=10){
            turn =0;
        }
        normal_tuple.push_back(temp_normal_tuple);
    }
    for(int i=0;i<err_tuples;++i){
        int r=rand();//生成随机数[20210101,20211231]
        int r_nums=r%(normal_tuple.size()-1);
        if(r_nums==normal_tuple.size()-1) r_nums--;
        fprintf(fp,"%s ,",normal_tuple[r_nums][0].c_str());//第0个属性值
        
        r=rand();//生成随机数[0,5]，决定违反哪个FD
        int r_FD=r%6;
        //违反第r_FD个FD
        if(nums_FD==6){}
        else if(nums_FD==5){
            if(r_FD==3){
                r_FD=0;
            }
        }
        else if(nums_FD==4){
            if(r_FD==1){
                r_FD=0;
            }
            else if(r_FD==3){
                r_FD=2;
            }
        }
        else if(nums_FD==3){
            if(r_FD==1){
                r_FD=0;
            }
            else if(r_FD==2){
                r_FD=4;
            }
            else if(r_FD==3){
                r_FD=5;
            }
        }
        else if(nums_FD==2){
            if(r_FD==0){
                r_FD=4;
            }
            else if(r_FD==1){
                r_FD=5;
            }
            else if(r_FD==2){
                r_FD=4;
            }
            else if(r_FD==3){
                r_FD=5;
            }
        }
        else if(nums_FD==1){
            r_FD=4;
        }
        if(r_FD!=5){
            //第0/1/2/3/4个FD
            r=rand();
            int r_error_type=r%1;
            if(r_FD==0){
                if(r_error_type==0){
                    //typo error
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][1]+"r").c_str());//第1个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][2].c_str());//第2个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][3].c_str());//第3个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][4].c_str());//第4个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][5].c_str());//第5个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][6].c_str());//第6个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][7].c_str());//第7个属性值
                    fprintf(fp,"%s\n",normal_tuple[r_nums][8].c_str());//第8个属性值
                }
                else if(r_error_type==1){
                    //FD error
                    fprintf(fp,"%s ,",(normal_tuple[r_nums+1][1]).c_str());//第1个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][2].c_str());//第2个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][3].c_str());//第3个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][4].c_str());//第4个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][5].c_str());//第5个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][6].c_str());//第6个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][7].c_str());//第7个属性值
                    fprintf(fp,"%s\n",normal_tuple[r_nums][8].c_str());//第8个属性值
                }
            }
            else if(r_FD==1){
                if(r_error_type==0){
                    //typo error
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][1]).c_str());//第1个属性值
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][2]+"r").c_str());//第2个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][3].c_str());//第3个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][4].c_str());//第4个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][5].c_str());//第5个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][6].c_str());//第6个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][7].c_str());//第7个属性值
                    fprintf(fp,"%s\n",normal_tuple[r_nums][8].c_str());//第8个属性值
                }
                else if(r_error_type==1){
                    //FD error
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][1]).c_str());//第1个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums+1][2].c_str());//第2个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][3].c_str());//第3个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][4].c_str());//第4个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][5].c_str());//第5个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][6].c_str());//第6个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][7].c_str());//第7个属性值
                    fprintf(fp,"%s\n",normal_tuple[r_nums][8].c_str());//第8个属性值
                }
            }
            else if(r_FD==2){
                if(r_error_type==0){
                    //typo error
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][1]).c_str());//第1个属性值
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][2]).c_str());//第2个属性值
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][3]).c_str());//第3个属性值
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][4]+"r").c_str());//第4个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][5].c_str());//第5个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][6].c_str());//第6个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][7].c_str());//第7个属性值
                    fprintf(fp,"%s\n",normal_tuple[r_nums][8].c_str());//第8个属性值
                }
                else if(r_error_type==1){
                    //FD error
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][1]).c_str());//第1个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][2].c_str());//第2个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][3].c_str());//第3个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums+1][4].c_str());//第4个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][5].c_str());//第5个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][6].c_str());//第6个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][7].c_str());//第7个属性值
                    fprintf(fp,"%s\n",normal_tuple[r_nums][8].c_str());//第8个属性值
                }
            }
            else if(r_FD==3){
                if(r_error_type==0){
                    //typo error
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][1]).c_str());//第1个属性值
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][2]).c_str());//第2个属性值
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][3]).c_str());//第3个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][4].c_str());//第4个属性值
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][5]+"r").c_str());//第5个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][6].c_str());//第6个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][7].c_str());//第7个属性值
                    fprintf(fp,"%s\n",normal_tuple[r_nums][8].c_str());//第8个属性值
                }
                else if(r_error_type==1){
                    //FD error
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][1]).c_str());//第1个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][2].c_str());//第2个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][3].c_str());//第3个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][4].c_str());//第4个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums+1][5].c_str());//第5个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][6].c_str());//第6个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][7].c_str());//第7个属性值
                    fprintf(fp,"%s\n",normal_tuple[r_nums][8].c_str());//第8个属性值
                }
            }
            else if(r_FD==4){
                if(r_error_type==0){
                    //typo error
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][1]).c_str());//第1个属性值
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][2]).c_str());//第2个属性值
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][3]).c_str());//第3个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][4].c_str());//第4个属性值
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][5]).c_str());//第5个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][6].c_str());//第6个属性值
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][7]+"r").c_str());//第7个属性值
                    fprintf(fp,"%s\n",normal_tuple[r_nums][8].c_str());//第8个属性值
                }
                else if(r_error_type==1){
                    //FD error
                    fprintf(fp,"%s ,",(normal_tuple[r_nums][1]).c_str());//第1个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][2].c_str());//第2个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][3].c_str());//第3个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][4].c_str());//第4个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][5].c_str());//第5个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums][6].c_str());//第6个属性值
                    fprintf(fp,"%s ,",normal_tuple[r_nums+1][7].c_str());//第7个属性值
                    fprintf(fp,"%s\n",normal_tuple[r_nums][8].c_str());//第8个属性值
                }
            }
        }
        else if(r_FD==5){
            //第5个FD
            r=rand();
            int r_error_type=r%1;
            if(r_error_type==0){
                //typo error
                fprintf(fp,"%s ,",(normal_tuple[r_nums][1]).c_str());//第1个属性值
                fprintf(fp,"%s ,",normal_tuple[r_nums][2].c_str());//第2个属性值
                fprintf(fp,"%s ,",normal_tuple[r_nums][3].c_str());//第3个属性值
                fprintf(fp,"%s ,",normal_tuple[r_nums][4].c_str());//第4个属性值
                fprintf(fp,"%s ,",normal_tuple[r_nums][5].c_str());//第5个属性值
                fprintf(fp,"%s ,",normal_tuple[r_nums][6].c_str());//第6个属性值
                fprintf(fp,"%s ,",normal_tuple[r_nums][7].c_str());//第7个属性值
                fprintf(fp,"%s\n",(normal_tuple[r_nums][8]+"r").c_str());//第8个属性值
            }
            else if(r_error_type==1){
                //FD error
                fprintf(fp,"%s ,",(normal_tuple[r_nums][1]).c_str());//第1个属性值
                fprintf(fp,"%s ,",normal_tuple[r_nums][2].c_str());//第2个属性值
                fprintf(fp,"%s ,",normal_tuple[r_nums][3].c_str());//第3个属性值
                fprintf(fp,"%s ,",normal_tuple[r_nums][4].c_str());//第4个属性值
                fprintf(fp,"%s ,",normal_tuple[r_nums][5].c_str());//第5个属性值
                fprintf(fp,"%s ,",normal_tuple[r_nums][6].c_str());//第6个属性值
                fprintf(fp,"%s ,",normal_tuple[r_nums][7].c_str());//第7个属性值
                fprintf(fp,"%s\n",(normal_tuple[r_nums+1][8]).c_str());//第8个属性值
            }
        }
        
        r=rand();//生成随机数(0,3]
        double t=r%3+1+xiaoshu[turn];
        if(t>3||t<0) fprintf(fp_weight,"%.1f\n",1.5);
        else fprintf(fp_weight,"%.1f\n",r%3+1+xiaoshu[turn]);
        turn ++;
        if(turn>=10){
            turn =0;
        }
    }
    fclose(fp);
    fclose(fp_weight);
}
/*--------------------以上pollute数据----------------*/


/*--------------------以下为元组赋随机weight----------------
void Generator::random_tuple_weight(vector<Tuple> &tuple){
    //FILE *fp;
    //int nums=tuple.size();
    //char result_file_name[]="/Users/andy/Documents/Data/Flight/flight";
    //char postfix[]=".csv";
    //char jmy[]="jmy";
    //char middle[6];
    //sprintf(middle,"%d",nums);
    //strcat(result_file_name,middle);
    //strcat(result_file_name,jmy);
    //strcat(result_file_name,postfix);
    //fp = fopen(result_file_name, "w");
    int i=0;
    for(i=0;i<tuple.size();++i){
        if(tuple[i].in_conflict>0){
            int j=rand();//生成随机数(0,3]
            tuple[i].tuple_weight=j%3+1;
            //fprintf(fp,"%d\n",j%(3+1));
            //(rand () % (b-a))+ a + 1;
        }
        else if(tuple[i].in_conflict==0){
            int j=rand();//生成随机数[7,10]
            tuple[i].tuple_weight=j%4+7;
            //fprintf(fp,"%d\n",j%4+7);
            //(rand () % (b-a+1))+ a;
        }
    }
    //fclose (fp);
}
--------------------以上为元组赋随机weight----------------*/



/*--------------------以下将soft_repair结果写到csv文件里----------------*/
void Generator::write_soft_result(char* soft_repair_result_path){
    //
    FILE *fp;
    fp = fopen(soft_repair_result_path,"ab");
    if(fp==NULL) cout<<"大失败！"<<endl;
    else cout<<"大成功！"<<endl;
    for(int i=0;i<SR.size();++i){
        //cout<<"aaaaa"<<SR[i].nums_tuples<<endl;
        fprintf(fp,"%d ,",SR[i].nums_tuples);
        //cout<<"bbbbb"<<SR[i].tuple_in_conflicts<<endl;
        fprintf(fp,"%d ,",SR[i].tuple_in_conflicts);
        //cout<<"ccccc"<<SR[i].nums_conflicts<<endl;
        fprintf(fp,"%d ,",SR[i].nums_conflicts);
        fprintf(fp,"%.5f ,",SR[i].error_rate);
        fprintf(fp,"%.5f ,",SR[i].Grepair_time);
        fprintf(fp,"%.5f ,",SR[i].Grepair_fopt);
        fprintf(fp,"%.5f ,",SR[i].Grepair_ropt);
        fprintf(fp,"%.5f ,",SR[i].Grepair_tleft);
        fprintf(fp,"%.5f ,",SR[i].Grepair_cleft);
        fprintf(fp,"%.5f ,",SR[i].Grepair_wcleft);
        fprintf(fp,"%.5f ,",SR[i].Greedy_time);
        fprintf(fp,"%.5f ,",SR[i].Greedy_ropt);
        fprintf(fp,"%.5f ,",SR[i].Greedy_tleft);
        fprintf(fp,"%.5f ,",SR[i].Greedy_cleft);
        fprintf(fp,"%.5f\n",SR[i].Greedy_wcleft);
    }
    fclose (fp);
}
/*--------------------以上将soft_repair结果写到csv文件里----------------*/


/*--------------------以下是set cover问题的LP算法----------------*/
vector<vector<int> > Generator::lp_sc(vector<int> X, vector<vector<int> > F) {
    vector<vector<int> > C;
    vector<double> x = lp_solver_sc(X, F);
    /*以下jmy*/
    for(int i=0;i<x.size();++i) cout<<"hh:"<<x[i]<<endl;
    /*以上jmy*/
    int f = get_f(X, F);
    // std::cout << f << std::endl;
    for (int i = 0; i < x.size(); i++) {
        /*以下jmy*/
        for(int m=0;m<F[i].size();++m){
            cout<<"应该是所有sets:"<<F[i][m]<<", ";
        }
        cout<<endl;
        /*以上jmy*/
        // std::cout << "i " << x[i] << std::endl;
        if (x[i] >= 1.0/f) {
            /*以下jmy*/
            for(int m=0;m<F[i].size();++m){
                cout<<"应该是被选中的sets:"<<F[i][m]<<", ";
            }
            cout<<endl;
            /*以上jmy*/
            C.push_back(F[i]);
            //在这里改soft_lp_opt/soft_lp_wtuple/soft_lp_wconflict
            //迭代器从weight_bef_lp.begin()走i步
            auto it_lp=weight_bef_lp.begin();
            for(int d=0;d<i;++d){
                it_lp++;
            }
            //如果是标记0,则加入soft_lp_wtuple
            if(it_lp->second==0){
                soft_lp_wtuple=soft_lp_wtuple+it_lp->first;
            }
            //如果是标记1,则加入soft_lp_wconflict
            if(it_lp->second==1){
                soft_lp_wconflict=soft_lp_wconflict+it_lp->first;
                soft_lp_conflict++;
            }
        }
        else{
            /*以下jmy*/
            for(int m=0;m<F[i].size();++m){
                cout<<"应该是没被选中的sets:"<<F[i][m]<<", ";
            }
            cout<<endl;
            /*以上jmy*/
        }
    }
    /*以下jmy*/
    for(int i=0;i<C.size();++i){
        for(int j=0;j<C[i].size();++j)  cout<<"ss:"<<C[i][j];
        cout<<endl;
    }
    /*以上jmy*/
    /*以下jmy*/
    cout<<"weight_bef_lp"<<endl;
    for(auto it:weight_bef_lp){
        cout<<it.first<<"..."<<it.second<<endl;
    }
    cout<<"weight_aft_lp"<<endl;
    for(auto it:weight_aft_lp){
        cout<<it.first<<"..."<<it.second<<endl;
    }
    /*以上jmy*/
    
    /*以下jmy*/
    soft_lp_opt=soft_lp_wtuple+soft_lp_wconflict;
    //输出soft_lp_opt/soft_lp_wtuple/soft_lp_wconflict/soft_lp_conflict康康
    cout<<"输出soft_lp_opt康康:"<<soft_lp_opt<<endl;
    cout<<"输出soft_lp_wtuple康康:"<<soft_lp_wtuple<<endl;
    cout<<"输出soft_lp_conflict康康:"<<soft_lp_conflict<<endl;
    cout<<"输出soft_lp_wconflict康康:"<<soft_lp_wconflict<<endl;
    /*以上jmy*/
    return C;
}
/*--------------------以上是set cover问题的LP算法----------------*/

/*--------------------以下是lp_solver_sc----------------*/
vector<double> Generator::lp_solver_sc(vector<int> X, vector<vector<int> > F) {
    vector<double> x;
    int n = X.size();
    int m = F.size();
    // std::cout << n << m << std::endl;
initialize:
    glp_prob *lp;
    lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MIN);
auxiliary_variables_rows:
    glp_add_rows(lp, n);
    for (int i = 1; i <= n; i++) glp_set_row_bnds(lp, i, GLP_LO, 1.0, 0.0);
variables_columns:
    glp_add_cols(lp, m);
    for (int i = 1; i <= m; i++) glp_set_col_bnds(lp, i, GLP_DB, 0.0, 1.0);
to_minimize:
    for (int i = 1; i <= m; i++) glp_set_obj_coef(lp, i, 1.0);
constrant_matrix:
    int size = m*n;
    // std::cout << size << std::endl;
    int *ia = new int[size + 1];
    int *ja = new int[size + 1];
    // std::cout << size << std::endl;
    double *ar = new double[size + 1];
    // std::cout << size << std::endl;
    for (int i = 1; i <= n; i++) {
        // std::cout << i << std::endl;
        for (int j = 1; j <= m; j++) {
            int k = (i - 1)*m + j;
            // std::cout << k << std::endl;
            ia[k] = i;
            ja[k] = j;
            vector<int> f = F[j-1];
            vector<int>::iterator it = find(f.begin(), f.end(), i-1);
            if (it != f.end()) ar[k] = 1;
            else ar[k] = 0;
        }
    }
    glp_load_matrix(lp, size, ia, ja, ar);
calculate:
    glp_simplex(lp, NULL);
output:
    // std::cout << "opt:" << glp_get_obj_val(lp) << std::endl;
    for (int i = 1; i <= m; i++) x.push_back(glp_get_col_prim(lp, i));
cleanup:
    glp_delete_prob(lp);
    return x;
}
/*--------------------以上是lp_solver_sc----------------*/

/*--------------------以下是get_f----------------*/
int Generator::get_f(vector<int> X, vector<vector<int> > F) {
    int count = 0, maxcount = 0;
    for (int i = 0; i < X.size(); i++) {
        for (int j =  0; j < F.size(); j++) {
            vector<int> S = F[j];
            vector<int>::iterator it = find(S.begin(), S.end(), X[i]);
            if (it != S.end()) count++;
        }
        if (count > maxcount) maxcount = count;
        count = 0;
    }
    return maxcount;
}
/*--------------------以上是get_f----------------*/

/*--------------------以下将soft_repair_lp结果写到csv文件里----------------*/
void Generator::write_soft_result_lp(char* soft_repair_result_path){
    FILE *fp;
    fp = fopen(soft_repair_result_path,"ab");
    if(fp==NULL) cout<<"大失败！"<<endl;
    else cout<<"大成功！"<<endl;
    fprintf(fp,"%.5f ,",soft_lp_opt);
    fprintf(fp,"%.5f ,",soft_lp_wtuple);
    fprintf(fp,"%d ,",soft_lp_conflict);
    fprintf(fp,"%.5f ,",soft_lp_wconflict);
    fclose (fp);
}
/*--------------------以上将soft_repair_lp结果写到csv文件里----------------*/

