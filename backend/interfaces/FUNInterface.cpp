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


using std::cout;
using std::endl;
using std::vector;

extern tree_code toBeMutated;
extern tree_code mutateTo;

extern int start_line;
extern int end_line;

extern const char* VarName;

// 定义一个插件信息结构体
struct plugin_info my_plugin_info = {
  .version = "1.0",
  .help = "-fplugin-arg-plugin-start_line=<num>"
          "-fplugin-arg-plugin-end_line=<num>"
          "-fplugin-arg-plugin-var_name=<name>"
};

// 定义一个gimple pass的数据
struct pass_data mutate_pass_data =
{
  .type = GIMPLE_PASS,
  .name = "mutate",
  .optinfo_flags = OPTGROUP_NONE,
  .tv_id = TV_NONE,
  .properties_required = PROP_gimple_any,
  .properties_provided = PROP_gimple_any,
};


//记录函数的信息，包括了函数名，参数个数，参数类型和函数声明的tree
class MyFunction {
 private:
  // 函数名
  const char* fun_name;
  // 参数个数
  int num_args;
  // 参数类型
  vector <const char*> type_args;  
  // 函数声明的tree
  tree fun_decl;

 public:
  // 构造函数，初始化参数个数为0
  MyFunction(){ num_args = 0; };

  //获得函数的函数名
  const char* get_fun_name(){return fun_name;};
  //设置函数的函数名
  void set_fun_name(const char* funname){fun_name = funname;};

  //获得函数的参数个数
  int get_num_args(){return num_args;};
  //设置函数的参数个数
  void set_num_args(int argnum){num_args = argnum;};

  //设置函数的参数类型
  void set_type_args(const char* type)
  {
    type_args.push_back(type);
    //自动设置参数个数＋1
    set_num_args(get_num_args() + 1);
  }
  //获得函数的参数类型
  vector <const char*> get_arg_list()
  {
    return type_args;
  };

  //获得函数的声明的tree
  tree get_fun_decl(){ return fun_decl;};
  //设置函数的声明的tree
  void set_fun_decl(tree fundecl){ fun_decl = fundecl;};

  //打印函数的信息
  void print_fun()
  {
    cout<<"Function name: "<<get_fun_name()<<"   ";
    cout<<"Parameter list: (";

    for(int i = 0;i < get_num_args();i++){
      cout<<" "<<type_args[i];
      if(i != get_num_args() - 1)
        cout<<",";
    }
    cout<<" )"<<endl;
  };
};

//记录函数的信息的vector
typedef vector<MyFunction> MyFunctions;

//记录所有函数的信息的类
class MyFunctionList {
 public:
  //记录所有函数的信息的vector
  MyFunctions myFunctionlist;

  MyFunctionList(){ };

  //打印所有函数的信息
  void print_funs()
  {
    for( long unsigned int i = 0;i<myFunctionlist.size();i++){
      myFunctionlist[i].print_fun();
    }
  };

  //比较两个函数的参数列表是否相同
  bool compare_arg_list(MyFunction fun)
  {
    for(long unsigned int i = 0;i < myFunctionlist.size();i++)
    {
      //如果函数名不同
      if(strcmp(fun.get_fun_name(),myFunctionlist[i].get_fun_name()) != 0)
      {
        if(fun.get_num_args() == myFunctionlist[i].get_num_args())
        {
          vector <const char*> arg1 = myFunctionlist[i].get_arg_list(); 
          vector <const char*> arg2 = fun.get_arg_list();   
          for(int j = 0;j<fun.get_num_args();j++)
          {
            if(strcmp(arg1[j],arg2[j]) == 0)
              if(j == fun.get_num_args() - 1)
                return true;
          }
        }
      }
    }
    return false;
  };

  //比较两个函数的参数列表是否相同，如果相同，返回函数的声明的tree
  tree compare_arg_list_return_tree(MyFunction fun)
  {
    for(long unsigned int i = 0;i < myFunctionlist.size();i++)
    {
      //如果函数名不同
      if(strcmp(fun.get_fun_name(),myFunctionlist[i].get_fun_name()) != 0)
      {
        if(fun.get_num_args() == myFunctionlist[i].get_num_args())
        {
          vector <const char*> arg1 = myFunctionlist[i].get_arg_list(); 
          vector <const char*> arg2 = fun.get_arg_list();   
          for(int j = 0;j<fun.get_num_args();j++)
          {
            if(strcmp(arg1[j],arg2[j]) == 0)
              if(j == fun.get_num_args() - 1)
                return myFunctionlist[i].get_fun_decl();
          }
        }
      }
    }
    return NULL;
  };
};

MyFunctionList funs;

// 定义一个回调函数，用于在每个函数体之前执行
unsigned int execute_mutate()
{
  struct cgraph_node *n;
  MyFunctionList funcs;

  // 遍历每个函数
  FOR_EACH_FUNCTION (n)
  {
    MyFunction funt;
    funt.set_fun_decl(n->decl);
    const char *name = IDENTIFIER_POINTER (DECL_NAME (n->decl));
    funt.set_fun_name(name);

    // 获取函数的参数列表
    tree args = DECL_ARGUMENTS (n->decl);
    // 如果参数列表不为空
    if (args)
    {
      // 遍历参数列表
      while (args)
      {
        // 获取参数的类型
        tree arg_type = TREE_TYPE (args);
        funt.set_type_args(IDENTIFIER_POINTER (TYPE_IDENTIFIER ( (arg_type))));
        // 移动到下一个参数
        args = TREE_CHAIN (args);
      }
    }else
    {
      // 否则，打印空参数列表
      funt.set_type_args("void");
    }
    funcs.myFunctionlist.push_back(funt);
  }

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

      // 如果语句是一个赋值语句
      if (gimple_code(stmt) == GIMPLE_CALL)
      {
        // 获取函数的名称
        tree call_expr = gimple_call_fndecl(stmt);
        MyFunction funt;
        const char *function_name = IDENTIFIER_POINTER(DECL_NAME(call_expr));
        funt.set_fun_name(function_name);

        // 获取函数的参数列表
        // tree args = DECL_ARGUMENTS (call_expr);
        for (uint i = 0; i < gimple_call_num_args(stmt); ++i) 
        {
          tree arg = gimple_call_arg(stmt, i);
          // 如果参数列表不为空
          if (arg)
          {
            // 获取参数的类型
            tree arg_type = TREE_TYPE (arg);
            if(TYPE_IDENTIFIER ( (arg_type)) != NULL)
            {
              //const char *function_name = IDENTIFIER_POINTER (TYPE_IDENTIFIER ( (arg_type)));
              funt.set_type_args(IDENTIFIER_POINTER (TYPE_IDENTIFIER ( (arg_type))));
            }
          }else
          {
            // 否则，打印空参数列表
            funt.set_type_args("void");
          }
        }

        if(funcs.compare_arg_list(funt))
        {
          tree matching_fn = funcs.compare_arg_list_return_tree(funt);
        
          int com = comptypes (matching_fn,  gimple_call_fndecl(stmt));

          if( comptypes (matching_fn,  gimple_call_fndecl(stmt)) )
          {
            // Create a new function call with the same arguments
            gimple* new_call = gimple_build_call(matching_fn, gimple_call_num_args(stmt));

            // Copy the arguments from the original call to the new call
            for (unsigned int i = 0; i < gimple_call_num_args(stmt); i++) 
            {
                tree arg = gimple_call_arg(stmt, i);
                gimple_call_set_arg(new_call, i, arg);
            }

            // Replace the original call with the new call
            gsi_replace(&gsi, new_call, true);
            printf("A function with consistent parameter lists and return values with %s has been found :",IDENTIFIER_POINTER (DECL_NAME (call_expr)));
            print_gimple_stmt(stdout,new_call,0,TDF_SLIM | TDF_DETAILS);
          }
        }
        printf("Call :");
        funt.print_fun();
      }
    }
  }

  // 返回0表示没有改变
  return 0;
}