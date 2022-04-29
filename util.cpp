//
//  main.cpp
//  OPTGRSRTR5.0
//
//  Created by Andy on 2022/3/14.
//  这一版完成:
//  了很多


#include <iostream>
#include <fstream>
#include "util.hpp"
#include <time.h>

vector<vector<int> > greedy_sc(vector<int> X, vector<vector<int> > F);//soft repair的贪心算法

void temporal_db_repair(char* tuple_path,char* tuple_weight_path,char* fd_path,char* fd_weight_path){
    clock_t start,end;//计算各种算法的运行时间
    
    /*--------------------以下加载元组--------------------*/
    char ch[]="t";//标记为加载temporal data
    Generator gen(tuple_path,ch);//加载tuple到gen.source_data中
    gen.load_tuple_weight("",gen.source_tuple);//加载tuple's weight,当第一个参数为""时,默认每个元组的weight都是lambda
    gen.load_tuple_weight(tuple_weight_path,gen.source_tuple);//加载tuple's weight到gen.source_data中
    gen.print_attrs_name();//输出temporal data的全部属性名称
    /*--------------------以上加载元组--------------------*/
    
    cout<<endl;
    
    /*--------------------以下加载FD--------------------*/
    gen.load_fd_file(fd_path);//加载FD到gen.fun_denp中
    gen.load_fd_weight("",gen.fun_denp);//加载fd's weight,当第一个参数为""时,默认每个元组的FD都是w_conflict
    gen.load_fd_weight(fd_weight_path,gen.fun_denp);//加载fd's weight到gen.fun_denp中
    cout<<"FD数量:"<<gen.number_of_fds()<<endl;
    gen.print_every_fds();//输出全部FD
    /*--------------------以上加载FD--------------------*/
    
    cout<<endl;
    
    /*--------------------以下构建原始元组的冲突对--------------------*/
    start=clock();
    gen.construct_conflict(gen.source_tuple,gen.relationship_S,ch);//将gen.source_tuple的冲突对关系存储在gen.relationship_S中
    end=clock();
    cout<<"原始元组构建冲突对的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    /*--------------------以上构建原始元组的冲突对--------------------*/
    
    cout<<endl;
    
    /*--------------------以下打印元组相关信息--------------------*/
    cout<<"分段前元组数量:"<<gen.number_of_tuples(gen.source_tuple)<<endl;
    //以下输出元组size
    ofstream output;
    output.open("source_size&cost&NNorS.txt",ios::app);
    if(!output){
        cout<<"doesn't exist"<<endl;
    }
    else{
        cout<<"exists"<<endl;
    }
    if(!output.is_open()){
        cout<<"open fail"<<endl;
    }
    else{
        cout<<"open success"<<endl;
    }
    output<<gen.number_of_tuples(gen.source_tuple)<<",";
    output.close();
    //以上输出元组size
    cout<<"分段前元组中参与冲突的元组数量:"<<gen.number_of_tuples_in_conflict(gen.source_tuple)<<endl;
    cout<<"分段前元组错误率:"<<gen.print_error_percentage(gen.source_tuple)<<endl;
    cout<<"分段前冲突对儿数量:"<<gen.number_of_conflicts(gen.relationship_S)<<endl;
    cout<<"分段前相邻对儿数量:"<<gen.number_of_adjacency(gen.relationship_S)<<endl;
    /*--------------------以上打印元组相关信息--------------------*/
    
    cout<<endl;
    
    /*--------------------以下无normalization,S_repair--------------------*/
    start=clock();
    gen.lp_solver_S_repair(gen.source_tuple,gen.relationship_S);//用S_repair算法
    end=clock();
    cout<<"无normalization,S_repair的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    start=clock();
    gen.rounding(gen.source_tuple,gen.relationship_S);//为gen.source_tuple的x做舍入
    end=clock();
    cout<<"无normalization,S_repair的rounding时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    cout<<"无normalization,S_repair的分数最优解为:"<<gen.calculate_opt_f_S(gen.source_tuple)<<endl;
    cout<<"无normalization,S_repair的整数近似最优解为:"<<gen.calculate_opt_r_S(gen.source_tuple)<<endl;
    //以下输出cost
    output.open("source_size&cost&NNorS.txt",ios::app);
    if(!output){
        cout<<"doesn't exist"<<endl;
    }
    else{
        cout<<"exists"<<endl;
    }
    if(!output.is_open()){
        cout<<"open fail"<<endl;
    }
    else{
        cout<<"open success"<<endl;
    }
    output<<gen.calculate_opt_r_S(gen.source_tuple)<<endl;
    output.close();
    //以上输出cost
    gen.construct_left_tuple(gen.source_tuple);//把留在repair中的元组放到gen.left_tuple中
    cout<<"无normalization,S_repair,留下的元组个数:"<<gen.left_tuple.size()<<endl;
    gen.construct_adjacency(gen.left_tuple, gen.relationship_left);//建立gen.left_tuple中的相邻对到gen.relationship_left中
    cout<<"无normalization,S_repair,留下相邻对个数:"<<gen.number_of_adjacency(gen.relationship_left)<<endl;
    cout<<"无normalization,S_repair,留下相邻对个数/留下元组个数:"<<(double)gen.number_of_adjacency(gen.relationship_left)/gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.coalescing(gen.left_tuple);//为gen.left_tuple中元组做coalescing操作
    cout<<"无normalization,S_repair,coalescing后,留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    //cout<<"无normalization,S_repair,coalescing后的span:"<<gen.span(gen.left_tuple)<<endl;
    gen.left_tuple.clear();
    gen.relationship_left.clear();
    /*--------------------以上无normalization,S_repair--------------------*/
    
    cout<<endl;
    
    /*--------------------以下normalization1--------------------*/
    gen.for_weight.clear();
    start=clock();
    gen.normalization1();
    end=clock();
    cout<<"normalization1时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    cout<<"normalization1后的元组数量:"<<gen.number_of_tuples(gen.normalized_tuple)<<endl;
    start=clock();
    gen.construct_adjacency(gen.normalized_tuple,gen.relationship_G);//将normalized_tuple的相邻对关系存储在relationship_G中
    end=clock();
    cout<<"normalization1后建立相邻元组对时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    start=clock();
    gen.construct_conflict(gen.normalized_tuple,gen.relationship_G,ch);//将normalized_tuple的冲突对关系存储在relationship_G中
    end=clock();
    cout<<"normalization1后建立冲突元组对时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    cout<<"normalization1后元组中参与冲突的元组数量:"<<gen.number_of_tuples_in_conflict(gen.normalized_tuple)<<endl;
    cout<<"normalization1后元组错误率:"<<gen.print_error_percentage(gen.normalized_tuple)<<endl;
    cout<<"normalization1后冲突对儿数量:"<<gen.number_of_conflicts(gen.relationship_G)<<endl;
    cout<<"normalization1后相邻对儿数量:"<<gen.number_of_adjacency(gen.relationship_G)<<endl;
    /*--------------------以上normalization1--------------------*/
    
    cout<<endl;
    
    /*--------------------以下normalization1，S-repair--------------------*/
    start=clock();
    gen.lp_solver_S_repair(gen.normalized_tuple,gen.relationship_G);
    end=clock();
    cout<<"normalization1后,S_repair时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    start=clock();
    gen.rounding(gen.normalized_tuple,gen.relationship_G);//为normalized_tuple的x和relationship_G的y做舍入
    end=clock();
    cout<<"normalization1后,S_repair的rounding时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    cout<<"normalization1后,S_repair的分数最优解为:"<<gen.calculate_opt_f_S(gen.normalized_tuple)<<endl;
    cout<<"normalization1后,S_repair的整数近似最优解为:"<<gen.calculate_opt_r_S(gen.normalized_tuple)<<endl;
    //以下输出cost
    output.open("source_size&cost&NorS.txt",ios::app);
    if(!output){
        cout<<"doesn't exist"<<endl;
    }
    else{
        cout<<"exists"<<endl;
    }
    if(!output.is_open()){
        cout<<"open fail"<<endl;
    }
    else{
        cout<<"open success"<<endl;
    }
    output<<gen.number_of_tuples(gen.source_tuple)<<","<<gen.calculate_opt_r_S(gen.normalized_tuple)<<endl;
    output.close();
    //以上输出cost
    gen.construct_left_tuple(gen.normalized_tuple);//把留在repair中的元组放到gen.left_tuple中
    cout<<"normalization1后,S_repair,留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.construct_adjacency(gen.left_tuple, gen.relationship_left);//建立gen.left_tuple中的相邻对到gen.relationship_left中
    cout<<"normalization1后,S_repair,留下相邻对个数:"<<gen.number_of_adjacency(gen.relationship_left)<<endl;
    cout<<"normalization1后,S_repair,留下相邻对个数/留下元组个数:"<<(double)gen.number_of_adjacency(gen.relationship_left)/gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.coalescing(gen.left_tuple);//为gen.left_tuple中元组做coalescing操作
    cout<<"normalization1后,S_repair,coalescing后,留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    //cout<<"normalization1后,S_repair,coalescing后的span:"<<gen.span(gen.left_tuple)<<endl;
    gen.left_tuple.clear();
    gen.relationship_left.clear();
    /*--------------------以上normalization1，S-repair--------------------*/
    
    cout<<endl;
    
    /*--------------------以下normalization1，G-repair--------------------*/
    start=clock();
    gen.lp_solver_G_repair(gen.normalized_tuple,gen.relationship_G);
    end=clock();
    cout<<"normalization1后,G_repair的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    if(gen.effectiveness_of_solution_G(gen.relationship_G)==true)
        cout<<"G_repair解有效,"<<"w>0时,y_i,j=min(xi+xj,1);w<0时,y_i,j=max(xi,xj)"<<endl;
    else cout<<"G_repair解无效"<<endl;
    start=clock();
    gen.rounding(gen.normalized_tuple,gen.relationship_G);//为normalized_tuple的x和relationship_G的y做舍入
    end=clock();
    cout<<"normalization1后,G_repair的rounding时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    cout<<"normalization1后,G_repair的分数最优解为:"<<gen.calculate_opt_f_G(gen.normalized_tuple, gen.relationship_G)<<endl;
    cout<<"normalization1后,G_repair的整数近似最优解为:"<<gen.calculate_opt_r_G(gen.normalized_tuple, gen.relationship_G)<<endl;
    //以下输出cost
    output.open("source_size&cost&NorG.txt",ios::app);
    if(!output){
        cout<<"doesn't exist"<<endl;
    }
    else{
        cout<<"exists"<<endl;
    }
    if(!output.is_open()){
        cout<<"open fail"<<endl;
    }
    else{
        cout<<"open success"<<endl;
    }
    output<<gen.number_of_tuples(gen.source_tuple)<<","<<gen.calculate_opt_r_G(gen.normalized_tuple, gen.relationship_G)<<endl;
    output.close();
    //以上输出cost
    gen.construct_left_tuple(gen.normalized_tuple);//把留在repair中的元组放到gen.left_tuple中
    cout<<"normalization1后,G_repair,留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.construct_adjacency(gen.left_tuple, gen.relationship_left);//建立gen.left_tuple中的相邻对到gen.relationship_left中
    cout<<"normalization1后,G_repair,留下相邻对个数:"<<gen.number_of_adjacency(gen.relationship_left)<<endl;
    cout<<"normalization1后,G_repair,留下相邻对个数/留下元组个数:"<<(double)gen.number_of_adjacency(gen.relationship_left)/gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.coalescing(gen.left_tuple);//为gen.left_tuple中元组做coalescing操作
    cout<<"normalization1后,G_repair,coalescing后,留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    //cout<<"normalization1后,G_repair,coalescing后的span:"<<gen.span(gen.left_tuple)<<endl;
    gen.left_tuple.clear();
    gen.relationship_left.clear();
    /*--------------------以上normalization1，G-repair--------------------*/
    
}
void soft_repair(char* tuple_path,char* tuple_weight_path,char* fd_path,char* fd_weight_path){
    clock_t start,end;//计算时间
    SoftResult temp_RS;
    /*--------------------以下加载元组--------------------*/
    char ch[]="r";
    Generator gen(tuple_path,ch);//加载tuple到gen.source_data中
    //gen.load_tuple_weight("",gen.source_tuple);//加载tuple's weight,当第一个参数为""时,默认每个元组的weight都是lambda
    gen.load_tuple_weight(tuple_weight_path,gen.source_tuple);//加载tuple's weight到gen.source_data中
    gen.print_attrs_name();//输出relational data全部属性名称
    /*--------------------以上加载元组--------------------*/
    //for(int i=0;i<gen.source_tuple.size();++i) cout<<"attention:"<<gen.source_tuple[i].tuple_weight<<endl;
    cout<<endl;
    gen.pollute_flight(gen.source_tuple,10000,0.1);
    //gen.random_tuple_weight(10000,0.05);
    //gen.pollute(gen.source_tuple,10000,0.01);
    
    /*--------------------以下加载FD--------------------*/
    gen.load_fd_file(fd_path);//加载FD到gen.fun_denp
    //gen.load_fd_weight("",gen.fun_denp);//加载fd's weight,当第一个参数为""时,默认每个元组的FD都是w_conflict
    gen.load_fd_weight(fd_weight_path,gen.fun_denp);//加载fd's weight到到gen.fun_denp
    cout<<"FD数量:"<<gen.number_of_fds()<<endl;
    gen.print_every_fds();//输出全部FD
    /*--------------------以上加载FD--------------------*/
    
    cout<<endl;
    
    /*--------------------以下构建原始元组的冲突对--------------------*/
    start=clock();
    gen.construct_conflict(gen.source_tuple,gen.relationship_G,ch);//将source_tuple的冲突对关系存储在relationship_G中
    //gen.random_tuple_weight(gen.source_tuple);
    end=clock();
    cout<<"原始元组构建冲突对的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    /*--------------------以上构建原始元组的冲突对--------------------*/
    
    cout<<endl;
    
    /*--------------------以下打印元组相关信息--------------------*/
    cout<<"元组数量:"<<gen.number_of_tuples(gen.source_tuple)<<endl;
    temp_RS.nums_tuples=gen.number_of_tuples(gen.source_tuple);
    cout<<"元组中参与冲突的元组数量:"<<gen.number_of_tuples_in_conflict(gen.source_tuple)<<endl;
    temp_RS.tuple_in_conflicts=gen.number_of_tuples_in_conflict(gen.source_tuple);
    cout<<"冲突对儿数量:"<<gen.number_of_conflicts(gen.relationship_G)<<endl;
    temp_RS.nums_conflicts=gen.number_of_conflicts(gen.relationship_G);
    cout<<"元组错误率:"<<gen.print_error_percentage(gen.source_tuple)<<endl;
    temp_RS.error_rate=gen.print_error_percentage(gen.source_tuple);
    /*--------------------以上打印元组相关信息--------------------*/
    
    cout<<endl;
    
    /*--------------------以下对source_tuple和relationship_G进行G-repair--------------------*/
    start=clock();
    gen.lp_solver_G_repair(gen.source_tuple,gen.relationship_G);
    end=clock();
    cout<<"soft repair的G_repair的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    temp_RS.Grepair_time=(double)(end - start)/CLOCKS_PER_SEC;
    if(gen.effectiveness_of_solution_G(gen.relationship_G)==true)
        cout<<"G_repair解有效,"<<"w>0时,y_i,j=min(xi+xj,1);w<0时,y_i,j=max(xi,xj)"<<endl;
    else cout<<"G_repair解无效"<<endl;
    start=clock();
    gen.rounding(gen.source_tuple,gen.relationship_G);//为normalized_tuple的x和relationship_G的y做舍入
    end=clock();
    cout<<"soft repair的G_repair的rounding时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    cout<<"soft repair的G_repair的分数最优解为:"<<gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G)<<endl;
    temp_RS.Grepair_fopt=gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G);
    cout<<"soft repair的G_repair的整数近似最优解为:"<<gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G)<<endl;
    temp_RS.Grepair_ropt=gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G);
    gen.construct_left_tuple(gen.source_tuple);//把留在repair中的元组放到gen.left_tuple中
    cout<<"soft repair的G_repair留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.construct_left_conflict(gen.left_tuple, gen.relationship_S);
    temp_RS.Grepair_tleft=gen.number_of_tuples(gen.left_tuple);
    cout<<"soft repair的G_repair留下的元组冲突对个数:"<<gen.number_of_conflicts(gen.relationship_S)<<endl;
    temp_RS.Grepair_cleft=gen.number_of_conflicts(gen.relationship_S);
    cout<<"soft repair的G_repair留下的元组冲突对加权和:"<<gen.weighted_left_conflicts(gen.relationship_S)<<endl;
    temp_RS.Grepair_wcleft=gen.weighted_left_conflicts(gen.relationship_S);
    gen.left_tuple.clear();
    gen.relationship_left.clear();
    gen.relationship_S.clear();
    /*--------------------以上对source_tuple和relationship_G进行G-repair--------------------*/
    
    cout<<endl;
    
    /*--------------------以下构造set cover问题的输入U和S1和S2----------------*/
    start=clock();
    gen.construct_sets();
    end=clock();
    cout<<"构造set cover问题的输入U和S时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    
    /*test
    for(int i=0;i<gen.U.size();++i){
        cout<<"test_elements:"<<gen.U[i].t1.tuple_id<<", "<<gen.U[i].t2.tuple_id<<", "<<gen.U[i].fd.fd_id<<endl;
    }
    for(int i=0;i<gen.S1.size();++i){
        cout<<"test_set1:"<<gen.S1[i].t.tuple_id<<endl;
        cout<<"test_set1_elements:";
        for(int j=0;j<gen.S1[i].element.size();++j){
            cout<<gen.S1[i].element[j].t1.tuple_id<<", "<<gen.S1[i].element[j].t2.tuple_id<<", "<<gen.S1[i].element[j].fd.fd_id<<", "<<", weight: "<<gen.S1[i].set_weight<<"----";
        }
        cout<<endl;
    }
    for(int i=0;i<gen.S2.size();++i){
        cout<<"test_set2:"<<gen.S2[i].t1.tuple_id<<", "<<gen.S2[i].t2.tuple_id<<", "<<gen.S2[i].fd.fd_id<<endl;
        cout<<"test_set2_elements:";
        for(int j=0;j<gen.S2[i].element.size();++j){
            cout<<gen.S2[i].element[j].t1.tuple_id<<", "<<gen.S2[i].element[j].t2.tuple_id<<", "<<gen.S2[i].element[j].fd.fd_id<<", weight: "<<gen.S2[i].set_weight<<"----";
        }
        cout<<endl;
    }
    test*/
    
    //gen.print_all_info_sc();
    /*--------------------以上构造set cover问题的输入U和S1和S2----------------*/
    
    cout<<endl;
    
    /*--------------------以下构造set cover问题的输入vector<int>和vector<vector<int>>----------------*/
    gen.construct_sc();
    
    /*test
    for(int i=0;i<gen.UU.size();++i){
        cout<<gen.UU[i]<<endl;
    }
    for(int i=0;i<gen.C.size();++i){
        for(int j=0;j<gen.C[i].size();++j){
            cout<<gen.C[i][j]<<endl;
        }
    }
    for(int i=0;i<gen.weight_bef.size();++i){
        cout<<"weight_bef:"<<gen.weight_bef[i].first<<endl;
    }
    test*/
    
    //gen.print_all_info_sc_int();
    /*--------------------以上构造set cover问题的输入vector<int>和vector<vector<int>>----------------*/
    
    cout<<endl;
    
    /*--------------------以下是set cover问题的贪心算法----------------*/
    gen.clear_source_tuple();
    vector<int> res;
    start=clock();
    res=gen.greedy_sc(gen.UU,gen.C);
    end=clock();
    cout<<"soft repair的贪心算法时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    temp_RS.Greedy_time=(double)(end - start)/CLOCKS_PER_SEC;
    /*test
    for(int i=0;i<res.size();++i){
        cout<<res[i]<<", ";
    }
    */
    //cout<<"greedy cost:"<<gen.greedy_opt<<endl;
    
    //double bef0=0.0;
    double bef1=0.0;
    //double aft0=0.0;
    double aft1=0.0;
    int num_bef0=0;
    int num_bef1=0;
    int num_aft0=0;
    int num_aft1=0;
    for(int i=0;i<gen.weight_bef.size();++i){
        if(gen.weight_bef[i].second==0){
            num_bef0++;
        }
    }
    for(int i=0;i<gen.weight_bef.size();++i){
        if(gen.weight_bef[i].second==1){
            num_bef1++;
            bef1=bef1+gen.weight_bef[i].first;
        }
    }
    for(int i=0;i<gen.weight_aft.size();++i){
        if(gen.weight_aft[i].second==0){
            num_aft0++;
        }
    }
    for(int i=0;i<gen.weight_aft.size();++i){
        if(gen.weight_aft[i].second==1){
            num_aft1++;
            aft1+=gen.weight_aft[i].first;
        }
    }
    
    cout<<"soft repair的贪心整数近似最优解为:"<<gen.greedy_opt<<endl;
    temp_RS.Greedy_ropt=gen.greedy_opt;
    cout<<"soft repair的贪心算法留下的元组个数:"<<num_aft0<<endl;
    temp_RS.Greedy_tleft=num_aft0;
    cout<<"soft repair的贪心算法留下的元组冲突对个数:"<<num_bef1-num_aft1<<endl;
    temp_RS.Greedy_cleft=num_bef1-num_aft1;
    cout<<"soft repair的贪心算法留下的元组冲突对加权和:"<<bef1-aft1<<endl;
    temp_RS.Greedy_wcleft=bef1-aft1;
    //cout<<"111h"<<bef1<<endl;
    //cout<<"222h"<<aft1<<endl;
    /*test*/
    /*for(int i=0;i<res.size();++i){
        if(res[i]>0&&res[i]<gen.S1.size()){
            gen.source_tuple[res[i]].xr=1;
        }
    }
    cout<<"soft repair的贪心整数近似最优解为:"<<gen.greedy_opt<<endl;
    //cout<<"soft repair的贪心整数近似最优解为:"<<gen.calculate_opt_r_Greedy()<<endl;
    cout<<"soft repair的贪心算法留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    cout<<"soft repair的贪心算法留下的元组冲突对个数:"<<gen.number_of_conflicts(gen.relationship_S)<<endl;
    //没错,留下元组的冲突对放在relationship_S中了～！
    cout<<"soft repair的贪心算法留下的元组冲突对加权和:"<<gen.weighted_left_conflicts(gen.relationship_S)<<endl;*/
    gen.left_tuple.clear();
    gen.relationship_left.clear();
    gen.relationship_S.clear();
    /*--------------------以上是set cover问题的贪心算法----------------*/
}

int main(int argc, const char * argv[]) {
    cout<<"--------------------temporal db repair--------------------"<<endl;
    char tem_tuple_path[]="/Users/andy/Documents/Data/Emp/Emp100jmy.csv";
    char tem_tuple_weight_path[]="";
    char tem_fd_path[]="/Users/andy/Documents/Data/Emp/Emp_FDs.txt";
    char tem_fd_weight_path[]="/Users/andy/Documents/Data/Emp/Emp_FDs_weight.txt";
    //temporal_db_repair(tem_tuple_path,tem_tuple_weight_path,tem_fd_path,tem_fd_weight_path);
    cout<<"--------------------temporal db repair--------------------"<<endl;
    
    cout<<endl;

    /*cout<<"--------------------soft repair--------------------"<<endl;
    char rel_tuple_path[]="/Users/andy/Documents/Data/test/testsoftgreedy/flight6.csv";
    char rel_tuple_weight_path[]="/Users/andy/Documents/Data/test/testsoftgreedy/flight6weight.txt";
    char rel_fd_path[]="/Users/andy/Documents/Data/test/testsoftgreedy/flight_FDs.txt";
    char rel_fd_weight_path[]="/Users/andy/Documents/Data/test/testsoftgreedy/flight_FDs_weight.txt";
    soft_repair(rel_tuple_path,rel_tuple_weight_path,rel_fd_path,rel_fd_weight_path);
    cout<<"--------------------soft repair--------------------"<<endl;*/
    
    cout<<"--------------------soft repair--------------------"<<endl;
    char rel_tuple_path[]="/Users/andy/Documents/Data/Flight/flight50kjmy.csv";
    char rel_tuple_weight_path[]="";
    char rel_fd_path[]="/Users/andy/Documents/Data/Flight/flight_FDs.txt";
    char rel_fd_weight_path[]="";
    soft_repair(rel_tuple_path,rel_tuple_weight_path,rel_fd_path,rel_fd_weight_path);
    cout<<"--------------------soft repair--------------------"<<endl;
    
    /*for(int i=0;i<3;++i){
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path40[]="/home/hadoop/JMYAndy/Data/Flight/flight40000jmy0.050000.csv";
        char rel_tuple_weight_path40[]="/home/hadoop/JMYAndy/Data/Flight/flight40000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path40,rel_tuple_weight_path40,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path38[]="/home/hadoop/JMYAndy/Data/Flight/flight38000jmy0.050000.csv";
        char rel_tuple_weight_path38[]="/home/hadoop/JMYAndy/Data/Flight/flight38000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path38,rel_tuple_weight_path38,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path36[]="/home/hadoop/JMYAndy/Data/Flight/flight36000jmy0.050000.csv";
        char rel_tuple_weight_path36[]="/home/hadoop/JMYAndy/Data/Flight/flight36000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path36,rel_tuple_weight_path36,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path34[]="/home/hadoop/JMYAndy/Data/Flight/flight34000jmy0.050000.csv";
        char rel_tuple_weight_path34[]="/home/hadoop/JMYAndy/Data/Flight/flight34000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path34,rel_tuple_weight_path34,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path32[]="/home/hadoop/JMYAndy/Data/Flight/flight32000jmy0.050000.csv";
        char rel_tuple_weight_path32[]="/home/hadoop/JMYAndy/Data/Flight/flight32000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path32,rel_tuple_weight_path32,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path30[]="/home/hadoop/JMYAndy/Data/Flight/flight30000jmy0.050000.csv";
        char rel_tuple_weight_path30[]="/home/hadoop/JMYAndy/Data/Flight/flight30000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path30,rel_tuple_weight_path30,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path28[]="/home/hadoop/JMYAndy/Data/Flight/flight28000jmy0.050000.csv";
        char rel_tuple_weight_path28[]="/home/hadoop/JMYAndy/Data/Flight/flight28000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path28,rel_tuple_weight_path28,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path26[]="/home/hadoop/JMYAndy/Data/Flight/flight26000jmy0.050000.csv";
        char rel_tuple_weight_path26[]="/home/hadoop/JMYAndy/Data/Flight/flight26000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path26,rel_tuple_weight_path26,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path24[]="/home/hadoop/JMYAndy/Data/Flight/flight24000jmy0.050000.csv";
        char rel_tuple_weight_path24[]="/home/hadoop/JMYAndy/Data/Flight/flight24000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path24,rel_tuple_weight_path24,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path22[]="/home/hadoop/JMYAndy/Data/Flight/flight22000jmy0.050000.csv";
        char rel_tuple_weight_path22[]="/home/hadoop/JMYAndy/Data/Flight/flight22000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path22,rel_tuple_weight_path22,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path20[]="/home/hadoop/JMYAndy/Data/Flight/flight20000jmy0.050000.csv";
        char rel_tuple_weight_path20[]="/home/hadoop/JMYAndy/Data/Flight/flight20000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path20,rel_tuple_weight_path20,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path18[]="/home/hadoop/JMYAndy/Data/Flight/flight18000jmy0.050000.csv";
        char rel_tuple_weight_path18[]="/home/hadoop/JMYAndy/Data/Flight/flight18000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path18,rel_tuple_weight_path18,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path16[]="/home/hadoop/JMYAndy/Data/Flight/flight16000jmy0.050000.csv";
        char rel_tuple_weight_path16[]="/home/hadoop/JMYAndy/Data/Flight/flight16000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path16,rel_tuple_weight_path16,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path14[]="/home/hadoop/JMYAndy/Data/Flight/flight14000jmy0.050000.csv";
        char rel_tuple_weight_path14[]="/home/hadoop/JMYAndy/Data/Flight/flight14000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path14,rel_tuple_weight_path14,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path12[]="/home/hadoop/JMYAndy/Data/Flight/flight12000jmy0.050000.csv";
        char rel_tuple_weight_path12[]="/home/hadoop/JMYAndy/Data/Flight/flight12000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path12,rel_tuple_weight_path12,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
        cout<<"--------------------soft repair--------------------"<<endl;
        char rel_tuple_path10[]="/home/hadoop/JMYAndy/Data/Flight/flight10000jmy0.050000.csv";
        char rel_tuple_weight_path10[]="/home/hadoop/JMYAndy/Data/Flight/flight10000jmy0.050000weight.txt";
        //char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs.txt";
        //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/flight_FDs_weight.txt";
        soft_repair(rel_tuple_path10,rel_tuple_weight_path10,rel_fd_path,rel_fd_weight_path);
        cout<<"--------------------soft repair--------------------"<<endl;
        cout<<"----------------------第"<<i+1<<"次----------------------"<<endl;
    }*/
    
    
    return 0;
}




