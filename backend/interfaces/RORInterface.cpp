#include <stdio.h>
#include <gcc-plugin.h>
#include <tree.h>
#include <gimple.h>
#include <tree-pass.h>
#include <gimple-iterator.h>
#include <gimple-pretty-print.h>

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

static void mutate_ROR(gimple *stmt, tree_code code, gimple_stmt_iterator *gsi)
{
  if (code == toBeMutated)
  {
    // 将操作符改为减法
    printf("Before mutation:");
    print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);

    // 如果语句是一个条件跳转语句
    if (gimple_code(stmt) == GIMPLE_COND)
    {
      gimple_assign_set_rhs_code(stmt, mutateTo);
      gsi_replace(gsi, stmt, true);

      printf("After mutation:");
      print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);
    }
    else if (is_gimple_assign(stmt)) // 如果语句是一个赋值语句
    {
      // 使用 gimple_build_cond 函数创建一个新的 gimple_cond 语句，将 pred_code 修改为 NE_EXPR
      // gcond *new_gc = gimple_build_cond(mutateTo
      //                                   ,gimple_cond_lhs(stmt)
      //                                   ,gimple_cond_rhs(stmt)
      //                                   ,gimple_cond_true_label((const gcond *)stmt)
      //                                   ,gimple_cond_false_label((const gcond *)stmt));
      // 用新的 gimple_cond 语句替换原来的语句
      gimple_assign_set_rhs_code(stmt, mutateTo);
      gsi_replace(gsi, stmt, true);

      printf("After mutation:");
      print_gimple_stmt(stdout, stmt, 0, TDF_SLIM | TDF_DETAILS);
    }

    // 输出变异信息
    printf("Mutated: changed %s to %s at %s:%d\n", get_tree_code_name(toBeMutated),
           get_tree_code_name(mutateTo), LOCATION_FILE(gimple_location(stmt)), LOCATION_LINE(gimple_location(stmt)));
  }
}

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

      // 如果语句是一个条件跳转语句
      if (gimple_code(stmt) == GIMPLE_COND)
      {
        // 获取 gimple_cond 的 pred_code
        enum tree_code code = gimple_cond_code(stmt);

        // 链接外部具体的算子
        mutate_ROR(stmt, code, &gsi);
      }
      else if (is_gimple_assign(stmt)) // 如果语句是一个赋值语句
      {
        // 获取赋值语句的操作符
        enum tree_code code = gimple_assign_rhs_code(stmt);

        // 链接外部具体的算子
        mutate_ROR(stmt, code, &gsi);
      }
    }
  }

  // 返回0表示成功
  return 0;
}