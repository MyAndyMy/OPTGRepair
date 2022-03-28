//
//  main.cpp
//  OPTGRSRTR5.0
//
//  Created by Andy on 2022/3/14.
//  这一版完成:
//  了很多


#include <iostream>
#include "util.hpp"
#include <time.h>
//
vector<vector<int> > greedy_sc(vector<int> X, vector<vector<int> > F);//soft repair的贪心算法

void temporal_db_repair(char* tuple_path,char* tuple_weight_path,char* fd_path,char* fd_weight_path){
    clock_t start,end;//计算各种算法的运行时间
    
    /*--------------------以下加载元组--------------------*/
    char ch[]="t";//标记为加载temporal data
    Generator gen(tuple_path,ch);//加载tuple到gen.source_data中
    //gen.load_tuple_weight("",gen.source_tuple);//加载tuple's weight,当第一个参数为""时,默认每个元组的weight都是lambda
    gen.load_tuple_weight(tuple_weight_path,gen.source_tuple);//加载tuple's weight到gen.source_data中
    gen.print_attrs_name();//输出temporal data的全部属性名称
    /*--------------------以上加载元组--------------------*/
    
    cout<<endl;
    
    /*--------------------以下加载FD--------------------*/
    gen.load_fd_file(fd_path);//加载FD到gen.fun_denp中
    //gen.load_fd_weight("",gen.fun_denp);//加载fd's weight,当第一个参数为""时,默认每个元组的FD都是w_conflict
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
    cout<<"分段前元组中参与冲突的元组数量:"<<gen.number_of_tuples_in_conflict(gen.source_tuple)<<endl;
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
    gen.construct_left_tuple(gen.source_tuple);//把留在repair中的元组放到gen.left_tuple中
    cout<<"无normalization,S_repair,留下的元组个数:"<<gen.left_tuple.size()<<endl;
    gen.construct_adjacency(gen.left_tuple, gen.relationship_left);//建立gen.left_tuple中的相邻对到gen.relationship_left中
    cout<<"无normalization,S_repair,留下相邻对个数:"<<gen.number_of_adjacency(gen.relationship_left)<<endl;
    cout<<"无normalization,S_repair,留下相邻对个数/留下元组个数:"<<(double)gen.number_of_adjacency(gen.relationship_left)/gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.coalescing(gen.left_tuple);//为gen.left_tuple中元组做coalescing操作
    cout<<"normalization1后,S_repair,coalescing后,留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    cout<<"normalization1后,S_repair,coalescing后的span:"<<gen.span(gen.left_tuple)<<endl;
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
    gen.construct_left_tuple(gen.normalized_tuple);//把留在repair中的元组放到gen.left_tuple中
    cout<<"normalization1后,S_repair,留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.construct_adjacency(gen.left_tuple, gen.relationship_left);//建立gen.left_tuple中的相邻对到gen.relationship_left中
    cout<<"normalization1后,S_repair,留下相邻对个数:"<<gen.number_of_adjacency(gen.relationship_left)<<endl;
    cout<<"normalization1后,S_repair,留下相邻对个数/留下元组个数:"<<(double)gen.number_of_adjacency(gen.relationship_left)/gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.coalescing(gen.left_tuple);//为gen.left_tuple中元组做coalescing操作
    cout<<"normalization1后,S_repair,coalescing后,留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    cout<<"normalization1后,S_repair,coalescing后的span:"<<gen.span(gen.left_tuple)<<endl;
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
    gen.construct_left_tuple(gen.normalized_tuple);//把留在repair中的元组放到gen.left_tuple中
    cout<<"normalization1后,G_repair,留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.construct_adjacency(gen.left_tuple, gen.relationship_left);//建立gen.left_tuple中的相邻对到gen.relationship_left中
    cout<<"normalization1后,G_repair,留下相邻对个数:"<<gen.number_of_adjacency(gen.relationship_left)<<endl;
    cout<<"normalization1后,G_repair,留下相邻对个数/留下元组个数:"<<(double)gen.number_of_adjacency(gen.relationship_left)/gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.coalescing(gen.left_tuple);//为gen.left_tuple中元组做coalescing操作
    cout<<"normalization1后,G_repair,coalescing后,留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    cout<<"normalization1后,G_repair,coalescing后的span:"<<gen.span(gen.left_tuple)<<endl;
    gen.left_tuple.clear();
    gen.relationship_left.clear();
    /*--------------------以上normalization1，G-repair--------------------*/
    
}
void soft_repair(char* tuple_path,char* tuple_weight_path,char* fd_path,char* fd_weight_path){
    clock_t start,end;//计算时间
    
    /*--------------------以下加载元组--------------------*/
    char ch[]="r";
    Generator gen(tuple_path,ch);//加载tuple到gen.source_data中
    //gen.load_tuple_weight("",gen.source_tuple);//加载tuple's weight,当第一个参数为""时,默认每个元组的weight都是lambda
    gen.load_tuple_weight(tuple_weight_path,gen.source_tuple);//加载tuple's weight到gen.source_data中
    gen.print_attrs_name();//输出relational data全部属性名称
    /*--------------------以上加载元组--------------------*/
    
    cout<<endl;
    
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
    end=clock();
    cout<<"原始元组构建冲突对的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    /*--------------------以上构建原始元组的冲突对--------------------*/
    
    cout<<endl;
    
    /*--------------------以下打印元组相关信息--------------------*/
    cout<<"元组数量:"<<gen.number_of_tuples(gen.source_tuple)<<endl;
    cout<<"元组中参与冲突的元组数量:"<<gen.number_of_tuples_in_conflict(gen.source_tuple)<<endl;
    cout<<"冲突对儿数量:"<<gen.number_of_conflicts(gen.relationship_G)<<endl;
    /*--------------------以上打印元组相关信息--------------------*/
    
    cout<<endl;
    
    /*--------------------以下对source_tuple和relationship_G进行G-repair--------------------*/
    start=clock();
    gen.lp_solver_G_repair(gen.source_tuple,gen.relationship_G);
    end=clock();
    cout<<"soft repair的G_repair的时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    if(gen.effectiveness_of_solution_G(gen.relationship_G)==true)
        cout<<"G_repair解有效,"<<"w>0时,y_i,j=min(xi+xj,1);w<0时,y_i,j=max(xi,xj)"<<endl;
    else cout<<"G_repair解无效"<<endl;
    start=clock();
    gen.rounding(gen.source_tuple,gen.relationship_G);//为normalized_tuple的x和relationship_G的y做舍入
    end=clock();
    cout<<"soft repair的G_repair的rounding时间:"<<(double)(end - start)/CLOCKS_PER_SEC<<endl;
    cout<<"soft repair的G_repair的分数最优解为:"<<gen.calculate_opt_f_G(gen.source_tuple, gen.relationship_G)<<endl;
    cout<<"soft repair的G_repair的整数近似最优解为:"<<gen.calculate_opt_r_G(gen.source_tuple, gen.relationship_G)<<endl;
    gen.construct_left_tuple(gen.source_tuple);//把留在repair中的元组放到gen.left_tuple中
    cout<<"soft repair的G_repair留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    gen.construct_left_conflict(gen.left_tuple, gen.relationship_S);
    cout<<"soft repair的G_repair留下的元组冲突对个数:"<<gen.number_of_conflicts(gen.relationship_S)<<endl;
    cout<<"soft repair的G_repair留下的元组冲突对加权和:"<<gen.weighted_left_conflicts(gen.relationship_S)<<endl;
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
    for(int i=0;i<res.size();++i){
        if(res[i]>0&&res[i]<gen.S1.size()){
            gen.source_tuple[res[i]].xr=1;
        }
    }
    cout<<"soft repair的贪心整数近似最优解为:"<<gen.calculate_opt_r_Greedy()<<endl;
    cout<<"soft repair的贪心算法留下的元组个数:"<<gen.number_of_tuples(gen.left_tuple)<<endl;
    cout<<"soft repair的贪心算法留下的元组冲突对个数:"<<gen.number_of_conflicts(gen.relationship_S)<<endl;
    //没错,留下元组的冲突对放在relationship_S中了～！
    cout<<"soft repair的贪心算法留下的元组冲突对加权和:"<<gen.weighted_left_conflicts(gen.relationship_S)<<endl;
    gen.left_tuple.clear();
    gen.relationship_left.clear();
    gen.relationship_S.clear();
    /*--------------------以上是set cover问题的贪心算法----------------*/
}

int main(int argc, const char * argv[]) {
    cout<<"--------------------temporal db repair--------------------"<<endl;
    char tem_tuple_path[]="/Users/andy/Documents/Data/Emp100jmy.csv";
    char tem_tuple_weight_path[]="/Users/andy/Documents/Data/Emp100weight.csv";
    char tem_fd_path[]="/Users/andy/Documents/Data/Emp_FDs.txt";
    char tem_fd_weight_path[]="/Users/andy/Documents/Data/Emp_FDs_weight.txt";
    temporal_db_repair(tem_tuple_path,tem_tuple_weight_path,tem_fd_path,tem_fd_weight_path);
    cout<<"--------------------temporal db repair--------------------"<<endl;
    
    cout<<endl;
    
    cout<<"--------------------soft repair--------------------"<<endl;
    char rel_tuple_path[]="/Users/andy/Documents/Data/Hospital100.csv";
    char rel_tuple_weight_path[]="/Users/andy/Documents/Data/Hospital100weight.csv";
    char rel_fd_path[]="/Users/andy/Documents/Data/Hospital_FDs.txt";
    char rel_fd_weight_path[]="/Users/andy/Documents/Data/Hospital_FDs_weight.txt";
    soft_repair(rel_tuple_path,rel_tuple_weight_path,rel_fd_path,rel_fd_weight_path);
    cout<<"--------------------soft repair--------------------"<<endl;
    
    return 0;
}


