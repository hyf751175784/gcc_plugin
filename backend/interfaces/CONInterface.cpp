#include <gcc-plugin.h>
#include <plugin-version.h>
#include <tree.h>
#include <tree-pass.h>
#include <context.h>
#include <gimple.h>
#include <gimple-iterator.h>
#include <gimple-walk.h>
#include <cgraph.h>
#include <gimple-pretty-print.h>
#include <c-tree.h>

#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <string.h>

extern const char* toBeMutated;
extern const char* mutateTo;
// extern void mutate_AOR(gimple *, tree_code, gimple_stmt_iterator *);

extern int start_line;
extern int end_line;

extern const char* VarName;

using std::cout;
using std::endl;
using std::vector;

struct pass_data mutate_pass_data = {
    .type = GIMPLE_PASS,
    .name = "mutate",
    .optinfo_flags = OPTGROUP_NONE,
    .tv_id = TV_NONE,
    .properties_required = PROP_gimple_any,
    .properties_provided = PROP_gimple_any,
};

struct plugin_info my_plugin_info = {
    .version = "1.0",
    .help = "-fplugin-arg-plugin-start_line=<num>"
            "-fplugin-arg-plugin-end_line=<num>"
            "-fplugin-arg-plugin-var_name=<name>"
};


//记录函数的信息，包括了函数名，参数个数，参数类型和函数声明的tree
class VarRecord {
 private:
  // 函数名
  const char* var_name;
  tree var_decl;

 public:
  // 构造函数，初始化参数个数为0
  VarRecord(){ };

  //获得函数的函数名
  const char* get_var_name(){return var_name;};
  //设置函数的函数名
  void set_var_name(const char* varname){var_name = varname;};

  //获得函数的声明的tree
  tree get_var_decl(){ return var_decl;};
  //设置函数的声明的tree
  void set_var_decl(tree vardecl){ var_decl = vardecl;};

  //打印函数的信息
  void print_var()
  {
    cout<<"Var name: "<<get_var_name();
    cout<<" Var type: "<<get_tree_code_name(TREE_CODE(TREE_TYPE(var_decl)))<<endl;
  };
};

//记录函数的信息的vector
typedef vector<VarRecord> VarRecords;

//记录所有函数的信息的类
class VarRecordList {
 public:
  //记录所有函数的信息的vector
  VarRecords varRecordlist;

  VarRecordList(){ };

  //打印所有函数的信息
  void print_vars()
  {
    for( long unsigned int i = 0;i<varRecordlist.size();i++){
      varRecordlist[i].print_var();
    }
  };

    //比较两个函数的参数列表是否相同
  bool compare_type(tree varR)
  {
    for(long unsigned int i = 0;i < varRecordlist.size();i++)
    {
      tree type1 = TREE_TYPE(varR);
      tree type2 = TREE_TYPE(varRecordlist[i].get_var_decl());

      if(type1 == type2)
        return true;
    }
    return false;
  };

  //比较两个函数的参数列表是否相同，如果相同，返回函数的声明的tree
  tree compare_type_return_tree(tree varR)
  {
    for(long unsigned int i = 0;i < varRecordlist.size();i++)
    {
      tree type1 = TREE_TYPE(varR);
      tree type2 = TREE_TYPE(varRecordlist[i].get_var_decl());

      if(type1 == type2)
        return varRecordlist[i].get_var_decl();
    }
    return NULL;
  };

};

// 定义一个回调函数，用于在每个函数体之前执行
unsigned int execute_mutate(void)
{
  VarRecordList vars;
  tree var;
  unsigned i;
  FOR_EACH_LOCAL_DECL(cfun, i, var)
  {
    VarRecord varR;
    /* 排除未出现在源代码中的变量 */
    if (!DECL_ARTIFICIAL(var))
    {
      const char *tmp = get_name(var);
      varR.set_var_name(tmp);
      varR.set_var_decl(var);
      // varR.print_var();
      vars.varRecordlist.push_back(varR);
    }
  }

  vars.print_vars();

  basic_block bb;

  // 遍历gimple序列中的每个语句
  FOR_EACH_BB_FN(bb, cfun)
  {
    for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
    {
      // 获取当前语句
      gimple *stmt = gsi_stmt(gsi);

      if (!(start_line <= LOCATION_LINE(gimple_location(stmt)) &&
            (end_line >= LOCATION_LINE(gimple_location(stmt)) || end_line < 0)))
      {
        continue;
      }
      
      if(strcmp(toBeMutated,"var to const") == 0)
      {
        // 如果语句是一个赋值语句
        if (gimple_code(stmt) == GIMPLE_ASSIGN)
        {
          for (unsigned i = 0; i < gimple_num_ops(stmt); ++i) 
          {
            tree op = gimple_op(stmt, i);
            if(op != NULL_TREE)
            {
              if(TREE_CODE(op) == VAR_DECL || TREE_CODE(op) == PARM_DECL )
              {
                if(i != 0 && strcmp(get_name(op),VarName) == 0)
                {
                  printf("Before mutation:");
                  print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                  //构建一个int类型的常量
                  tree sub = build_int_cst(TREE_TYPE(op), 42);
                  gimple_set_op(stmt,i,sub);

                  printf("After mutation:");
                  print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                  printf("Mutated: change %s to const at %s:%d\n", get_name(op), LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));

                }
              }
            }
          }

        }else if (gimple_code(stmt) == GIMPLE_COND)
        {
          //printf("\n %d \n ",gimple_has_ops(stmt));
          if(gimple_has_ops(stmt))
          {
            //printf("Number of ops: %d \n ",gimple_num_ops(stmt));
            for (unsigned i = 0; i < gimple_num_ops(stmt); ++i) 
            {
              tree op = gimple_op(stmt, i);
              if(op != NULL_TREE)
              {
                if(TREE_CODE(op) == VAR_DECL || TREE_CODE(op) == PARM_DECL)
                {
                  //print_generic_stmt(stdout, op, TDF_SLIM);
                  //printf("TREE_CODE of node: %s\n", get_tree_code_name(TREE_CODE(op)));
                  //printf("TREE_TYPE of node: %s\n", get_tree_code_name(TREE_CODE(TREE_TYPE(op))));
                  if(i != 0 && strcmp(get_name(op),VarName) == 0)
                  {
                    printf("Before mutation:");
                    print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                    tree sub = build_int_cst(TREE_TYPE(op), 42);
                    gimple_set_op(stmt,i,sub);

                    printf("After mutation:");
                    print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                    printf("Mutated: change %s to const at %s:%d\n", get_name(op), LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));

                  }
                }
              }
            }
          }

        }else if (gimple_code(stmt) == GIMPLE_SWITCH)
        {
          //printf("\n %d \n ",gimple_has_ops(stmt));
          if(gimple_has_ops(stmt))
          {
            printf("Number of ops: %d \n ",gimple_num_ops(stmt));
            for (unsigned i = 0; i < gimple_num_ops(stmt); ++i) 
            {
              tree op = gimple_op(stmt, i);
              if(op != NULL_TREE)
              {
                if(TREE_CODE(op) == VAR_DECL || TREE_CODE(op) == PARM_DECL)
                {
                  // print_generic_stmt(stdout, op, TDF_SLIM);
                  // printf("TREE_CODE of node: %s\n", get_tree_code_name(TREE_CODE(op)));
                  // printf("TREE_TYPE of node: %s\n", get_tree_code_name(TREE_CODE(TREE_TYPE(op))));
                  if(i == 0 && strcmp(get_name(op),VarName) == 0)
                  {
                    printf("Before mutation:");
                    print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                    tree sub = build_int_cst(TREE_TYPE(op), 42);
                    gimple_set_op(stmt,i,sub);

                    printf("After mutation:");
                    print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                    printf("Mutated: change %s to const at %s:%d\n", get_name(op), LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));

                  }
                }
              }
            }
          }

        }else if (gimple_code(stmt) == GIMPLE_CALL)
        {
          //printf("\n %d \n ",gimple_has_ops(stmt));
          if(gimple_has_ops(stmt))
          {
            printf("Number of ops: %d \n ",gimple_num_ops(stmt));
            for (unsigned i = 0; i < gimple_num_ops(stmt); ++i) 
            {
              tree op = gimple_op(stmt, i);
              if(op != NULL_TREE)
              {
                if(TREE_CODE(op) == VAR_DECL || TREE_CODE(op) == PARM_DECL)
                {
                  // print_generic_stmt(stdout, op, TDF_SLIM);
                  // printf("TREE_CODE of node: %s\n", get_tree_code_name(TREE_CODE(op)));
                  // printf("TREE_TYPE of node: %s\n", get_tree_code_name(TREE_CODE(TREE_TYPE(op))));
                  if(i != 0 && strcmp(get_name(op),VarName) == 0)
                  {
                    if(vars.compare_type(op))
                    {
                      printf("Before mutation:");
                      print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                      tree sub = vars.compare_type_return_tree(op);
                      gimple_set_op(stmt,i,sub);

                      printf("After mutation:");
                      print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                      printf("Mutated: change %s to const at %s:%d\n", get_name(op), LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));
                    }
                  }
                }
              }
            }
          }
        }
      }else if(strcmp(toBeMutated,"insert const") == 0)
      {
        // 如果语句是一个赋值语句
        if (gimple_code(stmt) == GIMPLE_ASSIGN)
        {
          for (unsigned i = 0; i < gimple_num_ops(stmt); ++i) 
          {
            tree op = gimple_op(stmt, i);
            if(op != NULL_TREE)
            {
              if(TREE_CODE(op) == VAR_DECL || TREE_CODE(op) == PARM_DECL )
              {
                if(i != 0 && strcmp(get_name(op),VarName) == 0)
                {
                  tree var_scope = decl_function_context(op);
                  if (var_scope == NULL_TREE) 
                  {
                      // 变量在全局作用域中声明，是全局变量
                      printf("%s is a global variable.\n",get_name(op));
                  } else 
                  {
                      // 变量在某个函数的局部作用域中声明
                    printf("%s is a local variable.\n",get_name(op));
                    if(vars.compare_type(op))
                    {
                      tree sub = build_int_cst(TREE_TYPE(op), 42);
                      
                      op = build2(PLUS_EXPR, TREE_TYPE(op), op, sub);

                      if(comptypes(TREE_TYPE(op),TREE_TYPE(sub)))
                      {
                        //stmt = gimple_build_assign(lhs, rhs1);
                        //gsi_replace(&gsi, stmt, true);
                        printf("Before mutation:");
                        print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                        gimple_set_op(stmt,i,op);

                        printf("After mutation:");
                        print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                        printf("Mutated: insert const with %s at %s:%d\n", get_name(op), LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));

                      }
                    }
                  }
                }
              }
            }
          }

        }else if (gimple_code(stmt) == GIMPLE_COND)
        {
          //printf("\n %d \n ",gimple_has_ops(stmt));
          if(gimple_has_ops(stmt))
          {
            //printf("Number of ops: %d \n ",gimple_num_ops(stmt));
            for (unsigned i = 0; i < gimple_num_ops(stmt); ++i) 
            {
              tree op = gimple_op(stmt, i);
              if(op != NULL_TREE)
              {
                if(TREE_CODE(op) == VAR_DECL || TREE_CODE(op) == PARM_DECL)
                {
                  if(i == 0 && strcmp(get_name(op),VarName) == 0)
                  {
                    tree var_scope = decl_function_context(op);
                    if (var_scope == NULL_TREE) 
                    {
                        // 变量在全局作用域中声明，是全局变量
                        printf("%s is a global variable.\n",get_name(op));
                    } else 
                    {
                        // 变量在某个函数的局部作用域中声明
                      printf("%s is a local variable.\n",get_name(op));
                      if(vars.compare_type(op))
                      {
                        tree sub = build_int_cst(TREE_TYPE(op), 42);
                        
                        op = build2(PLUS_EXPR, TREE_TYPE(op), op, sub);

                        if(comptypes(TREE_TYPE(op),TREE_TYPE(sub)))
                        {
                          //stmt = gimple_build_assign(lhs, rhs1);
                          //gsi_replace(&gsi, stmt, true);
                          printf("Before mutation:");
                          print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                          gimple_set_op(stmt,i,op);
                          printf("After mutation:");
                          print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                          printf("Mutated: insert const with %s at %s:%d\n", get_name(op), LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));

                        }
                      }
                    }
                  }
                }
              }
            }
          }

        }else if (gimple_code(stmt) == GIMPLE_SWITCH)
        {
          //printf("\n %d \n ",gimple_has_ops(stmt));
          if(gimple_has_ops(stmt))
          {
            printf("Number of ops: %d \n ",gimple_num_ops(stmt));
            for (unsigned i = 0; i < gimple_num_ops(stmt); ++i) 
            {
              tree op = gimple_op(stmt, i);
              if(op != NULL_TREE)
              {
                if(TREE_CODE(op) == VAR_DECL || TREE_CODE(op) == PARM_DECL)
                {
                  if(i == 0 && strcmp(get_name(op),VarName) == 0)
                  {
                    tree var_scope = decl_function_context(op);
                    if (var_scope == NULL_TREE) 
                    {
                        // 变量在全局作用域中声明，是全局变量
                        printf("%s is a global variable.\n",get_name(op));
                    } else 
                    {
                        // 变量在某个函数的局部作用域中声明
                      printf("%s is a local variable.\n",get_name(op));
                      if(vars.compare_type(op))
                      {
                        tree sub = build_int_cst(TREE_TYPE(op), 42);
                        
                        op = build2(PLUS_EXPR, TREE_TYPE(op), op, sub);

                        if(comptypes(TREE_TYPE(op),TREE_TYPE(sub)))
                        {
                          //stmt = gimple_build_assign(lhs, rhs1);
                          //gsi_replace(&gsi, stmt, true);
                          printf("Before mutation:");
                          print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                          gimple_set_op(stmt,i,op);
                          printf("After mutation:");
                          print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                          printf("Mutated: insert const with %s at %s:%d\n", get_name(op), LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));

                        }
                      }
                    }
                  }
                }
              }
            }
          }

        }else if (gimple_code(stmt) == GIMPLE_CALL)
        {
          //printf("\n %d \n ",gimple_has_ops(stmt));
          if(gimple_has_ops(stmt))
          {
            printf("Number of ops: %d \n ",gimple_num_ops(stmt));
            for (unsigned i = 0; i < gimple_num_ops(stmt); ++i) 
            {
              tree op = gimple_op(stmt, i);
              if(op != NULL_TREE)
              {
                if(TREE_CODE(op) == VAR_DECL || TREE_CODE(op) == PARM_DECL)
                {
                  // print_generic_stmt(stdout, op, TDF_SLIM);
                  // printf("TREE_CODE of node: %s\n", get_tree_code_name(TREE_CODE(op)));
                  // printf("TREE_TYPE of node: %s\n", get_tree_code_name(TREE_CODE(TREE_TYPE(op))));
                  if(i != 0 && strcmp(get_name(op),VarName) == 0)
                  {
                    tree var_scope = decl_function_context(op);
                    if (var_scope == NULL_TREE) 
                    {
                      // 变量在全局作用域中声明，是全局变量
                      printf("%s is a global variable.\n",get_name(op));
                    } else 
                    {
                        // 变量在某个函数的局部作用域中声明
                      printf("%s is a local variable.\n",get_name(op));
                      if(vars.compare_type(op))
                      {
                        tree sub = build_int_cst(TREE_TYPE(op), 42);
                        
                        op = build2(PLUS_EXPR, TREE_TYPE(op), op, sub);

                        if(comptypes(TREE_TYPE(op),TREE_TYPE(sub)))
                        {
                          //stmt = gimple_build_assign(lhs, rhs1);
                          //gsi_replace(&gsi, stmt, true);
                          printf("Before mutation:");
                          print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                          gimple_set_op(stmt,i,op);
                          printf("After mutation:");
                          print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                          printf("Mutated: insert const with %s at %s:%d\n", get_name(op), LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));

                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }else if(strcmp(toBeMutated,"const to const") == 0)
      {
        // 如果语句是一个赋值语句
        if (gimple_code(stmt) == GIMPLE_ASSIGN)
        {
          for (unsigned i = 0; i < gimple_num_ops(stmt); ++i) 
          {
            tree op = gimple_op(stmt, i);
            if(op != NULL_TREE)
            {
              if(TREE_CODE(op) ==  INTEGER_CST)
              {
                if(i != 0 && strcmp(get_name(op),VarName) == 0)
                {
                  printf("Before mutation:");
                  print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                  tree sub = build_int_cst(TREE_TYPE(op), 42);
                  //op = build2(PLUS_EXPR, TREE_TYPE(op), op, sub);

                  gimple_set_op(stmt,i,sub);
                  printf("After mutation:");
                  print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                  printf("Mutated: change const to const at %s:%d\n", LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));

                }
              }
            }
          }

        }else if (gimple_code(stmt) == GIMPLE_COND)
        {
          //printf("\n %d \n ",gimple_has_ops(stmt));
          if(gimple_has_ops(stmt))
          {
            //printf("Number of ops: %d \n ",gimple_num_ops(stmt));
            for (unsigned i = 0; i < gimple_num_ops(stmt); ++i) 
            {
              tree op = gimple_op(stmt, i);
              if(op != NULL_TREE)
              {
                if(TREE_CODE(op) ==  INTEGER_CST)
                {
                  if(i != 0 && strcmp(get_name(op),VarName) == 0)
                  {
                    printf("Before mutation:");
                    print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                    tree sub = build_int_cst(TREE_TYPE(op), 42);
                    //op = build2(PLUS_EXPR, TREE_TYPE(op), op, sub);

                    gimple_set_op(stmt,i,sub);
                    printf("After mutation:");
                    print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                    printf("Mutated: change const to const at %s:%d\n", LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));

                  }
                }
              }
            }
          }

        }else if (gimple_code(stmt) == GIMPLE_SWITCH)
        {
          //printf("\n %d \n ",gimple_has_ops(stmt));
          if(gimple_has_ops(stmt))
          {
            printf("Number of ops: %d \n ",gimple_num_ops(stmt));
            for (unsigned i = 0; i < gimple_num_ops(stmt); ++i) 
            {
              tree op = gimple_op(stmt, i);
              if(op != NULL_TREE)
              {
                if(TREE_CODE(op) ==  INTEGER_CST)
                {
                  if(i != 0 && strcmp(get_name(op),VarName) == 0)
                  {
                    printf("Before mutation:");
                    print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                    tree sub = build_int_cst(TREE_TYPE(op), 42);
                    //op = build2(PLUS_EXPR, TREE_TYPE(op), op, sub);

                    gimple_set_op(stmt,i,sub);
                    printf("After mutation:");
                    print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                    printf("Mutated: change const to const at %s:%d\n", LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));

                  }
                }
              }
            }
          }

        }else if (gimple_code(stmt) == GIMPLE_CALL)
        {
          //printf("\n %d \n ",gimple_has_ops(stmt));
          if(gimple_has_ops(stmt))
          {
            printf("Number of ops: %d \n ",gimple_num_ops(stmt));
            for (unsigned i = 0; i < gimple_num_ops(stmt); ++i) 
            {
              tree op = gimple_op(stmt, i);
              if(op != NULL_TREE)
              {
                if(TREE_CODE(op) ==  INTEGER_CST)
                {
                  if(i != 0 && strcmp(get_name(op),VarName) == 0 )
                  {
                    printf("Before mutation:");
                    print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                    tree sub = build_int_cst(TREE_TYPE(op), 42);
                    //op = build2(PLUS_EXPR, TREE_TYPE(op), op, sub);

                    gimple_set_op(stmt,i,sub);
                    printf("After mutation:");
                    print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

                    printf("Mutated: change const to const at %s:%d\n", LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));

                  }
                }
              }
            }
          }
//
        }
      }
    }
  }

  // 返回0表示没有改变
  return 0;
}