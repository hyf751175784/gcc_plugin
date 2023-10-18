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
  // 获取当前函数的gimple序列
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
        // tree lhs = gimple_assign_lhs(stmt);
        // tree rhs = gimple_assign_rhs1(stmt);

        // check if the right hand side is a bit not expression (~x) | -x
        if (gimple_assign_rhs_code(stmt) == toBeMutated || gimple_assign_rhs_code(stmt) == mutateTo)
        {
          tree_code code = gimple_assign_rhs_code(stmt);

          printf("Before mutation:");
          print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

          gimple_assign_set_rhs_code(stmt, NOP_EXPR);
          gsi_replace(&gsi, stmt, true);

          printf("After mutation:");
          print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

          printf("Mutated:delete %s at %s:%d\n",get_tree_code_name(code), LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));

        }
      }
    }
  }

  // 返回0表示没有改变
  return 0;
}