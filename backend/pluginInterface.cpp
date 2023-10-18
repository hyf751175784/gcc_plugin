// 变异测试插件
// gcc版本：11.4
// 功能：将+号改为-号

#include <stdio.h>
#include <stdlib.h>
#include <gcc-plugin.h>
#include <coretypes.h>
#include <tree.h>
#include <context.h>
#include <tree-pass.h>
#include <gimple.h>
#include <gimple-iterator.h>
#include <gimple-walk.h>
#include <plugin-version.h>
#include <gimple-pretty-print.h>

// 声明插件信息
int plugin_is_GPL_compatible;

//开始变异行数
int start_line;
//结束变异行数
int end_line;

// 变异变量名
const char* VarName;

// 定义插件信息
extern struct plugin_info my_plugin_info;

// 定义一个回调函数，用于在每个函数体之前执行
extern unsigned int execute_mutate(void);

// 定义一个通道描述符，用于注册回调函数
extern struct pass_data mutate_pass_data;

// 定义一个通道实例，用于绑定回调函数和通道描述符
class mutate_pass : public gimple_opt_pass
{
public:
  mutate_pass(gcc::context *ctx) : gimple_opt_pass(mutate_pass_data, ctx) {}
  unsigned int execute(function *fun) { return execute_mutate(); }
};

// 注册通道并将其插入到通道列表中的适当位置
struct register_pass_info my_passinfo
{
  .pass = new mutate_pass(g),
  .reference_pass_name = "cfg",
  //.reference_pass_name = "ssa",
  .ref_pass_instance_number = 1,
  .pos_op = PASS_POS_INSERT_AFTER
  //.pos_op = PASS_POS_INSERT_BEFORE
};

// 定义插件初始化函数，用于注册通道
int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
  // 检查gcc版本是否匹配
  if (!plugin_default_version_check(version, &gcc_version))
  {
    printf("This plugin is for gcc %s\n", version->basever);
    return 1;
  }

  for (int i = 0; i < plugin_info->argc; i++) 
  {
    if (strncmp("start_line", plugin_info->argv[i].key, 10) == 0) 
    {
        start_line = atoi(plugin_info->argv[i].value);
        continue;
    }

    if (strncmp("end_line", plugin_info->argv[i].key, 8) == 0) 
    {
        end_line = atoi(plugin_info->argv[i].value);
        continue;
    }

    if (strncmp("var_name", plugin_info->argv[i].key, 8) == 0) 
    {
        VarName = plugin_info->argv[i].value;
        continue;
    }
  }
  
  // 注册插件信息和参数
  register_callback(plugin_info->base_name, PLUGIN_INFO, NULL, (void *)&my_plugin_info);
  register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &my_passinfo);

  return 0;
}