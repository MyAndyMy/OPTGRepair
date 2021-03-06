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
void soft_repair(char* tuple_path,char* tuple_weight_path,char* fd_path,char* fd_weight_path,char* result_path,char* result_path_lp){
    clock_t start,end;//计算时间
    SoftResult temp_SR;
    /*--------------------以下加载元组--------------------*/
    char ch[]="r";
    Generator gen(tuple_path,ch);//加载tuple到gen.source_data中
    //gen.load_tuple_weight("",gen.source_tuple);//加载tuple's weight,当第一个参数为""时,默认每个元组的weight都是lambda
    gen.load_tuple_weight(tuple_weight_path,gen.source_tuple);//加载tuple's weight到gen.source_data中
    gen.print_attrs_name();//输出relational data全部属性名称
    /*--------------------以上加载元组--------------------*/
    //for(int i=0;i<gen.source_tuple.size();++i) cout<<"attention:"<<gen.source_tuple[i].tuple_weight<<endl;
    cout<<endl;
    
    double ropt_GRepair=0.0;
    double ropt_Greedy=0.0;
    double ropt_sc=0.0;
    
    //pollute_flight_data
    int start_nums=10000;
    int end_nums=40000;
    /*while(start_nums<=end_nums){
        gen.pollute_flight(gen.source_tuple,start_nums,0.05,"/Users/andy/Documents/Data/Flight/five_FDs/fli5ght",5);
        start_nums=start_nums+2000;
    }*/
    
    start_nums=10000;
    /*while(start_nums<=end_nums){
        gen.pollute_flight(gen.source_tuple,start_nums,0.05,"/Users/andy/Documents/Data/Flight/four_FDs/fli4ght",4);
        start_nums=start_nums+2000;
    }*/
    
    /*start_nums=10000;
    while(start_nums<=end_nums){
        gen.pollute_flight(gen.source_tuple,start_nums,0.05,"/Users/andy/Documents/Data/Flight/three_FDs/fli3ght",3);
        start_nums=start_nums+2000;
    }
    
    start_nums=10000;
    while(start_nums<=end_nums){
        gen.pollute_flight(gen.source_tuple,start_nums,0.05,"/Users/andy/Documents/Data/Flight/two_FDs/fli2ght",2);
        start_nums=start_nums+2000;
    }
    
    start_nums=10000;
    while(start_nums<=end_nums){
        gen.pollute_flight(gen.source_tuple,start_nums,0.05,"/Users/andy/Documents/Data/Flight/one_FD/fli1ght",1);
        start_nums=start_nums+2000;
    }
    //gen.pollute_flight(gen.source_tuple,32000,0.05,"/Users/andy/Documents/Data/Flight/four_FDs/fli4ght",4);
    */
    //gen.pollute_flight(gen.source_tuple,10000,0.05,"/Users/andy/Documents/Data/Flight/test_vector/",4);
    /*pollute_flight_data*/
    
    //gen.pollute_flight_graph(gen.source_tuple,1000,0.05,"/Users/andy/Documents/Data/Flight/test_graph/",6);
    
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
    for(int i=0;i<gen.relationship_G.size();++i){
        //gen.relationship_G[i].rel_weight=1.5;
        //gen.relationship_G[i].print_all_info();
        cout<<endl;
    }
    //gen.random_tuple_weight(gen.source_tuple);
    end=clock();
    cout<<"原始元组构建冲突对的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    /*--------------------以上构建原始元组的冲突对--------------------*/
    
    cout<<endl;
    
    /*--------------------以下打印元组相关信息--------------------*/
    cout<<"元组数量:"<<gen.number_of_tuples(gen.source_tuple)<<endl;
    temp_SR.nums_tuples=gen.number_of_tuples(gen.source_tuple);
    //cout<<"temp_SR.nums_tuples:"<<temp_SR.nums_tuples<<endl;
    cout<<"元组中参与冲突的元组数量:"<<gen.number_of_tuples_in_conflict(gen.source_tuple)<<endl;
    temp_SR.tuple_in_conflicts=gen.number_of_tuples_in_conflict(gen.source_tuple);
    //cout<<"temp_SR.tuple_in_conflicts:"<<temp_SR.tuple_in_conflicts<<endl;
    cout<<"冲突对儿数量:"<<gen.number_of_conflicts(gen.relationship_G)<<endl;
    temp_SR.nums_conflicts=gen.number_of_conflicts(gen.relationship_G);
    //cout<<"temp_SR.nums_conflicts:"<<temp_SR.nums_conflicts<<endl;
    cout<<"元组错误率:"<<gen.print_error_percentage(gen.source_tuple)<<endl;
    temp_SR.error_rate=gen.print_error_percentage(gen.source_tuple);
    /*--------------------以上打印元组相关信息--------------------*/
    
    cout<<endl;
    
    /*--------------------以下对source_tuple和relationship_G进行G-repair--------------------*/
    start=clock();
    gen.lp_solver_G_repair(gen.source_tuple,gen.relationship_G);
    end=clock();
    cout<<"soft repair的G_repair的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    temp_SR.Grepair_time=(double)(end - start)/CLOCKS_PER_SEC;
    if(gen.effectiveness_of_solution_G(gen.relationship_G)==true)
        cout<<"G_repair解有效,"<<"w>0时,y_i,j=min(xi+xj,1);w<0时,y_i,j=max(xi,xj)"<<endl;
    else cout<<"G_repair解无效"<<endl;
    start=clock();
    gen.rounding(gen.source_tuple,gen.relationship_G);//为normalized_tuple的x和relationship_G的y做舍入
    end=clock();
    cout<<"soft repair的G_repair的rounding时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    cout<<"soft repair的G_repair的分数最优解为:"<<gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G)<<endl;
    temp_SR.Grepair_fopt=gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G);
    cout<<"soft repair的G_repair的整数近似最优解为:"<<gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G)<<endl;
    ropt_GRepair=gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G);
    temp_SR.Grepair_ropt=gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G);
    gen.construct_left_tuple(gen.source_tuple);//把留在repair中的元组放到gen.left_tuple中
    cout<<"soft repair的G_repair留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.construct_left_conflict(gen.left_tuple, gen.relationship_S);
    temp_SR.Grepair_tleft=gen.number_of_tuples(gen.left_tuple);
    cout<<"soft repair的G_repair留下的元组冲突对个数:"<<gen.number_of_conflicts(gen.relationship_S)<<endl;
    temp_SR.Grepair_cleft=gen.number_of_conflicts(gen.relationship_S);
    cout<<"soft repair的G_repair留下的元组冲突对加权和:"<<gen.weighted_left_conflicts(gen.relationship_S)<<endl;
    temp_SR.Grepair_wcleft=gen.weighted_left_conflicts(gen.relationship_S);
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
    /*for(int i=0;i<gen.UU.size();++i){
        cout<<gen.UU[i]<<endl;
    }
    cout<<"hhh"<<endl;
    for(int i=0;i<gen.C.size();++i){
        for(int j=0;j<gen.C[i].size();++j){
            cout<<gen.C[i][j]<<", "<<endl;
        }
    }*/
    //为了求sc_lp而注释
    res=gen.greedy_sc(gen.UU,gen.C);
    end=clock();
    cout<<"soft repair的贪心算法时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    temp_SR.Greedy_time=(double)(end - start)/CLOCKS_PER_SEC;
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
    ropt_Greedy=gen.greedy_opt;
    temp_SR.Greedy_ropt=gen.greedy_opt;
    cout<<"soft repair的贪心算法留下的元组个数:"<<num_aft0<<endl;
    temp_SR.Greedy_tleft=num_aft0;
    cout<<"soft repair的贪心算法留下的元组冲突对个数:"<<num_bef1-num_aft1<<endl;
    temp_SR.Greedy_cleft=num_bef1-num_aft1;
    cout<<"soft repair的贪心算法留下的元组冲突对加权和:"<<bef1-aft1<<endl;
    temp_SR.Greedy_wcleft=bef1-aft1;
    gen.SR.push_back(temp_SR);
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
    
    //为了求sc_lp而注释
    gen.write_soft_result(result_path);
    /*--------------------以上是set cover问题的贪心算法----------------*/
    /*for(int i=0;i<gen.UU_lp.size();++i){
        cout<<gen.UU[i]<<endl;
    }
    cout<<"sss"<<endl;
    for(int i=0;i<gen.C_lp.size();++i){
        for(int j=0;j<gen.C_lp[i].size();++j){
            cout<<gen.C_lp[i][j]<<", "<<endl;
        }
    }*/
    
    vector<vector<int>> res_lp;
    /*for(int i=0;i<gen.UU.size();++i){
        cout<<gen.UU[i]<<endl;
    }
    cout<<"hhh"<<endl;
    for(int i=0;i<gen.C.size();++i){
        for(int j=0;j<gen.C[i].size();++j){
            cout<<gen.C[i][j]<<", "<<endl;
        }
    }*/
    gen.left_tuple.clear();
    start=clock();
    res_lp=gen.lp_sc(gen.UU_lp,gen.C_lp);
    end=clock();
    
    /*test*/
    cout<<"soft repair set cover留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.construct_left_conflict(gen.left_tuple, gen.relationship_S);
    cout<<"soft repair set cover留下的元组冲突对个数:"<<gen.number_of_conflicts(gen.relationship_S)<<endl;
    cout<<"soft repair set cover留下的元组冲突对加权和:"<<gen.weighted_left_conflicts(gen.relationship_S)<<endl;
    /*test*/
    
    gen.sc_lp_time=(double)(end - start)/CLOCKS_PER_SEC;
    cout<<"soft repair set cover LP 算法时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    cout<<"soft repair set cover LP cost:"<<gen.soft_lp_opt<<endl;
    ropt_sc=gen.soft_lp_opt;
    cout<<"soft repair set cover LP 被删元组个数:"<<gen.soft_lp_tuple<<endl;
    cout<<"soft repair set cover LP 被删元组加权和:"<<gen.soft_lp_wtuple<<endl;
    cout<<"soft repair set cover LP 留下冲突个数:"<<gen.soft_lp_conflict<<endl;
    cout<<"soft repair set cover LP 留下冲突加权和:"<<gen.soft_lp_wconflict<<endl;
    //20220503 11:30改到这里！
    gen.write_soft_result_lp(result_path_lp);
    
    
    cout<<"soft repair的G_repair的整数近似最优解为:"<<ropt_GRepair<<endl;
    cout<<"soft repair的贪心整数近似最优解为:"<<ropt_Greedy<<endl;
    cout<<"soft repair set cover LP cost:"<<ropt_sc<<endl;
}

/*--------------------以下将soft_repair属性名称写到csv文件里----------------*/
void write_soft_result_name(char* soft_repair_result_name_path){
    //
    FILE *fp;
    fp = fopen(soft_repair_result_name_path, "w");
    if(fp==NULL) cout<<"大失败！"<<endl;
    else{
        cout<<"大成功！"<<endl;
        fprintf(fp,"%s,","nums_tuples");
        fprintf(fp,"%s,","tuple_in_conflicts");
        fprintf(fp,"%s,","nums_conflicts");
        fprintf(fp,"%s,","error_rate");
        fprintf(fp,"%s,","Grepair_time");
        fprintf(fp,"%s,","Grepair_fopt");
        fprintf(fp,"%s,","Grepair_ropt");
        fprintf(fp,"%s,","Grepair_tleft");
        fprintf(fp,"%s,","Grepair_cleft");
        fprintf(fp,"%s,","Grepair_wcleft");
        fprintf(fp,"%s,","Greedy_time");
        fprintf(fp,"%s,","Greedy_ropt");
        fprintf(fp,"%s,","Greedy_tleft");
        fprintf(fp,"%s,","Greedy_cleft");
        fprintf(fp,"%s\n","Greedy_wcleft");
    }
    fclose (fp);
}
/*--------------------以上将soft_repair属性名称写到csv文件里----------------*/

/*--------------------以下在hadoop执行soft repair6FD----------------*/
void soft_repair_6FDs_hadoop(int start,int end){
    string rel_tuple_path="/home/hadoop/JMYAndy/Data/Flight/six_FDs/flight";
    string rel_tuple_weight_path="/home/hadoop/JMYAndy/Data/Flight/six_FDs/flight";
    char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/six_FDs/flight_FDs.txt";
    char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/six_FDs/flight_FDs_weight.txt";
    char result_path[]="/home/hadoop/JMYAndy/Data/results/result_6FD.csv";
    
    write_soft_result_name(result_path);
    
    int nums=start;
    string post="jmy0.050000.csv";
    string weight_post="jmy0.050000weight.txt";
    
    while(nums<=end){
        string middle=to_string(nums);
        rel_tuple_path=rel_tuple_path+middle+post;
        //cout<<"xixi:"<<rel_tuple_path<<endl;
        
        rel_tuple_weight_path=rel_tuple_weight_path+middle+weight_post;
        //cout<<"xixi:"<<rel_tuple_weight_path<<endl;
        
        nums=nums+2000;


        //char ch1[100];
        //char ch2[100];
        //strcpy(ch1,rel_tuple_path.c_str());
        //strcpy(ch2,rel_tuple_weight_path.c_str());
        char c1[68];//6FD和5FD是不一样的喔！！！
        char c2[74];//6FD和5FD是不一样的喔！！！
        int i=0;
        for(i=0;i<rel_tuple_path.size();++i){
            c1[i]=rel_tuple_path[i];
        }
        c1[i]='\0';

        i=0;
        for(i=0;i<rel_tuple_weight_path.size();++i){
            c2[i]=rel_tuple_weight_path[i];
        }
        c2[i]='\0';
        //soft_repair(rel_tuple_path.c_str(),rel_tuple_weight_path.c_str(),rel_fd_path,rel_fd_weight_path,result_path);
        soft_repair(c1,c2,rel_fd_path,rel_fd_weight_path,result_path,"/home/hadoop/JMYAndy/Data/results/result_6FD_lp.csv");
        
        
        rel_tuple_path.erase(47, 20);
        rel_tuple_weight_path.erase(47, 26);
    }
}
/*--------------------以上在hadoop执行soft repair6FD----------------*/

/*--------------------以下在hadoop执行soft repair5FD----------------*/
void soft_repair_5FDs_hadoop(int start,int end){
    string rel_tuple_path="/home/hadoop/JMYAndy/Data/Flight/five_FDs/fli5ght";
    string rel_tuple_weight_path="/home/hadoop/JMYAndy/Data/Flight/five_FDs/fli5ght";
    char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/five_FDs/flight_FDs5.txt";
    char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/five_FDs/flight_FDs_weight5.txt";
    char result_path[]="/home/hadoop/JMYAndy/Data/results/result_5FD.csv";
    
    write_soft_result_name(result_path);
    
    int nums=start;
    string post="jmy0.050000.csv";
    string weight_post="jmy0.050000weight.txt";
    
    while(nums<=end){
        string middle=to_string(nums);
        rel_tuple_path=rel_tuple_path+middle+post;
        //cout<<"xixi:"<<rel_tuple_path<<endl;
        
        rel_tuple_weight_path=rel_tuple_weight_path+middle+weight_post;
        //cout<<"xixi:"<<rel_tuple_weight_path<<endl;
        
        nums=nums+2000;


        //char ch1[100];
        //char ch2[100];
        //strcpy(ch1,rel_tuple_path.c_str());
        //strcpy(ch2,rel_tuple_weight_path.c_str());
        char c1[70];//6FD和5FD是不一样的喔！！！
        char c2[76];//6FD和5FD是不一样的喔！！！
        int i=0;
        for(i=0;i<rel_tuple_path.size();++i){
            c1[i]=rel_tuple_path[i];
        }
        c1[i]='\0';

        i=0;
        for(i=0;i<rel_tuple_weight_path.size();++i){
            c2[i]=rel_tuple_weight_path[i];
        }
        c2[i]='\0';
        //soft_repair(rel_tuple_path.c_str(),rel_tuple_weight_path.c_str(),rel_fd_path,rel_fd_weight_path,result_path);
        soft_repair(c1,c2,rel_fd_path,rel_fd_weight_path,result_path,"/home/hadoop/JMYAndy/Data/results/result_5FD_lp.csv");
        
        //cout<<"xixi:"<<c1<<endl;
        //cout<<"xixi:"<<c2<<endl;
        rel_tuple_path.erase(49, 20);
        rel_tuple_weight_path.erase(48, 26);
    }
}
/*--------------------以上在hadoop执行soft repair5FD----------------*/

/*--------------------以下在hadoop执行soft repair4FD----------------*/
void soft_repair_4FDs_hadoop(int start,int end,char* rel_fd_weight_path,char* result_path,char *result_path_lp){
    string rel_tuple_path="/home/hadoop/JMYAndy/Data/Flight/four_FDs/fli4ght";
    string rel_tuple_weight_path="/home/hadoop/JMYAndy/Data/Flight/four_FDs/fli4ght";
    char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/four_FDs/flight_FDs4.txt";
    //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/four_FDs/flight_FDs_weight4.txt";
    //char result_path[]="/home/hadoop/JMYAndy/Data/results/result_4FD.csv";
    
    write_soft_result_name(result_path);
    
    int nums=start;
    string post="jmy0.050000.csv";
    string weight_post="jmy0.050000weight.txt";
    
    while(nums<=end){
        string middle=to_string(nums);
        rel_tuple_path=rel_tuple_path+middle+post;
        //cout<<"xixi:"<<rel_tuple_path<<endl;
        
        rel_tuple_weight_path=rel_tuple_weight_path+middle+weight_post;
        //cout<<"xixi:"<<rel_tuple_weight_path<<endl;
        
        nums=nums+2000;


        //char ch1[100];
        //char ch2[100];
        //strcpy(ch1,rel_tuple_path.c_str());
        //strcpy(ch2,rel_tuple_weight_path.c_str());
        char c1[70];//6FD和5FD是不一样的喔！！！
        char c2[76];//6FD和5FD是不一样的喔！！！
        int i=0;
        for(i=0;i<rel_tuple_path.size();++i){
            c1[i]=rel_tuple_path[i];
        }
        c1[i]='\0';

        i=0;
        for(i=0;i<rel_tuple_weight_path.size();++i){
            c2[i]=rel_tuple_weight_path[i];
        }
        c2[i]='\0';
        //soft_repair(rel_tuple_path.c_str(),rel_tuple_weight_path.c_str(),rel_fd_path,rel_fd_weight_path,result_path);
        soft_repair(c1,c2,rel_fd_path,rel_fd_weight_path,result_path,result_path_lp);
        
        //cout<<"xixi:"<<c1<<endl;
        //cout<<"xixi:"<<c2<<endl;
        
        rel_tuple_path.erase(49, 20);
        rel_tuple_weight_path.erase(48, 26);
    }
}
/*--------------------以上在hadoop执行soft repair4FD----------------*/

/*--------------------以下在hadoop执行soft repair3FD----------------*/
void soft_repair_3FDs_hadoop(int start,int end){
    string rel_tuple_path="/home/hadoop/JMYAndy/Data/Flight/three_FDs/fli3ght";
    string rel_tuple_weight_path="/home/hadoop/JMYAndy/Data/Flight/three_FDs/fli3ght";
    char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/three_FDs/flight_FDs3.txt";
    char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/three_FDs/flight_FDs_weight3.txt";
    char result_path[]="/home/hadoop/JMYAndy/Data/results/result_3FD.csv";
    
    write_soft_result_name(result_path);
    
    int nums=start;
    string post="jmy0.050000.csv";
    string weight_post="jmy0.050000weight.txt";
    
    while(nums<=end){
        string middle=to_string(nums);
        rel_tuple_path=rel_tuple_path+middle+post;
        //cout<<"xixi:"<<rel_tuple_path<<endl;
        
        rel_tuple_weight_path=rel_tuple_weight_path+middle+weight_post;
        //cout<<"xixi:"<<rel_tuple_weight_path<<endl;
        
        nums=nums+2000;


        //char ch1[100];
        //char ch2[100];
        //strcpy(ch1,rel_tuple_path.c_str());
        //strcpy(ch2,rel_tuple_weight_path.c_str());
        char c1[71];//6FD和5FD是不一样的喔！！！
        char c2[77];//6FD和5FD是不一样的喔！！！
        int i=0;
        for(i=0;i<rel_tuple_path.size();++i){
            c1[i]=rel_tuple_path[i];
        }
        c1[i]='\0';

        i=0;
        for(i=0;i<rel_tuple_weight_path.size();++i){
            c2[i]=rel_tuple_weight_path[i];
        }
        c2[i]='\0';
        //soft_repair(rel_tuple_path.c_str(),rel_tuple_weight_path.c_str(),rel_fd_path,rel_fd_weight_path,result_path);
        soft_repair(c1,c2,rel_fd_path,rel_fd_weight_path,result_path,"/home/hadoop/JMYAndy/Data/results/result_3FD_lp.csv");
        
        //cout<<"xixi:"<<c1<<endl;
        //cout<<"xixi:"<<c2<<endl;
        
        rel_tuple_path.erase(50, 20);
        rel_tuple_weight_path.erase(49, 26);
    }
}
/*--------------------以上在hadoop执行soft repair3FD----------------*/

/*--------------------以下在hadoop执行soft repair2FD----------------*/
void soft_repair_2FDs_hadoop(int start,int end){
    string rel_tuple_path="/home/hadoop/JMYAndy/Data/Flight/two_FDs/fli2ght";
    string rel_tuple_weight_path="/home/hadoop/JMYAndy/Data/Flight/two_FDs/fli2ght";
    char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/two_FDs/flight_FDs2.txt";
    char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/two_FDs/flight_FDs_weight2.txt";
    char result_path[]="/home/hadoop/JMYAndy/Data/results/result_2FD.csv";
    
    write_soft_result_name(result_path);
    
    int nums=start;
    string post="jmy0.050000.csv";
    string weight_post="jmy0.050000weight.txt";
    
    while(nums<=end){
        string middle=to_string(nums);
        rel_tuple_path=rel_tuple_path+middle+post;
        //cout<<"xixi:"<<rel_tuple_path<<endl;
        
        rel_tuple_weight_path=rel_tuple_weight_path+middle+weight_post;
        //cout<<"xixi:"<<rel_tuple_weight_path<<endl;
        
        nums=nums+2000;


        //char ch1[100];
        //char ch2[100];
        //strcpy(ch1,rel_tuple_path.c_str());
        //strcpy(ch2,rel_tuple_weight_path.c_str());
        char c1[69];//6FD和5FD是不一样的喔！！！
        char c2[75];//6FD和5FD是不一样的喔！！！
        int i=0;
        for(i=0;i<rel_tuple_path.size();++i){
            c1[i]=rel_tuple_path[i];
        }
        c1[i]='\0';

        i=0;
        for(i=0;i<rel_tuple_weight_path.size();++i){
            c2[i]=rel_tuple_weight_path[i];
        }
        c2[i]='\0';
        //soft_repair(rel_tuple_path.c_str(),rel_tuple_weight_path.c_str(),rel_fd_path,rel_fd_weight_path,result_path);
        soft_repair(c1,c2,rel_fd_path,rel_fd_weight_path,result_path,"/home/hadoop/JMYAndy/Data/results/result_2FD_lp.csv");
        
        //cout<<"xixi:"<<c1<<endl;
        //cout<<"xixi:"<<c2<<endl;
        
        rel_tuple_path.erase(48, 20);
        rel_tuple_weight_path.erase(47, 26);
    }
}
/*--------------------以上在hadoop执行soft repair2FD----------------*/

/*--------------------以下在hadoop执行soft repair1FD----------------*/
void soft_repair_1FD_hadoop(int start,int end,char* rel_fd_weight_path,char* result_path,char *result_path_lp){
    string rel_tuple_path="/home/hadoop/JMYAndy/Data/Flight/one_FD/fli1ght";
    string rel_tuple_weight_path="/home/hadoop/JMYAndy/Data/Flight/one_FD/fli1ght";
    char rel_fd_path[]="/home/hadoop/JMYAndy/Data/Flight/one_FD/flight_FDs1.txt";
    //char rel_fd_weight_path[]="/home/hadoop/JMYAndy/Data/Flight/one_FD/flight_FDs_weight1.txt";
    //char result_path[]="/home/hadoop/JMYAndy/Data/results/result_1FD.csv";
    
    write_soft_result_name(result_path);
    
    int nums=start;
    string post="jmy0.050000.csv";
    string weight_post="jmy0.050000weight.txt";
    
    while(nums<=end){
        string middle=to_string(nums);
        rel_tuple_path=rel_tuple_path+middle+post;
        //cout<<"xixi:"<<rel_tuple_path<<endl;
        
        rel_tuple_weight_path=rel_tuple_weight_path+middle+weight_post;
        //cout<<"xixi:"<<rel_tuple_weight_path<<endl;
        
        nums=nums+2000;


        //char ch1[100];
        //char ch2[100];
        //strcpy(ch1,rel_tuple_path.c_str());
        //strcpy(ch2,rel_tuple_weight_path.c_str());
        char c1[68];//6FD和5FD是不一样的喔！！！
        char c2[74];//6FD和5FD是不一样的喔！！！
        int i=0;
        for(i=0;i<rel_tuple_path.size();++i){
            c1[i]=rel_tuple_path[i];
        }
        c1[i]='\0';

        i=0;
        for(i=0;i<rel_tuple_weight_path.size();++i){
            c2[i]=rel_tuple_weight_path[i];
        }
        c2[i]='\0';
        //soft_repair(rel_tuple_path.c_str(),rel_tuple_weight_path.c_str(),rel_fd_path,rel_fd_weight_path,result_path);
        soft_repair(c1,c2,rel_fd_path,rel_fd_weight_path,result_path,result_path_lp);
        
        //cout<<"xixi:"<<c1<<endl;
        //cout<<"xixi:"<<c2<<endl;
        
        rel_tuple_path.erase(47, 20);
        rel_tuple_weight_path.erase(46, 26);
    }
}
/*--------------------以上在hadoop执行soft repair1FD----------------*/

/*--------------------以下为了在hadoop上surprise文件，好悲桑----------------*/
void soft_repair_test(char* tuple_path,char* tuple_weight_path,char* fd_path,char* fd_weight_path,int a,int b,int count){
    /*--------------------以下加载元组--------------------*/
    char ch[]="r";
    Generator gen(tuple_path,ch);//加载tuple到gen.source_data中
    //gen.load_tuple_weight("",gen.source_tuple);//加载tuple's weight,当第一个参数为""时,默认每个元组的weight都是lambda
    gen.load_tuple_weight(tuple_weight_path,gen.source_tuple);//加载tuple's weight到gen.source_data中
    gen.print_attrs_name();//输出relational data全部属性名称
    /*--------------------以上加载元组--------------------*/
    //for(int i=0;i<gen.source_tuple.size();++i) cout<<"attention:"<<gen.source_tuple[i].tuple_weight<<endl;
    cout<<endl;
    
    /*--------------------以下加载FD--------------------*/
    gen.load_fd_file(fd_path);//加载FD到gen.fun_denp
    //gen.load_fd_weight("",gen.fun_denp);//加载fd's weight,当第一个参数为""时,默认每个元组的FD都是w_conflict
    gen.load_fd_weight(fd_weight_path,gen.fun_denp);//加载fd's weight到到gen.fun_denp
    cout<<"FD数量:"<<gen.number_of_fds()<<endl;
    gen.print_every_fds();//输出全部FD
    /*--------------------以上加载FD--------------------*/
    
    
        SoftResult temp_SR;
        clock_t start,end;//计算时间
        //在这里改FD的weight
        int r=rand();//生成随机数[1,10]
        int test_FD_weight0=(rand() % (b-a+1))+ a;
        gen.fun_denp[0].fd_weight=test_FD_weight0;
        cout<<"随机生成的fd_weight0:"<<test_FD_weight0<<endl;
        
        r=rand();//生成随机数[1,10]
        int test_FD_weight1=(rand() % (b-a+1))+ a;
        gen.fun_denp[1].fd_weight=test_FD_weight1;
        cout<<"随机生成的fd_weight1:"<<test_FD_weight1<<endl;
        
        r=rand();//生成随机数[1,10]
        int test_FD_weight2=(rand() % (b-a+1))+ a;
        gen.fun_denp[2].fd_weight=test_FD_weight2;
        cout<<"随机生成的fd_weight2:"<<test_FD_weight2<<endl;
        
        r=rand();//生成随机数[1,10]
        int test_FD_weight3=(rand() % (b-a+1))+ a;
        gen.fun_denp[3].fd_weight=test_FD_weight3;
        cout<<"随机生成的fd_weight3:"<<test_FD_weight3<<endl;
        
    //pollute_flight_data
    int start_nums=10000;
    int end_nums=40000;
    /*while(start_nums<=end_nums){
        gen.pollute_flight(gen.source_tuple,start_nums,0.05,"/Users/andy/Documents/Data/Flight/five_FDs/fli5ght",5);
        start_nums=start_nums+2000;
    }*/
    
    start_nums=10000;
    /*while(start_nums<=end_nums){
        gen.pollute_flight(gen.source_tuple,start_nums,0.05,"/Users/andy/Documents/Data/Flight/four_FDs/fli4ght",4);
        start_nums=start_nums+2000;
    }*/
    
    /*start_nums=10000;
    while(start_nums<=end_nums){
        gen.pollute_flight(gen.source_tuple,start_nums,0.05,"/Users/andy/Documents/Data/Flight/three_FDs/fli3ght",3);
        start_nums=start_nums+2000;
    }
    
    start_nums=10000;
    while(start_nums<=end_nums){
        gen.pollute_flight(gen.source_tuple,start_nums,0.05,"/Users/andy/Documents/Data/Flight/two_FDs/fli2ght",2);
        start_nums=start_nums+2000;
    }
    
    start_nums=10000;
    while(start_nums<=end_nums){
        gen.pollute_flight(gen.source_tuple,start_nums,0.05,"/Users/andy/Documents/Data/Flight/one_FD/fli1ght",1);
        start_nums=start_nums+2000;
    }
    //gen.pollute_flight(gen.source_tuple,32000,0.05,"/Users/andy/Documents/Data/Flight/four_FDs/fli4ght",4);
    
    //gen.pollute_flight(gen.source_tuple,10000,0.05,"/Users/andy/Documents/Data/Flight/five_FDs/",5);
    /*pollute_flight_data*/
    
    
    
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
    temp_SR.nums_tuples=gen.number_of_tuples(gen.source_tuple);
    //cout<<"temp_SR.nums_tuples:"<<temp_SR.nums_tuples<<endl;
    cout<<"元组中参与冲突的元组数量:"<<gen.number_of_tuples_in_conflict(gen.source_tuple)<<endl;
    temp_SR.tuple_in_conflicts=gen.number_of_tuples_in_conflict(gen.source_tuple);
    //cout<<"temp_SR.tuple_in_conflicts:"<<temp_SR.tuple_in_conflicts<<endl;
    cout<<"冲突对儿数量:"<<gen.number_of_conflicts(gen.relationship_G)<<endl;
    temp_SR.nums_conflicts=gen.number_of_conflicts(gen.relationship_G);
    //cout<<"temp_SR.nums_conflicts:"<<temp_SR.nums_conflicts<<endl;
    cout<<"元组错误率:"<<gen.print_error_percentage(gen.source_tuple)<<endl;
    temp_SR.error_rate=gen.print_error_percentage(gen.source_tuple);
    /*--------------------以上打印元组相关信息--------------------*/
    
    cout<<endl;
    
    /*--------------------以下对source_tuple和relationship_G进行G-repair--------------------*/
    start=clock();
    gen.lp_solver_G_repair(gen.source_tuple,gen.relationship_G);
    end=clock();
    cout<<"soft repair的G_repair的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    temp_SR.Grepair_time=(double)(end - start)/CLOCKS_PER_SEC;
    if(gen.effectiveness_of_solution_G(gen.relationship_G)==true)
        cout<<"G_repair解有效,"<<"w>0时,y_i,j=min(xi+xj,1);w<0时,y_i,j=max(xi,xj)"<<endl;
    else cout<<"G_repair解无效"<<endl;
    start=clock();
    gen.rounding(gen.source_tuple,gen.relationship_G);//为normalized_tuple的x和relationship_G的y做舍入
    end=clock();
    cout<<"soft repair的G_repair的rounding时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    cout<<"soft repair的G_repair的分数最优解为:"<<gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G)<<endl;
    temp_SR.Grepair_fopt=gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G);
    cout<<"soft repair的G_repair的整数近似最优解为:"<<gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G)<<endl;
    temp_SR.Grepair_ropt=gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G);
    gen.construct_left_tuple(gen.source_tuple);//把留在repair中的元组放到gen.left_tuple中
    cout<<"soft repair的G_repair留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.construct_left_conflict(gen.left_tuple, gen.relationship_S);
    temp_SR.Grepair_tleft=gen.number_of_tuples(gen.left_tuple);
    cout<<"soft repair的G_repair留下的元组冲突对个数:"<<gen.number_of_conflicts(gen.relationship_S)<<endl;
    temp_SR.Grepair_cleft=gen.number_of_conflicts(gen.relationship_S);
    cout<<"soft repair的G_repair留下的元组冲突对加权和:"<<gen.weighted_left_conflicts(gen.relationship_S)<<endl;
    temp_SR.Grepair_wcleft=gen.weighted_left_conflicts(gen.relationship_S);
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
    /*for(int i=0;i<gen.UU.size();++i){
        cout<<gen.UU[i]<<endl;
    }
    cout<<"hhh"<<endl;
    for(int i=0;i<gen.C.size();++i){
        for(int j=0;j<gen.C[i].size();++j){
            cout<<gen.C[i][j]<<", "<<endl;
        }
    }*/
    //为了求sc_lp而注释
    res=gen.greedy_sc(gen.UU,gen.C);
    end=clock();
    cout<<"soft repair的贪心算法时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    temp_SR.Greedy_time=(double)(end - start)/CLOCKS_PER_SEC;
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
    temp_SR.Greedy_ropt=gen.greedy_opt;
    cout<<"soft repair的贪心算法留下的元组个数:"<<num_aft0<<endl;
    temp_SR.Greedy_tleft=num_aft0;
    cout<<"soft repair的贪心算法留下的元组冲突对个数:"<<num_bef1-num_aft1<<endl;
    temp_SR.Greedy_cleft=num_bef1-num_aft1;
    cout<<"soft repair的贪心算法留下的元组冲突对加权和:"<<bef1-aft1<<endl;
    temp_SR.Greedy_wcleft=bef1-aft1;
    gen.SR.push_back(temp_SR);
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
    
    gen.UU.clear();
    gen.C.clear();
    //为了求sc_lp而注释
    
    /*--------------------以上是set cover问题的贪心算法----------------*/
    /*for(int i=0;i<gen.UU_lp.size();++i){
        cout<<gen.UU[i]<<endl;
    }
    cout<<"sss"<<endl;
    for(int i=0;i<gen.C_lp.size();++i){
        for(int j=0;j<gen.C_lp[i].size();++j){
            cout<<gen.C_lp[i][j]<<", "<<endl;
        }
    }*/
    
    vector<vector<int>> res_lp;
    /*for(int i=0;i<gen.UU.size();++i){
        cout<<gen.UU[i]<<endl;
    }
    cout<<"hhh"<<endl;
    for(int i=0;i<gen.C.size();++i){
        for(int j=0;j<gen.C[i].size();++j){
            cout<<gen.C[i][j]<<", "<<endl;
        }
    }*/
    start=clock();
    res_lp=gen.lp_sc(gen.UU_lp,gen.C_lp);
    end=clock();
    
    /*test*/
    cout<<"soft repair set cover留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.construct_left_conflict(gen.left_tuple, gen.relationship_S);
    cout<<"soft repair set cover留下的元组冲突对个数:"<<gen.number_of_conflicts(gen.relationship_S)<<endl;
    if(gen.number_of_conflicts(gen.relationship_S)>0){
        cout<<"终于啊终于啊终于等到你!"<<endl;
        //exit(0);
    }
    cout<<"soft repair set cover留下的元组冲突对加权和:"<<gen.weighted_left_conflicts(gen.relationship_S)<<endl;
    /*test*/
    
    gen.sc_lp_time=(double)(end - start)/CLOCKS_PER_SEC;
    cout<<"soft repair set cover LP 算法时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    cout<<"soft repair set cover LP cost:"<<gen.soft_lp_opt<<endl;
    cout<<"soft repair set cover LP 被删元组个数:"<<gen.soft_lp_tuple<<endl;
    cout<<"soft repair set cover LP 被删元组加权和:"<<gen.soft_lp_wtuple<<endl;
    cout<<"soft repair set cover LP 留下冲突个数:"<<gen.soft_lp_conflict<<endl;
    cout<<"soft repair set cover LP 留下冲突加权和:"<<gen.soft_lp_wconflict<<endl;
    //20220503 11:30改到这里！
    gen.left_tuple.clear();
    gen.relationship_left.clear();
    gen.relationship_S.clear();
        
    
    vector<vector<int>> surprise;
    double g1,g2;
    
        g1=gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G);
        g2=gen.greedy_opt;
        if(g2-g1>100||gen.number_of_conflicts(gen.relationship_S)>0||gen.soft_lp_conflict>0){
            vector<int> temp;
            temp.push_back(gen.fun_denp[0].fd_weight);
            temp.push_back(gen.fun_denp[1].fd_weight);
            temp.push_back(gen.fun_denp[2].fd_weight);
            temp.push_back(gen.fun_denp[4].fd_weight);
            surprise.push_back(temp);
            
            string result_path="surprise"+to_string(count);
            string result_path_lp="surprise_lp"+to_string(count);
            
            char c1[13];//6FD和5FD是不一样的喔！！！
            char c2[16];//6FD和5FD是不一样的喔！！！
            int ii=0;
            for(ii=0;ii<result_path.size();++ii){
                c1[ii]=result_path[ii];
            }
            c1[ii]='\0';

            ii=0;
            for(ii=0;ii<result_path_lp.size();++ii){
                c2[ii]=result_path_lp[ii];
            }
            c2[ii]='\0';
            
            gen.write_soft_result(c1);
            gen.write_soft_result_lp(c2);
        }
        
}
/*--------------------以下为了在hadoop上surprise文件，好悲桑----------------*/

/*--------------------以下为了在hadoop上生成数据，存surprise，但没有好结果能存到surprise里----------------*/
void test_pollte_data_on_hadooop(char * tuple_path, char* fd_path,char* result_path){
    int size=10000;
    double err=0.05;
    int fd=6;
    int p=size*err;
    double shift=1;
    while(shift<=1.3){
        //cout<<"第shift="<<shift<<"次"<<endl;
        clock_t start,end;//计算时间
        SoftResult temp_SR;
        /*--------------------以下加载元组--------------------*/
        char ch[]="r";
        Generator gen(tuple_path,ch);
        gen.load_tuple_weight("",gen.source_tuple);
        gen.print_attrs_name();//输出relational data全部属性名称
        /*--------------------以上加载元组--------------------*/
        /*--------------------以下加载FD--------------------*/
        gen.load_fd_file(fd_path);//加载FD到gen.fun_denp
        //gen.load_fd_weight("",gen.fun_denp);//加载fd's weight,当第一个参数为""时,默认每个元组的FD都是w_conflict
        gen.load_fd_weight("",gen.fun_denp);//加载fd's weight到到gen.fun_denp
        cout<<"FD数量:"<<gen.number_of_fds()<<endl;
        
        /*--------------------以上加载FD--------------------*/
        gen.pollute_flight_on_hadoop(gen.source_tuple,size,err,fd,p);
        p=p*shift;
        //在此给每个FD随机赋值哦！
        int r=rand();//生成随机数(0,5](rand() % (b-a))+ a + 1
        double t=(r % 5)+ 1;
        if(t>5||t<0){
            gen.fun_denp[0].fd_weight=2.5;
        }
        else{
            gen.fun_denp[0].fd_weight=t;
        }
        gen.fun_denp[1].fd_weight=400000;
        r=rand();//生成随机数(0,5](rand() % (b-a))+ a + 1
        t=(r % 5)+ 1;
        if(t>5||t<0){
            gen.fun_denp[2].fd_weight=2.5;
        }
        else{
            gen.fun_denp[2].fd_weight=t;
        }
        gen.fun_denp[3].fd_weight=400000;
        r=rand();//生成随机数(0,5](rand() % (b-a))+ a + 1
        t=(r % 5)+ 1;
        if(t>5||t<0){
            gen.fun_denp[4].fd_weight=2.5;
        }
        else{
            gen.fun_denp[4].fd_weight=t;
        }
        r=rand();//生成随机数(0,5](rand() % (b-a))+ a + 1
        t=(r % 5)+ 1;
        if(t>5||t<0){
            gen.fun_denp[5].fd_weight=2.5;
        }
        else{
            gen.fun_denp[5].fd_weight=t;
        }
        gen.print_every_fds();//输出全部FD
        /*--------------------以下构建原始元组的冲突对--------------------*/
        start=clock();
        gen.construct_conflict(gen.source_tuple,gen.relationship_G,ch);//将source_tuple的冲突对关系存储在relationship_G中
        //gen.random_tuple_weight(gen.source_tuple);
        end=clock();
        //cout<<"cuole"<<endl;
        cout<<"原始元组构建冲突对的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
        /*--------------------以上构建原始元组的冲突对--------------------*/
        
        cout<<endl;
        
        /*--------------------以下打印元组相关信息--------------------*/
        cout<<"元组数量:"<<gen.number_of_tuples(gen.source_tuple)<<endl;
        temp_SR.nums_tuples=gen.number_of_tuples(gen.source_tuple);
        //cout<<"temp_SR.nums_tuples:"<<temp_SR.nums_tuples<<endl;
        cout<<"元组中参与冲突的元组数量:"<<gen.number_of_tuples_in_conflict(gen.source_tuple)<<endl;
        temp_SR.tuple_in_conflicts=gen.number_of_tuples_in_conflict(gen.source_tuple);
        //cout<<"temp_SR.tuple_in_conflicts:"<<temp_SR.tuple_in_conflicts<<endl;
        cout<<"冲突对儿数量:"<<gen.number_of_conflicts(gen.relationship_G)<<endl;
        temp_SR.nums_conflicts=gen.number_of_conflicts(gen.relationship_G);
        //cout<<"temp_SR.nums_conflicts:"<<temp_SR.nums_conflicts<<endl;
        cout<<"元组错误率:"<<gen.print_error_percentage(gen.source_tuple)<<endl;
        temp_SR.error_rate=gen.print_error_percentage(gen.source_tuple);
        /*--------------------以上打印元组相关信息--------------------*/
        
        cout<<endl;
        
        /*--------------------以下对source_tuple和relationship_G进行G-repair--------------------*/
        start=clock();
        gen.lp_solver_G_repair(gen.source_tuple,gen.relationship_G);
        end=clock();
        cout<<"soft repair的G_repair的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
        temp_SR.Grepair_time=(double)(end - start)/CLOCKS_PER_SEC;
        if(gen.effectiveness_of_solution_G(gen.relationship_G)==true)
            cout<<"G_repair解有效,"<<"w>0时,y_i,j=min(xi+xj,1);w<0时,y_i,j=max(xi,xj)"<<endl;
        else cout<<"G_repair解无效"<<endl;
        start=clock();
        gen.rounding(gen.source_tuple,gen.relationship_G);//为normalized_tuple的x和relationship_G的y做舍入
        end=clock();
        cout<<"soft repair的G_repair的rounding时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
        cout<<"soft repair的G_repair的分数最优解为:"<<gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G)<<endl;
        temp_SR.Grepair_fopt=gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G);
        cout<<"soft repair的G_repair的整数近似最优解为:"<<gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G)<<endl;
        double G_repair_ropt=gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G);
        temp_SR.Grepair_ropt=gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G);
        gen.construct_left_tuple(gen.source_tuple);//把留在repair中的元组放到gen.left_tuple中
        cout<<"soft repair的G_repair留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
        gen.construct_left_conflict(gen.left_tuple, gen.relationship_S);
        temp_SR.Grepair_tleft=gen.number_of_tuples(gen.left_tuple);
        cout<<"soft repair的G_repair留下的元组冲突对个数:"<<gen.number_of_conflicts(gen.relationship_S)<<endl;
        temp_SR.Grepair_cleft=gen.number_of_conflicts(gen.relationship_S);
        cout<<"soft repair的G_repair留下的元组冲突对加权和:"<<gen.weighted_left_conflicts(gen.relationship_S)<<endl;
        temp_SR.Grepair_wcleft=gen.weighted_left_conflicts(gen.relationship_S);
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
        
        //gen.print_all_info_sc();
        /*--------------------以上构造set cover问题的输入U和S1和S2----------------*/
        
        cout<<endl;
        
        /*--------------------以下构造set cover问题的输入vector<int>和vector<vector<int>>----------------*/
        gen.construct_sc();
        
        //gen.print_all_info_sc_int();
        /*--------------------以上构造set cover问题的输入vector<int>和vector<vector<int>>----------------*/
        
        cout<<endl;
        
        /*--------------------以下是set cover问题的贪心算法----------------*/
        gen.clear_source_tuple();
        vector<int> res;
        start=clock();
        //为了求sc_lp而注释
        res=gen.greedy_sc(gen.UU,gen.C);
        end=clock();
        cout<<"soft repair的贪心算法时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
        temp_SR.Greedy_time=(double)(end - start)/CLOCKS_PER_SEC;
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
        temp_SR.Greedy_ropt=gen.greedy_opt;
        cout<<"soft repair的贪心算法留下的元组个数:"<<num_aft0<<endl;
        temp_SR.Greedy_tleft=num_aft0;
        cout<<"soft repair的贪心算法留下的元组冲突对个数:"<<num_bef1-num_aft1<<endl;
        temp_SR.Greedy_cleft=num_bef1-num_aft1;
        cout<<"soft repair的贪心算法留下的元组冲突对加权和:"<<bef1-aft1<<endl;
        temp_SR.Greedy_wcleft=bef1-aft1;
        gen.SR.push_back(temp_SR);
        
        gen.left_tuple.clear();
        gen.relationship_left.clear();
        gen.relationship_S.clear();
        
        cout<<gen.greedy_opt<<endl;
        cout<<G_repair_ropt<<endl;
        if(gen.greedy_opt>G_repair_ropt){
            FILE *fp;
            FILE *fp_weight;
            string postfix=".csv";
            string weight_postfix=".txt";
            string jmy="jmy_pollute_on_hadoop";
            //string middle=to_string(p);
            //string back=to_string(err_rate);
            //fprintf(fp,"%s,","scheduled_arrival");
            string size_name=to_string(size);
            string err_name=to_string(err);
            string p_name=to_string(p);
            
            string result_tuple_path=result_path+jmy+size_name+"_"+err_name+"_"+p_name+postfix;
            string result_weight_path=result_path+jmy+size_name+"_"+err_name+"_"+p_name+"weight"+weight_postfix;
            fp = fopen(result_tuple_path.data(), "w");
            fp_weight = fopen(result_weight_path.data(), "w");
            if(fp==NULL) cout<<"大失败！"<<endl;
            else cout<<"大成功！"<<endl;
            fprintf(fp,"%s,","flight");
            fprintf(fp,"%s,","scheduled_dept");
            fprintf(fp,"%s,","actual_dept");
            fprintf(fp,"%s,","dept_gate");
            fprintf(fp,"%s,","scheduled_arrival");
            fprintf(fp,"%s,","actual_arrival");
            fprintf(fp,"%s,","arrival_gate");
            fprintf(fp,"%s,","airline");
            
            fprintf(fp,"%s\n","destination");
            for(int m=0;m<gen.source_tuple.size();++m){
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[0].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[1].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[2].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[3].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[4].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[5].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[6].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[7].c_str());
                fprintf(fp,"%s\n",gen.source_tuple[m].attrs_value[8].c_str());
                fprintf(fp_weight,"%.2f\n",gen.source_tuple[m].tuple_weight);
            }
            fclose(fp);
            fclose(fp_weight);
            string soft="soft_result";
            string soft_result_path=result_path+soft+size_name+"_"+err_name+"_"+p_name+".csv";
            //gen.write_soft_result(soft_result_path.data());
        }
        shift=shift+0.1;
    }
}
/*--------------------以下为了在hadoop上生成数据，存surprise，但没有好结果能存到surprise里----------------*/

/*--------------------以下为了在本机生成vc反例----------------*/
void test_for_validvc(){
    //以下的图结构中（即vc贪心不好用的反例），cost(GRepair)<cost(Greedy)
    Generator gen("/Users/andy/Documents/Data/Flight/test_counterexample/counterexample.csv","r");//加载tuple到gen.source_data中
    gen.load_tuple_weight("",gen.source_tuple);
    
    FD fdd;
    fdd.fd_weight=1.5;
    FD& fd=fdd;
    
    gen.source_tuple[19].tuple_weight=2;
    
    Relationship temp_rel0(gen.source_tuple[0],gen.source_tuple[6]);
    temp_rel0.rel_type=1;
    temp_rel0.rel_id=0;
    temp_rel0.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel0);
    int element_id=(int)gen.U.size();
    Element temp_ele0(gen.source_tuple[0],gen.source_tuple[6],fd);
    temp_ele0.ele_id=element_id;
    gen.U.push_back(temp_ele0);
    
    Relationship temp_rel1(gen.source_tuple[0],gen.source_tuple[12]);
    temp_rel1.rel_type=1;
    temp_rel1.rel_id=1;
    temp_rel1.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel1);
    element_id=(int)gen.U.size();
    Element temp_ele1(gen.source_tuple[0],gen.source_tuple[12],fd);
    temp_ele1.ele_id=element_id;
    gen.U.push_back(temp_ele1);
    
    Relationship temp_rel2(gen.source_tuple[0],gen.source_tuple[15]);
    temp_rel2.rel_type=1;
    temp_rel2.rel_id=2;
    temp_rel2.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel2);
    element_id=(int)gen.U.size();
    Element temp_ele2(gen.source_tuple[0],gen.source_tuple[15],fd);
    temp_ele2.ele_id=element_id;
    gen.U.push_back(temp_ele2);
    
    Relationship temp_rel3(gen.source_tuple[0],gen.source_tuple[17]);
    temp_rel3.rel_type=1;
    temp_rel3.rel_id=3;
    temp_rel3.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel3);
    element_id=(int)gen.U.size();
    Element temp_ele3(gen.source_tuple[0],gen.source_tuple[17],fd);
    temp_ele3.ele_id=element_id;
    gen.U.push_back(temp_ele3);
    
    Relationship temp_rel4(gen.source_tuple[0],gen.source_tuple[18]);
    temp_rel4.rel_type=1;
    temp_rel4.rel_id=4;
    temp_rel4.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel4);
    element_id=(int)gen.U.size();
    Element temp_ele4(gen.source_tuple[0],gen.source_tuple[18],fd);
    temp_ele4.ele_id=element_id;
    gen.U.push_back(temp_ele4);
    
    Relationship temp_rel5(gen.source_tuple[0],gen.source_tuple[19]);
    temp_rel5.rel_type=1;
    temp_rel5.rel_id=5;
    temp_rel5.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel5);
    element_id=(int)gen.U.size();
    Element temp_ele5(gen.source_tuple[0],gen.source_tuple[19],fd);
    temp_ele5.ele_id=element_id;
    gen.U.push_back(temp_ele5);
    
    Relationship temp_rel6(gen.source_tuple[1],gen.source_tuple[7]);
    temp_rel6.rel_type=1;
    temp_rel6.rel_id=6;
    temp_rel6.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel6);
    element_id=(int)gen.U.size();
    Element temp_ele6(gen.source_tuple[1],gen.source_tuple[17],fd);
    temp_ele6.ele_id=element_id;
    gen.U.push_back(temp_ele6);
    
    Relationship temp_rel7(gen.source_tuple[1],gen.source_tuple[12]);
    temp_rel7.rel_type=1;
    temp_rel7.rel_id=7;
    temp_rel7.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel7);
    element_id=(int)gen.U.size();
    Element temp_ele7(gen.source_tuple[1],gen.source_tuple[12],fd);
    temp_ele7.ele_id=element_id;
    gen.U.push_back(temp_ele7);
    
    Relationship temp_rel8(gen.source_tuple[1],gen.source_tuple[15]);
    temp_rel8.rel_type=1;
    temp_rel8.rel_id=8;
    temp_rel8.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel8);
    element_id=(int)gen.U.size();
    Element temp_ele8(gen.source_tuple[1],gen.source_tuple[15],fd);
    temp_ele8.ele_id=element_id;
    gen.U.push_back(temp_ele8);
    
    Relationship temp_rel9(gen.source_tuple[1],gen.source_tuple[17]);
    temp_rel9.rel_type=1;
    temp_rel9.rel_id=9;
    temp_rel9.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel9);
    element_id=(int)gen.U.size();
    Element temp_ele9(gen.source_tuple[1],gen.source_tuple[17],fd);
    temp_ele9.ele_id=element_id;
    gen.U.push_back(temp_ele9);
    
    Relationship temp_rel10(gen.source_tuple[1],gen.source_tuple[18]);
    temp_rel10.rel_type=1;
    temp_rel10.rel_id=10;
    temp_rel10.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel10);
    element_id=(int)gen.U.size();
    Element temp_ele10(gen.source_tuple[1],gen.source_tuple[18],fd);
    temp_ele10.ele_id=element_id;
    gen.U.push_back(temp_ele10);
    
    Relationship temp_rel11(gen.source_tuple[1],gen.source_tuple[19]);
    temp_rel11.rel_type=1;
    temp_rel11.rel_id=11;
    temp_rel11.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel11);
    element_id=(int)gen.U.size();
    Element temp_ele11(gen.source_tuple[1],gen.source_tuple[19],fd);
    temp_ele11.ele_id=element_id;
    gen.U.push_back(temp_ele11);
    
    Relationship temp_rel12(gen.source_tuple[2],gen.source_tuple[8]);
    temp_rel12.rel_type=1;
    temp_rel12.rel_id=12;
    temp_rel12.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel12);
    element_id=(int)gen.U.size();
    Element temp_ele12(gen.source_tuple[2],gen.source_tuple[8],fd);
    temp_ele12.ele_id=element_id;
    gen.U.push_back(temp_ele12);
    
    Relationship temp_rel13(gen.source_tuple[2],gen.source_tuple[13]);
    temp_rel13.rel_type=1;
    temp_rel13.rel_id=13;
    temp_rel13.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel13);
    element_id=(int)gen.U.size();
    Element temp_ele13(gen.source_tuple[2],gen.source_tuple[13],fd);
    temp_ele13.ele_id=element_id;
    gen.U.push_back(temp_ele13);
    
    Relationship temp_rel14(gen.source_tuple[2],gen.source_tuple[15]);
    temp_rel14.rel_type=1;
    temp_rel14.rel_id=14;
    temp_rel14.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel14);
    element_id=(int)gen.U.size();
    Element temp_ele14(gen.source_tuple[2],gen.source_tuple[15],fd);
    temp_ele14.ele_id=element_id;
    gen.U.push_back(temp_ele14);
    
    Relationship temp_rel15(gen.source_tuple[2],gen.source_tuple[17]);
    temp_rel15.rel_type=1;
    temp_rel15.rel_id=15;
    temp_rel15.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel15);
    element_id=(int)gen.U.size();
    Element temp_ele15(gen.source_tuple[2],gen.source_tuple[17],fd);
    temp_ele15.ele_id=element_id;
    gen.U.push_back(temp_ele15);
    
    Relationship temp_rel16(gen.source_tuple[2],gen.source_tuple[18]);
    temp_rel16.rel_type=1;
    temp_rel16.rel_id=16;
    temp_rel16.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel16);
    element_id=(int)gen.U.size();
    Element temp_ele16(gen.source_tuple[2],gen.source_tuple[18],fd);
    temp_ele16.ele_id=element_id;
    gen.U.push_back(temp_ele16);
    
    Relationship temp_rel17(gen.source_tuple[2],gen.source_tuple[19]);
    temp_rel17.rel_type=1;
    temp_rel17.rel_id=17;
    temp_rel17.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel17);
    element_id=(int)gen.U.size();
    Element temp_ele17(gen.source_tuple[2],gen.source_tuple[19],fd);
    temp_ele17.ele_id=element_id;
    gen.U.push_back(temp_ele17);
    
    Relationship temp_rel18(gen.source_tuple[3],gen.source_tuple[9]);
    temp_rel18.rel_type=1;
    temp_rel18.rel_id=18;
    temp_rel18.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel18);
    element_id=(int)gen.U.size();
    Element temp_ele18(gen.source_tuple[3],gen.source_tuple[9],fd);
    temp_ele18.ele_id=element_id;
    gen.U.push_back(temp_ele18);
    
    Relationship temp_rel19(gen.source_tuple[3],gen.source_tuple[13]);
    temp_rel19.rel_type=1;
    temp_rel19.rel_id=19;
    temp_rel19.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel19);
    element_id=(int)gen.U.size();
    Element temp_ele19(gen.source_tuple[3],gen.source_tuple[13],fd);
    temp_ele19.ele_id=element_id;
    gen.U.push_back(temp_ele19);
    
    Relationship temp_rel20(gen.source_tuple[3],gen.source_tuple[16]);
    temp_rel20.rel_type=1;
    temp_rel20.rel_id=20;
    temp_rel20.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel20);
    element_id=(int)gen.U.size();
    Element temp_ele20(gen.source_tuple[3],gen.source_tuple[16],fd);
    temp_ele20.ele_id=element_id;
    gen.U.push_back(temp_ele20);
    
    Relationship temp_rel21(gen.source_tuple[3],gen.source_tuple[17]);
    temp_rel21.rel_type=1;
    temp_rel21.rel_id=21;
    temp_rel21.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel21);
    element_id=(int)gen.U.size();
    Element temp_ele21(gen.source_tuple[3],gen.source_tuple[17],fd);
    temp_ele21.ele_id=element_id;
    gen.U.push_back(temp_ele21);
    
    Relationship temp_rel22(gen.source_tuple[3],gen.source_tuple[18]);
    temp_rel22.rel_type=1;
    temp_rel22.rel_id=22;
    temp_rel22.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel22);
    element_id=(int)gen.U.size();
    Element temp_ele22(gen.source_tuple[3],gen.source_tuple[18],fd);
    temp_ele22.ele_id=element_id;
    gen.U.push_back(temp_ele22);
    
    Relationship temp_rel23(gen.source_tuple[3],gen.source_tuple[19]);
    temp_rel23.rel_type=1;
    temp_rel23.rel_id=23;
    temp_rel23.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel23);
    element_id=(int)gen.U.size();
    Element temp_ele23(gen.source_tuple[3],gen.source_tuple[19],fd);
    temp_ele23.ele_id=element_id;
    gen.U.push_back(temp_ele23);
    
    Relationship temp_rel24(gen.source_tuple[4],gen.source_tuple[10]);
    temp_rel24.rel_type=1;
    temp_rel24.rel_id=24;
    temp_rel24.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel24);
    element_id=(int)gen.U.size();
    Element temp_ele24(gen.source_tuple[4],gen.source_tuple[10],fd);
    temp_ele24.ele_id=element_id;
    gen.U.push_back(temp_ele24);
    
    Relationship temp_rel25(gen.source_tuple[4],gen.source_tuple[14]);
    temp_rel25.rel_type=1;
    temp_rel25.rel_id=25;
    temp_rel25.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel25);
    element_id=(int)gen.U.size();
    Element temp_ele25(gen.source_tuple[4],gen.source_tuple[14],fd);
    temp_ele25.ele_id=element_id;
    gen.U.push_back(temp_ele25);
    
    Relationship temp_rel26(gen.source_tuple[4],gen.source_tuple[16]);
    temp_rel26.rel_type=1;
    temp_rel26.rel_id=26;
    temp_rel26.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel26);
    element_id=(int)gen.U.size();
    Element temp_ele26(gen.source_tuple[4],gen.source_tuple[16],fd);
    temp_ele26.ele_id=element_id;
    gen.U.push_back(temp_ele26);
    
    Relationship temp_rel27(gen.source_tuple[4],gen.source_tuple[18]);
    temp_rel27.rel_type=1;
    temp_rel27.rel_id=27;
    temp_rel27.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel27);
    element_id=(int)gen.U.size();
    Element temp_ele27(gen.source_tuple[4],gen.source_tuple[18],fd);
    temp_ele27.ele_id=element_id;
    gen.U.push_back(temp_ele27);
    
    Relationship temp_rel28(gen.source_tuple[4],gen.source_tuple[19]);
    temp_rel28.rel_type=1;
    temp_rel28.rel_id=28;
    temp_rel28.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel28);
    element_id=(int)gen.U.size();
    Element temp_ele28(gen.source_tuple[4],gen.source_tuple[19],fd);
    temp_ele28.ele_id=element_id;
    gen.U.push_back(temp_ele28);
    
    Relationship temp_rel29(gen.source_tuple[5],gen.source_tuple[11]);
    temp_rel29.rel_type=1;
    temp_rel29.rel_id=29;
    temp_rel29.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel29);
    element_id=(int)gen.U.size();
    Element temp_ele29(gen.source_tuple[5],gen.source_tuple[11],fd);
    temp_ele29.ele_id=element_id;
    gen.U.push_back(temp_ele29);
    
    Relationship temp_rel30(gen.source_tuple[5],gen.source_tuple[14]);
    temp_rel30.rel_type=1;
    temp_rel30.rel_id=30;
    temp_rel30.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel30);
    element_id=(int)gen.U.size();
    Element temp_ele30(gen.source_tuple[5],gen.source_tuple[14],fd);
    temp_ele30.ele_id=element_id;
    gen.U.push_back(temp_ele30);
    
    Relationship temp_rel31(gen.source_tuple[5],gen.source_tuple[16]);
    temp_rel31.rel_type=1;
    temp_rel31.rel_id=31;
    temp_rel31.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel31);
    element_id=(int)gen.U.size();
    Element temp_ele31(gen.source_tuple[5],gen.source_tuple[16],fd);
    temp_ele31.ele_id=element_id;
    gen.U.push_back(temp_ele31);
    
    Relationship temp_rel32(gen.source_tuple[5],gen.source_tuple[19]);
    temp_rel32.rel_type=1;
    temp_rel32.rel_id=32;
    temp_rel32.rel_weight=1.5;
    gen.relationship_G.push_back(temp_rel32);
    element_id=(int)gen.U.size();
    Element temp_ele32(gen.source_tuple[5],gen.source_tuple[19],fd);
    temp_ele32.ele_id=element_id;
    gen.U.push_back(temp_ele32);
    
    /*--------------------以下打印元组相关信息--------------------*/
    cout<<"元组数量:"<<gen.number_of_tuples(gen.source_tuple)<<endl;
    //temp_SR.nums_tuples=gen.number_of_tuples(gen.source_tuple);
    //cout<<"temp_SR.nums_tuples:"<<temp_SR.nums_tuples<<endl;
    cout<<"元组中参与冲突的元组数量:"<<gen.number_of_tuples_in_conflict(gen.source_tuple)<<endl;
    //temp_SR.tuple_in_conflicts=gen.number_of_tuples_in_conflict(gen.source_tuple);
    //cout<<"temp_SR.tuple_in_conflicts:"<<temp_SR.tuple_in_conflicts<<endl;
    cout<<"冲突对儿数量:"<<gen.number_of_conflicts(gen.relationship_G)<<endl;
    //temp_SR.nums_conflicts=gen.number_of_conflicts(gen.relationship_G);
    //cout<<"temp_SR.nums_conflicts:"<<temp_SR.nums_conflicts<<endl;
    cout<<"元组错误率:"<<gen.print_error_percentage(gen.source_tuple)<<endl;
    //temp_SR.error_rate=gen.print_error_percentage(gen.source_tuple);
    /*--------------------以上打印元组相关信息--------------------*/
    
    cout<<endl;
    
    /*--------------------以下对source_tuple和relationship_G进行G-repair--------------------*/
    //start=clock();
    gen.lp_solver_G_repair(gen.source_tuple,gen.relationship_G);
    //end=clock();
    //cout<<"soft repair的G_repair的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    //temp_SR.Grepair_time=(double)(end - start)/CLOCKS_PER_SEC;
    if(gen.effectiveness_of_solution_G(gen.relationship_G)==true)
        cout<<"G_repair解有效,"<<"w>0时,y_i,j=min(xi+xj,1);w<0时,y_i,j=max(xi,xj)"<<endl;
    else cout<<"G_repair解无效"<<endl;
    //start=clock();
    gen.rounding(gen.source_tuple,gen.relationship_G);//为normalized_tuple的x和relationship_G的y做舍入
    //end=clock();
    //cout<<"soft repair的G_repair的rounding时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    cout<<"soft repair的G_repair的分数最优解为:"<<gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G)<<endl;
    //temp_SR.Grepair_fopt=gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G);
    cout<<"soft repair的G_repair的整数近似最优解为:"<<gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G)<<endl;
    double G_repair_ropt=gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G);
    //temp_SR.Grepair_ropt=gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G);
    gen.construct_left_tuple(gen.source_tuple);//把留在repair中的元组放到gen.left_tuple中
    cout<<"soft repair的G_repair留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.construct_left_conflict(gen.left_tuple, gen.relationship_S);
    //temp_SR.Grepair_tleft=gen.number_of_tuples(gen.left_tuple);
    cout<<"soft repair的G_repair留下的元组冲突对个数:"<<gen.number_of_conflicts(gen.relationship_S)<<endl;
    //temp_SR.Grepair_cleft=gen.number_of_conflicts(gen.relationship_S);
    cout<<"soft repair的G_repair留下的元组冲突对加权和:"<<gen.weighted_left_conflicts(gen.relationship_S)<<endl;
    //temp_SR.Grepair_wcleft=gen.weighted_left_conflicts(gen.relationship_S);
    gen.left_tuple.clear();
    gen.relationship_left.clear();
    gen.relationship_S.clear();
    /*--------------------以上对source_tuple和relationship_G进行G-repair--------------------*/
    
    cout<<endl;
    
    /*--------------------以下构造set cover问题的输入U和S1和S2----------------*/
    //start=clock();
    gen.construct_sets();
    //end=clock();
    
    //gen.print_all_info_sc();
    /*--------------------以上构造set cover问题的输入U和S1和S2----------------*/
    
    cout<<endl;
    
    /*--------------------以下构造set cover问题的输入vector<int>和vector<vector<int>>----------------*/
    gen.construct_sc();
    
    
    
    cout<<endl;
    
    /*--------------------以下是set cover问题的贪心算法----------------*/
    gen.clear_source_tuple();
    vector<int> res;
    //为了求sc_lp而注释
    res=gen.greedy_sc(gen.UU,gen.C);
    double bef1=0.0;
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
    //temp_SR.Greedy_ropt=gen.greedy_opt;
    cout<<"soft repair的贪心算法留下的元组个数:"<<num_aft0<<endl;
    //temp_SR.Greedy_tleft=num_aft0;
    cout<<"soft repair的贪心算法留下的元组冲突对个数:"<<num_bef1-num_aft1<<endl;
    //temp_SR.Greedy_cleft=num_bef1-num_aft1;
    cout<<"soft repair的贪心算法留下的元组冲突对加权和:"<<bef1-aft1<<endl;
    //temp_SR.Greedy_wcleft=bef1-aft1;
    //gen.SR.push_back(temp_SR);
    
    gen.left_tuple.clear();
    gen.relationship_left.clear();
    gen.relationship_S.clear();
    
    cout<<gen.greedy_opt<<endl;
    cout<<G_repair_ropt<<endl;
    //以上的图结构中（即vc贪心不好用的反例），cost(GRepair)<cost(Greedy)
}
/*--------------------以上为了在本机生成vc反例----------------*/

void test_vc_invalid(char * tuple_path, char* fd_path,char* result_path){
        //cout<<"第shift="<<shift<<"次"<<endl;
        clock_t start,end;//计算时间
        SoftResult temp_SR;
        /*--------------------以下加载元组--------------------*/
        char ch[]="r";
        Generator gen(tuple_path,ch);
        gen.load_tuple_weight("",gen.source_tuple);
        gen.print_attrs_name();//输出relational data全部属性名称
        /*--------------------以上加载元组--------------------*/
        /*--------------------以下加载FD--------------------*/
        gen.load_fd_file(fd_path);//加载FD到gen.fun_denp
        //gen.load_fd_weight("",gen.fun_denp);//加载fd's weight,当第一个参数为""时,默认每个元组的FD都是w_conflict
        gen.load_fd_weight("",gen.fun_denp);//加载fd's weight到到gen.fun_denp
        cout<<"FD数量:"<<gen.number_of_fds()<<endl;
        
        /*--------------------以上加载FD--------------------*/
        gen.pollute_flight_forinvalidvc(gen.source_tuple,"");
        gen.print_every_fds();//输出全部FD
        /*--------------------以下构建原始元组的冲突对--------------------*/
        start=clock();
        gen.construct_conflict(gen.source_tuple,gen.relationship_G,ch);//将source_tuple的冲突对关系存储在relationship_G中
        //gen.random_tuple_weight(gen.source_tuple);
        end=clock();
        //cout<<"cuole"<<endl;
        cout<<"原始元组构建冲突对的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
        /*--------------------以上构建原始元组的冲突对--------------------*/
        
        cout<<endl;
        
        /*--------------------以下打印元组相关信息--------------------*/
        cout<<"元组数量:"<<gen.number_of_tuples(gen.source_tuple)<<endl;
        temp_SR.nums_tuples=gen.number_of_tuples(gen.source_tuple);
        //cout<<"temp_SR.nums_tuples:"<<temp_SR.nums_tuples<<endl;
        cout<<"元组中参与冲突的元组数量:"<<gen.number_of_tuples_in_conflict(gen.source_tuple)<<endl;
        temp_SR.tuple_in_conflicts=gen.number_of_tuples_in_conflict(gen.source_tuple);
        //cout<<"temp_SR.tuple_in_conflicts:"<<temp_SR.tuple_in_conflicts<<endl;
        cout<<"冲突对儿数量:"<<gen.number_of_conflicts(gen.relationship_G)<<endl;
        temp_SR.nums_conflicts=gen.number_of_conflicts(gen.relationship_G);
        //cout<<"temp_SR.nums_conflicts:"<<temp_SR.nums_conflicts<<endl;
        cout<<"元组错误率:"<<gen.print_error_percentage(gen.source_tuple)<<endl;
        temp_SR.error_rate=gen.print_error_percentage(gen.source_tuple);
        /*--------------------以上打印元组相关信息--------------------*/
        
        cout<<endl;
        
        /*--------------------以下对source_tuple和relationship_G进行G-repair--------------------*/
        start=clock();
        gen.lp_solver_G_repair(gen.source_tuple,gen.relationship_G);
        end=clock();
        cout<<"soft repair的G_repair的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
        temp_SR.Grepair_time=(double)(end - start)/CLOCKS_PER_SEC;
        if(gen.effectiveness_of_solution_G(gen.relationship_G)==true)
            cout<<"G_repair解有效,"<<"w>0时,y_i,j=min(xi+xj,1);w<0时,y_i,j=max(xi,xj)"<<endl;
        else cout<<"G_repair解无效"<<endl;
        start=clock();
        gen.rounding(gen.source_tuple,gen.relationship_G);//为normalized_tuple的x和relationship_G的y做舍入
        end=clock();
        cout<<"soft repair的G_repair的rounding时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
        cout<<"soft repair的G_repair的分数最优解为:"<<gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G)<<endl;
        temp_SR.Grepair_fopt=gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G);
        cout<<"soft repair的G_repair的整数近似最优解为:"<<gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G)<<endl;
        double G_repair_ropt=gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G);
        temp_SR.Grepair_ropt=gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G);
        gen.construct_left_tuple(gen.source_tuple);//把留在repair中的元组放到gen.left_tuple中
        cout<<"soft repair的G_repair留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
        gen.construct_left_conflict(gen.left_tuple, gen.relationship_S);
        temp_SR.Grepair_tleft=gen.number_of_tuples(gen.left_tuple);
        cout<<"soft repair的G_repair留下的元组冲突对个数:"<<gen.number_of_conflicts(gen.relationship_S)<<endl;
        temp_SR.Grepair_cleft=gen.number_of_conflicts(gen.relationship_S);
        cout<<"soft repair的G_repair留下的元组冲突对加权和:"<<gen.weighted_left_conflicts(gen.relationship_S)<<endl;
        temp_SR.Grepair_wcleft=gen.weighted_left_conflicts(gen.relationship_S);
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
        
        //gen.print_all_info_sc();
        /*--------------------以上构造set cover问题的输入U和S1和S2----------------*/
        
        cout<<endl;
        
        /*--------------------以下构造set cover问题的输入vector<int>和vector<vector<int>>----------------*/
        gen.construct_sc();
        
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
        temp_SR.Greedy_time=(double)(end - start)/CLOCKS_PER_SEC;
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
        temp_SR.Greedy_ropt=gen.greedy_opt;
        cout<<"soft repair的贪心算法留下的元组个数:"<<num_aft0<<endl;
        temp_SR.Greedy_tleft=num_aft0;
        cout<<"soft repair的贪心算法留下的元组冲突对个数:"<<num_bef1-num_aft1<<endl;
        temp_SR.Greedy_cleft=num_bef1-num_aft1;
        cout<<"soft repair的贪心算法留下的元组冲突对加权和:"<<bef1-aft1<<endl;
        temp_SR.Greedy_wcleft=bef1-aft1;
        gen.SR.push_back(temp_SR);
        
        gen.left_tuple.clear();
        gen.relationship_left.clear();
        gen.relationship_S.clear();
        
        cout<<gen.greedy_opt<<endl;
        cout<<G_repair_ropt<<endl;
        if(gen.greedy_opt>G_repair_ropt){
            FILE *fp;
            FILE *fp_weight;
            string postfix=".csv";
            string weight_postfix=".txt";
            string jmy="jmy_pollute_on_hadoop";
            string result_tuple_path=result_path+jmy+postfix;
            string result_weight_path=result_path+jmy+"_"+"weight"+weight_postfix;
            fp = fopen(result_tuple_path.data(), "w");
            fp_weight = fopen(result_weight_path.data(), "w");
            if(fp==NULL) cout<<"大失败！"<<endl;
            else cout<<"大成功！"<<endl;
            fprintf(fp,"%s,","flight");
            fprintf(fp,"%s,","scheduled_dept");
            fprintf(fp,"%s,","actual_dept");
            fprintf(fp,"%s,","dept_gate");
            fprintf(fp,"%s,","scheduled_arrival");
            fprintf(fp,"%s,","actual_arrival");
            fprintf(fp,"%s,","arrival_gate");
            fprintf(fp,"%s,","airline");
            
            fprintf(fp,"%s\n","destination");
            for(int m=0;m<gen.source_tuple.size();++m){
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[0].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[1].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[2].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[3].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[4].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[5].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[6].c_str());
                fprintf(fp,"%s,",gen.source_tuple[m].attrs_value[7].c_str());
                fprintf(fp,"%s\n",gen.source_tuple[m].attrs_value[8].c_str());
                fprintf(fp_weight,"%.2f\n",gen.source_tuple[m].tuple_weight);
            }
            fclose(fp);
            fclose(fp_weight);
            string soft="soft_result";
            string soft_result_path=result_path+soft+".csv";
            //gen.write_soft_result(soft_result_path.data());
        }
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

    //soft_repair_5FDs_hadoop(10000,40000);
    
    //soft_repair_6FDs_hadoop(10000,40000);
    //soft_repair_5FDs_hadoop(10000,40000);
    //soft_repair_4FDs_hadoop(10000,10000);
    //soft_repair_3FDs_hadoop(10000,40000);
    //soft_repair_2FDs_hadoop(10000,40000);
    //soft_repair_4FDs_hadoop(10000,10000,"/home/hadoop/JMYAndy/Data/Flight/four_FDs/flight_FDs_weight41.txt","/home/hadoop/JMYAndy/Data/results/result_4FD1.csv","/home/hadoop/JMYAndy/Data/results/result_4FD_lp1.csv");
    
    //soft_repair_4FDs_hadoop(10000,10000,"/home/hadoop/JMYAndy/Data/Flight/four_FDs/flight_FDs_weight42.txt","/home/hadoop/JMYAndy/Data/results/result_4FD2.csv","/home/hadoop/JMYAndy/Data/results/result_4FD_lp2.csv");
    
    //soft_repair_4FDs_hadoop(10000,10000,"/home/hadoop/JMYAndy/Data/Flight/four_FDs/flight_FDs_weight43.txt","/home/hadoop/JMYAndy/Data/results/result_4FD3.csv","/home/hadoop/JMYAndy/Data/results/result_4FD_lp3.csv");
    
    //soft_repair_4FDs_hadoop(10000,10000,"/home/hadoop/JMYAndy/Data/Flight/four_FDs/flight_FDs_weight44.txt","/home/hadoop/JMYAndy/Data/results/result_4FD4.csv","/home/hadoop/JMYAndy/Data/results/result_4FD_lp4.csv");
    
    //soft_repair_4FDs_hadoop(10000,10000,"/home/hadoop/JMYAndy/Data/Flight/four_FDs/flight_FDs_weight45.txt","/home/hadoop/JMYAndy/Data/results/result_4FD5.csv","/home/hadoop/JMYAndy/Data/results/result_4FD_lp5.csv");
    
    //soft_repair("/home/hadoop/JMYAndy/Data/Flight/test_vector/10000jmy0.100000.csv","/home/hadoop/JMYAndy/Data/Flight/test_vector/10000jmy0.100000weight.txt","/home/hadoop/JMYAndy/Data/Flight/test_vector/flight_FDs4.txt","/home/hadoop/JMYAndy/Data/Flight/test_vector/flight_FDs_weight4.txt","/home/hadoop/JMYAndy/Data/Flight/test_vector/GRandRr.txt","/home/hadoop/JMYAndy/Data/Flight/test_vector/sc_lp.txt");
    
    /*int c=1000;
    while(c<1500){
        soft_repair_test("/home/hadoop/JMYAndy/Data/Flight/test_vector/10000jmy0.050000.csv","/home/hadoop/JMYAndy/Data/Flight/test_vector/10000jmy0.050000weight.txt","/home/hadoop/JMYAndy/Data/Flight/test_vector/flight_FDs4.txt","/home/hadoop/JMYAndy/Data/Flight/test_vector/flight_FDs_weight4.txt",1,10,c);
    }
    while(c<2000){
        soft_repair_test("/home/hadoop/JMYAndy/Data/Flight/test_vector/10000jmy0.050000.csv","/home/hadoop/JMYAndy/Data/Flight/test_vector/10000jmy0.050000weight.txt","/home/hadoop/JMYAndy/Data/Flight/test_vector/flight_FDs4.txt","/home/hadoop/JMYAndy/Data/Flight/test_vector/flight_FDs_weight4.txt",1,100,c);
    }
    while(c<2500){
        soft_repair_test("/home/hadoop/JMYAndy/Data/Flight/test_vector/10000jmy0.050000.csv","/home/hadoop/JMYAndy/Data/Flight/test_vector/10000jmy0.050000weight.txt","/home/hadoop/JMYAndy/Data/Flight/test_vector/flight_FDs4.txt","/home/hadoop/JMYAndy/Data/Flight/test_vector/flight_FDs_weight4.txt",10,30,c);
    }*/
    /*cout<<"--------------------test_data--------------------"<<endl;
    char rel_tuple_path[]="/Users/andy/Documents/Data/test/testsoftgreedy/flight6.csv";
    char rel_tuple_weight_path[]="/Users/andy/Documents/Data/test/testsoftgreedy/flight6weight.txt";
    char rel_fd_path[]="/Users/andy/Documents/Data/test/testsoftgreedy/flight_FDs.txt";
    char rel_fd_weight_path[]="/Users/andy/Documents/Data/test/testsoftgreedy/flight_FDs_weight.txt";
    char result_path[]="/Users/andy/Documents/Data/test/testsoftgreedy/result6.csv";
    soft_repair(rel_tuple_path,rel_tuple_weight_path,rel_fd_path,rel_fd_weight_path,result_path,"");
    cout<<"--------------------test_data--------------------"<<endl;*/
    
    /*cout<<"--------------------test_data--------------------"<<endl;
    char rel_tuple_path5[]="/Users/andy/Documents/Data/test/testsoftgreedy/flight6.csv";
    char rel_tuple_weight_path5[]="/Users/andy/Documents/Data/test/testsoftgreedy/flight6weight.txt";
    char rel_fd_path[]="/Users/andy/Documents/Data/test/testsoftgreedy/flight_FDs.txt";
    char rel_fd_weight_path[]="/Users/andy/Documents/Data/test/testsoftgreedy/flight_FDs_weight.txt";
    char result_path[]="/Users/andy/Documents/Data/test/testsoftgreedy/result6.csv";
    char result_path_lp[]="/Users/andy/Documents/Data/test/testsoftgreedy/result6lp.csv";
    soft_repair(rel_tuple_path5,rel_tuple_weight_path5,rel_fd_path,rel_fd_weight_path,result_path,result_path_lp);
    cout<<"--------------------test_data--------------------"<<endl;*/
    
    
    
    /*soft_repair("/home/hadoop/JMYAndy/Data/Flight/test_graph/1000jmy_graph0.050000.csv","/home/hadoop/JMYAndy/Data/Flight/test_graph/1000jmy_graph0.050000weight.txt","/home/hadoop/JMYAndy/Data/Flight/test_graph/flight_FDs.txt","/home/hadoop/JMYAndy/Data/Flight/test_graph/flight_FDs_weight.txt","/home/hadoop/JMYAndy/Data/Flight/test_graph/result_graph.csv","/home/hadoop/JMYAndy/Data/Flight/test_graph/result_graph_lp.csv");*/
    
    /*cout<<"--------------------pollute_data--------------------"<<endl;
    char rel_tuple_path[]="/Users/andy/Documents/Data/Flight/flight50kjmy.csv";
    char rel_tuple_weight_path[]="";
    char rel_fd_path[]="/Users/andy/Documents/Data/Flight/flight_FDs.txt";
    char rel_fd_weight_path[]="";
    soft_repair(rel_tuple_path,rel_tuple_weight_path,rel_fd_path,rel_fd_weight_path,"","");
    cout<<"--------------------pollute_data--------------------"<<endl;*/
    
    //test_pollte_data_on_hadooop("/home/hadoop/JMYAndy/Data/Flight/test_on_hadoop/flight50kjmy.csv","/home/hadoop/JMYAndy/Data/Flight/test_on_hadoop/flight_FDs.txt","/Users/andy/Documents/Data/Flight/test_on_hadoop/");
    
    
    //test_for_validvc();
    Generator gen("/Users/andy/Documents/Data/Flight/flight50kjmy.csv","r");
    gen.load_tuple_weight("",gen.source_tuple);
    gen.print_attrs_name();//输出relational data全部属性名称
    gen.load_fd_file("/Users/andy/Documents/Data/Flight/test_counterexample/0510flight_FDs.txt");//加载FD到gen.fun_denp中
    //gen.load_fd_weight("",gen.fun_denp);//加载fd's weight,当第一个参数为""时,默认每个元组的FD都是w_conflict
    gen.load_fd_weight("/Users/andy/Documents/Data/Flight/test_counterexample/0510flight_FDs_weight.txt",gen.fun_denp);//加载fd's weight到gen.fun_denp中
    cout<<"FD数量:"<<gen.number_of_fds()<<endl;
    gen.print_every_fds();//输出全部FD
    //gen.pollute_flight_forinvalidvc(gen.source_tuple,"/Users/andy/Documents/Data/Flight/test_counterexample/");
    //soft_repair("/Users/andy/Documents/Data/Flight/0510jmy_forinvalidvc.csv","/Users/andy/Documents/Data/Flight/jmy_forinvalidvcweight.txt","/Users/andy/Documents/Data/Flight/test_counterexample/flight_FDs.txt","/Users/andy/Documents/Data/Flight/test_counterexample/flight_FDs_weight.txt","/Users/andy/Documents/Data/Flight/test_counterexample/soft.csv","/Users/andy/Documents/Data/Flight/test_counterexample/soft_lp.csv");
    gen.pollute_flight_forinvalidvc0510(gen.source_tuple,"/Users/andy/Documents/Data/Flight/test_counterexample/0510",1000,400);
    soft_repair("/Users/andy/Documents/Data/Flight/test_counterexample/0510jmy_forinvalidvc.csv","/Users/andy/Documents/Data/Flight/test_counterexample/0510jmy_forinvalidvcweight.txt","/Users/andy/Documents/Data/Flight/test_counterexample/0510flight_FDs.txt","/Users/andy/Documents/Data/Flight/test_counterexample/0510flight_FDs_weight.txt","/Users/andy/Documents/Data/Flight/test_counterexample/0510soft.csv","/Users/andy/Documents/Data/Flight/test_counterexample/0510soft_lp.csv");
    
    //soft_repair("/home/hadoop/JMYAndy/Data/Flight/test_counterexample/0510jmy_forinvalidvc.csv","/home/hadoop/JMYAndy/Data/Flight/test_counterexample/0510jmy_forinvalidvcweight.txt","/home/hadoop/JMYAndy/Data/Flight/test_counterexample/0510flight_FDs.txt","/home/hadoop/JMYAndy/Data/Flight/test_counterexample/0510flight_FDs_weight.txt","/home/hadoop/JMYAndy/Data/Flight/test_counterexample/0510soft.csv","/home/hadoop/JMYAndy/Data/Flight/test_counterexample/0510soft_lp.csv");
    
    
    
    
    
    return 0;
}



