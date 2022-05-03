//
//  util.hpp
//  OPTGRSRTR5.0
//
//  Created by Andy on 2022/3/14.
//

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
using namespace std;

#ifndef util_hpp
#define util_hpp

#include <stdio.h>

extern double opt_f;
extern int opt_r;

extern "C" {
 #include "glpk.h"
}

//所有编号均从1开始

class Tuple{
public:
    size_t tuple_id;
    vector<string> attrs_value;
    string s_time;
    string e_time;
    double tuple_weight;
    map<string,int> break_point;
    int in_conflict;//in_conflict=1表示该元组参与冲突了
    size_t source_id;
    int x_star;
    double x;
    int xr;
    Tuple():tuple_id(0),tuple_weight(0),in_conflict(0),source_id(0),x_star(0),x(0.0),xr(0){}
    void print_all_info();
};

class Relationship{
public:
    pair<Tuple&,Tuple&> rel;
    size_t rel_id;
    double rel_weight;
    int rel_type;
    double y;
    int yr;
    Relationship(Tuple &t1, Tuple &t2):rel_id(0),rel_type(0),y(0.0),yr(0),rel(t1,t2){}
    void print_all_info();
};

class FD{
public:
    vector<int> lhs;
    vector<int> rhs;
    size_t fd_id;
    double fd_weight;
    FD():fd_id(0),fd_weight(0){}
    void print_all_info();
};
namespace std{
    template <>
    struct hash<std::pair<int, int> > {
    public:
            size_t operator()(pair<int, int> x) const throw() {
                 size_t h = 0;//something with x
                 return h;
            }
    };
}
class Element{
public:
    Tuple& t1;
    Tuple& t2;
    FD& fd;
    int ele_id;
    Element(Tuple &t1, Tuple &t2,FD &fd):t1(t1),t2(t2),fd(fd),ele_id(0){}
};

class Set1{
public:
    Tuple& t;
    double set_weight;
    vector<Element> element;
    int set_type;//1是元组t的set
    Set1(Tuple &t):t(t),set_weight(0.0),set_type(0){}
};

class Set2{
public:
    Tuple& t1;
    Tuple& t2;
    FD& fd;
    double set_weight;
    vector<Element> element;
    int set_type;//2是三元组(t1,t2,f)的set
    Set2(Tuple &t1, Tuple &t2,FD &fd):t1(t1),t2(t2),fd(fd),set_weight(0.0),set_type(0){}
};

class SoftResult{
public:
    size_t nums_tuples;
    size_t tuple_in_conflicts;
    size_t nums_conflicts;
    double error_rate;
    double Grepair_time;
    double Grepair_fopt;
    double Grepair_ropt;
    double Grepair_tleft;
    double Grepair_cleft;
    double Grepair_wcleft;
    double Greedy_time;
    double Greedy_ropt;
    double Greedy_tleft;
    double Greedy_cleft;
    double Greedy_wcleft;
    SoftResult():nums_tuples(0),tuple_in_conflicts(0),nums_conflicts(0),error_rate(0.0),Grepair_time(0.0),Grepair_fopt(0.0),Grepair_ropt(0.0),Grepair_tleft(0.0),Grepair_cleft(0.0),Grepair_wcleft(0.0),Greedy_time(0.0),Greedy_ropt(0.0),Greedy_tleft(0.0),Greedy_cleft(0.0),Greedy_wcleft(0.0){}
};

class Generator{
public:
    vector<string> attrs_name;
    vector<Tuple> source_tuple;
    vector<FD> fun_denp;
    vector<Tuple> normalized_tuple;
    vector<Tuple> left_tuple;
    unordered_map<pair<int,int>,double> for_weight;//relationship_S和relationship_G的冲突对weight是由for_weight来记录的
    unordered_map<pair<int,int>,double> for_weight_soft;//relationship_S的冲突对weight是由for_weight来记录的
    vector<Relationship> relationship_S;//1(冲突关系)/2（相邻关系）/3（association关系）
    vector<Relationship> relationship_G;//1(冲突关系)/2（相邻关系）/3（association关系）
    vector<Relationship> relationship_left;//留下的关系
    
    vector<Element> U;
    vector<Set1> S1;
    vector<Set2> S2;
    
    vector<int> UU;//for sc_greedy
    vector<vector<int>> C;//for sc_greedy
    
    vector<int> UU_lp;//for sc_lp
    vector<vector<int>> C_lp;//for sc_lp
    
    vector<pair<double,int>> weight_bef;//for sc_greedy
    vector<pair<double,int>> weight_aft;//for sc_greedy
    
    vector<pair<double,int>> weight_bef_lp;//for sc_lp
    vector<pair<double,int>> weight_aft_lp;//for sc_lp
    
    double greedy_opt=0;//for sc_greedy
    
    double soft_lp_opt=0;//for sc_lp: soft_lp_opt=soft_lp_wtuple+soft_lp_wconflict
    double soft_lp_wtuple=0;//for sc_lp被删元组权值加权和
    double soft_lp_wconflict=0;//for sc_lp留下冲突权值加权和
    int soft_lp_conflict=0;//for sc_lp留下冲突个数
    
    vector<SoftResult> SR;
    
    Generator():source_tuple(),normalized_tuple(),relationship_S(),relationship_G(){}
    Generator(char* data_file_path, char* data_type);
        //char* fd_file_path, char* conflict_file_path, char* associate_file_path
    void load_source_data(char* data_file_path, char* data_type);
    void load_fd_file(char* fd_file_path);
    void load_tuple_weight(char* data_weight_file_path,vector<Tuple> &source_tuple);
    void load_fd_weight(char* fd_weight_file_path,vector<FD> &fun_denp);
    void load_conflict_file();//没写呢
    void load_association_file();//没写呢
    void construct_conflict(vector<Tuple> &tuple, vector<Relationship> &relationship,char* con_type);
    void construct_left_conflict(vector<Tuple> &tuple, vector<Relationship> &relationship);
    void print_attrs_name();
    size_t number_of_tuples(vector<Tuple> &tuple);
    void print_every_tuple(vector<Tuple> &tuple);
    size_t number_of_fds();
    void print_every_fds();
    size_t number_of_tuples_in_conflict(vector<Tuple> &tuple);
    size_t number_of_conflicts(vector<Relationship> relationship);
    void print_every_rel(vector<Relationship> relationship);
    void normalization1();
    void Sort();
    void construct_adjacency(vector<Tuple> &tuple, vector<Relationship> &relationship);
    size_t number_of_adjacency(vector<Relationship> relationship);
    size_t number_of_association(vector<Relationship> relationship);
    void lp_solver_S_repair(vector<Tuple> &tuple, vector<Relationship> &relationship);
    void lp_solver_G_repair(vector<Tuple> &tuple, vector<Relationship> &relationship);
    /*
     //  无xi+xj>=1
     //  对yij的约束为：
     //  w>0
     //  yij<=xi+xj
     //  w<0
     //  yij>=xi
     //  yij>=xj
     //  没给association赋参数
    */
    void rounding(vector<Tuple> &tuple, vector<Relationship> &relationship);
    double calculate_opt_f_G(vector<Tuple> &tuple,vector<Relationship> &relationship);
    double calculate_opt_r_G(vector<Tuple> &tuple,vector<Relationship> &relationship);
    double calculate_opt_f_S(vector<Tuple> &tuple);
    double calculate_opt_r_S(vector<Tuple> &tuple);
    bool effectiveness_of_solution_G(vector<Relationship> &relationship);
    int span(vector<Tuple> &tuple);
    void construct_left_tuple(vector<Tuple> &tuple);
    void coalescing(vector<Tuple> &tuple);
    double weighted_left_conflicts(vector<Relationship> &relationship);
    
    void construct_sets();//真elements和sets
    void construct_sc();//将elements和sets构建成vector<int>和vector<vector<int>>
    void print_all_info_sc();//打印真elements和sets
    void print_all_info_sc_int();//打印vector<int>和vector<vector<int>>
    
    vector<int> v_difference(vector<int> &va, vector<int> &vb);
    vector<int> v_intersection(vector<int> &va, vector<int> &vb);
    int max_union(vector<int> &U, vector<vector<int> > &F);
    vector<int> greedy_sc(vector<int> &X, vector<vector<int> > &F);
    void clear_source_tuple();
    double calculate_opt_r_Greedy();
    double print_error_percentage(vector<Tuple> &tuple);
    void pollute_flight(vector<Tuple> &tuple,int size,double err_rate,string result_tuple_path,int nums_FD);
    void write_soft_result(char* soft_repair_result_path);
    //void random_tuple_weight(int size,double err_rate);
    vector<vector<int> > lp_sc(vector<int> X, vector<vector<int> > F);
    vector<double> lp_solver_sc(vector<int> X, vector<vector<int> > F);
    int get_f(vector<int> X, vector<vector<int> > F);
    void write_soft_result_lp(char* soft_repair_result_path);
};

#endif /* util_hpp */


