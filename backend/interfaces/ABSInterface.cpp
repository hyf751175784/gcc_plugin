#include <stdio.h>
#include <gcc-plugin.h>
#include <tree.h>
#include <gimple.h>
#include <tree-pass.h>
#include <gimple-iterator.h>
#include <gimple-pretty-print.h>
#include <symtab.h>

extern tree_code toBeMutated;
extern tree_code mutateTo;
// extern void mutate_AOR(gimple *, tree_code, gimple_stmt_iterator *);

extern int start_line;
extern int end_line;

extern const char* VarName;

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

// 定义一个回调函数，用于在每个函数体之前执行
unsigned int execute_mutate(void)
{
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
      if (is_gimple_assign(stmt))
      {

        tree lhs, rhs1, rhs2;
        lhs = gimple_assign_lhs(stmt);
        rhs1 = gimple_assign_rhs1(stmt);
        rhs2 = gimple_assign_rhs2(stmt);

        // if(lhs != NULL){
        //   if(TREE_CODE(lhs) == VAR_DECL || TREE_CODE(lhs) == PARM_DECL){
        //     if(DECL_NAME(lhs) != NULL && strcmp(IDENTIFIER_POINTER(DECL_NAME(lhs)),"e") == 0){
        //     }
        //   }
        // }
        // 在lsh的局部变量前面插abs感觉怪怪的，先注释掉

        if (rhs1 != NULL)
        {
          if (TREE_CODE(rhs1) == VAR_DECL || TREE_CODE(rhs1) == PARM_DECL)
          {
            if (DECL_NAME(rhs1) != NULL && strcmp(IDENTIFIER_POINTER(DECL_NAME(rhs1)), "e") == 0)
            {

              printf("Before mutation:");
              print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

              rhs1 = build1(ABS_EXPR, TREE_TYPE(lhs), rhs1);
              rhs1 = build2(gimple_assign_rhs_code(stmt), TREE_TYPE(lhs), rhs1, rhs2);
              stmt = gimple_build_assign(lhs, rhs1);
              gsi_replace(&gsi, stmt, true);

              printf("After mutation:");
              print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

              // 输出变异信息
              printf("Mutated: insert %s at %s:%d\n", get_tree_code_name(ABS_EXPR), LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));
            }
          }
        }

        if (rhs2 != NULL)
        {
          if (TREE_CODE(rhs2) == VAR_DECL || TREE_CODE(rhs2) == PARM_DECL)
          {
            if (DECL_NAME(rhs2) != NULL && strcmp(IDENTIFIER_POINTER(DECL_NAME(rhs2)), "e") == 0)
            {

              printf("Before mutation:");
              print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

              rhs2 = build1(ABS_EXPR, TREE_TYPE(lhs), rhs2);
              rhs1 = build2(gimple_assign_rhs_code(stmt), TREE_TYPE(lhs), rhs1, rhs2);
              stmt = gimple_build_assign(lhs, rhs1);
              gsi_replace(&gsi, stmt, true);

              printf("After mutation:");
              print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

              // 输出变异信息
              printf("Mutated: insert %s at %s:%d\n", get_tree_code_name(ABS_EXPR), LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));
            }
          }
        }
      }
    }
  }

  // 返回0表示成功
  return 0;
}