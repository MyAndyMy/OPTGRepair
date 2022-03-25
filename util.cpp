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
template <class Type>
Type stringToNum(const string& str){
    istringstream iss(str);
    Type num;
    iss >> num;
    return num;
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
        getline(fin, line);
        istringstream sin(line);
        vector<string> objs;
        string obj;
        while (getline(sin, obj, ',')){}
        size_t w_id=0;//记录元组的tuple_id
        while (getline(fin, line)){
            istringstream sin(line);
            vector<string> fields;//所有属性值
            string field;//属性值
            while(getline(sin, field, ','))
                fields.push_back(field);
            source_tuple[w_id].tuple_weight=stringToNum<double>(Trim(fields[0]));
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
            fun_denp[fd_id].fd_weight=stringToNum<double>(Trim(objs1[0]));
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
    unsigned int size = conflicts*tuples;
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
            tuple[i].xr=1;
        }
        else tuple[i].xr=tuple[i].x;
    }
    for(size_t i=0;i<relationship.size();++i){
        if(relationship[i].y==0.5){
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
        glp_set_obj_coef(lp, i, -for_weight[make_pair(relationship_G[i-tuples-1].rel.first.tuple_id, relationship_G[i-tuples-1].rel.second.tuple_id)]);//y(conflict)
        //cout<<"con"<<for_weight[make_pair(relationship_G[i-tuples-1].rel.first.tuple_id, relationship_G[i-tuples-1].rel.second.tuple_id)]<<endl;
    }
constrant_matrix:
    unsigned int size = (conflicts +  2 * adjacency)*(tuples + conflicts + adjacency);
    int *ia = new int[size + 1];
    int *ja = new int[size + 1];
    double *ar = new double[size + 1];
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
    }
    vector<int> temp;
    for(int i=0;i<S1.size();++i){
        for(int j=0;j<S1[i].element.size();++j){
            temp.push_back(S1[i].element[j].ele_id);
        }
        C.push_back(temp);
        temp.clear();
    }
    for(int i=0;i<S2.size();++i){
        for(int j=0;j<S2[i].element.size();++j){
            temp.push_back(S2[i].element[j].ele_id);
        }
        C.push_back(temp);
        temp.clear();
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
    sort(va.begin(), va.end());
    sort(vb.begin(), vb.end());
    it = set_difference(va.begin(), va.end(), vb.begin(), vb.end(), vc.begin());
    vc.resize(it - vc.begin());
    return vc;
}

vector<int> Generator::v_intersection(vector<int> &va, vector<int> &vb) {
    vector<int> vc(va.size() + vb.size());
    vector<int>::iterator it;
    sort(va.begin(), va.end());
    sort(vb.begin(), vb.end());
    it = set_intersection(va.begin(), va.end(), vb.begin(), vb.end(), vc.begin());
    vc.resize(it - vc.begin());
    return vc;
}
int Generator::max_union(vector<int> U, vector<vector<int> > F) {
    int minsize = INT_MAX, index = 0;
    for (int i = 0; i < F.size(); i++) {
        vector<int> uf = v_intersection(U, F[i]);
        if(uf.size()!=0){
            size_t len=uf.size();
            if(len!=0){
                double t = 0.0;
                if(i<S1.size()){
                    t=(double)S1[i].set_weight/len;
                }
                if(i>=S1.size()&&i<S2.size()){
                    t=(double)S2[i].set_weight/len;
                }
                if (t < minsize) {
                    minsize = t;
                    index = i;
                }
            }
        }
    }
    return index;
}
vector<int> Generator::greedy_sc(vector<int> X, vector<vector<int> > F) {
    vector<int> U = X;
    vector<int> C;
    while (U.size() != 0) {
        // std::cout << U.size() << std::endl;
        int S = max_union(U, F);
        U = v_difference(U, F[S]);
        C.push_back(S);
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
    vector<Tuple>::iterator last=tuple.begin();
    while(last!=tuple.end()){
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
